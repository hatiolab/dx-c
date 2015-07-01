// Copyright (c) 2015 - 2015 All Right Reserved, http://imagenext.co.kr
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

#include "omnid.h"

int od_host_handler_file_list(int fd, dx_packet_t* p);
int od_host_handler_file_get(int fd, dx_packet_t* p);
int od_host_handler_file_get_list(int fd, dx_packet_t* p);
int od_host_handler_file_delete(int fd, dx_packet_t* p);
int od_host_handler_file(int fd, dx_packet_t* p);

int od_host_handler_get_file_list(int fd, dx_packet_t* packet) {
    switch(packet->header.code) {

	case OD_FILE_GET_LIST : 	/* 파일리스트 요청 */
		od_host_handler_file_get_list(fd, packet);
		break;
	case OD_FILE_LIST : /* 파일리스트 정보 */
		od_host_handler_file_list(fd, packet);
		break;
	case OD_FILE_GET : /* 부분 파일 내용 요청 */
		od_host_handler_file_get(fd, packet);
		break;
	case OD_FILE : /* 부분 파일 내용 */
		od_host_handler_file(fd, packet);
		break;
	case OD_FILE_DELETE : /* 파일 삭제 요청 */
		od_host_handler_file_delete(fd, packet);
		break;
    }

	return 0;
}

int od_host_handler_file_list(int fd, dx_packet_t* p) {
	int count = 0;
	int i = 0;
	dx_fileinfo_array_packet_t* packet = (dx_fileinfo_array_packet_t*)p;
	char timebuf[20];
	time_t time;

	count = ntohl(packet->array.len);
    printf("(Host Event Handling) FileList(path: %s, count : %d)\n", packet->array.path, count);

    for(i = 0;i < count;i++) {
    	dx_fileinfo_data_t* info = &(packet->array.fileinfo[i]);
    	time = ntohl(info->mtime);
    	strftime(timebuf, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
    	printf("%-32s [%d] %-20s\n", info->path, ntohl(info->size), timebuf);
    }

	return 0;
}

int od_host_handler_file_get(int fd, dx_packet_t* p) {

	return 0;
}

int od_host_handler_file_get_list(int fd, dx_packet_t* p) {

	return 0;
}

int od_host_handler_file_delete(int fd, dx_packet_t* p) {
	return 0;
}

int od_host_handler_file(int fd, dx_packet_t* p) {
	int total_len, offset_begin, offset_end;
	dx_file_packet_t* packet = (dx_file_packet_t*)p;
	char path[DX_PATH_MAX_SIZE + 1];

	total_len = ntohl(packet->file.total_len);
//	partial_len = ntohl(packet->file.partial_len);
	offset_begin = ntohl(packet->file.offset_begin);
	offset_end = ntohl(packet->file.offset_end);
	bzero(path, DX_PATH_MAX_SIZE + 1);
	strncpy(path, (char*)packet->file.path, DX_PATH_MAX_SIZE);

	printf("(Host Event Handling) File(path : %s, size : %d - %d of %d)\n", path, offset_begin, offset_end, total_len);

	if(offset_end < total_len - 1) {
		offset_begin = offset_end + 1;
		offset_end = (offset_end + DX_FILE_PARTIAL_MAX_SIZE) > (total_len - 1) ? (total_len - 1) : (offset_end + DX_FILE_PARTIAL_MAX_SIZE);
		dx_packet_get_file(fd, path, offset_begin, offset_end);
	} else {
		printf("(Host Event Handling) File(path : %s, size : %d - %d of %d) Done.\n", path, offset_begin, offset_end, total_len);
	}

    // TODO do something

	return 0;
}
