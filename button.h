/*
 * button.h
 *
 *  Created on: May 21, 2024
 *      Author: mikol
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "adc.h"




#define DEBOUNCE_TIME 	50 // 50ms for unwanted input noise
#define START_TIMER		1
#define STOP_TIMER		0
#define TOGGLE_PERIOD	200	// period of time to toggle led while button is pressed
#define PRESSED			GPIO_PIN_RESET
#define RELEASED		GPIO_PIN_SET
#define IDLE			RELEASED
#define BAT_CHECK		1000			// press max 1s to check battery state
#define WIFI_CONNECTION	5000			// press between 1 - 5 seconds
										// to set wifi connection
#define LONG_PRESS		WIFI_CONNECTION	// time after program indicate pressing
										// button is a long press and have to
										// ignore it

/// Test only values (ADC return 0-4095 12-bit resolution)
#define BAT_LOW_ADC_VALUE		0		// Bat is low
#define BAT_MID_ADC_VALUE		2400	// Bat is in good state
#define BAT_HIGH_ADC_VALUE		3800	// Bat is high
#define BAT_CHECK_PERIOD		5000	// show battery level for 5s
#define BAT_LOW_TOGGLE_PERIOD	100		// period of toggling led if low level
#define BAT_MID_TOGGLE_PERIOD	500		// period of toggling led if medium lvl
#define BAT_HIGH_TOGGLE_PERIOD	500		// period of toggling led if high level
#define LONG_PRESS_PERIOD		2000	// Flash with white light



enum BUTTON_STATE{
	BUTTON_SLEEP 			= -1,
	BUTTON_DEBOUNCE 		= 0,
	BUTTON_PRESS 			= 1,
	BUTTON_BAT_CHECK 		= 2,
	BUTTON_WIFI_CONNECTION 	= 3,
	BUTTON_LONG_PRESS 		= 4,
	BUTTON_GO_TO_SLEEP		= 5,
};


// values defines
extern int button_flag;
extern uint32_t button_time;

// HAL function defines
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

// Functions defines
void BUTTON_Init(int *button_flag);
void BUTTON_Main(int *button_flag);


#endif /* INC_BUTTON_H_ */
