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

#ifndef __OMNID_H
#define __OMNID_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "dx.h"

#include "dx_debug_malloc.h"
#include "dx_debug_assert.h"

#include "dx_util_schedule.h"
#include "dx_util_log.h"

#include "dx_file_avi.h"

#include "dx_console.h"

#include "dx_event_mplexer.h"

#include "dx_net_discovery.h"
#include "dx_net_server.h"
#include "dx_net_client.h"
#include "dx_net_packet.h"
#include "dx_net_packet_file.h"
#include "dx_net_packet_io.h"
#include "dx_net_packet_movie_playback.h"

#include "dx_camera.h"

/*
 * Definitions
 */
typedef struct od_drive_value {
	uint8_t				state;
	dx_primitive_data_t	data;
} od_drive_value_t;

extern od_drive_value_t od_drive_settings[];
extern od_drive_value_t od_drive_status[];

typedef struct od_host_value {
	uint8_t				state;
	dx_primitive_data_t	data;
} od_host_value_t;

extern od_host_value_t od_host_settings[];
extern od_host_value_t od_host_status[];

int od_drive_handler_hb(int fd, dx_packet_t* packet);
int od_drive_handler_get_setting(int fd, dx_packet_t* packet);
int od_drive_handler_set_setting(int fd, dx_packet_t* packet);
int od_drive_handler_get_state(int fd, dx_packet_t* packet);
int od_drive_handler_set_state(int fd, dx_packet_t* packet);
int od_drive_handler_event(int fd, dx_packet_t* packet);
int od_drive_handler_command(int fd, dx_packet_t* packet);
int od_drive_handler_file(int fd, dx_packet_t* packet);
int od_drive_handler_movie(int fd, dx_packet_t* packet);

int od_host_handler_hb(int fd, dx_packet_t* packet);
int od_host_handler_get_setting(int fd, dx_packet_t* packet);
int od_host_handler_set_setting(int fd, dx_packet_t* packet);
int od_host_handler_get_state(int fd, dx_packet_t* packet);
int od_host_handler_set_state(int fd, dx_packet_t* packet);
int od_host_handler_event(int fd, dx_packet_t* packet);
int od_host_handler_command(int fd, dx_packet_t* packet);
int od_host_handler_get_file_list(int fd, dx_packet_t* packet);
int od_host_handler_file_list(int fd, dx_packet_t* packet);
int od_host_handler_get_file(int fd, dx_packet_t* packet);
int od_host_handler_file(int fd, dx_packet_t* packet);
int od_host_handler_stream(int fd, dx_packet_t* packet);
int od_host_handler_movie(int fd, dx_packet_t* packet);

/* codes */
/* Streaming View Modes */

#define OD_VIEW_NONE                    0x00    /* 스트리밍을 오프시킴 */
#define OD_VIEW_TOP_FRONT               0x01    /* 탑뷰 + 전방 : 800 x 480 or 680 x 480 */
#define OD_VIEW_TOP_REAR                0x02    /* 탑뷰 + 후방 : 800 x 480 or 680 x 480 */
#define OD_VIEW_TOP_LEFT                0x03    /* 탑뷰 + 좌측 : 800 x 480 or 680 x 480 */
#define OD_VIEW_TOP_RIGHT               0x04    /* 탑뷰 + 우측 : 800 x 480 or 680 x 480 */
#define OD_VIEW_TOP_LEFT_RIGHT          0x05    /* 탑뷰 + 양측 : 800 x 480 or 680 x 480 */
#define OD_VIEW_FULL_TOP                0x06    /* 전면 탑뷰 : 800 x 480 or 680 x 480 */
#define OD_VIEW_FULL_FRONT              0x07    /* 전면 전방 : 800 x 480 or 680 x 480 */
#define OD_VIEW_FULL_REAR               0x08    /* 전면 후방 : 800 x 480 or 680 x 480 */
#define OD_VIEW_TOP                     0x09    /* 탑뷰 : 264 x 600 or 264 x 480 */
#define OD_VIEW_4CH                     0x0A    /* 4분면의 RAW 영상 : 1440 x 960 */
#define OD_VIEW_HD						0x0B	/* HD 카메라 영상 : 1280 X 720 */

/* Discovery Code */
#define OD_DISCOVERY_BASE				0x00
#define OD_DISCOVERY_REQ				(OD_DISCOVERY_BASE + 0)	/* Discovery 요청 */
#define OD_DISCOVERY_RESP				(OD_DISCOVERY_BASE + 1)	/* Discovery 응답 */

