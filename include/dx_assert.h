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

#ifndef __DX_ASSERT_H
#define __DX_ASSERT_H

#include <stdio.h>

/*
 * Definitions
 */

#define ASSERT(s,t)	if(!t) { printf("[ASSERT] : %s (%s(%d))\n", s, __FILE__, __LINE__); }

#define MALLOC(z) (dxalloc(z, __FILE__, __LINE__))
#define FREE(p) (dxfree(p))

#endif /* DX_ASSERT_H */
