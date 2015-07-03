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

#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include "dx.h"

#ifndef F_GETPATH
#define F_GETPATH	(1024 + 7)
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

char dx_filepath_buffer[PATH_MAX];

char* dx_file_get_path(int fd) {

	if(fcntl(fd, F_GETPATH, dx_filepath_buffer) != -1)
		return dx_filepath_buffer;

	return NULL;
}

int dx_file_is_closed(int fd) {
	return fcntl(fd, F_GETFL) < 0 && errno == EBADF;
}