/* OmniView Settings */

#define OD_SET_OMNIVUE_BASE             0x00
#define OD_SET_VIEW_DEFAULT             (OD_SET_OMNIVUE_BASE + 0)   /* 디폴트 뷰 */
#define OD_SET_VIEW_ON_R_GEAR     (OD_SET_OMNIVUE_BASE + 1)   /* 후진기어모드 상태시 뷰 */
#define OD_SET_VIEW_ON_TURN_RIGHT       (OD_SET_OMNIVUE_BASE + 2)   /* 우측 방향지시등 상태시 뷰 */
#define OD_SET_VIEW_ON_TURN_LEFT        (OD_SET_OMNIVUE_BASE + 3)   /* 좌측 방향지시등 상태시 뷰 */
#define OD_SET_VIEW_ON_EMERGENCY_SIGNAL (OD_SET_OMNIVUE_BASE + 4)   /* 비상등 점멸시 뷰 */
#define OD_SET_VIEW_ON_PARKING          (OD_SET_OMNIVUE_BASE + 5)   /* 주차 상황시 뷰 */
#define OD_SET_VIEW_ON_CROSSING         (OD_SET_OMNIVUE_BASE + 6)   /* 교차로 주행 상황시 뷰 */
#define OD_SET_VIEW_ON_ALLEY            (OD_SET_OMNIVUE_BASE + 7)   /* 좁은 골목길 주행 상황시 뷰 */
#define OD_SET_FRONT_VIEW_HOLDING_TIME  (OD_SET_OMNIVUE_BASE + 8)   /* 전방 뷰 유지 시간 - 옴니뷰 OFF 상태에서 후진기어 입력 후 해제시 전방 뷰 화면 유지시간 */
#define OD_SET_SPEED_LIMIT_USE          (OD_SET_OMNIVUE_BASE + 9)   /* 설정 속도 이상으로 주행시 화면 꺼짐 동작 여부 */
#define OD_SET_SPEED_LIMIT              (OD_SET_OMNIVUE_BASE + 10)  /* 옴니뷰 제한 속도 */
#define OD_SET_VIEW_ON_DRIVING          (OD_SET_OMNIVUE_BASE + 11)  /* 속도 제한 이상 주행시 뷰 */
#define OD_SET_PARKING_ASSIST_LINE      (OD_SET_OMNIVUE_BASE + 12)  /* 파킹 가이드라인 표시 여부 */

#define OD_SET_ALARM_BSD                (OD_SET_OMNIVUE_BASE + 14)  /* 후측방 사각지대 물체 인식시 경고 여부 */
#define OD_SET_ALARM_LDWS               (OD_SET_OMNIVUE_BASE + 15)  /* 차선 이탈시 경고 여부 */
#define OD_SET_INFO_ON_DASHBOARD        (OD_SET_OMNIVUE_BASE + 16)  /* 화면의 특정위치에 표시할 정보 (시계/주행속도) */
#ifdef  OD_SET_INFO_ON_DASHBOARD
    #define OD_VAL_CLOCK    0x00
    #define OD_VAL_SPEED    0x01
#endif
#define OD_SET_DUAL_MODE                (OD_SET_OMNIVUE_BASE + 17)  /* 듀얼화면 사용 여부 */
#define OD_SET_APP_AUTO_START           (OD_SET_OMNIVUE_BASE + 18)  /* 패드 부팅시 앱 자동 시작 */
#define OD_SET_SHOW_HELP                (OD_SET_OMNIVUE_BASE + 19)  /* 도움말 화면 표시 여부 */

#define OD_SET_APP_VERSION              (OD_SET_OMNIVUE_BASE + 20)  /* 어플리케이션 버전 - READ ONLY */
#define OD_SET_FW_VERSION               (OD_SET_OMNIVUE_BASE + 21)  /* 펌웨어 버전 - READ ONLY */
#define OD_SET_MICOM_VERSION            (OD_SET_OMNIVUE_BASE + 22)  /* 마이콤 버전 - READ ONLY */

#define OD_SET_SPEED_INFO_FROM_ECU		(OD_SET_OMNIVUE_BASE + 23)  /* ECU의 속도정보를 받는지 여부. */

/* My Car Settings */

