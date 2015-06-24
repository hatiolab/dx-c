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

int od_drive_handler_event(int fd, dx_packet_t* packet) {
    printf("(Drive Event Handling) Event(%d)\n", packet->header.code);

    switch(packet->header.code) {

    case OD_EVT_CONNECT                  :   /* 옴니드라이브가 연결되었을 때 발생 */
        break;
    case OD_EVT_DISCONNECT               :   /* 옴니드라이브 연결이 끊겼을 때 발생 */
        break;
    case OD_EVT_ERROR                    :   /* 오류 발생 이벤트 */
        break;
    case OD_EVT_ALARM                    :   /* 경고 발생 이벤트 */
        break;
    case OD_EVT_CHANGE_VIEW_MODE         :   /* 현재 뷰모드의 변화 */
        break;
    case OD_EVT_CHAGET_TURN_SIGNAL       :   /* 방향지시등 상태 변화 : LEFT, WRITE, NONE */
        break;
    case OD_EVT_CHANGE_R_GEAR      :   /* 후진 기어 상태 변화 : ON/OFF */
        break;
    case OD_EVT_CHANGE_EMERGENCY_SIGNAL  :   /* 비상등 상태 변화 : ON/OFF */
        break;
    case OD_EVT_CHANGE_GPS_ONOFF         :   /* GPS 상태 변화 : ON/OFF */
        break;
    case OD_EVT_CHANGE_EMERGENCY_RECORD  :   /* 긴급녹화 상태 변화 : BEGIN / END */
        break;
    case OD_EVT_CHANGE_RECORD            :   /* 블랙박스 상시녹화 상태 변화 : BEGIN / END */
        break;
    case OD_EVT_CHANGE_STORAGE_USAGE     :   /* 자장장치 사용량 변화 */
        break;
    case OD_EVT_UPDATE_BEGIN             :   /* 옴니드라이브 프로그램 업데이트 시작 */
        break;
    case OD_EVT_UPDATE_PROGRESS          :   /* 옴니드라이브 프로그램 업데이트 진행율 변화 */
        break;
    case OD_EVT_UPDATE_END               :   /* 옴니드라이브 프로그램 업데이트 종료 */
        break;
    case OD_EVT_CHANGE_RECORDING_TIME    :   /* 녹화 진행 시간 변화 */
        break;

    default :
        break;

    }

    return 0;
}
