/*
 * uart.h
 *
 *  Created on: Aug 12, 2024
 *      Author: Chaks
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32f7xx_hal.h"

#define TX_BUFFER_SIZE 1000
#define RX_BUFFER_SIZE 64
#define UART_INSTANCES 2

#define RX_MAX_STRING_SIZE 1000

typedef struct {
    UART_HandleTypeDef *huart;
    uint8_t txBuffer[TX_BUFFER_SIZE];
    uint16_t txHead;
    uint16_t txTail;
    uint8_t txOngoing;

    uint8_t rxBuffer[RX_BUFFER_SIZE];
    uint8_t stringBuffer[RX_MAX_STRING_SIZE];
	volatile uint16_t rxIndex;
	volatile uint16_t stringIndex;
	int (*RxCpltCallback)(struct myUART_HandleTypeDef *uart_handle, uint8_t data);
	void (*StringReceivedCallback)(struct myUART_HandleTypeDef *uart_handle, char *str);
} myUART_HandleTypeDef;

myUART_HandleTypeDef* UART_Init(UART_HandleTypeDef *huart);

void UART_TX_Send(myUART_HandleTypeDef *uart_handle, uint8_t *data, uint16_t size);
void UART_TX_SendString(myUART_HandleTypeDef *uart_handle, uint8_t *data);

void UART_RX_Start(myUART_HandleTypeDef *uart_handle);
void UART_RX_SetCallback(myUART_HandleTypeDef *uart_handle, void (*callback)(myUART_HandleTypeDef *uart_handle, uint8_t data));
void UART_RX_SetStringCallback(myUART_HandleTypeDef *uart_handle, void (*callback)(char *str));
void UART_RX_IRQHandler(UART_HandleTypeDef *huart);

#endif /* INC_UART_H_ */