#define OD_SET_MYCAR_BASE               0x20
#define OD_SET_CAR_FULL_WIDTH           (OD_SET_MYCAR_BASE + 0)     /* 전폭 */
#define OD_SET_CAR_FULL_LENGTH          (OD_SET_MYCAR_BASE + 1)     /* 전장 */
#define OD_SET_CAR_WHEEL_BASE           (OD_SET_MYCAR_BASE + 2)     /* 축거 */

/* Camera Settings */

#define OD_SET_CAMERA_BASE              0x30
#define OD_SET_CAR_IMAGE_ID             (OD_SET_CAMERA_BASE + 0)    /* TOP_VIEW에서 블라인드 영역에 사용되는 차량 TOP 이미지 */
#define OD_SET_CAR_IMAGE_WIDTH          (OD_SET_CAMERA_BASE + 1)    /* 이미지가 디스플레이될 영역의 폭 */
#define OD_SET_CAR_IMAGE_HEIGHT         (OD_SET_CAMERA_BASE + 2)    /* 이미지가 디스플레이될 영역의 높이 */
#define OD_SET_CAR_IMAGE_X              (OD_SET_CAMERA_BASE + 3)    /* 이미지가 디스플레이될 영역의 X 포지션 */
#define OD_SET_CAR_IMAGE_Y              (OD_SET_CAMERA_BASE + 4)    /* 이미지가 디스플레이될 영역의 Y 포지션 */
#define OD_SET_BLIND_MASK_WIDTH         (OD_SET_CAMERA_BASE + 5)    /* 블라인드 영역의 폭 */
#define OD_SET_BLIND_MASK_HEIGHT        (OD_SET_CAMERA_BASE + 6)    /* 블라인드 영역의 높이 */
#define OD_SET_BLIND_MASK_X             (OD_SET_CAMERA_BASE + 7)    /* 블라인드 영역의 X 포지션 */
#define OD_SET_BLIND_MASK_Y             (OD_SET_CAMERA_BASE + 8)    /* 블라인드 영역의 Y 포지션 */
#define OD_SET_VIDEO_PARAMS             (OD_SET_CAMERA_BASE + 9)    /* 공차보정 후 생성되는 영상 변환 파라미터들 */

#define OD_SET_VIEW_TOP_FRONT_X         (OD_SET_CAMERA_BASE + 10)   /* TOP-FRONT VIEW의 OFFSET-X */
#define OD_SET_VIEW_TOP_FRONT_Y         (OD_SET_CAMERA_BASE + 11)   /* TOP-FRONT VIEW의 OFFSET-Y */
#define OD_SET_VIEW_TOP_FRONT_WIDTH     (OD_SET_CAMERA_BASE + 12)   /* TOP-FRONT VIEW의 WIDTH */
#define OD_SET_VIEW_TOP_FRONT_HEIGHT    (OD_SET_CAMERA_BASE + 13)   /* TOP-FRONT VIEW의 HEIGHT */
#define OD_SET_VIEW_TOP_FRONT_ZOOM      (OD_SET_CAMERA_BASE + 14)   /* TOP-FRONT VIEW의 ZOOM */
#define OD_SET_VIEW_TOP_FRONT_ROTATE    (OD_SET_CAMERA_BASE + 15)   /* TOP-FRONT VIEW의 ROTATE */

#define OD_SET_VIEW_TOP_REAR_X          (OD_SET_CAMERA_BASE + 16)   /* TOP-REAR VIEW의 OFFSET-X */
#define OD_SET_VIEW_TOP_REAR_Y          (OD_SET_CAMERA_BASE + 17)   /* TOP-REAR VIEW의 OFFSET-Y */
#define OD_SET_VIEW_TOP_REAR_WIDTH      (OD_SET_CAMERA_BASE + 18)   /* TOP-REAR VIEW의 WIDTH */
#define OD_SET_VIEW_TOP_REAR_HEIGHT     (OD_SET_CAMERA_BASE + 19)   /* TOP-REAR VIEW의 HEIGHT */
#define OD_SET_VIEW_TOP_REAR_ZOOM       (OD_SET_CAMERA_BASE + 20)   /* TOP-REAR VIEW의 ZOOM */
#define OD_SET_VIEW_TOP_REAR_ROTATE     (OD_SET_CAMERA_BASE + 21)   /* TOP-REAR VIEW의 ROTATE */

