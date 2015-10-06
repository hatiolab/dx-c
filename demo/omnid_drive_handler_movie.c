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

#include "fcntl.h"

#include "omnid.h"

#include "dx_net_packet_movie_playback.h"

int od_handler_movie_get_info(int fd, dx_packet_t* packet);
int od_handler_movie_command_start(int fd, dx_packet_t* packet);
int od_handler_movie_command_stop(int fd, dx_packet_t* packet);
int od_handler_movie_command_resume(int fd, dx_packet_t* packet);
int od_handler_movie_command_pause(int fd, dx_packet_t* packet);

int od_drive_handler_movie(int fd, dx_packet_t* packet) {
    CONSOLE("(Drive Event Handling) Command(%d)\n", packet->header.code);

    switch(packet->header.code) {

	case OD_MOVIE_GET_INFO				: /* 영상파일 정보 전송 요청 */
		return od_handler_movie_get_info(fd, packet);

	case OD_MOVIE_COMMAND_START			: /* 영상파일 전송 시작 */
		return od_handler_movie_command_start(fd, packet);

	case OD_MOVIE_COMMAND_STOP			: /* 영상파일 전송 종료 */
		return od_handler_movie_command_stop(fd, packet);

	case OD_MOVIE_COMMAND_RESUME		: /* 영상파일 전송 재시작 */
		return od_handler_movie_command_resume(fd, packet);

	case OD_MOVIE_COMMAND_PAUSE			: /* 영상파일 전송 멈춤 */
		return od_handler_movie_command_pause(fd, packet);
    }

    return 0;
}

dx_movie_context_t* demo_movie_playback_context = NULL;
dx_schedule_t* demo_movie_playback_stream_schedule = NULL;

int demo_movie_playback_schedule_callback(void* sender_fd) {
	dx_movie_context_t* context = demo_movie_playback_context;

	if(!dx_movie_frame_eof(context)) {
		if(-1 != dx_packet_send_movie_frame((int)sender_fd, context))
			return 0;
	}

	dx_movie_context_destroy(demo_movie_playback_context);
	demo_movie_playback_context = NULL;

	dx_schedule_cancel(demo_movie_playback_stream_schedule);
	dx_event_mplexer_wakeup();

	CONSOLE("Playback Stream End.\n");
	return -1;
}

int od_handler_movie_get_info(int fd, dx_packet_t* packet) {

	dx_packet_movie_get_info_t* p = (dx_packet_movie_get_info_t*)packet;

	demo_movie_playback_context = dx_movie_context_create((char*)p->data.path);
	if(demo_movie_playback_context == NULL) {
		ERROR("동영상 파일 오픈에 실패하였습니다.");
		return -1;
	}

	dx_packet_send_movie_info(fd, (char*)p->data.path, demo_movie_playback_context);

	return 0;
}

int od_handler_movie_command_start(int fd, dx_packet_t* packet) {

	dx_packet_movie_command_t* p= (dx_packet_movie_command_t*)packet;
	long interval;
	long frames_per_sec;

//	p->data.frames_per_sec;
//	p->data.path;
//	p->data.start_frame;
//	p->data.stop_frame;

	/* 스케쥴러를 만들어서 프레임을 던지기 시작한다. */
	/* 만약 현재 동작중인 스케쥴러가 있으면, 동작하지 않음 */
	if(demo_movie_playback_stream_schedule != NULL && demo_movie_playback_stream_schedule->next_schedule != 0) {
		ERROR("이미 동작중인 스트리밍 스케쥴러가 있습니다.");
		return -1;
	}

	if(demo_movie_playback_context != NULL) {
		close(demo_movie_playback_context->fd);
		CONSOLE("현재 열려있는 비디오 파일을 닫았습니다.\n");
		dx_movie_context_destroy(demo_movie_playback_context);
		demo_movie_playback_context = NULL;
	}

	demo_movie_playback_context = dx_movie_context_create((char*)p->data.path);

	if(demo_movie_playback_context == NULL) {
		ERROR("동영상 파일 오픈에 실패하였습니다.");
		return -1;
	}

	CONSOLE("\nFrame Rate : %d\n", demo_movie_playback_context->framerate);
	CONSOLE("Total Frames : %d\n", demo_movie_playback_context->total_frame);
	CONSOLE("Total Fragments : %d\n", demo_movie_playback_context->total_fragment);
	CONSOLE("Play Time : %d\n", demo_movie_playback_context->playtime);
	CONSOLE("Track Count : %d\n", demo_movie_playback_context->track_count);

	CONSOLE("Frame Offset : %ld\n", demo_movie_playback_context->frame_offset);
	CONSOLE("Index Offset : %ld\n", demo_movie_playback_context->index_offset);

	CONSOLE("Width : %d\n", demo_movie_playback_context->width);
	CONSOLE("Height : %d\n", demo_movie_playback_context->height);

	dx_movie_seek_frame(demo_movie_playback_context, ntohl(p->data.start_frame), SEEK_SET);

	/* 새로운 스트리밍 스케쥴러를 등록하고, 바로 시작합니다. */
	frames_per_sec = ntohs(p->data.frames_per_sec);
	if(frames_per_sec > 0)
		interval = 1000 / (frames_per_sec > 40 ? 40 : frames_per_sec);
	else
		interval = 1000 / 30;

	demo_movie_playback_stream_schedule = dx_schedule_register(0, interval, 1, demo_movie_playback_schedule_callback, (void*)fd);
	dx_event_mplexer_wakeup();

	return 0;
}

int od_handler_movie_command_stop(int fd, dx_packet_t* packet) {
	if(demo_movie_playback_stream_schedule != NULL && demo_movie_playback_stream_schedule->next_schedule != 0) {
		/* 스케쥴러를 취소합니다. */
		dx_schedule_cancel(demo_movie_playback_stream_schedule);
		demo_movie_playback_stream_schedule = NULL;
	}

	if(demo_movie_playback_context != NULL) {
		close(demo_movie_playback_context->fd);
		dx_movie_context_destroy(demo_movie_playback_context);
		demo_movie_playback_context = NULL;
	}

	return 0;
}

int od_handler_movie_command_resume(int fd, dx_packet_t* packet) {
//	dx_packet_movie_command_t* p= (dx_packet_movie_command_t*)packet;

//	p->data.frames_per_sec;
//	p->data.path;
//	p->data.start_frame;
//	p->data.stop_frame;

	/* 스케쥴러를 만들어서 프레임을 던지기 시작한다. */
	//	dx_packet_send_movie_frame(fd, demo_movie_context);

	return 0;
}

int od_handler_movie_command_pause(int fd, dx_packet_t* packet) {
	/* 스케쥴러를 잠시 멈춘다. */
	return 0;
}
