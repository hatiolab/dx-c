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

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

void print_usage()
{
    printf(
         "Options:\n"
         "-d | --device name   Video device name \n"
         "-h | --help     Print this message\n"
         "-m | --mmap     Use memory mapped buffers [default]\n"
         "-r | --read     Use read() calls\n"
         "-u | --userp   Use application allocated buffers\n"
         "-o | --output Outputs stream to stdout\n"
         "-f | --format Force format to 640x480 YUYV\n"
         "-c | --count   Number of frames to grab \n"
         ""
        );
}

const char short_options[] = "d:hmruofc:";

const struct option long_options[] = {
    { "device", required_argument, NULL, 'd' },
    { "help",   no_argument,       NULL, 'h' },
    { "mmap",   no_argument,       NULL, 'm' },
    { "read",   no_argument,       NULL, 'r' },
    { "userp",  no_argument,       NULL, 'u' },
    { "output", no_argument,       NULL, 'o' },
    { "format", no_argument,       NULL, 'f' },
    { "count",  required_argument, NULL, 'c' },
    { 0, 0, 0, 0 }
};
