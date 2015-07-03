// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the Hatio, Lab. Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

#include <stdio.h>
#include <sys/epoll.h>    // For epoll
#include <unistd.h>     // For STDIN_FILENO
#include <string.h>		// For strtok_r

#include "dx.h"

#include "dx_util_log.h"

#include "dx_console.h"
#include "dx_event_mplexer.h"

typedef dx_console_menu_t* (*dx_console_menu_traverse_callback)(dx_console_menu_t* menus, dx_console_menu_t* menu, void* closure, void** out);

int dx_console_handler(dx_event_context_t* context);

dx_console_menu_t* dx_console_menu_get_parent(dx_console_menu_t* menus, dx_console_menu_t* menu);
dx_console_menu_t* dx_console_menu_traverse(dx_console_menu_t* menus, dx_console_menu_t* current, dx_console_menu_traverse_callback callback, void* closure, void** out);

dx_console_menu_t* dx_console_menu_print_callback(dx_console_menu_t* menus, dx_console_menu_t* current, void* nothing, void** nothing2) {
	CONSOLE("- %s %s : %s\n", current->command, current->parameters, current->description);
	return NULL; /* keep going */
}

void dx_console_menu_recursive_prompt(dx_console_menu_t* menus, dx_console_menu_t* child, char* prompt) {
	const char top_menu[] = "TOP";

	/* end condition */
	if(child == NULL) {
		sprintf(prompt, top_menu);

		return;
	}

	dx_console_menu_t* parent = dx_console_menu_get_parent(menus, child);

	dx_console_menu_recursive_prompt(menus, parent, prompt);

	sprintf(prompt, "%s > %s", prompt, child->command);
}

void dx_print_console_prompt(dx_console_menu_t* menus, dx_console_menu_t* current) {
	char top_desc[] = "You are on the way to start (TOP MENU)";

	char* desc = (current == NULL) ? top_desc : current->description;

	char prompt[128] = {0};

	dx_console_menu_recursive_prompt(menus, current, prompt);

	CONSOLE("\n-------------------------------------------------\n\n");
	CONSOLE("[%s] %s\n\n", prompt, desc);
	dx_console_menu_traverse(menus, current, dx_console_menu_print_callback, NULL, NULL);
	CONSOLE("\n");
	CONSOLE("- up : move a step up\n");
	CONSOLE("- top : move to top\n");
	CONSOLE("- exit :  exit\n\n");
	CONSOLE("%s ", DX_CONSOLE_PROMPT);

	fflush(stdout);
}

int dx_console_start(dx_console_menu_t* menus, dx_console_exit_callback exit_callback) {
	dx_event_context_t* pcontext;

	// STDIN_FILENO를 이벤트 컨텍스트로 등록한다.
	pcontext = dx_event_context_create();
	pcontext->fd = STDIN_FILENO;
	pcontext->readable_handler = dx_console_handler;
	pcontext->writable_handler = NULL;
	pcontext->error_handler = NULL;

	pcontext->pdata = menus;
	pcontext->user_handler = exit_callback;

	dx_add_event_context(pcontext, EPOLLIN);

	dx_print_console_prompt(menus, NULL);

	return 0;
}

/*
 * 자신의 한단계 하위 메뉴들을 트래버스하면서, 중단된 메뉴를 리턴한다.
 * 모든 아이템이 다 트래버스되었다면, NULL을 리턴한다.
 */
dx_console_menu_t* dx_console_menu_traverse(dx_console_menu_t* menus, dx_console_menu_t* current, dx_console_menu_traverse_callback callback, void* closure, void** out) {
	int i;
	int current_id;
	dx_console_menu_t* found;

	if(current == NULL) {
		current_id = 0; /* root */
		i = -1; /* for all */
	} else {
		current_id = current->id;
		i = current - menus;
	}

	if(current != NULL && current_id == 0)
		return NULL; /* the last */

	/* for children */
	i++;
	while(menus[i].id != 0 && menus[i].id >= current_id * 10) {
		if(current_id == 0) {
			if(menus[i].id >= 10) {
				/* grand children 은 skip */
				i++;
				continue;
			}
		} else if(menus[i].id >= current_id * 100) {
			/* grand children 은 skip */
			i++;
			continue;
		}

		found = callback(menus, &menus[i], closure, out);
		if(NULL != found)
			return found;
		i++;
	}

	return NULL;
}

