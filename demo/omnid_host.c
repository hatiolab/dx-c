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

od_host_value_t od_host_settings[] = {

    [OD_SET_VIEW_DEFAULT].data.u32 = 0,
    [OD_SET_VIEW_ON_R_GEAR].data.u32 = 0,
    [OD_SET_VIEW_ON_TURN_RIGHT].data.u32 = 0,
    [OD_SET_VIEW_ON_TURN_LEFT].data.u32 = 0,
    [OD_SET_VIEW_ON_EMERGENCY_SIGNAL].data.u32 = 0,
    [OD_SET_VIEW_ON_PARKING].data.u32 = 0,
    [OD_SET_VIEW_ON_CROSSING].data.u32 = 0,
    [OD_SET_VIEW_ON_ALLEY].data.u32 = 0,
    [OD_SET_FRONT_VIEW_HOLDING_TIME].data.u32 = 0,
    [OD_SET_SPEED_LIMIT_USE].data.u32 = 0,
    [OD_SET_SPEED_LIMIT].data.u32 = 0,
    [OD_SET_VIEW_ON_DRIVING].data.u32 = 0,
    [OD_SET_PARKING_ASSIST_LINE].data.u32 = 0,
    [OD_SET_ALARM_BSD].data.u32 = 0,
    [OD_SET_ALARM_LDWS].data.u32 = 0,
    [OD_SET_INFO_ON_DASHBOARD].data.u32 = 0,
    [OD_SET_DUAL_MODE].data.u32 = 0,
    [OD_SET_APP_AUTO_START].data.u32 = 0,
    [OD_SET_SHOW_HELP].data.u32 = 0,
    [OD_SET_APP_VERSION].data.u32 = 0,
    [OD_SET_FW_VERSION].data.u32 = 0,
    [OD_SET_MICOM_VERSION].data.u32 = 0,

    /* My Car Settings */

    [OD_SET_CAR_FULL_WIDTH].data.u32 = 0,
    [OD_SET_CAR_FULL_LENGTH].data.u32 = 0,
    [OD_SET_CAR_WHEEL_BASE].data.u32 = 0,

    /* Camera Settings */

    [OD_SET_CAR_IMAGE_ID].data.u32 = 0,
    [OD_SET_CAR_IMAGE_WIDTH].data.u32 = 0,
    [OD_SET_CAR_IMAGE_HEIGHT].data.u32 = 0,
    [OD_SET_CAR_IMAGE_X].data.u32 = 0,
    [OD_SET_CAR_IMAGE_Y].data.u32 = 0,
    [OD_SET_BLIND_MASK_WIDTH].data.u32 = 0,
    [OD_SET_BLIND_MASK_HEIGHT].data.u32 = 0,
    [OD_SET_BLIND_MASK_X].data.u32 = 0,
    [OD_SET_BLIND_MASK_Y].data.u32 = 0,
    [OD_SET_VIDEO_PARAMS].data.u32 = 0,

    [OD_SET_VIEW_TOP_FRONT_X].data.u32 = 0,
    [OD_SET_VIEW_TOP_FRONT_Y].data.u32 = 0,
    [OD_SET_VIEW_TOP_FRONT_WIDTH].data.u32 = 0,
    [OD_SET_VIEW_TOP_FRONT_HEIGHT].data.u32 = 0,
    [OD_SET_VIEW_TOP_FRONT_ZOOM].data.u32 = 0,
    [OD_SET_VIEW_TOP_FRONT_ROTATE].data.u32 = 0,

    [OD_SET_VIEW_TOP_REAR_X].data.u32 = 0,
    [OD_SET_VIEW_TOP_REAR_Y].data.u32 = 0,
    [OD_SET_VIEW_TOP_REAR_WIDTH].data.u32 = 0,
    [OD_SET_VIEW_TOP_REAR_HEIGHT].data.u32 = 0,
    [OD_SET_VIEW_TOP_REAR_ZOOM].data.u32 = 0,
    [OD_SET_VIEW_TOP_REAR_ROTATE].data.u32 = 0,

    [OD_SET_VIEW_TOP_LEFT_X].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_Y].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_WIDTH].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_HEIGHT].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_ZOOM].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_ROTATE].data.u32 = 0,

    [OD_SET_VIEW_TOP_RIGHT_X].data.u32 = 0,
    [OD_SET_VIEW_TOP_RIGHT_Y].data.u32 = 0,
    [OD_SET_VIEW_TOP_RIGHT_WIDTH].data.u32 = 0,
    [OD_SET_VIEW_TOP_RIGHT_HEIGHT].data.u32 = 0,
    [OD_SET_VIEW_TOP_RIGHT_ZOOM].data.u32 = 0,
    [OD_SET_VIEW_TOP_RIGHT_ROTATE].data.u32 = 0,

    [OD_SET_VIEW_TOP_LEFT_RIGHT_X].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_RIGHT_Y].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_RIGHT_WIDTH].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_RIGHT_HEIGHT].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_RIGHT_ZOOM].data.u32 = 0,
    [OD_SET_VIEW_TOP_LEFT_RIGHT_ROTATE].data.u32 = 0,

    [OD_SET_VIEW_TOP_FULL_X].data.u32 = 0,
    [OD_SET_VIEW_TOP_FULL_Y].data.u32 = 0,
    [OD_SET_VIEW_TOP_FULL_WIDTH].data.u32 = 0,
    [OD_SET_VIEW_TOP_FULL_HEIGHT].data.u32 = 0,
    [OD_SET_VIEW_TOP_FULL_ZOOM].data.u32 = 0,
    [OD_SET_VIEW_TOP_FULL_ROTATE].data.u32 = 0,

    [OD_SET_VIEW_FRONT_FULL_X].data.u32 = 0,
    [OD_SET_VIEW_FRONT_FULL_Y].data.u32 = 0,
    [OD_SET_VIEW_FRONT_FULL_WIDTH].data.u32 = 0,
    [OD_SET_VIEW_FRONT_FULL_HEIGHT].data.u32 = 0,
    [OD_SET_VIEW_FRONT_FULL_ZOOM].data.u32 = 0,
    [OD_SET_VIEW_FRONT_FULL_ROTATE].data.u32 = 0,

    [OD_SET_VIEW_REAR_FULL_X].data.u32 = 0,
    [OD_SET_VIEW_REAR_FULL_Y].data.u32 = 0,
    [OD_SET_VIEW_REAR_FULL_WIDTH].data.u32 = 0,
    [OD_SET_VIEW_REAR_FULL_HEIGHT].data.u32 = 0,
    [OD_SET_VIEW_REAR_FULL_ZOOM].data.u32 = 0,
    [OD_SET_VIEW_REAR_FULL_ROTATE].data.u32 = 0,

    /* Dash Cam Settings */

	[OD_SET_DASHCAM_BASE].data.u32 = 0,
	[OD_SET_RECORD_MOTION_LEVEL].data.u32 = 0,
	[OD_SET_RECORD_POWER_CUT_VOLTAGE].data.u32 = 0,
	[OD_SET_RECORD_VOICE_ONOFF].data.u32 = 0,
	[OD_SET_RECORD_MIC_VOLUME].data.u32 = 0,
	[OD_SET_RECORD_SPEAKER_VOLUME].data.u32 = 0,
	[OD_SET_RECORD_MEANT_ONOFF].data.u32 = 0,
	[OD_SET_RECORD_SD_CARD_FORMAT].data.u32 = 0,
	[OD_SET_RECORD_SD_CARD_CHECK].data.u32 = 0,
	[OD_SET_G_SENSIBILITY].data.u32 = 0,
	[OD_SET_RECORD_SELECT_CAMERA].data.u32 = 0,
	[OD_SET_RECORD_VIDEO_QUALITY].data.u32 = 0,
	[OD_SET_RECORD_TIMEZONE].data.u32 = 0,

    /* Fleet Management Settings */

    [OD_SET_FLEET_SERVER_URL].data.u32 = 0,
    [OD_SET_FLEET_ID].data.u32 = 0,
    [OD_SET_FLEET_SIGNATURE].data.u32 = 0,
};

od_host_value_t od_host_status[] = {
    [OD_STAT_VIEW_MODE].data.u32 = 0,
    [OD_STAT_VIDEO_PARAMS].data.u32 = 0,
    [OD_STAT_GPS].data.u32 = 0,
    [OD_STAT_RECORDING].data.u32 = 0,
    [OD_STAT_UNCONFIRMED_EVENT].data.u32 = 0,
    [OD_STAT_LDWS].data.u32 = 0,
    [OD_STAT_BSD].data.u32 = 0,
    [OD_STAT_STORAGE_USAGE].data.u32 = 0,
    [OD_STAT_SPEED].data.u32 = 0,
    [OD_STAT_REVERSE_GEAR].data.u32 = 0,
    [OD_STAT_TURN_SIGNAL].data.u32 = 0,
    [OD_STAT_EMERGENCY_SIGNAL].data.u32 = 0,
    [OD_STAT_EMERGENCY_RECORDING].data.u32 = 0,
    [OD_STAT_RECORDING_TIME].data.u32 = 0,
};