#define OD_SET_VIEW_TOP_LEFT_X          (OD_SET_CAMERA_BASE + 22)   /* TOP-LEFT VIEW의 OFFSET-X */
#define OD_SET_VIEW_TOP_LEFT_Y          (OD_SET_CAMERA_BASE + 23)   /* TOP-LEFT VIEW의 OFFSET-Y */
#define OD_SET_VIEW_TOP_LEFT_WIDTH      (OD_SET_CAMERA_BASE + 24)   /* TOP-LEFT VIEW의 WIDTH */
#define OD_SET_VIEW_TOP_LEFT_HEIGHT     (OD_SET_CAMERA_BASE + 25)   /* TOP-LEFT VIEW의 HEIGHT */
#define OD_SET_VIEW_TOP_LEFT_ZOOM       (OD_SET_CAMERA_BASE + 26)   /* TOP-LEFT VIEW의 ZOOM */
#define OD_SET_VIEW_TOP_LEFT_ROTATE     (OD_SET_CAMERA_BASE + 27)   /* TOP-LEFT VIEW의 ROTATE */

#define OD_SET_VIEW_TOP_RIGHT_X         (OD_SET_CAMERA_BASE + 28)   /* TOP-RIGHT VIEW의 OFFSET-X */
#define OD_SET_VIEW_TOP_RIGHT_Y         (OD_SET_CAMERA_BASE + 29)   /* TOP-RIGHT VIEW의 OFFSET-Y */
#define OD_SET_VIEW_TOP_RIGHT_WIDTH     (OD_SET_CAMERA_BASE + 30)   /* TOP-RIGHT VIEW의 WIDTH */
#define OD_SET_VIEW_TOP_RIGHT_HEIGHT    (OD_SET_CAMERA_BASE + 31)   /* TOP-RIGHT VIEW의 HEIGHT */
#define OD_SET_VIEW_TOP_RIGHT_ZOOM      (OD_SET_CAMERA_BASE + 32)   /* TOP-RIGHT VIEW의 ZOOM */
#define OD_SET_VIEW_TOP_RIGHT_ROTATE    (OD_SET_CAMERA_BASE + 33)   /* TOP-RIGHT VIEW의 ROTATE */

#define OD_SET_VIEW_TOP_LEFT_RIGHT_X         (OD_SET_CAMERA_BASE + 34)   /* TOP-LEFT-RIGHT VIEW의 OFFSET-X */
#define OD_SET_VIEW_TOP_LEFT_RIGHT_Y         (OD_SET_CAMERA_BASE + 35)   /* TOP-LEFT-RIGHT VIEW의 OFFSET-Y */
#define OD_SET_VIEW_TOP_LEFT_RIGHT_WIDTH     (OD_SET_CAMERA_BASE + 36)   /* TOP-LEFT-RIGHT VIEW의 WIDTH */
#define OD_SET_VIEW_TOP_LEFT_RIGHT_HEIGHT    (OD_SET_CAMERA_BASE + 37)   /* TOP-LEFT-RIGHT VIEW의 HEIGHT */
#define OD_SET_VIEW_TOP_LEFT_RIGHT_ZOOM      (OD_SET_CAMERA_BASE + 38)   /* TOP-LEFT-RIGHT VIEW의 ZOOM */
#define OD_SET_VIEW_TOP_LEFT_RIGHT_ROTATE    (OD_SET_CAMERA_BASE + 39)   /* TOP-LEFT-RIGHT VIEW의 ROTATE */

#define OD_SET_VIEW_TOP_FULL_X          (OD_SET_CAMERA_BASE + 40)   /* TOP-FULL VIEW의 OFFSET-X */
#define OD_SET_VIEW_TOP_FULL_Y          (OD_SET_CAMERA_BASE + 41)   /* TOP-FULL VIEW의 OFFSET-Y */
#define OD_SET_VIEW_TOP_FULL_WIDTH      (OD_SET_CAMERA_BASE + 42)   /* TOP-FULL VIEW의 WIDTH */
#define OD_SET_VIEW_TOP_FULL_HEIGHT     (OD_SET_CAMERA_BASE + 43)   /* TOP-FULL VIEW의 HEIGHT */
#define OD_SET_VIEW_TOP_FULL_ZOOM       (OD_SET_CAMERA_BASE + 44)   /* TOP-FULL VIEW의 ZOOM */
#define OD_SET_VIEW_TOP_FULL_ROTATE     (OD_SET_CAMERA_BASE + 45)   /* TOP-FULL VIEW의 ROTATE */

