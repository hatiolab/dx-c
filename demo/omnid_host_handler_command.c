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

int od_host_handler_command(int fd, dx_packet_t* packet) {
    printf("(Host Event Handling) Command(%d)\n", packet->header.code);

    switch(packet->header.code) {

    case OD_CMD_RECORD_EMERGENCY_VIDEO       :   /* 긴급 녹화 명령 */
        break;
    case OD_CMD_CAPTURE_EMERGENCY_STILLCUT   :   /* 긴급 사진캡쳐 명령 */
        break;
    case OD_CMD_START_PLAYBACK :   /* 프레이백 스트림 시작 명령 */
    	break;
    case OD_CMD_STOP_PLAYBACK  :   /* 프레이백 스트림 종료 명령 */
    	break;
    default :
        break;

    }

    printf("(Host Event Handling) Command(%d).. Sent back.\n", packet->header.code);

    return 0;
}

