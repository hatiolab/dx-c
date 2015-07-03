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

#ifndef __DX_UTIL_LOG_H
#define __DX_UTIL_LOG_H

#include <stdio.h>

FILE* dx_logfile();

#ifdef DX_DEBUG

#define LOG(fmt, ...) do { fprintf(dx_logfile(), fmt "=> %s:%d:%s()\n", ##__VA_ARGS__, __FILE__, __LINE__, __func__); } while(0);
#define CONSOLE(fmt, ...) do { fprintf(stdout, fmt, ##__VA_ARGS__); } while(0);
#define DEBUG(fmt, ...) do { fprintf(stdout, "[[[\n" fmt "]]]\n", ##__VA_ARGS__); } while(0);
#define ERROR(fmt, ...) do { fprintf(stderr, fmt "=> %s:%d:%s()\n", ##__VA_ARGS__, __FILE__, __LINE__, __func__); } while(0);

#else

#define LOG(fmt, ...) do { fprintf(dx_logfile(), fmt "\n", ##__VA_ARGS__); } while(0);
#define CONSOLE(fmt, ...) do { fprintf(stdout, fmt, ##__VA_ARGS__); } while(0);
#define DEBUG(fmt, ...) do { fprintf(stdout, "[[[\n" fmt "]]]\n", ##__VA_ARGS__); } while(0);
#define ERROR(fmt, ...) do { fprintf(stderr, fmt "=> %s:%d:%s()\n", ##__VA_ARGS__, __FILE__, __LINE__, __func__); } while(0);

#endif

#endif /* __DX_UTIL_LOG_H */