#define OD_SET_VIEW_FRONT_FULL_X        (OD_SET_CAMERA_BASE + 46)   /* FRONT-FULL VIEW의 OFFSET-X */
#define OD_SET_VIEW_FRONT_FULL_Y        (OD_SET_CAMERA_BASE + 47)   /* FRONT-FULL VIEW의 OFFSET-Y */
#define OD_SET_VIEW_FRONT_FULL_WIDTH    (OD_SET_CAMERA_BASE + 48)   /* FRONT-FULL VIEW의 WIDTH */
#define OD_SET_VIEW_FRONT_FULL_HEIGHT   (OD_SET_CAMERA_BASE + 49)   /* FRONT-FULL VIEW의 HEIGHT */
#define OD_SET_VIEW_FRONT_FULL_ZOOM     (OD_SET_CAMERA_BASE + 50)   /* FRONT-FULL VIEW의 ZOOM */
#define OD_SET_VIEW_FRONT_FULL_ROTATE   (OD_SET_CAMERA_BASE + 51)   /* FRONT-FULL VIEW의 ROTATE */

#define OD_SET_VIEW_REAR_FULL_X         (OD_SET_CAMERA_BASE + 52)   /* REAR-FULL VIEW의 OFFSET-X */
#define OD_SET_VIEW_REAR_FULL_Y         (OD_SET_CAMERA_BASE + 53)   /* REAR-FULL VIEW의 OFFSET-Y */
#define OD_SET_VIEW_REAR_FULL_WIDTH     (OD_SET_CAMERA_BASE + 54)   /* REAR-FULL VIEW의 WIDTH */
#define OD_SET_VIEW_REAR_FULL_HEIGHT    (OD_SET_CAMERA_BASE + 55)   /* REAR-FULL VIEW의 HEIGHT */
#define OD_SET_VIEW_REAR_FULL_ZOOM      (OD_SET_CAMERA_BASE + 56)   /* REAR-FULL VIEW의 ZOOM */
#define OD_SET_VIEW_REAR_FULL_ROTATE    (OD_SET_CAMERA_BASE + 57)   /* REAR-FULL VIEW의 ROTATE */

/* Dash Cam Settings */

#define OD_SET_DASHCAM_BASE            	0x80
#define	OD_SET_RECORD_MOTION_LEVEL		(OD_SET_DASHCAM_BASE)		/* 모션 센싱 레벨 (0-9) */
#define	OD_SET_RECORD_POWER_CUT_VOLTAGE	(OD_SET_DASHCAM_BASE + 1)	/* 전원 차단 레벨 전압 */
#define	OD_SET_RECORD_VOICE_ONOFF		(OD_SET_DASHCAM_BASE + 2)	/* 녹음 여부 */
#define	OD_SET_RECORD_MIC_VOLUME		(OD_SET_DASHCAM_BASE + 3)	/* 마이크 볼륨 */
#ifdef  OD_SET_RECORD_MIC_VOLUME
    #define OD_VAL_MIC_VOLUME_LOW		0x00
    #define OD_VAL_MIC_VOLUME_MID		0x01
    #define OD_VAL_MIC_VOLUME_HIGH		0x02
#endif
#define	OD_SET_RECORD_SPEAKER_VOLUME	(OD_SET_DASHCAM_BASE + 4)	/* 스피커 볼륨 */
#ifdef  OD_SET_RECORD_MIC_VOLUME
    #define OD_VAL_SPEAKER_VOLUME_LOW		0x00
    #define OD_VAL_SPEAKER_VOLUME_MID		0x01
    #define OD_VAL_SPEAKER_VOLUME_HIGH		0x02
#endif
#define	OD_SET_RECORD_MEANT_ONOFF		(OD_SET_DASHCAM_BASE + 5)	/* MEANT 설정 ? */
#define	OD_SET_RECORD_SD_CARD_FORMAT	(OD_SET_DASHCAM_BASE + 6)	/* SD 카드 포맷 ? */
#ifdef  OD_SET_RECORD_SD_CARD_FORMAT
    #define OD_VAL_SD_CARD_ACTION		0x00
    #define OD_VAL_SD_CARD_CANCEL		0x01
#endif
#define	OD_SET_RECORD_SD_CARD_CHECK		(OD_SET_DASHCAM_BASE + 7)	/* SD 카드 체크 ? */
#define OD_SET_G_SENSIBILITY            (OD_SET_DASHCAM_BASE + 8)	/* G-Sensor 감도 */
#ifdef  OD_SET_G_SENSIBILITY
    #define OD_VAL_SENSIBILITY_LOW      0x00
    #define OD_VAL_SENSIBILITY_NORMAL   0x01
    #define OD_VAL_SENSIBILITY_HIGH     0x02
