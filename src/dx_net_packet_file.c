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

#include "dx_net_packet_file.h"

#include <stdio.h>    // For FILE, fopen, fclose
#include <stddef.h>   // For NULL
#include <string.h>   // For memcpy
#include <sys/socket.h>
#include <stdlib.h>   // For malloc
#include <fcntl.h>    // For read, write
#include <stdint.h>   // For uint32_t, ...
#include <sys/stat.h> // For stat
#include <dirent.h>   // For DIR, opendir, ...
#include <errno.h>    // For errno

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_net_packet_io.h"

/* Internal Functions */
char* __dx_scan_dir_path = NULL;

int dx_sort_by_ctime(const struct dirent** file1, const struct dirent** file2) {
  /* 시간의 역순으로 정렬 */
  struct stat info1, info2;
  char path1[1024];
  char path2[1024];

  sprintf(path1, "%s/%s", __dx_scan_dir_path, (*file1)->d_name);
  sprintf(path2, "%s/%s", __dx_scan_dir_path, (*file2)->d_name);

  stat(path1, &info1);
  stat(path2, &info2);

  double diff = difftime(info1.st_ctime, info2.st_ctime);

  if(diff < 0)
    return -1;

  if(diff > 0)
    return 1;

  return 0;
}

int dx_filter_fileonly(const struct dirent* file) {
  /* 디렉토리는 제외 */
  struct stat stats;
  char path[1024];

  sprintf(path, "%s/%s", __dx_scan_dir_path, file->d_name);

  return stat(path, &stats) == 0 && !S_ISDIR(stats.st_mode);
}

dx_fileinfo_array_packet_t* dx_packet_fill_fileinfo_array_packet(char* path) {
  struct dirent** entrylist;
  dx_fileinfo_array_packet_t* packet = NULL;
  uint32_t packet_len = 0;
  int count, i = 0;
  char fullpath[1024];
  struct stat statbuf;

  __dx_scan_dir_path = path;

  count = scandir(path, &entrylist, dx_filter_fileonly, dx_sort_by_ctime);
  if(count < 0) { // TODO 잘못된 폴더인 경우에는 빈 패킷을 만든다.
	  packet_len = DX_FILEINFO_ARRAY_PACKET_SIZE(0);
	  packet = (dx_fileinfo_array_packet_t*)MALLOC(packet_len);

	  bzero(packet, packet_len);
//	  strncpy((char*)packet->array.path, path, DX_PATH_MAX_SIZE);

	  dx_packet_set_header((dx_packet_t*)packet, packet_len, DX_PACKET_TYPE_FILE, DX_FILE_LIST, DX_DATA_TYPE_FILEINFO_ARRAY);

	  packet->array.len = htonl(0);
	  strncpy((char*)packet->array.path, path, DX_PATH_MAX_SIZE);

	  return packet;
  }

  packet_len = DX_FILEINFO_ARRAY_PACKET_SIZE(count);
  packet = (dx_fileinfo_array_packet_t*)MALLOC(packet_len);
//  strncpy((char*)packet->array.path, path, DX_PATH_MAX_SIZE);
  bzero(packet, packet_len);

  dx_packet_set_header((dx_packet_t*)packet, packet_len, DX_PACKET_TYPE_FILE, DX_FILE_LIST, DX_DATA_TYPE_FILEINFO_ARRAY);

  packet->array.len = htonl(count);
  strncpy((char*)packet->array.path, path, DX_PATH_MAX_SIZE);

  for(count--;count >= 0;count--,i++) {
    sprintf(fullpath, "%s/%s", path, entrylist[count]->d_name);

    if(-1 == stat(fullpath, &statbuf)) {
      perror("Packet - stat() error");
    } else if(S_ISREG(statbuf.st_mode)) {
      struct dx_fileinfo_data* info = &(packet->array.fileinfo[i]);

      strncpy((char*)info->path, entrylist[count]->d_name, DX_PATH_MAX_SIZE);
      info->size = htonl(statbuf.st_size);
      info->mtime = htonl(statbuf.st_mtime);
    }

    free(entrylist[count]); // Never Use FREE(..)
  }

  free(entrylist); // Never Use FREE(..)

  return packet;
}

