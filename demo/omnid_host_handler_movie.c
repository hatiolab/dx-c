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

int od_handler_movie_info(int fd, dx_packet_t* packet);
int od_handler_movie_frame(int fd, dx_packet_t* packet);

int od_host_handler_movie(int fd, dx_packet_t* packet) {
    LOG("(Host Event Handling) Movie(%d)", packet->header.code);

    switch(packet->header.code) {

	case OD_MOVIE_INFO				: /* 영상파일 정보 */
		return od_handler_movie_info(fd, packet);

	case OD_MOVIE_FRAME				: /* 영상파일 스트림 */
		return od_handler_movie_frame(fd, packet);
    }

    return 0;
}

int od_handler_movie_info(int fd, dx_packet_t* packet) {
	dx_packet_movie_info_t* movie= (dx_packet_movie_info_t*)packet;
	int track_count = movie->data.track_count;
	int i;

	CONSOLE("===============================================\n");
	CONSOLE("Frmae / Sec : %d\n", ntohs(movie->data.frames_per_sec));
	CONSOLE("Resolution (Hight) : %d\n", ntohs(movie->data.height));
	CONSOLE("Resolution (Width) : %d\n", ntohs(movie->data.width));
	CONSOLE("Playtime : %d seconds\n", ntohs(movie->data.playtime));
	CONSOLE("Frame Count : %d\n", ntohl(movie->data.total_frame));
	CONSOLE("Fragment Count : %d\n", ntohl(movie->data.total_fragment));
	CONSOLE("Track Count : %d\n", movie->data.track_count);

	for(i = 0;i < track_count;i++) {
		CONSOLE("\tTrack #%d [%.4s] [%.4s]\n", i, movie->data.track_info[i].id, movie->data.track_info[i].type);
	}

	CONSOLE("===============================================\n");

	return 0;
}

int od_handler_movie_frame(int fd, dx_packet_t* packet) {
	dx_packet_movie_frame_t* frame = (dx_packet_movie_frame_t*)packet;
	int frame_no = ntohl(frame->data.frameno);
	int frame_length = ntohl(frame->data.frame_length);
	int track_count = frame->data.index_count;
	int i;

	LOG("Frame #%d : %d sized.", frame_no, frame_length);

	for(i = 0;i < track_count;i++) {
		LOG("\tTrack #%d [%.4s] : %d Sized %d Offset", i,
			frame->data.track_index[i].track_id,
			ntohl(frame->data.track_index[i].length),
			ntohl(frame->data.track_index[i].offset));
	}

//	frame->data.flags;
//	frame->data.frameno;
//	frame->data.frame_length;
//	frame->data.index_count;
//	frame->data.track_index[0];

	return 0;
}