#endif
#define OD_SET_RECORD_SELECT_CAMERA     (OD_SET_DASHCAM_BASE + 9)	/* 녹화 대상 카메라 */
#ifdef  OD_SET_RECORD_SELECT_CAMERA
    #define OD_VAL_CAMERA_HD			0x00
    #define OD_VAL_CAMERA_4VGA			0x01
    #define OD_VAL_CAMERA_AVM			0x02
#endif
#define OD_SET_RECORD_VIDEO_QUALITY     (OD_SET_DASHCAM_BASE + 10)	/* 녹화 영상 품질 */
#ifdef  OD_SET_RECORD_VIDEO_QUALITY
    #define OD_VAL_QUALITY_LOW			0x00
    #define OD_VAL_QUALITY_MID			0x01
    #define OD_VAL_QUALITY_HIGH			0x02
#endif
#define OD_SET_RECORD_TIMEZONE			(OD_SET_DASHCAM_BASE + 11)	/* 타임존 설정 */
#ifdef  OD_SET_RECORD_TIMEZONE
#define OD_VAL_UTC						0x00
#define	OD_VAL_TIMEZONE					0x01
#endif

/* Fleet Management Settings */

#define OD_SET_FLEET_MANAGEMENT_BASE    0x90
#define OD_SET_FLEET_SERVER_URL         (OD_SET_FLEET_MANAGEMENT_BASE + 0)  /* 관제 서버 연결 정보 */
#define OD_SET_FLEET_ID                 (OD_SET_FLEET_MANAGEMENT_BASE + 1)  /* 관리 차량 고유 아이디 */
#define OD_SET_FLEET_SIGNATURE          (OD_SET_FLEET_MANAGEMENT_BASE + 2)  /* 관제 서버에 연결하기위한 단말기 인증 정보 */

/* Volatile Status - 상태 */

#define OD_STAT_BASE                    0x00
#define OD_STAT_VIEW_MODE               (OD_STAT_BASE + 0)  /* 현재 뷰모드 설정 */
#define OD_STAT_VIDEO_PARAMS            (OD_STAT_BASE + 1)  /* 영상 변환 파라미터들 */
#define OD_STAT_GPS                     (OD_STAT_BASE + 2)  /* GPS 수신 상태 */
#define OD_STAT_RECORDING               (OD_STAT_BASE + 3)  /* 블랙박스 상시 녹화 상태 */
#define OD_STAT_UNCONFIRMED_EVENT       (OD_STAT_BASE + 4)  /* 미확인 충격 이벤트 등 알림 여부 */
#define OD_STAT_LDWS                    (OD_STAT_BASE + 5)  /* 차선 이탈 상태 */
#define OD_STAT_BSD                     (OD_STAT_BASE + 6)  /* 후측방 물체 감지 여부 */
#define OD_STAT_STORAGE_USAGE           (OD_STAT_BASE + 7)  /* 블랙박스용 스토리지 전체 사용량 */
#define OD_STAT_SPEED                   (OD_STAT_BASE + 8)  /* 현재 속도 */
#define OD_STAT_REVERSE_GEAR            (OD_STAT_BASE + 9)  /* 후진 기어 상태 */
#define OD_STAT_TURN_SIGNAL             (OD_STAT_BASE + 10) /* 방향 지시등 상태 */
#define OD_STAT_EMERGENCY_SIGNAL        (OD_STAT_BASE + 11) /* 비상등 상태 */
#define OD_STAT_EMERGENCY_RECORDING     (OD_STAT_BASE + 12) /* 블랙박스 비상 녹화 상태 */
#define OD_STAT_RECORDING_TIME          (OD_STAT_BASE + 13) /* 블랙박스 상시 녹화 경과 시간 */
#define OD_STAT_STORAGE_1_USAGE			(OD_STAT_BASE + 14) /* SD카드 1 사용량 */
#define OD_STAT_STORAGE_2_USAGE			(OD_STAT_BASE + 15) /* SD카드 2 사용량 */

/* Event Types */

