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

int od_drive_handler_file_get_list(int fd, dx_packet_t* p);
int od_drive_handler_file_list(int fd, dx_packet_t* p);
int od_drive_handler_file_get(int fd, dx_packet_t* p);
int od_drive_handler_file_delete(int fd, dx_packet_t* p);

int od_drive_handler_file(int fd, dx_packet_t* packet) {
    printf("(Drive Event Handling) File(%d)\n", packet->header.code);

    switch(packet->header.code) {

    case OD_FILE_GET_LIST	:   /* 파일리스트 요청 */
    	od_drive_handler_file_get_list(fd, packet);
        break;
    case OD_FILE_LIST   	:   /* 파일리스트 정보 */
    	od_drive_handler_file_list(fd, packet);
        break;
    case OD_FILE_GET		:	/* 부분 파일 요청 */
    	od_drive_handler_file_get(fd, packet);
    	break;
    case OD_FILE			:	/* 부분 파일 정보 */
    	od_drive_handler_file(fd, packet);
    	break;
    case OD_FILE_DELETE		:	/* 파일 삭제 요청 */
    	od_drive_handler_file_delete(fd, packet);
    	break;

    default :
        break;

    }

    return 0;
}

int od_drive_handler_file_get_list(int fd, dx_packet_t* p) {
	int pathlen;
	char buf[129];
	dx_u8_array_packet_t* packet = (dx_u8_array_packet_t*)p;

	pathlen = ntohl(packet->array.len);
	bzero(buf, sizeof(buf));
	strncpy(buf, (char*)packet->array.data, pathlen > DX_PATH_MAX_SIZE ? DX_PATH_MAX_SIZE : pathlen);

    printf("(Drive Event Handling) FileList(path : %s)\n", buf);

	dx_packet_send_filelist(fd, buf);

	return 0;
}

int od_drive_handler_file_list(int fd, dx_packet_t* p) {

	return 0;
}

int od_drive_handler_file_get(int fd, dx_packet_t* p) {
	char buf[DX_PATH_MAX_SIZE + 1];
	dx_file_query_packet_t* packet = (dx_file_query_packet_t*)p;
	uint32_t begin, end;

	bzero(buf, sizeof(buf));
	strncpy(buf, (char*)packet->file.path, DX_PATH_MAX_SIZE);
	begin = ntohl(packet->file.offset_begin);
	end = ntohl(packet->file.offset_end);

    printf("(Drive Event Handling) File(path : %s [%d-%d])\n", buf, begin, end);

    dx_packet_send_file(fd, buf, begin, end);

	return 0;
}

int od_drive_handler_file_delete(int fd, dx_packet_t* p) {
	int pathlen;
	char buf[129];
	dx_u8_array_packet_t* packet = (dx_u8_array_packet_t*)p;

	pathlen = ntohl(packet->array.len);
	bzero(buf, sizeof(buf));
	strncpy(buf, (char*)packet->array.data, pathlen > DX_PATH_MAX_SIZE ? DX_PATH_MAX_SIZE : pathlen);

    printf("(Drive Event Handling) File Delete(path : %s)\n", buf);

    dx_packet_delete_file(fd, buf);

	return 0;
}
