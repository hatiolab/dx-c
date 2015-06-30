#include "demo.h"

int demo_server_event_handler(dx_event_context_t* context, dx_packet_t* packet) {

	switch(packet->header.type) {
	case DX_PACKET_TYPE_HB : /* Heart Beat */
		od_drive_handler_hb(context->fd, packet);
		break;
	case DX_PACKET_TYPE_GET_SETTING	: /* Get Setting */
		od_drive_handler_get_setting(context->fd, packet);
		break;
	case DX_PACKET_TYPE_SET_SETTING : /* Set Setting */
		od_drive_handler_set_setting(context->fd, packet);
		break;
	case DX_PACKET_TYPE_GET_STATE : /* Get State */
		od_drive_handler_get_state(context->fd, packet);
		break;
	case DX_PACKET_TYPE_SET_STATE : /* Set State */
		od_drive_handler_set_state(context->fd, packet);
		break;
	case DX_PACKET_TYPE_EVENT : /* Event */
		od_drive_handler_event(context->fd, packet);
		break;
	case DX_PACKET_TYPE_COMMAND : /* Command */
		od_drive_handler_command(context->fd, packet);
		break;
	case DX_PACKET_TYPE_FILE 	: /* File */
		od_drive_handler_file(context->fd, packet);
		break;
	case DX_PACKET_TYPE_STREAM 	: /* Stream */
		printf("Why here.\n");
//		od_drive_handler_stream(context->fd, packet);
		break;
	case DX_PACKET_TYPE_MOVIE 	: /* Movie Playback */
		printf("Why here.\n");
		break;
	default:	/* Should not reach to here */
		ASSERT("Drive Event Handling.. should not reach to here.", !!0);
		break;
	}

	return 0;
}
