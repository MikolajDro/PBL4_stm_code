/*
 * uart.c
 *
 *  Created on: May 18, 2024
 *      Author: mikol
 */

#include "uart.h"


int	uart_flag = UART_SLEEP;
uint8_t ReceiveBuffer[BUFFER_SIZE];
uint8_t BufferCoursor = 0;

/**
 * TODO: IT callback
 */


void buffer_cleaner(uint8_t* buffer, uint16_t size)
{
	if(buffer == NULL)
		return;
	for(uint16_t i = 0; i < size; i++)
		buffer[i]=0;
	BufferCoursor = 0;

	uart_flag = UART_SLEEP;
}


/// HAL receive
void
HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		uart_flag = UART_NEW_MESSAGE;
	}

	HAL_UART_Receive_DMA(huart, ReceiveBuffer, BUFFER_SIZE);
}


void
AT_device_version_handler(UART_HandleTypeDef *huart)
{
	static const uint8_t at_device_version[] = "Device version 1.0v\n";
	HAL_UART_Transmit_IT(huart, at_device_version,
			sizeof(at_device_version));
}

void
AT_ok(UART_HandleTypeDef *huart)
{
	static const uint8_t ok[] = "Ok\n";
	HAL_UART_Transmit_IT(huart, ok, sizeof(ok));
}


int
UART_AT_command_handler(UART_HandleTypeDef *huart, uint8_t* buffer)
{
	// "AT+" commands
	if(buffer[0] == 'A' && buffer[1] == 'T' && buffer[2] == '+')
	{
		if(buffer[3] == 'D' && buffer[4] == 'V' && buffer[5] == '?')
		{
			AT_device_version_handler(huart);
			uart_flag = UART_AT_CALLBACK;
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		}



	}
	else
	{
		uart_flag = UART_AT_CALLBACK;
	}
//	uart_flag = UART_LISINING;
	return UART_AT_CALLBACK;
}


void UART_Init(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_DMA(huart, ReceiveBuffer, BUFFER_SIZE);
//	AT_ok(huart);
}


void UART_Main(UART_HandleTypeDef *huart, uint8_t* buffer)
{
	switch(uart_flag)
	{
		case UART_SLEEP:
			break;
		case UART_NEW_MESSAGE:
			UART_AT_command_handler(huart, buffer);
			break;
		case UART_AT_CALLBACK:
			buffer_cleaner(buffer, BUFFER_SIZE);
			break;
		case UART_OK:
			break;

		default:
			break;
	}


}