/* Global Functions */

int dx_packet_get_filelist(int fd, char* path) {
  return dx_packet_send_string(fd, DX_PACKET_TYPE_FILE, DX_FILE_GET_LIST, path);
}

int dx_packet_send_filelist(int fd, char* path) {
  dx_fileinfo_array_packet_t* packet = dx_packet_fill_fileinfo_array_packet(path);

  if(packet != NULL)
    dx_write(fd, packet, ntohl(packet->header.len), 0);

  return 0;
}

int dx_packet_get_file(int fd, char* path, uint32_t begin, uint32_t end) {
  dx_file_query_packet_t* packet;
  uint32_t len = DX_FILE_QUERY_PACKET_SIZE;

  packet = (dx_file_query_packet_t*)MALLOC(len);
  bzero(packet, len);

  dx_packet_set_header((dx_packet_t*)packet, len, DX_PACKET_TYPE_FILE, DX_FILE_GET, DX_DATA_TYPE_FILE_PARTIAL_QUERY);

  packet->file.offset_begin = htonl(begin);
  packet->file.offset_end = htonl(end);

  strncpy((char*)&(packet->file.path), path, DX_PATH_MAX_SIZE);

  dx_write(fd, packet, len, 0);

  return 0;
}

int dx_packet_delete_file(int fd, char* path) {
  struct stat stats;

  if(stat(path, &stats) == 0 && !S_ISDIR(stats.st_mode))
    remove(path);

  return 0;
}

int dx_packet_send_file(int fd, char* path, uint32_t begin, uint32_t end) {
  uint8_t* data = NULL;
  int total_len = 0;
  int partial_len = 0;
  int file;

  file = open(path, O_RDONLY);
  if(-1 == file) {
	total_len = 0;
	begin = 0;
	end = 0;
	partial_len = 0;

    perror("Packet - open() error");
  } else {
    int nread = 0, n;
    struct stat st;

    stat(path, &st);
    total_len = st.st_size;

    begin = begin > (total_len - 1) ? total_len - 1 : begin;
    end = end > (total_len - 1) ? total_len - 1 : end;
    partial_len = end - begin + 1;

    nread = 0, n;
    data = MALLOC(partial_len);
    /* TODO Memory Allocation 없앨 것. 직접 패킷에서 읽을 것. */

    lseek(file, begin, SEEK_SET);

    while(partial_len - nread > 0) {
      n = dx_read_with_block_mode(file, data + nread, partial_len - nread);
      if(n > 0) {
        nread += n;
      } else {
        perror("Packet - read() error"); // TODO Error Handling. 에러 응답이 가능할 것. 패킷헤더의 flag를 이용.
        if(errno == EINPROGRESS)
          continue;
        break;
      }
    }
    close(file);
  }

  {
    dx_file_packet_t* packet;
    uint32_t packet_len = DX_FILE_PACKET_SIZE(partial_len);

    packet = (dx_file_packet_t*)MALLOC(packet_len);
    bzero(packet, packet_len);

    dx_packet_set_header((dx_packet_t*)packet, packet_len, DX_PACKET_TYPE_FILE, DX_FILE, DX_DATA_TYPE_FILE_PARTIAL);

    packet->file.total_len = htonl(total_len);
    packet->file.partial_len = htonl(partial_len);
    packet->file.offset_begin = htonl(begin);
    packet->file.offset_end = htonl(end);

    strncpy((char*)packet->file.path, path, DX_PATH_MAX_SIZE);

    if(partial_len > 0)
      memcpy(&(packet->file.content), data, partial_len);

    dx_write(fd, packet, packet_len, 0);
  }

  if(data)
    FREE(data);

  return 0;
}
