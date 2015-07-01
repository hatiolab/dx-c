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

int od_drive_handler_set_setting(int fd, dx_packet_t* packet) {
    printf("(Drive Event Handling) SetSetting(%d)\n", packet->header.code);

    switch(packet->header.code) {
    /* Omniview Settings */

//    case OD_SET_VIEW_DEFAULT            :  /* 디폴트 뷰 */
//		{
//	    	dx_primitive_packet_t* primitive_packet = (dx_primitive_packet_t*)packet;
//
//	    	uint32_t viewmode = ntohl(primitive_packet->data.u32);
//
//	    	printf("(Drive Event Handling) SetSetting(OD_SET_VIEW_DEFAULT, %d)\n", viewmode);
//
//	        od_drive_settings[OD_SET_VIEW_DEFAULT].data.u32 = viewmode;
//
//	    	dx_packet_send_set_setting_u32(fd, OD_SET_VIEW_DEFAULT, od_drive_settings[OD_SET_VIEW_DEFAULT].data.u32);
//		}
//        break;
//    case OD_SET_VIEW_ON_R_GEAR     :  /* 후진기어모드 상태시 뷰 */
//        break;
//    case OD_SET_VIEW_ON_TURN_RIGHT       :  /* 우측 방향지시등 상태시 뷰 */
//        break;
//    case OD_SET_VIEW_ON_TURN_LEFT        :  /* 좌측 방향지시등 상태시 뷰 */
//        break;
//    case OD_SET_VIEW_ON_EMERGENCY_SIGNAL :  /* 비상등 점멸시 뷰 */
//        break;
//    case OD_SET_VIEW_ON_PARKING          :  /* 주차 상황시 뷰 */
//        break;
//    case OD_SET_VIEW_ON_CROSSING         :  /* 교차로 주행 상황시 뷰 */
//        break;
//    case OD_SET_VIEW_ON_ALLEY            :  /* 좁은 골목길 주행 상황시 뷰 */
//        break;
//    case OD_SET_FRONT_VIEW_HOLDING_TIME  :  /* 전방 뷰 유지 시간 - 옴니뷰 OFF 상태에서 후진기어 입력 후 해제시 전방 뷰 화면 유지시간 */
//        break;
//    case OD_SET_SPEED_LIMIT_USE          :  /* 설정 속도 이상으로 주행시 화면 꺼짐 동작 여부 */
//        break;
//    case OD_SET_SPEED_LIMIT              :  /* 옴니뷰 제한 속도 */
//        break;
//    case OD_SET_VIEW_ON_DRIVING          :  /* 속도 제한 이상 주행시 뷰 */
//        break;
//    case OD_SET_PARKING_ASSIST_LINE      :  /* 파킹 가이드라인 표시 여부 */
//        break;
//
//    case OD_SET_ALARM_BSD                :  /* 후측방 사각지대 물체 인식시 경고 여부 */
//		{
//			dx_primitive_packet_t* primitive_packet = (dx_primitive_packet_t*)packet;
//			printf("(Drive Event Handling) SetSetting(OD_SET_ALARM_BSD, %d)\n", primitive_packet->data.u8);
//		}
//        break;
//    case OD_SET_ALARM_LDWS               :  /* 차선 이탈시 경고 여부 */
//        break;
//    case OD_SET_INFO_ON_DASHBOARD        :  /* 화면의 특정위치에 표시할 정보 (시계/주행속도) */
//        break;
//    case OD_SET_DUAL_MODE                :  /* 듀얼화면 사용 여부 */
//        break;
//    case OD_SET_APP_AUTO_START           :  /* 패드 부팅시 앱 자동 시작 */
//        break;
//    case OD_SET_SHOW_HELP                :  /* 도움말 화면 표시 여부 */
//        break;
//    case OD_SET_APP_VERSION              :  /* 어플리케이션 버전 - READ ONLY */
//        break;
//    case OD_SET_FW_VERSION               :  /* 펌웨어 버전 - READ ONLY */
//        break;
//    case OD_SET_MICOM_VERSION            :  /* 마이콤 버전 - READ ONLY */
//        break;
//
//    /* My Car Settings */
//
//    case OD_SET_CAR_FULL_WIDTH           :   /* 전폭 */
//        break;
//    case OD_SET_CAR_FULL_LENGTH          :   /* 전장 */
//        break;
//    case OD_SET_CAR_WHEEL_BASE           :   /* 축거 */
//        break;
//
//    /* Camera Settings */
//
//    case OD_SET_CAR_IMAGE_ID             :   /* TOP_VIEW에서 블라인드 영역에 사용되는 차량 TOP 이미지 */
//        break;
//    case OD_SET_CAR_IMAGE_WIDTH          :   /* 이미지가 디스플레이될 영역의 폭 */
//        break;
//    case OD_SET_CAR_IMAGE_HEIGHT         :   /* 이미지가 디스플레이될 영역의 높이 */
//        break;
//    case OD_SET_CAR_IMAGE_X              :   /* 이미지가 디스플레이될 영역의 X 포지션 */
//        break;
//    case OD_SET_CAR_IMAGE_Y              :   /* 이미지가 디스플레이될 영역의 Y 포지션 */
//        break;
//    case OD_SET_BLIND_MASK_WIDTH         :   /* 블라인드 영역의 폭 */
//        break;
//    case OD_SET_BLIND_MASK_HEIGHT        :   /* 블라인드 영역의 높이 */
//        break;
//    case OD_SET_BLIND_MASK_X             :   /* 블라인드 영역의 X 포지션 */
//        break;
//    case OD_SET_BLIND_MASK_Y             :   /* 블라인드 영역의 Y 포지션 */
//        break;
//    case OD_SET_VIDEO_PARAMS   :   /* 공차보정 후 생성되는 영상 변환 파라미터들 */
//        break;
//
//    /* Dash Cam Settings */
//
//	case OD_SET_RECORD_MOTION_LEVEL			:	/* 모션 센싱 레벨 (0-9) */
//		break;
//	case OD_SET_RECORD_POWER_CUT_VOLTAGE	:	/* 전원 차단 레벨 전압 */
//		break;
//	case OD_SET_RECORD_VOICE_ONOFF			:	/* 녹음 여부 */
//		break;
//	case OD_SET_RECORD_MIC_VOLUME			:	/* 마이크 볼륨 */
//		break;
//	case OD_SET_RECORD_SPEAKER_VOLUME		:	/* 스피커 볼륨 */
//		break;
//	case OD_SET_RECORD_MEANT_ONOFF			:	/* MEANT 설정 ? */
//		break;
//	case OD_SET_RECORD_SD_CARD_FORMAT		:	/* SD 카드 포맷 ? */
//		break;
//	case OD_SET_RECORD_SD_CARD_CHECK		:	/* SD 카드 체크 ? */
//		break;
//	case OD_SET_G_SENSIBILITY				:	/* G-Sensor 감도 */
//		break;
//	case OD_SET_RECORD_SELECT_CAMERA		:	/* 녹화 대상 카메라 */
//		break;
//	case OD_SET_RECORD_VIDEO_QUALITY		:	/* 녹화 영상 품질 */
//		break;
//	case OD_SET_RECORD_TIMEZONE				:	/* 타임존 설정 */
//		break;
//
//    /* Fleet Management Settings */
//
//    case OD_SET_FLEET_SERVER_URL         :   /* 관제 서버 연결 정보 */
//        break;
//    case OD_SET_FLEET_ID                 :   /* 관리 차량 고유 아이디 */
//        break;
//    case OD_SET_FLEET_SIGNATURE          :   /* 관제 서버에 연결하기위한 단말기 인증 정보 */
//        break;

    default :
		{
			dx_primitive_packet_t* primitive_packet = (dx_primitive_packet_t*)packet;

			uint8_t code = primitive_packet->header.code;
			uint32_t data = ntohl(primitive_packet->data.u32);

			printf("(Drive Event Handling) SetSetting(%d, %d)\n", code, data);

			od_drive_settings[code].data.u32 = data;

			dx_packet_send_set_setting_u32(fd, code, od_drive_settings[code].data.u32);
		}
    }

    printf("(Drive Event Handling) SetSetting(%d).. Sent back.\n", packet->header.code);

    return 0;
}

