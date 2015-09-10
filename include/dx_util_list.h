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

#ifndef __DX_UTIL_LIST_H
#define __DX_UTIL_LIST_H

#include <stdarg.h>
#include <pthread.h>

#include "dx.h"

/*
 * Definitions
 */

typedef int (*dx_find_function)(void* data1, void* data2);
typedef int (*dx_destroyer_function)(void* data1);

typedef void (*dx_list_iterator_callback)(void* node_data, va_list ap);

typedef struct dx_list_node dx_list_node_t;

struct dx_list_node {
  dx_list_node_t  *prev;
  dx_list_node_t  *next;

  void      *data;
};

typedef struct dx_list {
  dx_list_node_t      *head;
  dx_list_node_t      *tail;
  dx_find_function    finder;
  dx_destroyer_function   destroyer;
#ifdef DX_MULTITHREADED
  pthread_mutexattr_t mutex_attr;
  pthread_mutex_t	mutex;
#endif
} dx_list_t;

int dx_list_init(dx_list_t* plist, dx_find_function finder, dx_destroyer_function destroyer);
void dx_list_close(dx_list_t* plist);

int dx_list_size(dx_list_t* plist);
int dx_list_add(dx_list_t* plist, void* data);
int dx_list_remove(dx_list_t* plist, void* data);
int dx_list_clear(dx_list_t* plist);
int dx_list_iterator(dx_list_t* plist, dx_list_iterator_callback callback, ...);

#endif /* __DX_UTIL_LIST_H */
