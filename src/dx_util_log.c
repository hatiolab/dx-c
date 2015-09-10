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
#include <execinfo.h>
#include <malloc.h>

#include "dx.h"

#include "dx_util_log.h"

#define DX_TRACE_STACK_SIZE 100

FILE* _dx_logfile = NULL;

void dx_loginit() {
	_dx_logfile = fopen("dx.log", "a");
}

FILE* dx_logfile() {
	if(_dx_logfile == NULL)
		dx_loginit();

	return _dx_logfile;
}

void dx_trace() {
	/* -rdynamic link option 필요 */
	void* buffer[DX_TRACE_STACK_SIZE];
	char** funcNames;
	int i, nptrs;

	nptrs = backtrace(buffer, DX_TRACE_STACK_SIZE);

	funcNames = backtrace_symbols(buffer, nptrs);

	if(funcNames == NULL) {
		perror("Backtrace Symbols");
		return;
	}

	for(i = 0;i < nptrs;i++)
		CONSOLE("%s\n", funcNames[i]);

	free(funcNames);
}