#define OD_EVT_BASE                     0x00
#define OD_EVT_CONNECT                  (OD_EVT_BASE + 0)   /* 옴니드라이브가 연결되었을 때 발생 */
#define OD_EVT_DISCONNECT               (OD_EVT_BASE + 1)   /* 옴니드라이브 연결이 끊겼을 때 발생 */
#define OD_EVT_ERROR                    (OD_EVT_BASE + 2)   /* 오류 발생 이벤트 */
#define OD_EVT_ALARM                    (OD_EVT_BASE + 3)   /* 경고 발생 이벤트 */
#define OD_EVT_CHANGE_VIEW_MODE         (OD_EVT_BASE + 4)   /* 현재 뷰모드의 변화 */
#define OD_EVT_CHANGE_TURN_SIGNAL       (OD_EVT_BASE + 5)   /* 방향지시등 상태 변화 : LEFT, WRITE, NONE */
#define OD_EVT_CHANGE_R_GEAR            (OD_EVT_BASE + 6)   /* 후진 기어 상태 변화 : ON/OFF */
#define OD_EVT_CHANGE_EMERGENCY_SIGNAL  (OD_EVT_BASE + 7)   /* 비상등 상태 변화 : ON/OFF - 사용안함. (OD_EVT_CHANGE_TURN_SIGNAL로 커버함) */
#define OD_EVT_CHANGE_GPS_ONOFF         (OD_EVT_BASE + 8)   /* GPS 상태 변화 : ON/OFF */
#define OD_EVT_CHANGE_EMERGENCY_RECORD  (OD_EVT_BASE + 9)   /* 긴급녹화 상태 변화 : BEGIN / END */
#define OD_EVT_CHANGE_RECORD            (OD_EVT_BASE + 10)  /* 블랙박스 상시녹화 상태 변화 : BEGIN / END */
#define OD_EVT_CHANGE_STORAGE_USAGE     (OD_EVT_BASE + 11)  /* 자장장치 사용량 변화 */
#define OD_EVT_UPDATE_BEGIN             (OD_EVT_BASE + 12)  /* 옴니드라이브 프로그램 업데이트 시작 */
#define OD_EVT_UPDATE_PROGRESS          (OD_EVT_BASE + 13)  /* 옴니드라이브 프로그램 업데이트 진행율 변화 */
#define OD_EVT_UPDATE_END               (OD_EVT_BASE + 14)  /* 옴니드라이브 프로그램 업데이트 종료 */
#define OD_EVT_CHANGE_RECORDING_TIME    (OD_EVT_BASE + 15)  /* 녹화 진행 시간 변화 */
#define OD_EVT_RECORD_EVENT_FINISH		(OD_EVT_BASE + 16)	/* 이벤트 녹화 완료 */
#define OD_EVT_FORMAT_SDCARD_1_FINISH	(OD_EVT_BASE + 17)  /* SD카드 1 포맷 완료 */
#define OD_EVT_FORMAT_SDCARD_2_FINISH	(OD_EVT_BASE + 18)  /* SD카드 2 포맷 완료 */
#define OD_EVT_NEXT_VIEW_MODE			(OD_EVT_BASE + 19)  /* 다음 뷰모드로 전환 */
#define OD_EVT_APP_ON_OFF				(OD_EVT_BASE + 20)  /* APP시작/종료 */

/* Alarm Code */

#define OD_ALM_BASE                     0x00
#define OD_ALM_EVENT_UNCONFIRMED        (OD_ALM_BASE + 0)   /* 확인하지 않은 이벤트가 있습니다. */
#define OD_ALM_EVENT_G_SENSOR           (OD_ALM_BASE + 1)   /* 충격 이벤트가 발생했습니다. */
#define OD_ALM_EVENT_EMERGENCY_BUTTON   (OD_ALM_BASE + 2)   /* 비상 버튼이 실행되었습니다. */
#define OD_ALM_REAR_BLINDSPOT           (OD_ALM_BASE + 3)   /* 후방측면에 물체가 감지되었습니다. */
#define OD_ALM_LANE_DEPARTURE           (OD_ALM_BASE + 4)   /* 차선을 벗어났습니다. */
#define OD_ALM_FLASH_CARD_1_EMPTY       (OD_ALM_BASE + 5)   /* 플래쉬 카드 1번이 없습니다. */
#define OD_ALM_FLASH_CARD_2_EMPTY       (OD_ALM_BASE + 6)   /* 플래쉬 카드 2번이 없습니다. */

/* Error Code */

#define OD_ERR_BASE                     0x00
#define OD_ERR_UNKNOWN                  (OD_ERR_BASE + 0)   /* 비정상적인 시스템 오류 */
#define OD_ERR_SETTING_NOT_FOUND        (OD_ERR_BASE + 1)   /* 설정 파일을 찾을 수 없습니다. */
#define OD_ERR_FLASH_CARD_IO            (OD_ERR_BASE + 2)   /* 저장장치에 쓰거나 읽을 수가 없습니다. */
#define OD_ERR_CAMERA_IO                (OD_ERR_BASE + 3)   /* 카메라로부터 영상을 받을 수 없습니다. */

