/*
 * button.c
 *
 *  Created on: May 21, 2024
 *      Author: mikol
 */


#include "button.h"

/**
 * TODO: is any pointer is NULL callback
 */

int button_flag;
uint8_t is_pressed = START_TIMER;
uint32_t button_time = 0;
uint32_t adc_value = 0;

void
HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	if(GPIO_Pin == B1_Pin)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		button_flag = BUTTON_DEBOUNCE;
		is_pressed = START_TIMER;
	}
}

void
HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM7)
	{
		button_time++;
		HAL_TIM_Base_Start_IT(htim);
	}
}


void
BUTTON_go_to_sleep(TIM_HandleTypeDef *htim, uint32_t *time)
{
	HAL_TIM_Base_Stop_IT(htim);
	time = 0;
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		/// TODO: reset all diodes
	button_flag = BUTTON_SLEEP;
}

void
BUTTON_Debounce_Callback(TIM_HandleTypeDef *htim, uint32_t *time)
{
	if(*time < DEBOUNCE_TIME)
	{
		if(is_pressed == START_TIMER)
		{
			is_pressed = STOP_TIMER;
			HAL_TIM_Base_Start_IT(htim);
		}
		return;
	}
	else
	{
		button_flag = BUTTON_PRESS;
		return;
	}
}

void
BUTTON_release(TIM_HandleTypeDef *htim, uint32_t *time)
{
	HAL_TIM_Base_Stop_IT(htim);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

	if(*time <= BAT_CHECK)
	{
		is_pressed = START_TIMER;
		button_flag = BUTTON_BAT_CHECK;
	}
	else if(*time > BAT_CHECK && *time <= WIFI_CONNECTION)
	{
		is_pressed = START_TIMER;
		button_flag = BUTTON_WIFI_CONNECTION;
	}
	else
	{
		is_pressed = START_TIMER;
		button_flag = BUTTON_LONG_PRESS;
	}

	*time = 0;
}


void
BUTTON_Is_Pressed(TIM_HandleTypeDef *htim, uint32_t *time)
{
	if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == PRESSED)
	{
		if(button_time % TOGGLE_PERIOD == 0)
		{
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}
		if(button_time > LONG_PRESS)
		{
			button_flag = BUTTON_GO_TO_SLEEP;
		}
		return;
	}
	else
	{
		BUTTON_release(htim, time);
	}
}


void
BUTTON_Battery_Check_Calback(TIM_HandleTypeDef *htim, uint32_t *time)
{
	if(*time >= BAT_CHECK_PERIOD)
	{
		button_flag = BUTTON_SLEEP;
	}
	if(is_pressed == START_TIMER)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		adc_value = HAL_ADC_GetValue(&hadc1);
		HAL_TIM_Base_Start_IT(htim);
		is_pressed = STOP_TIMER;
		HAL_ADC_Stop(&hadc1);
	}
	if(adc_value >= BAT_HIGH_ADC_VALUE){
		if(*time % BAT_HIGH_TOGGLE_PERIOD){
			HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
		}
	}
	else if(adc_value >= BAT_MID_ADC_VALUE){
		if(*time % BAT_MID_TOGGLE_PERIOD){
			HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		}
	}
	else{
		if(*time % BAT_LOW_TOGGLE_PERIOD){
			HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		}
	}
}

void
BUTTON_Wifi_Connection_Calback(TIM_HandleTypeDef *htim, uint32_t *time)
{
	/**
	 * TODO: Make all communication with esp32 via UART using AT commands
	 */
	button_flag = BUTTON_GO_TO_SLEEP;
}


void
BUTTON_Long_Press_Calback(TIM_HandleTypeDef *htim, uint32_t *time)
{
	if(*time >= BAT_CHECK_PERIOD)
	{
		button_flag = BUTTON_SLEEP;
	}
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void
BUTTON_Init(int *button_flag)
{
	/**
	 * TODO: init timer here
	 */
	*button_flag = BUTTON_SLEEP;
}

void
BUTTON_Main(int *button_flag)
{
	switch(*button_flag)
	{
		case BUTTON_SLEEP:
			// do nothing
			break;
		case BUTTON_DEBOUNCE:
			BUTTON_Debounce_Callback(&htim7, &button_time);
			break;
		case BUTTON_PRESS:
			BUTTON_Is_Pressed(&htim7, &button_time);
			break;
		case BUTTON_BAT_CHECK:
			BUTTON_Battery_Check_Calback(&htim7, &button_time);
			break;
		case BUTTON_WIFI_CONNECTION:
			BUTTON_Wifi_Connection_Calback(&htim7, &button_time);
			break;
		case BUTTON_LONG_PRESS:
			BUTTON_Long_Press_Calback(&htim7, &button_time);
			break;
		case BUTTON_GO_TO_SLEEP:
			BUTTON_go_to_sleep(&htim7, &button_time);
			break;
		default:
			/// TODO: error callback
			break;
	}
}
