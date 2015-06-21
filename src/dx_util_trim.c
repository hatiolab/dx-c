// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the ImageNext Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//

#include "dx_util_trim.h"

#include <fcntl.h>

char* strcpytrim(char*dest, char* src, int mode, char* delim) {
	char* o = dest;	// save origin.
	char* e = 0;
	char dtab[256] = {0};

	if(!src || !dest)
		return	0;

	if(!delim)
		delim = " \t\n\f";
	while(*delim)
		dtab[*delim++] = 1;

	while((*dest = *src++) != 0) {
		if(!dtab[0xFF & (unsigned int)*dest]) {
			e = 0;
		} else {
			if(!e)
				e = dest;
			if(mode == STRLIB_MODE_ALL || ((mode != STRLIB_MODE_RIGHT) && (dest == o)))
				continue;
		}
		dest++;
	}
	if(mode != STRLIB_MODE_LEFT && e) {
		*e = 0;
	}
	return o;
}

char* strtriml(char* d, char* s) {
	return strcpytrim(d, s, STRLIB_MODE_LEFT, 0);
}

char* strtrimr(char* d, char* s) {
	return strcpytrim(d, s, STRLIB_MODE_RIGHT, 0);
}

char* strtrim(char* d, char* s) {
	return strcpytrim(d, s, STRLIB_MODE_BOTH, 0);
}

char* strkill(char* d, char* s) {
	return strcpytrim(d, s, STRLIB_MODE_ALL, 0);
}

char* triml(char* s) {
	return strcpytrim(s, s, STRLIB_MODE_LEFT, 0);
}

char* trimlr(char* s) {
	return strcpytrim(s, s, STRLIB_MODE_RIGHT, 0);
}

char* trim(char* s) {
	return strcpytrim(s, s, STRLIB_MODE_BOTH, 0);
}

char* trimkill(char* s) {
	return strcpytrim(s, s, STRLIB_MODE_ALL, 0);
}

