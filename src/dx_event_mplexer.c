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

#include "dx_event_mplexer.h"

#include <stdio.h>
#include <stdlib.h>     // For exit, ..
#include <sys/epoll.h>    // For epoll
#include <unistd.h>     // For close
#include <string.h>		// For memset

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_buffer.h"
#include "dx_util_list.h"
#include "dx_util_log.h"

#include "dx_net_packet.h"
#include "dx_event_control.h"

dx_event_mplexer_t* __dx_mplexer;

int dx_event_context_compare(void* context1, void* context2);
int dx_event_context_destroyer(void* data);

int dx_event_mplexer_create() {

  if(__dx_mplexer)
    return 0; // Already exist.

  __dx_mplexer = (dx_event_mplexer_t*)MALLOC(sizeof(dx_event_mplexer_t));

  dx_list_init(&__dx_mplexer->context_list, dx_event_context_compare, dx_event_context_destroyer);

  __dx_mplexer->fd = epoll_create1(EPOLL_CLOEXEC);
  if(__dx_mplexer->fd < 0) {
    perror("Multiplexer - epoll_create() error");
    return -1;
  }

  __dx_mplexer->events = (struct epoll_event*)MALLOC(sizeof(struct epoll_event) * DX_MAX_EVENT_POLL_SIZE);

  dx_event_control_start(&__dx_mplexer->control_fd);

  return 0;
}

int dx_event_mplexer_destroy() {
  /*
   * 현재 이벤트 폴링을 진행중인 쓰레드만이 이 메쏘드를 호출하도록 제한한다.
   */
  if(__dx_mplexer->state == DX_EVENT_MPLEXER_STATE_POLLING)
    ASSERT("Only Polling Thread can destroy Mplexer", __dx_mplexer->polling_thread)

  dx_clear_event_context();
  dx_list_close(&__dx_mplexer->context_list);

  FREE(__dx_mplexer->events);

  close(__dx_mplexer->fd);
  __dx_mplexer->fd = 0;

  FREE(__dx_mplexer);
  __dx_mplexer = NULL;

  return 0;
}

int dx_event_mplexer_poll(int ts) {
  int i, n;

  /*
   * 현재 쓰레드를 저장한다.
   */
  __dx_mplexer->state = DX_EVENT_MPLEXER_STATE_POLLING;
  __dx_mplexer->polling_thread = pthread_self();

  n = epoll_wait(__dx_mplexer->fd, __dx_mplexer->events, DX_MAX_EVENT_POLL_SIZE, ts);
  if(-1 == n) {
    perror("Multiplexer - epoll_wait() error");
    exit(1);
  }
  /*
   * TODO turn over된 태스크를 여기서 처리한다.
   */

  /*
   * 발생한 모든 이벤트를 처리한다.
   */
  for(i = 0;i < n;i++) {
    struct dx_event_context *context = (struct dx_event_context*)__dx_mplexer->events[i].data.ptr;
    uint32_t flags = __dx_mplexer->events[i].events;

    if((flags & EPOLLIN) && context->readable_handler) {
      context->readable_handler(context);
    }
    if((flags & EPOLLOUT) && context->writable_handler) {
      context->writable_handler(context);
    }
    if((flags & EPOLLERR) && context->error_handler) {
      context->error_handler(context);
    }
  }

  /*
   * pthread_t 변수값을 초기화하는 공식적인 방법이 없으므로, 항상 mplexer의 state를 먼저 확인한다.
   */
  __dx_mplexer->state = DX_EVENT_MPLEXER_STATE_NOTHING;

  return __dx_mplexer->state;
}

int dx_event_mplexer_wakeup() {
  /*
   * 컨트롤 파이프에 한바이트 보내서, event_mplexer를 깨운다.
   */
  uint8_t control = 0;
  write(__dx_mplexer->control_fd, &control, 1);

  return 0;
}

/*
 * event_context_list에서 event_list를 찾기 위해 사용되는 비교함수이다.
 * event_context의 fd가 같으면 같은 event_context로 간주한다.
 */
int dx_event_context_compare(void* context1, void* context2) {
  return ((dx_event_context_t*)context1)->fd - ((dx_event_context_t*)context2)->fd;
}

dx_event_context_t* dx_event_context_create() {
  dx_event_context_t* pcontext;

  pcontext = (dx_event_context_t*)MALLOC(sizeof(struct dx_event_context));
  memset(pcontext, 0x0, sizeof(struct dx_event_context));
  pcontext->fd = -1;

  return pcontext;
}

/*
 * 클로즈된 이벤트 컨텍스트를 파괴한다.
 *
 * 읽기 전용 바이트버퍼를 해제하고, 이벤트 컨텍스트에 할당된 메모리를 해제한다.
 */
int dx_event_context_destroyer(void* data) {
  dx_event_context_t* context = (dx_event_context_t*)data;

  epoll_ctl(__dx_mplexer->fd, EPOLL_CTL_DEL, context->fd, NULL);

  /*
   * 바이트버퍼들을 해제한다.
   */
  if(context->pbuf_reading != NULL)
    dx_buffer_free(context->pbuf_reading);

  if(context->plist_writing != NULL) {
    dx_list_close(context->plist_writing);
    FREE(context->plist_writing);
    context->plist_writing = NULL;
  }

  /* 각자 사용한 pdata를 해제할 수 있는 기회를 제공한다. */
  if(context->on_destroy != NULL)
    context->on_destroy(context->pdata);

  FREE(context);

  return 0;
}

int dx_add_event_context(struct dx_event_context* pcontext, uint32_t events) {
  struct epoll_event event;

  dx_list_add(&__dx_mplexer->context_list, pcontext);

  event.events = events;
  event.data.ptr = pcontext;

  epoll_ctl(__dx_mplexer->fd, EPOLL_CTL_ADD, pcontext->fd, &event);

  return 0;
}

int dx_mod_event_context(struct dx_event_context* context, uint32_t events) {
  struct epoll_event event;

  event.events = events;
  event.data.ptr = context;
  if(0 > epoll_ctl(__dx_mplexer->fd, EPOLL_CTL_MOD, context->fd, &event)) {
    perror("Multiplexer - epoll_ctl (EPOLL_CTL_MOD) error.");
  }

  return 0;
}

int dx_del_event_context(struct dx_event_context* context) {
  dx_list_remove(&__dx_mplexer->context_list, context);

  return 0;
}

int dx_clear_event_context() {
  dx_list_clear(&__dx_mplexer->context_list);

  return 0;
}

dx_event_context_t* dx_get_event_context(int fd) {
  dx_list_t* plist = &__dx_mplexer->context_list;

  dx_list_node_t* pnode = plist->head;

  while(pnode != NULL && ((dx_event_context_t*)pnode->data)->fd != fd)
    pnode = pnode->next;

  return pnode == NULL ? NULL : (dx_event_context_t*)pnode->data;
}

void dx_event_context_touch(dx_event_context_t* context) {
	if(context == NULL)
		return;
	dx_clock_get_abs_msec(&context->last_clock_touch);
}
