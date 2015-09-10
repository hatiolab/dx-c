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

#ifndef __DX_UTIL_LOCK_H
#define __DX_UTIL_LOCK_H

#include "dx.h"

#ifdef DX_MULTITHREADED

#define	DX_LOCK_GLOBAL_INIT()	dx_lock_global_init();
#define	DX_LOCK_GLOBAL_DESTROY()	dx_lock_global_destroy();
#define	DX_LOCK_GLOBAL()	dx_lock_global();
#define	DX_UNLOCK_GLOBAL()	dx_unlock_global();

#define	DX_LOCK_INIT(m,a)	dx_lock_init(m,a);
#define	DX_LOCK_DESTROY(m)	dx_lock_destroy(m);
#define	DX_LOCK(m)	dx_lock(m);
#define	DX_UNLOCK(m)	dx_unlock(m);

void dx_lock_global_init();
void dx_lock_global_destroy();
void dx_lock_global();
void dx_unlock_global();

void dx_lock_init(pthread_mutex_t* mutex, pthread_mutexattr_t* attr);
void dx_lock_destroy(pthread_mutex_t* mutex);
void dx_lock(pthread_mutex_t* mutex);
void dx_unlock(pthread_mutex_t* mutex);

#else

#define	DX_LOCK_GLOBAL_INIT()
#define	DX_LOCK_GLOBAL_DESTROY()
#define	DX_LOCK_GLOBAL()
#define	DX_UNLOCK_GLOBAL()

#define	DX_LOCK_INIT(m,a)
#define	DX_LOCK_DESTROY(m)
#define	DX_LOCK(m)
#define	DX_UNLOCK(m)

#endif

#endif /* __DX_UTIL_LOCK_H */
