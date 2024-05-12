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

uint8_t RTC_FLAG = RTC_SLEEP;		// creating a flag for rtc
struct RTC_AlarmSet_t *AlarmsArray;	// creating a alarms first element

/**
 *
 * @param hrtc			rtc handler
 * @param AlarmsArray	one-way list of alarms
 * @return 		successful status
 */
int RTC_Init(RTC_HandleTypeDef *hrtc, struct RTC_AlarmSet_t *AlarmsArray)
{
	if((AlarmsArray = (struct RTC_AlarmSet_t *) \
			malloc(sizeof(struct RTC_AlarmSet_t))) == NULL)
	{
		return MEMORY_ALLOCATION_ERROR;
	}
	// Only for testing purpose

	AlarmsArray->years		= 0;
	AlarmsArray->months		= 1;
	AlarmsArray->days 		= 1;
	AlarmsArray->hours		= 0;
	AlarmsArray->minutes	= 0;
	AlarmsArray->seconds	= 5;
	AlarmsArray->NextAlarm  = AlarmsArray;

	return RTC_OK;
}

/**
 *
 * @param str	table of chars
 * @param start start position
 * @param end 	end position
 * @return		successful status
 */
uint8_t strtoi(const uint8_t *str, uint16_t start, uint16_t end) {
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
struct RTC_AlarmSet_t RTC_ParseMessage_NewAlarm(const uint8_t *Message)
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

	//TODO auto-checking macro or function to prevent bad time values

	return parsedData;
}

/**
 *
 * @param AlarmsArray passing AlarmsArray to change
 * @param Message message from uart
 * @return
 */
int	RTC_Alarm_Update(struct RTC_AlarmSet_t *AlarmsArray,
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

	struct RTC_AlarmSet_t *CurrAlarm;
	CurrAlarm = AlarmsArray;
	struct RTC_AlarmSet_t *Alarm, parsedData;
	if((Alarm = (struct RTC_AlarmSet_t *) \
				malloc(sizeof(struct RTC_AlarmSet_t))) == NULL)
	{
		return MEMORY_ALLOCATION_ERROR;
	}
	parsedData = RTC_ParseMessage_NewAlarm(Message);

	if(parsedData.seconds == -1)
	{
		return MESSAGE_VALUE_ERROR;
	}
	Alarm->seconds 	= parsedData.seconds;
	Alarm->minutes 	= parsedData.minutes;
	Alarm->hours 	= parsedData.hours;
	Alarm->days 	= parsedData.days;
	Alarm->months 	= parsedData.months;
	Alarm->years 	= parsedData.years;
	Alarm->NextAlarm 	= NULL;

	while(CurrAlarm->NextAlarm != NULL)
	{
		if(Alarm->years > CurrAlarm->years){
			;
		}else{
			if(Alarm->months > CurrAlarm->months)
			{
				;
			}else{
				if(Alarm->days > CurrAlarm->days)
				{
					;
				}else{
					if(Alarm->hours > CurrAlarm->hours)
					{
						;
					}else{
						if(Alarm->hours > CurrAlarm->hours)
						{
							;
						}else{
							if(Alarm->minutes > CurrAlarm->minutes)
							{
								;
							}else{
								if(Alarm->seconds > CurrAlarm->seconds)
								{
									;
								}
								else{
									break;
								}
							}
						}
					}
				}
			}
		}
		CurrAlarm = CurrAlarm->NextAlarm;
	}
	if((AlarmsArray = (struct RTC_AlarmSet_t *) \
			malloc(sizeof(struct RTC_AlarmSet_t))) == NULL)
	{
		return MEMORY_ALLOCATION_ERROR;
	}

	Alarm->NextAlarm 		= CurrAlarm->NextAlarm;
	CurrAlarm->NextAlarm 	= Alarm;

	free(Alarm);

	return RTC_OK;
}

int RTC_Alarm_Next(struct RTC_AlarmSet_t *AlarmsArray)
{
	if(AlarmsArray->NextAlarm == NULL)
	{
		return NO_MORE_A_ALARMS;
	}
	struct RTC_AlarmSet_t *NextAlarm;
	NextAlarm = AlarmsArray->NextAlarm;
	free(AlarmsArray);
	AlarmsArray = NextAlarm;

	return RTC_OK;
}

int RTC_Alarm_Reset(RTC_HandleTypeDef *hrtc,
		struct RTC_AlarmSet_t *AlarmsArray, const uint8_t *Message)
{

	RTC_Alarm_Update(AlarmsArray, Message);
	if(RTC_Alarm_Next(AlarmsArray))
	{
		return NO_MORE_A_ALARMS;
	}
	RTC_AlarmTypeDef sAlarm = {0};
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

	RTC_FLAG = RTC_SLEEP;

	return RTC_OK;
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

	///TODO add debug printf

	RTC_FLAG = RTC_WATER;
}


/*
 * !Water pouring section
 */

//Values for HX711
uint16_t TargetWaterWeight = 100;
uint16_t Weight, OldWeight;
uint8_t  WeightTimeOut = 5;

/**
 *
 * @return		successful status
 */
int RTC_Water()
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
	Weight = HX711_Weight();
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
		StartPouringWater();
		RTC_FLAG = RTC_WATER;
		//Continue pouring water
		return RTC_WATER;
	}
	else
	{
		StopPouringWater();
		RTC_FLAG = RTC_NEW_ALARM;
		return RTC_NEW_ALARM;
	}
}
/**
 * @fn int RTC_Main(RTC_HandleTypeDef*, struct RTC_AlarmSet_t*, const uint8_t*)
 * @param hrtc
 * @param AlarmsArray
 * @param Message
 * @return
 */
int RTC_Main(RTC_HandleTypeDef *hrtc, struct RTC_AlarmSet_t *AlarmsArray,
		const uint8_t *Message)
{
	/*!
	 * TODO interpret of RTC_FLAG
	 */

	switch(RTC_FLAG)
	{
		case RTC_NEW_ALARM:
			RTC_Alarm_Reset(hrtc, AlarmsArray, Message);
			break;
		case RTC_WATER:
			RTC_Water();
			break;
		default:
			break;
	}



	return 1;
}
