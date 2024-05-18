/*
 * rtc_G.c
 *
 *  Created on: May 12, 2024
 *      Author: mikol
 */
#include <rtc_alarmA.h>


//defines
#define MEASURE_WEIGHT_DRIFT 	5 // 5 grams
#define WEIGHT_TIME_OUT			5 // 5 * measurement


/*
 * RTC Alarm section
 */

uint8_t RTC_FLAG = RTC_SLEEP;				/// flag for RTC state machine

// creating a alarms first element
//struct RTC_AlarmSet_t *AlarmsArray;			/// main pointer
static struct RTC_AlarmSet_t CurrentAlarm;	/// value for functions


/**
 *	Initialize RTC with necessary configuration
 *	+ set testing values
 *
 * @param hrtc			RTC handler
 * @param AlarmsArray	one-way list of alarms
 * @return 		successful status
 */
int 
RTC_Init(RTC_HandleTypeDef *hrtc, struct RTC_AlarmSet_t *AlarmsArray)
{
	//TODO: RTC init from cube mx


	// Only for testing purpose
	struct RTC_AlarmSet_t *NewAlarm;

	if((NewAlarm = (struct RTC_AlarmSet_t *) \
				malloc(sizeof(struct RTC_AlarmSet_t))) == NULL)
	{
		return MEMORY_ALLOCATION_ERROR;
	}
	NewAlarm->seconds 	= 5;
	NewAlarm->minutes 	= 0;
	NewAlarm->hours 	= 0;
	NewAlarm->days 		= 1;
	NewAlarm->months 	= 1;
	NewAlarm->years 	= 0;
	NewAlarm->NextAlarm = NULL;

	AlarmsArray = NewAlarm;

	return RTC_OK;
}


/**
 * @fn int RTC_is_alarmArray_limit(struct RTC_AlarmSet_t*)
 * @param AlarmsArray
 * @return size of AlarmArray
 */
int
RTC_is_alarmArray_limit(struct RTC_AlarmSet_t *AlarmsArray)
{
	uint16_t sizeCounter = 0;
	RTC_AlarmSet_t *currAlarm = AlarmsArray;
	
	do
	{
		currAlarm = currAlarm->NextAlarm;
		sizeCounter++;
	}
	while(currAlarm != NULL);
	
	if(sizeCounter == 0)
		return MISSING_ALARM;
	
	return sizeCounter;
}

/**
 *	Convert string to integer
 *
 * @param str	table of chars
 * @param start start position
 * @param end 	end position
 * @return		Integer from string
 */
int
strtoi(const uint8_t *str, uint16_t start, uint16_t end) {
    int result = 0;
    int sign = 1;
    int i = start;

    if (str[start] == '-')
    {
        sign = -1;
        i = start + 1;
    }

    for(;(str[i] != '\0' && i < end);i=i+1) {
        if (str[i] >= '0' && str[i] <= '9')
        {
            result = result * 10 + (str[i] - '0');
        }
        else
        	break;
    }

    return sign * result;
}


/**
 * @fn struct RTC_AlarmSet_t RTC_parseMessage_NewAlarm(const uint8_t*)
 * @param Message
 * @return
 */
