/*
 * uart.c
 *
 *  Created on: Aug 12, 2024
 *      Author: Chaks
 */


#include "uart.h"
#include "string.h"

static myUART_HandleTypeDef uartHandles[UART_INSTANCES];
static int uartHandles_index = 0;

myUART_HandleTypeDef* UART_Init(UART_HandleTypeDef *huart) {
	myUART_HandleTypeDef *uart = &uartHandles[uartHandles_index++];
	uart->huart = huart;
	uart->txHead = 0;
	uart->txTail = 0;
	uart->txOngoing = 0;

    uart->rxIndex = 0;
    uart->RxCpltCallback = NULL;
    uart->StringReceivedCallback = NULL;

    return uart;
}

static void UART_TX_Call(myUART_HandleTypeDef *uart_handle){
	if(uart_handle->txTail < uart_handle->txHead){
		// Transmit the data from txTail to txHead (no wrap-around)
		HAL_UART_Transmit_IT(uart_handle->huart, &uart_handle->txBuffer[uart_handle->txTail], uart_handle->txHead - uart_handle->txTail);
		uart_handle->txTail = uart_handle->txHead;
	}
	else if(uart_handle->txTail > uart_handle->txHead){
		// Transmit the data from txTail to the end of the buffer (wrap-around case)
		HAL_UART_Transmit_IT(uart_handle->huart, &uart_handle->txBuffer[uart_handle->txTail], TX_BUFFER_SIZE - uart_handle->txTail);
		uart_handle->txTail = 0;
	}
	else{
		// No data to transmit, transmission complete
		uart_handle->txOngoing = 0;
	}
}

static myUART_HandleTypeDef* get_uart_handle(UART_HandleTypeDef *huart) {
    for (int i = 0; i < UART_INSTANCES; i++) {
        if (uartHandles[i].huart == huart) {
            return &uartHandles[i];
        }
    }
    return NULL; // Handle not found
}

void UART_TX_Send(myUART_HandleTypeDef *uart_handle, uint8_t *data, uint16_t size) {
	uint16_t spaceAvailable = (uart_handle->txTail + TX_BUFFER_SIZE - uart_handle->txHead - 1) % TX_BUFFER_SIZE;
	if (size > spaceAvailable) {
		// Buffer overflow
		Error_Handler();
	}

	// First part of the data that can be copied without wrapping
	uint16_t firstPart = TX_BUFFER_SIZE - uart_handle->txHead - 1;
	if (size < firstPart) {
		firstPart = size;
		memcpy(&uart_handle->txBuffer[uart_handle->txHead], data, firstPart);
		uart_handle->txHead = (uart_handle->txHead + firstPart) % TX_BUFFER_SIZE;
	}

	// Check if there's a second part that needs to be wrapped around
	if (firstPart < size) {
		uint16_t secondPart = size - firstPart;
		memcpy(uart_handle->txBuffer, &data[firstPart], secondPart);
		uart_handle->txHead = secondPart;
	}

    if (!uart_handle->txOngoing) {
        uart_handle->txOngoing = 1;
        UART_TX_Call(uart_handle);
    }
}

void UART_TX_SendString(myUART_HandleTypeDef *uart_handle, uint8_t *data){
	UART_TX_Send(uart_handle, data, (uint16_t)strlen(data));
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	myUART_HandleTypeDef *uart_handle = get_uart_handle(huart);
	if (uart_handle != NULL) {
		UART_TX_Call(uart_handle);
	}
}

void UART_RX_Start(myUART_HandleTypeDef *uart_handle) {
    HAL_UART_Receive_IT(uart_handle->huart, uart_handle->rxBuffer, 1);
}

void UART_RX_SetCallback(myUART_HandleTypeDef *uart_handle, void (*callback)(myUART_HandleTypeDef *uart_handle, uint8_t data)) {
    uart_handle->RxCpltCallback = callback;
}

void UART_RX_IRQHandler(UART_HandleTypeDef *huart) {
	myUART_HandleTypeDef *uart_handle = get_uart_handle(huart);
	HAL_UART_Receive_IT(uart_handle->huart, uart_handle->rxBuffer + ((uart_handle->rxIndex + 1)%RX_BUFFER_SIZE), 1);

	uint8_t received_data = uart_handle->rxIndex
	            ? uart_handle->rxBuffer[uart_handle->rxIndex]
	            : uart_handle->huart->Instance->RDR;

	// Execute the user-defined callback function, if set
	if (uart_handle->RxCpltCallback != NULL) {
		if(uart_handle->RxCpltCallback(uart_handle, received_data)){
			return;
		}
	}


	if (received_data != '\n' && received_data != '\r') {
		uart_handle->stringBuffer[uart_handle->stringIndex++] = received_data;

		if (uart_handle->stringIndex >= RX_MAX_STRING_SIZE) {
			//buffer overflow
		}
	}
	else {
		// Null-terminate the string
		uart_handle->stringBuffer[uart_handle->stringIndex] = '\0';

		// Execute the user-defined callback function, if set
		if (uart_handle->StringReceivedCallback != NULL) {
			uart_handle->StringReceivedCallback(uart_handle, (char *)uart_handle->stringBuffer);
		}

		// Reset the string index for the next string
		uart_handle->stringIndex = 0;
	}
	uart_handle->rxIndex++;
	if (uart_handle->rxIndex >= RX_BUFFER_SIZE) {
		uart_handle->rxIndex = 0;  // Wrap around
	}

}
