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

#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "dx.h"

#include "dx_util_clock.h"

#ifndef CLOCK_MONOTONIC
# warning "Old glibc (< 2.3.4) does not provide this constant. We use syscall directly so this definition is safe."
# define CLOCK_MONOTONIC 1
#endif

#if !defined(SYS_clock_gettime)
# define SYS_clock_gettime __NR_clock_gettime
#endif

int dx_clock_get_abs_msec(LONGLONG *s_msec_ptr)
{
    struct timespec s_timespec;

    if(syscall(SYS_clock_gettime /* __NR_clock_gettime */, (int)CLOCK_MONOTONIC, (void *)(&s_timespec)) != 0) {
        return(-1);
    }

    *s_msec_ptr = (((LONGLONG)s_timespec.tv_sec) * ((LONGLONG)1000u)) + (((LONGLONG)s_timespec.tv_nsec) / ((LONGLONG)1000000u));

    return(0);
}
