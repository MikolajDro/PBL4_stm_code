/*
 * uart.h
 *
 *  Created on: May 18, 2024
 *      Author: mikol
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"

#define BUFFER_SIZE			20 /// Size of buffer 1-256
#define TRANSMIT_TIMEOUT	100

extern UART_HandleTypeDef huart1;


extern int	uart_flag;
extern uint8_t ReceiveBuffer[BUFFER_SIZE];

enum UART_STATE{
	UART_SLEEP 			= -1,
	UART_NEW_MESSAGE 	= 0,
	UART_AT_CALLBACK	= 1,
	UART_OK				= 2,
	UART_LISINING		= 3,


};

enum UART_AT_LOG{
	MISSING_AT				= 0,
	DEVICE_VERSION			= 1,

};


enum UART_ERROR_LOG{
	MISSING_AT_COMMAND		= -1,

};

void AT_ok(UART_HandleTypeDef *huart);
void UART_Init(UART_HandleTypeDef *huart);
void UART_Main(UART_HandleTypeDef *huart, uint8_t* buffer);

#endif /* INC_UART_H_ */
