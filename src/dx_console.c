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

#include "dx_console.h"
#include "dx_event_mplexer.h"

typedef dx_console_menu_t* (*dx_console_menu_traverse_callback)(dx_console_menu_t* menus, dx_console_menu_t* menu, void* closure, void** out);

dx_console_menu_t* dx_console_menu_get_parent(dx_console_menu_t* menus, dx_console_menu_t* menu);
dx_console_menu_t* dx_console_menu_traverse(dx_console_menu_t* menus, dx_console_menu_t* current, dx_console_menu_traverse_callback callback, void* closure, void** out);

void* dx_console_menu_print_callback(dx_console_menu_t* menus, dx_console_menu_t* current, void* nothing, void** nothing2) {
	printf("- [%s] %s\n", current->command, current->description);
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
	const char top_desc[] = "You are on the way to start (TOP MENU)";

	char* desc = (current == NULL) ? top_desc : current->description;

	char prompt[128] = {0};

	dx_console_menu_recursive_prompt(menus, current, prompt);

	printf("-------------------------------------------------\n\n");
	printf("[%s] %s\n", prompt, desc);
	dx_console_menu_traverse(menus, current, dx_console_menu_print_callback, NULL, NULL);
	printf("\n");
	printf("- [up] move a step up\n");
	printf("- [top] move to top\n");
	printf("- [exit] exit\n");
	printf(" %s ", DX_CONSOLE_PROMPT);

	fflush(stdout);
}

int dx_console_start(dx_event_handler handler, dx_console_menu_t* menus) {
	dx_event_context_t* pcontext;

	// STDIN_FILENO를 이벤트 컨텍스트로 등록한다.
	pcontext = dx_event_context_create();
	pcontext->fd = STDIN_FILENO;
	pcontext->readable_handler = handler;
	pcontext->writable_handler = NULL;
	pcontext->error_handler = NULL;

	pcontext->pdata = menus;

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
		} else if(menus[i].id > current_id * 100) {
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

void* dx_console_menu_search_callback(dx_console_menu_t* menus, dx_console_menu_t* menu, char* cmdline, char** trailer) {
	const char* whitespace = " \t\n\f";

	dx_console_menu_t* found;
	char cmd_clone[128];
	char* command;
	int sz;

	if(cmdline == NULL || strlen(cmdline) == 0)
		return NULL;

	/* cmdline 을 복사해둔다. */
	strncpy(cmd_clone, cmdline, 128);

	command = strtok_r(cmdline, whitespace, trailer);

	if(command == NULL || strlen(command) == 0) {
		/* 못찾았으므로 원본 cmdline 복구 */
		strncpy(cmdline, cmd_clone, 128);
		return NULL;
	}

	sz = strlen(command) > strlen(menu->command) ? strlen(menu->command) : strlen(command);

	if(strncmp(command, menu->command, sz) == 0) { /* match */
		found = dx_console_menu_traverse(menus, menu, dx_console_menu_search_callback, *trailer, trailer);
		if(found != NULL)
			return found;
		return menu;
	}

	/* 못찾았으므로 원본 cmdline 복구 */
	strncpy(cmdline, cmd_clone, 128);
	return NULL;
}

dx_console_menu_t* dx_console_menu_find_menu_by_command(dx_console_menu_t* menus, dx_console_menu_t* current, char* cmdline, void** trailer) {
	const char* whitespace = " \t\n\f";
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

