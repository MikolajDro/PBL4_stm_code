/*
 * rtc_G.h
 *
 *  Created on: May 12, 2024
 *      Author: mikol
 */

#ifndef INC_RTC_ALARMA_H_
#define INC_RTC_ALARMA_H_

#include "main.h"
#include <stdlib.h>


//defines
#define MEASURE_WEIGHT_DRIFT 	5 // 5 grams
#define WEIGHT_TIME_OUT			5 // 5 * measurement


typedef struct RTC_AlarmSet_t {
	uint8_t seconds	:6;		// 0-59
	uint8_t minutes	:6;		// 0-59
	uint8_t	hours	:5;		// 0-23
	uint8_t days	:5;		// 0-7  (Monday = 1)
	uint8_t months	:4;		// 0-12 (January = 1)
	uint8_t years	:6;		// 0-99

	struct RTC_AlarmSet_t *NextAlarm;
}RTC_AlarmSet_t;

enum RTC_FLAG_TypeDef{
	RTC_SLEEP		= -1,
	RTC_NEW_ALARM	= 0,
	RTC_WATER		= 1,
	RTC_NO_WATER	= 2,
};

enum RTC_LOG{
	RTC_OK					= 0,
	MEMORY_ALLOCATION_ERROR = 1,
	NO_MORE_A_ALARMS		= 2,
	ALARM_SET_IT_ERROR		= 3,
	WATER_GOOD				= 4,
	WATER_NO_NEED			= 5,
	RTC_NO_MESSAGE			= 6,
	MESSAGE_VALUE_ERROR		= 7,
	RTC_ALARM_ARRAY_MISSING	= 8,


};

extern RTC_HandleTypeDef hrtc;
extern uint16_t TargetWaterWeight;
extern uint8_t RTC_FLAG;
extern struct RTC_AlarmSet_t *AlarmsArray;

/**
 * @fn int RTC_Init(RTC_HandleTypeDef*, struct RTC_AlarmSet_t*)
 * @param hrtc
 * @param AlarmsArray
 * @return RTC_LOG status
 */
int RTC_Init(RTC_HandleTypeDef *hrtc, struct RTC_AlarmSet_t *AlarmsArray);

/**
 * @fn void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef*)
 * @param hrtc rtc handler
 *
 * RTC Alarm Callback
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);


#endif /* INC_RTC_ALARMA_H_ */
