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

#include "dx_util_list.h"

#include <stdlib.h> // For malloc

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

int dx_list_finder_default(void* p1, void* p2) {
  return p1 - p2;
}

int dx_list_destroyer_default(void* p) {
  return 0;
}

int dx_list_init(dx_list_t* plist, dx_find_function finder, dx_destroyer_function destroyer) {

  plist->head = NULL;
  plist->tail = NULL;

  plist->finder = finder == NULL ? dx_list_finder_default : finder;
  plist->destroyer = destroyer == NULL ? dx_list_destroyer_default : destroyer;

  return 0;
}

int dx_list_size(dx_list_t* plist) {
  int sz = 0;
  dx_list_node_t* pnode = plist->head;

  while(pnode != NULL) {
    sz++;
    pnode = pnode->next;
  }

  return sz;
}

int dx_list_add(dx_list_t* plist, void* data) {
  dx_list_node_t* pnode;

  pnode = (dx_list_node_t*)MALLOC(sizeof(dx_list_node_t));

  pnode->data = data;

  pnode->prev = plist->tail;
  pnode->next = NULL;

  if(pnode->prev != NULL)
    pnode->prev->next = pnode;
  else
    plist->head = pnode;

  plist->tail = pnode;

  return 0;
}

int dx_list_remove(dx_list_t* plist, void* data) {
  dx_list_node_t* pnode = plist->head;

  while(pnode && plist->finder(pnode->data, data) != 0) {
    pnode = pnode->next;
  }

  if(pnode == NULL)
    return -1; // Not found.

  if(pnode->prev == NULL) { // head
    plist->head = pnode->next;
  } else {
    pnode->prev->next = pnode->next;
  }

  if(pnode->next == NULL) { // tail
    plist->tail = pnode->prev;
  } else {
    pnode->next->prev = pnode->prev;
  }

  plist->destroyer(pnode->data);

  FREE(pnode);

  return 0;
}

int dx_list_iterator(dx_list_t* plist, dx_list_iterator_callback callback) {
  dx_list_node_t* pnode = plist->head;

  while(pnode != NULL) {
    callback(pnode->data);
    pnode = pnode->next;
  }

  return 0;
}

int dx_list_clear(dx_list_t* plist) {
  while(plist->head) {
    dx_list_remove(plist, plist->head->data);
  }

  ASSERT("plist->last should be NULL", !plist->tail);

  return 0;
}
