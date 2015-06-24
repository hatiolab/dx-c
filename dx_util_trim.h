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

/*
 * 이 코드는 아래 링크의 샘플 코드를 참조하였음을 밝힘.
 *
 * http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
 *
 **/

#ifndef __DX_UTIL_TRIM_H
#define __DX_UTIL_TRIM_H

/*
 * Definitions
 */

#define STRLIB_MODE_ALL     0
#define STRLIB_MODE_LEFT    1
#define STRLIB_MODE_RIGHT   2
#define STRLIB_MODE_BOTH    3

char* strcpytrim(char*dest, char* src, int mode, char* delim);

char* strtriml(char* d, char* s);
char* strtrimr(char* d, char* s);
char* strtrim(char* d, char* s);
char* strkill(char* d, char* s);
char* triml(char* s);
char* trimlr(char* s);
char* trim(char* s);
char* trimkill(char* s);

#endif /* __DX_UTIL_TRIM_H */