dx_console_menu_t* dx_console_menu_find_by_id(dx_console_menu_t* menus, int id) {
	int i = 0;

	if(id == 0)
		return NULL;

	while(menus[i].id != 0 && menus[i].id != id)
		i++;

	if(menus[i].id == 0)
		return NULL;

	return &menus[i];
}

dx_console_menu_t* dx_console_menu_get_parent(dx_console_menu_t* menus, dx_console_menu_t* menu) {
	int id;

	if(menu == NULL)
		return NULL;

	id = menu->id;

	id = (id - (id % 10)) / 10;

	return dx_console_menu_find_by_id(menus, id);
}

dx_console_menu_t* dx_console_menu_search_callback(dx_console_menu_t* menus, dx_console_menu_t* menu, void* cmdline, void** trailer) {
	const char* whitespace = " \t\n\f";

	dx_console_menu_t* found;
	char cmd_clone[128];
	char* command;
	int sz;

	if(cmdline == NULL || strlen((char*)cmdline) == 0)
		return NULL;

	/* cmdline 을 복사해둔다. */
	strncpy(cmd_clone, (char*)cmdline, 128);

	command = strtok_r((char*)cmdline, whitespace, (char**)trailer);

	if(command == NULL || strlen(command) == 0) {
		/* 못찾았으므로 원본 cmdline 복구 */
		strncpy((char*)cmdline, cmd_clone, 128);
		return NULL;
	}

	sz = strlen(command) > strlen(menu->command) ? strlen(menu->command) : strlen(command);

	if(strncmp(command, menu->command, sz) == 0) { /* match */
		found = dx_console_menu_traverse(menus, menu, dx_console_menu_search_callback, *trailer, (void**)trailer);
		if(found != NULL)
			return found;
		return menu;
	}

	/* 못찾았으므로 원본 cmdline 복구 */
	strncpy((char*)cmdline, cmd_clone, 128);
	return NULL;
}

dx_console_menu_t* dx_console_menu_find_menu_by_command(dx_console_menu_t* menus, dx_console_menu_t* current, char* cmdline, void** trailer) {
//	const char* whitespace = " \t\n\f";
	const char* command_up = "up";
	const char* command_top = "top";

	dx_console_menu_t* found;

	if(cmdline == NULL || strlen(cmdline) == 0)
		return current;

	if(strncmp(cmdline, command_top, strlen(command_top)) == 0)
		return NULL; /* return root */

	if(strncmp(cmdline, command_up, strlen(command_up)) == 0)
		return dx_console_menu_get_parent(menus, current); /* return parent */

	found = dx_console_menu_traverse(menus, current, dx_console_menu_search_callback, cmdline, trailer);
	if(found != NULL)
		return found;

	return current;
}

/* console handler */

dx_console_menu_t* dx_console_current_menu = NULL;

int dx_console_handler(dx_event_context_t* context) {
	const char* command_exit = "exit";
    char cmdline[128];
    char* remains = NULL;
	dx_console_menu_t* menus = (dx_console_menu_t*)context->pdata;
	dx_console_menu_t* copied;

    bzero(cmdline, 128);

    ssize_t nbytes = read(context->fd, cmdline, sizeof(cmdline));
    if(0 == nbytes) {
        CONSOLE("Console hung up\n");
        return -1;
    } else if(0 > nbytes) {
        perror("Console read() error");
        close(context->fd);
        dx_del_event_context(context);
        return -2;
    }

    /* demo exit */
	if(cmdline != NULL && strncmp(cmdline, command_exit, strlen(command_exit)) == 0) {
		if(context->user_handler != NULL)
			((dx_console_exit_callback)context->user_handler)();

		dx_event_mplexer_wakeup();

		CONSOLE("\nBye..\n\n");

		return 0;
	}

	/* find current menu using command line. */
	copied = dx_console_current_menu;
	dx_console_current_menu = dx_console_menu_find_menu_by_command(menus, dx_console_current_menu, cmdline, (void**)&remains);

	if(dx_console_current_menu != NULL && dx_console_current_menu->handler != NULL) {
		dx_console_current_menu->handler(remains);
		dx_console_current_menu = copied; /* 핸들러를 실행한 후에는 바로 전 메뉴모드로 돌아감 */
	} else if(dx_console_current_menu == copied && nbytes > 1) {
		/* nbytes 크기가 1인 경우는 그냥 enter key만 누른 경우 이므로 제외한다. */
		ERROR("invalid command.");
	}

   	dx_print_console_prompt(menus, dx_console_current_menu);

    return 0;
}
