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
