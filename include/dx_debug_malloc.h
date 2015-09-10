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

#ifndef __DX_UTIL_MALLOC_H
#define __DX_UTIL_MALLOC_H

#include <stddef.h>

#include "dx.h"

/*
 * Definitions
 */

#ifdef DX_MALLOC_DEBUG

void* dx_malloc(size_t sz, char* fname, int line);
void dx_free(void* p, char* filename, int line);
void dx_chkmem();

#define MALLOC(z) (dx_malloc(z, __FILE__, __LINE__))
#define FREE(p) (dx_free(p, __FILE__, __LINE__))
#define CHKMEM() (dx_chkmem());

#else

#define MALLOC(z) (malloc(z))
#define FREE(p) (free(p))
#define CHKMEM()

#endif /* DX_DEBUG */

#endif /* DX_UTIL_MALLOC_H */