struct RTC_AlarmSet_t 
RTC_parseMessage_NewAlarm(const uint8_t *Message)
{
	// "AT+NEWAL=SSMMHHDDMMYY" -> 20B
	// SS -> seconds 	Position 8, 9
	// MM -> minutes 	Position 10, 11
	// HH -> hours 		Position 12, 13
	// DD -> days 		Position 14
	// MM -> months 	Position 15, 16
	// YY -> years 		Position 17, 18
	struct RTC_AlarmSet_t parsedData;

	parsedData.seconds 		= strtoi(Message, 8, 9);
	if(parsedData.seconds < 0 || parsedData.seconds > 59){
		parsedData.seconds = -1;
		return parsedData;
	}
	parsedData.minutes 		= strtoi(Message, 11, 12);
	if(parsedData.minutes < 0 || parsedData.minutes > 59){
		parsedData.seconds = -1;
		return parsedData;
	}
	parsedData.hours 		= strtoi(Message, 13, 14);
	if(parsedData.hours < 0 || parsedData.hours > 59){
		parsedData.seconds = -1;
		return parsedData;
	}
	parsedData.days 		= strtoi(Message, 15, 15);
	if(parsedData.days < 0 || parsedData.days > 59){
		parsedData.seconds = -1;
		return parsedData;
	}
	parsedData.months 		= strtoi(Message, 16, 17);
	if(parsedData.months < 0 || parsedData.months > 59){
		parsedData.seconds = -1;
		return parsedData;
	}
	parsedData.years 		= strtoi(Message, 18, 19);
	if(parsedData.years < 0 || parsedData.years > 59){
		parsedData.seconds = -1;
		return parsedData;
	}
	parsedData.NextAlarm	= NULL;

	return parsedData;
}

/**
 * @fn int RTC_is_alarm_later(struct RTC_AlarmSet_t*, struct RTC_AlarmSet_t*)
 * @param NewAlarm
 * @param CurrAlarm
 * @return LATER if CurrAlarm is later than NewAlarm
 * 			EARLIER if CurrAlarm is earlier than NewAlarm
 */
int
RTC_is_alarm_later(struct RTC_AlarmSet_t *NewAlarm, struct RTC_AlarmSet_t *CurrAlarm)
{
	if(NewAlarm->years > CurrAlarm->years){
		;
	}else{
		if(NewAlarm->months > CurrAlarm->months)
		{
			;
		}else{
			if(NewAlarm->days > CurrAlarm->days)
			{
				;
			}else{
				if(NewAlarm->hours > CurrAlarm->hours)
				{
					;
				}else{
					if(NewAlarm->hours > CurrAlarm->hours)
					{
						;
					}else{
						if(NewAlarm->minutes > CurrAlarm->minutes)
						{
							;
						}else{
							if(NewAlarm->seconds > CurrAlarm->seconds)
							{
								;
							}
							else{
								return LATER;
							}
						}
					}
				}
			}
		}
	}
	
	return EARLIER;
}


/**
 * @fn int RTC_is_new_alarm_good(RTC_HandleTypeDef*, struct RTC_AlarmSet_t*)
 * @param hrtc
 * @param NewAlarm
 * @return RTC_OK if new alarm is later than time now
 */
int
RTC_is_new_alarm_good(RTC_HandleTypeDef *hrtc, struct RTC_AlarmSet_t *NewAlarm)
{
	RTC_TimeTypeDef sTime = {0};
	HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BIN);


	RTC_AlarmSet_t timeNow; //now
	timeNow.years = sDate.Year;
	timeNow.months = sDate.Month;
	timeNow.days = sDate.WeekDay;
	timeNow.hours = sTime.Hours;
	timeNow.minutes = sTime.Minutes;
	timeNow.seconds = sTime.Seconds + 1; // add 1 second to avoid problems with setting wrong

	if(RTC_is_alarm_later(&timeNow, NewAlarm) == LATER)
	{
		return RTC_OK;
	}
	return MESSAGE_VALUE_ERROR;
}

/**
 * Adds new alarm to AlarmArray
 * @param AlarmsArray passing AlarmsArray to change
 * @param Message message from uart
 * @return	RTC_NO_MESSAGE if message does not exist
 * 			RTC_ALARM_ARRAY_MISSING if AlarmArray pointer is missing
 * 			ALARM_LIMIT_ACCRUED if AlarmArray is missing
 * 			MESSAGE_VALUE_ERROR if message have bad values
 * 			MEMORY_ALLOCATION_ERROR if there is no more space to allocate
 * 			NEW_ALARM_ERROR if new alarm is earlier than time now
 * 			RTC_OK if success
 */
