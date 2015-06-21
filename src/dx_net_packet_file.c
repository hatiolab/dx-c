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

#include "dx_net_packet_file.h"

#include <stdio.h>		// For FILE, fopen, fclose
#include <stddef.h>		// For NULL
#include <string.h>		// For memcpy
#include <sys/socket.h>
#include <stdlib.h>		// For malloc
#include <fcntl.h>		// For read, write
#include <stdint.h>		// For uint32_t, ...
#include <sys/stat.h>	// For stat
#include <dirent.h>		// For DIR, opendir, ...
#include <errno.h>		// For errno

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_net_packet_io.h"

int dx_packet_get_filelist(int fd, char* path) {
	return dx_packet_send_string(fd, DX_PACKET_TYPE_FILE, DX_FILE_GET_LIST, path);
}

int dx_packet_send_filelist(int fd, char* path) {
	DIR* dir;
	struct dirent* entry;
	struct stat statbuf;
	char fullpath[1024];
	int count = 0;
	dx_fileinfo_array_packet_t* packet;
	uint32_t packet_len = 0;
	int i = 0;

	dir = opendir(path);
	if(NULL != dir) {
		/* scan to get count */
		while((entry = readdir(dir))) {
			sprintf(fullpath, "%s/%s", path, entry->d_name);
			if(-1 == stat(fullpath, &statbuf)) {
				perror("stat() error");
			} else if(S_ISREG(statbuf.st_mode)) {
				count++;
			}
		}
		closedir(dir);

		packet_len = DX_FILEINFO_ARRAY_PACKET_SIZE(count);
		packet = (dx_fileinfo_array_packet_t*)MALLOC(packet_len);
		packet->header.len = htonl(packet_len);
		packet->header.type = DX_PACKET_TYPE_FILE;
		packet->header.code = DX_FILE_LIST;
		packet->header.data_type = DX_DATA_TYPE_FILEINFO_ARRAY;
		packet->array.len = htonl(count);
		strncpy((char*)packet->array.path, path, DX_PATH_MAX_SIZE);

		/* reopen */
		dir = opendir(path);
		while((entry = readdir(dir))) {
			sprintf(fullpath, "%s/%s", path, entry->d_name);
			if(-1 == stat(fullpath, &statbuf)) {
				perror("Packet - stat() error");
			} else if(S_ISREG(statbuf.st_mode)) {
				struct dx_fileinfo_data* info = &(packet->array.fileinfo[i]);

				strncpy((char*)info->path, entry->d_name, DX_PATH_MAX_SIZE);
				info->size = htonl(statbuf.st_size);
				info->mtime = htonl(statbuf.st_mtime);
				if(++i >= count)
					break;
			}
		}
		closedir(dir);

		dx_write(fd, packet, packet_len);

		FREE(packet);
	}

	return 0;
}

int dx_packet_get_file(int fd, char* path, uint32_t begin, uint32_t end) {
	dx_file_query_packet_t* packet;
	uint32_t len = DX_FILE_QUERY_PACKET_SIZE;

	packet = (dx_file_query_packet_t*)MALLOC(len);

	packet->header.len = htonl(len);
	packet->header.type = DX_PACKET_TYPE_FILE;
	packet->header.code = DX_FILE_GET;
	packet->header.data_type = DX_DATA_TYPE_FILE_PARTIAL_QUERY;
	packet->file.offset_begin = htonl(begin);
	packet->file.offset_end = htonl(end);

	strncpy((char*)&(packet->file.path), path, DX_PATH_MAX_SIZE);

	dx_write(fd, packet, len);

	FREE(packet);

	return 0;
}

int dx_packet_delete_file(int fd, char* path) {
	return 0;
}

int dx_packet_send_file(int fd, char* path, uint32_t begin, uint32_t end) {
	uint8_t* data = NULL;
	int total_len = 0;
	int partial_len;
	int file;

	file = open(path, O_RDONLY);
	if(-1 == file) {
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

		lseek(file, begin, SEEK_SET);

		while(partial_len - nread > 0) {
			n = dx_read_with_block_mode(file, data + nread, partial_len - nread);
			if(n > 0) {
				nread += n;
			} else {
				perror("Packet - read() error"); // TODO Error Handling
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

		packet->header.len = htonl(packet_len);
		packet->header.type = DX_PACKET_TYPE_FILE;
		packet->header.code = DX_FILE;
		packet->header.data_type = DX_DATA_TYPE_FILE_PARTIAL;
		packet->file.total_len = htonl(total_len);
		packet->file.partial_len = htonl(partial_len);
		packet->file.offset_begin = htonl(begin);
		packet->file.offset_end = htonl(end);

		strncpy((char*)packet->file.path, path, DX_PATH_MAX_SIZE);

		if(partial_len)
			memcpy(&(packet->file.content), data, partial_len);

		dx_write(fd, packet, packet_len);

		FREE(packet);
	}

	if(data)
		FREE(data);

	return 0;
}