/* Commands */

#define OD_CMD_BASE                         0x00
#define OD_CMD_CONNECT                      (OD_CMD_BASE + 0)   /* 연결 시작 명령 */
#define OD_CMD_DISCONNECT                   (OD_CMD_BASE + 1)   /* 연결 종료 명령 */
#define OD_CMD_RECORD_EMERGENCY_VIDEO       (OD_CMD_BASE + 2)   /* 긴급 녹화 명령 */
#define OD_CMD_CAPTURE_EMERGENCY_STILLCUT   (OD_CMD_BASE + 3)   /* 긴급 사진캡쳐 명령 */
#define OD_CMD_CHANGE_VIEW_MODE             (OD_CMD_BASE + 4)   /* 뷰모드 변경 명령 */
#define OD_CMD_CAPTURE_STILLCUT             (OD_CMD_BASE + 5)   /* 현재 뷰모드 영상 스틸컷 캡쳐 명령 */
#define OD_CMD_START_UPGRADE                (OD_CMD_BASE + 6)   /* 소프트웨어 업그레이드 시작 명령 */
#define OD_CMD_START_PLAYBACK             	(OD_CMD_BASE + 7)   /* 프레이백 스트림 시작 명령 */
#define OD_CMD_STOP_PLAYBACK              	(OD_CMD_BASE + 8)   /* 프레이백 스트림 종료 명령 */
#define OD_CMD_START_STREAM             	(OD_CMD_BASE + 9)   /* 영상 스트림 시작 명령 */
#define OD_CMD_STOP_STREAM              	(OD_CMD_BASE + 10)  /* 영상 스트림 종료 명령 */
#define OD_CMD_START_LIVE_4CH             	(OD_CMD_BASE + 11)
#define OD_CMD_START_LIVE_HD             	(OD_CMD_BASE + 12)
#define OD_CMD_STOP_LIVE_4CH	            (OD_CMD_BASE + 13)
#define OD_CMD_STOP_LIVE_HD	            	(OD_CMD_BASE + 14)
#define OD_CMD_FORMAT_SDCARD_1				(OD_CMD_BASE + 15)  /* SD 카드 1 포맷 명령 */
#define OD_CMD_FORMAT_SDCARD_2				(OD_CMD_BASE + 16)  /* SD 카드 2 포맷 명령 */

/* File */
#define OD_FILE_BASE                    0x00
#define OD_FILE_GET_LIST                (OD_FILE_BASE + 0)
#define OD_FILE_LIST                    (OD_FILE_BASE + 1)
#define OD_FILE_GET                     (OD_FILE_BASE + 2)
#define OD_FILE                         (OD_FILE_BASE + 3)
#define OD_FILE_DELETE                  (OD_FILE_BASE + 4)
#define OD_FILE_GET_CALIBRATION_INFO	(OD_FILE_BASE + 10)		/* OmniVue Calibration 정보 파일 요청 */
#define OD_FILE_SET_CALIBRATION_INFO	(OD_FILE_BASE + 11)		/* OmniVue Calibration 정보 파일 업로드/다운로드 */

/* Stream */
#define OD_STREAM_BASE					0x00
#define OD_STREAM						(OD_STREAM_BASE + 0)
#define OD_STREAM_PLAYBACK				(OD_STREAM_BASE + 1)
#define OD_STREAM_LIVE_4CH				(OD_STREAM_BASE + 2)
#define OD_STREAM_LIVE_HD				(OD_STREAM_BASE + 3)

/* Movie Playback */
#define OD_MOVIE_BASE					0x00
#define OD_MOVIE_GET_INFO				(OD_MOVIE_BASE + 0)
#define OD_MOVIE_INFO					(OD_MOVIE_BASE + 1)
#define OD_MOVIE_COMMAND_START			(OD_MOVIE_BASE + 2)
#define OD_MOVIE_COMMAND_STOP			(OD_MOVIE_BASE + 3)
#define OD_MOVIE_COMMAND_RESUME			(OD_MOVIE_BASE + 4)
#define OD_MOVIE_COMMAND_PAUSE			(OD_MOVIE_BASE + 5)
#define OD_MOVIE_FRAME					(OD_MOVIE_BASE + 6)

#endif /* __OMNID_H */