int	
RTC_add_new_alarm(struct RTC_AlarmSet_t *AlarmsArray,
		const uint8_t *Message)
{
	if(Message == NULL)
	{
		return RTC_NO_MESSAGE;
	}
	if(AlarmsArray == NULL)
	{
		return RTC_ALARM_ARRAY_MISSING;
	}
	if(RTC_is_alarmArray_limit(AlarmsArray) >= ARRAY_SIZE_LIMIT)
	{
		return ALARM_LIMIT_ACCRUED;
	}

	struct RTC_AlarmSet_t *CurrAlarm;
	CurrAlarm = AlarmsArray;
	struct RTC_AlarmSet_t *NewAlarm, parsedData;
	
	parsedData = RTC_parseMessage_NewAlarm(Message);
	
	if(parsedData.seconds == -1)
	{
		return MESSAGE_VALUE_ERROR;
	}
	if((NewAlarm = (struct RTC_AlarmSet_t *) \
				malloc(sizeof(struct RTC_AlarmSet_t))) == NULL)
	{
		return MEMORY_ALLOCATION_ERROR;
	}
	NewAlarm->seconds 	= parsedData.seconds;
	NewAlarm->minutes 	= parsedData.minutes;
	NewAlarm->hours 	= parsedData.hours;
	NewAlarm->days 		= parsedData.days;
	NewAlarm->months 	= parsedData.months;
	NewAlarm->years 	= parsedData.years;
	NewAlarm->NextAlarm = NULL;


	uint8_t isNewAlarmEarlier = 0;
	isNewAlarmEarlier = RTC_is_alarm_later(NewAlarm, CurrAlarm);
	if(RTC_is_new_alarm_good(&hrtc, NewAlarm) == EARLIER)
	{
		return NEW_ALARM_ERROR;
	}
	if(isNewAlarmEarlier == EARLIER)
	{
		RTC_FLAG = RTC_SET_ALARM;
		return RTC_OK;
	}
	while(CurrAlarm->NextAlarm != NULL)
	{
		if(RTC_is_alarm_later(NewAlarm, CurrAlarm) == EARLIER)
				break;

		CurrAlarm = CurrAlarm->NextAlarm;
	}


	NewAlarm->NextAlarm 	= CurrAlarm->NextAlarm;
	CurrAlarm->NextAlarm 	= NewAlarm;

	return RTC_OK;
}


/**
 * @fn int RTC_get_next_alarm(struct RTC_AlarmSet_t*)
 * @param AlarmsArray
 * @return 	NO_MORE_ALARMS if now more alarms is in AlarmArray
 * 			RTC_OK if success
 */
int 
RTC_get_next_alarm(struct RTC_AlarmSet_t *AlarmsArray)
{
	if(AlarmsArray == NULL)
	{
		return NO_MORE_ALARMS;
	}
	struct RTC_AlarmSet_t *NextAlarm;
	NextAlarm = AlarmsArray->NextAlarm;
	free(AlarmsArray);
	AlarmsArray = NextAlarm;

	return RTC_OK;
}


/**
 * @fn int RTC_delate_alarm(struct RTC_AlarmSet_t*)
 * @param AlarmsArray
 * @return
 */
int
RTC_delate_alarm(struct RTC_AlarmSet_t *AlarmsArray)
{
	struct RTC_AlarmSet_t *NextAlarm;

	if(AlarmsArray->NextAlarm == NULL)
		return NO_MORE_ALARMS;

	NextAlarm = AlarmsArray;
	AlarmsArray = AlarmsArray->NextAlarm;
	free(NextAlarm);

	return RTC_OK;
}

//TODO: make a function to delete specific alarm from array

