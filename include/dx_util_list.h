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

#ifndef __DX_UTIL_LIST_H
#define __DX_UTIL_LIST_H

/*
 * Definitions
 */

typedef	int (*dx_find_function)(void* data1, void* data2);
typedef	int (*dx_destroyer_function)(void* data1);

typedef struct dx_list_node dx_list_node_t;

struct dx_list_node {
	dx_list_node_t	*prev;
	dx_list_node_t	*next;

	void			*data;
};

typedef struct dx_list {
	dx_list_node_t			*head;
	dx_list_node_t			*tail;
	dx_find_function		finder;
	dx_destroyer_function 	destroyer;
} dx_list_t;

int dx_list_init(dx_list_t* plist, dx_find_function finder, dx_destroyer_function destroyer);
int dx_list_add(dx_list_t* plist, void* data);
int dx_list_remove(dx_list_t* plist, void* data);
int dx_list_clear(dx_list_t* plist);

#endif /* __DX_UTIL_LIST_H */
