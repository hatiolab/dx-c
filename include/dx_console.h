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
#define DX_CONSOLE_PROMPT "#"

int dx_console_start(dx_event_handler handler);

void dx_print_console_prompt();

#endif /* __DX_CONSOLE_H */