/**
 * Set next alarm into Interrupt handler
 *
 * @fn int RTC_alarm_reset(RTC_HandleTypeDef*, struct RTC_AlarmSet_t*)
 * @param hrtc
 * @param AlarmsArray
 * @return	NO_MORE_ALARMS if no more alarms exist
 * 			ALARM_SET_IT_ERROR if HAL_RTC_SetAlarm_IT error occurred
 * 			MISSING_ALARM if AlarmArray is missing
 * 			RTC_OK if everything is good
 */
int 
RTC_alarm_reset(RTC_HandleTypeDef *hrtc,
		struct RTC_AlarmSet_t *AlarmsArray)
{
	if(RTC_get_next_alarm(AlarmsArray) == NO_MORE_ALARMS)
	{
		return NO_MORE_ALARMS;
	}
	CurrentAlarm = *AlarmsArray;
	static RTC_AlarmTypeDef sAlarm = {0};
	sAlarm.AlarmTime.Hours 		= AlarmsArray->hours;
	sAlarm.AlarmTime.Minutes 	= AlarmsArray->minutes;
	sAlarm.AlarmTime.Seconds 	= AlarmsArray->seconds;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmMask =
			RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS
			|RTC_ALARMMASK_MINUTES; // Testing purpose
	//! Leave only RTC_ALARMMASK_DATEWEEKDAY

	sAlarm.AlarmSubSecondMask 	= RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel 	= RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay 	= AlarmsArray->days;
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	{
		return ALARM_SET_IT_ERROR;
	}
	if (RTC_delate_alarm(AlarmsArray) != RTC_OK)
	{
		return MISSING_ALARM;
	}


	RTC_FLAG = RTC_SLEEP;

	return RTC_OK;
}

/// Change HAL callback
void
HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

	///TODO add debug printf

	RTC_FLAG = RTC_WATER;
}


/**
 * !Water pouring section
 */

/// Values for HX711
uint16_t TargetWaterWeight = 100; 	/// target weight can be changed
static uint16_t Weight, OldWeight;	/// Measuring purpose values
static uint8_t  WeightTimeOut = 5;	/// how many times can measured weight not change

/**
 *
 * @return		successful status
 */
int RTC_water()
{
	/**
	 * TODO: HX711_Weight()
	 * 		timer for measuring water weight every 0,4s
	 *
	 * TODO: StartPouringWater()
	 * 		Start pouring water
	 *
	 * TODO: StopPouringWater()
	 * 		Stop pouring water
	 */
	///Weight = HX711_Weight();
	if(Weight < TargetWaterWeight)
	{
		if(Weight <= OldWeight + MEASURE_WEIGHT_DRIFT)
		{
			WeightTimeOut = WeightTimeOut - 1;
		}else{
			WeightTimeOut = MEASURE_WEIGHT_DRIFT;
		}
		if(WeightTimeOut == 0)
		{
			RTC_FLAG = RTC_NO_WATER;
			return RTC_NO_WATER;
		}
		///StartPouringWater();
		RTC_FLAG = RTC_WATER;
		//Continue pouring water
		return RTC_WATER;
	}
	else
	{
		///StopPouringWater();
		RTC_FLAG = RTC_SET_ALARM;
		return RTC_SET_ALARM;
	}
}


/**
 * @fn int RTC_Main(RTC_HandleTypeDef*, struct RTC_AlarmSet_t*, const uint8_t*)
 * @param hrtc
 * @param AlarmsArray
 * @param Message
 * @return
 */
int
RTC_Main(RTC_HandleTypeDef *hrtc, struct RTC_AlarmSet_t *AlarmsArray,
		const uint8_t *Message)
{
	//TODO check battery level

	switch(RTC_FLAG)
	{
		case RTC_SET_ALARM:
			RTC_alarm_reset(hrtc, AlarmsArray);
			break;
		case RTC_WATER:
			RTC_water();
			break;
		case RTC_NEW_ALARM:
			RTC_add_new_alarm(AlarmsArray, Message);
			break;
		default:
			break;
	}



	return 1;
}
