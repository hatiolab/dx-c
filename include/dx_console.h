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

#ifndef __DX_CONSOLE_H
#define __DX_CONSOLE_H

#include "dx_event_mplexer.h"

/*
 * Definitions
 */
#define DX_CONSOLE_PROMPT "#!"

typedef void (*dx_console_exit_callback)();
typedef void (*dx_console_menu_handler)(char* cmdline);

typedef struct dx_console_menu dx_console_menu_t;

typedef struct dx_console_menu {
	int id;
	char* command;
	char* parameters;
	char* description;
	dx_console_menu_handler handler;
} dx_console_menu_t;

int dx_console_start(dx_console_menu_t* menus, dx_console_exit_callback exit_callback);

void dx_print_console_prompt(dx_console_menu_t* menus, dx_console_menu_t* current);

#endif /* __DX_CONSOLE_H */
