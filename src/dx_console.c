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

void dx_print_console_prompt() {
	printf("%s ", DX_CONSOLE_PROMPT);

	fflush(stdout);
}

int dx_console_start(dx_event_handler handler) {
	dx_event_context_t* pcontext;

	// STDIN_FILENO를 이벤트 컨텍스트로 등록한다.
	pcontext = dx_event_context_create();
	pcontext->fd = STDIN_FILENO;
	pcontext->readable_handler = handler;
	pcontext->writable_handler = NULL;
	pcontext->error_handler = NULL;

	dx_add_event_context(pcontext, EPOLLIN);

	printf("DX Console Started..\n");

	dx_print_console_prompt();

	return 0;
}

typedef dx_console_menu_t* (*dx_console_menu_traverse_callback)(dx_console_menu_t* menus, dx_console_menu_t* menu, void* closure);

/*
 * 자신의 한단계 하위 메뉴들을 트래버스하면서, 중단된 메뉴를 리턴한다.
 * 모든 아이템이 다 트래버스되었다면, NULL을 리턴한다.
 */
dx_console_menu_t* dx_console_menu_traverse(dx_console_menu_t* menus, dx_console_menu_t* current, dx_console_menu_traverse_callback callback, void* closure) {
	int i;
	int current_id;
	dx_console_menu_t* found;

	if(current == NULL) {
		current_id = 0; /* root */
		i = -1; /* for all */
	} else {
		current_id = current->id;
		i = (current - menus) / sizeof(dx_console_menu_t);
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

		found = callback(menus, &menus[i], closure);
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

	if(menus[i].id != 0)
		return NULL;

	return &menus[i];
}

dx_console_menu_t* dx_console_menu_get_parent(dx_console_menu_t* menus, dx_console_menu_t* menu) {
	int id = menu->id;

	id = (id - (id % 10)) / 10;

	return dx_console_menu_find_by_id(menus, id);
}

void* dx_console_menu_search_callback(dx_console_menu_t* menus, dx_console_menu_t* menu, char* cmdline) {
	const char* whitespace = " \t\n\f";

	dx_console_menu_t* found;
	char* trailer;
	char* command;
	int sz;

	command = strtok_r(cmdline, whitespace, &trailer);

	if(command == NULL || strlen(command) == 0)
		return NULL;

	sz = strlen(command) > strlen(menu->command) ? strlen(menu->command) : strlen(command);

	if(strncmp(command, menu->command, sz) == 0) { /* match */
		found = dx_console_menu_traverse(menus, menu, dx_console_menu_search_callback, trailer);
		if(found != NULL)
			return found;
		return menu;
	}

	return NULL;
}

dx_console_menu_t* dx_console_menu_find_menu_by_command(dx_console_menu_t* menus, dx_console_menu_t* current, char* cmdline) {
	const char* whitespace = " \t\n\f";
	const char* command_up = "up";
	const char* command_exit = "exit";

	char* next_token;
	char* command = strtok_r(cmdline, whitespace, &next_token);

	dx_console_menu_t* found;

	if(command == NULL || strlen(command) == 0)
		return current;

	if(strncmp(command, command_exit, strlen(command_exit)) == 0)
		return NULL; /* return root */

	if(strncmp(command, command_up, strlen(command_up)) == 0)
		return dx_console_menu_get_parent(menus, current); /* return parent */

	found = dx_console_menu_traverse(menus, current, dx_console_menu_search_callback, cmdline);
	if(found != NULL)
		return found;

	return current;
}

