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

int od_host_handler_get_state(int fd, dx_packet_t* packet) {
    printf("(Host Event Handling) GetState(%d)\n", packet->header.code);

//    switch(packet->header.code) {
//
//    case OD_STAT_VIEW_MODE               :  /* 현재 뷰모드 설정 */
//        break;
//    case OD_STAT_VIDEO_PARAMS  :  /* 영상 변환 파라미터들 */
//        break;
//    case OD_STAT_GPS                     :  /* GPS 수신 상태 */
//        break;
//    case OD_STAT_RECORDING               :  /* 블랙박스 상시 녹화 상태 */
//        break;
//    case OD_STAT_UNCONFIRMED_EVENT       :  /* 미확인 충격 이벤트 등 알림 여부 */
//        break;
//    case OD_STAT_LDWS                    :  /* 차선 이탈 상태 */
//        break;
//    case OD_STAT_BSD                     :  /* 후측방 물체 감지 여부 */
//        break;
//    case OD_STAT_STORAGE_USAGE           :  /* 블랙박스용 스토리지 전체 사용량 */
//        break;
//    case OD_STAT_SPEED                   :  /* 현재 속도 */
//        break;
//    case OD_STAT_REVERSE_GEAR            :  /* 후진 기어 상태 */
//        break;
//    case OD_STAT_TURN_SIGNAL             :  /* 방향 지시등 상태 */
//        break;
//    case OD_STAT_EMERGENCY_SIGNAL        :  /* 비상등 상태 */
//        break;
//    case OD_STAT_EMERGENCY_RECORDING     :  /* 블랙박스 비상 녹화 상태 */
//        break;
//    case OD_STAT_RECORDING_TIME          :  /* 블랙박스 상시 녹화 경과 시간 */
//        break;
//    default :
//        break;
//
//    }

    return 0;
}

