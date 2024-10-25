# STM32 UART Library

This library provides an interface for UART communication on STM32 microcontrollers. It supports both transmitting and receiving data asynchronously, utilizing interrupts for efficient data handling.

## Features

- **Asynchronous Transmission:** Transmit data without blocking the CPU.
- **Receive Data with Callbacks:** Handle incoming data via user-defined callback functions.
- **Circular Buffers:** Manage transmission and reception using circular buffers to avoid overflow.
- **String Handling:** Send and receive strings easily with built-in functions.

## Table of Contents

- [Getting Started](#getting-started)
- [Installation](#installation)
- [API Reference](#api-reference)
- [Examples](#examples)
- [Error Handling](#error-handling)
- [License](#license)

## Getting Started

### Prerequisites

- STM32 development board
- STM32 HAL library
- A suitable IDE (e.g. STM32CubeIDE)

### Installation

1. Clone the repository or download the files for the UART library.
2. Include the `uart.c` and `uart.h` files in your project.
3. Ensure that you have included the STM32 HAL library in your project.

### Include the Library

In your source files, include the UART header:

```c
#include "uart.h"
```

## API Reference

### `UART_HandleTypeDef* UART_Init(UART_HandleTypeDef *huart)`

Initializes a UART instance. Returns a pointer to a `myUART_HandleTypeDef` structure for further operations.

- **Parameters:**
  - `huart`: Pointer to a `UART_HandleTypeDef` structure that contains the configuration information for the specified UART module.

### `void UART_TX_Send(myUART_HandleTypeDef *uart_handle, uint8_t *data, uint16_t size)`

Sends a buffer of data using the specified UART handle.

- **Parameters:**
  - `uart_handle`: Pointer to the `myUART_HandleTypeDef` structure.
  - `data`: Pointer to the data buffer to be sent.
  - `size`: Size of the data buffer.

### `void UART_TX_SendString(myUART_HandleTypeDef *uart_handle, uint8_t *data)`

Sends a null-terminated string using the specified UART handle.

- **Parameters:**
  - `uart_handle`: Pointer to the `myUART_HandleTypeDef` structure.
  - `data`: Pointer to the null-terminated string to be sent.

### `void UART_RX_Start(myUART_HandleTypeDef *uart_handle)`

Starts receiving data in interrupt mode.

- **Parameters:**
  - `uart_handle`: Pointer to the `myUART_HandleTypeDef` structure.

### `void UART_RX_SetCallback(myUART_HandleTypeDef *uart_handle, void (*callback)(myUART_HandleTypeDef *uart_handle, uint8_t data))`

Sets a callback function for received data. The callback function must return 0 to accept current character.

- **Parameters:**
  - `uart_handle`: Pointer to the `myUART_HandleTypeDef` structure.
  - `callback`: Pointer to the function to be called when data is received.

### `void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)`

Callback function called by the HAL library when the transmission is complete.

- **Parameters:**
  - `huart`: Pointer to a `UART_HandleTypeDef` structure that contains the configuration information for the specified UART module.

### `void UART_RX_IRQHandler(UART_HandleTypeDef *huart)`

Interrupt handler for received data. This function should be called in the HAL UART interrupt handler.

- **Parameters:**
  - `huart`: Pointer to a `UART_HandleTypeDef` structure that contains the configuration information for the specified UART module.

## Examples

### Initializing UART

```c
UART_HandleTypeDef huart1;
myUART_HandleTypeDef *myUart1 = UART_Init(&huart1);
```

### Sending Data

```c
uint8_t message[] = "Hello, UART!";
UART_TX_SendString(myUart1, message);
```

### Receiving Data with a Callback

```c
void myRxCallback(myUART_HandleTypeDef *uart_handle, uint8_t data) {
    // Handle the received data
}

UART_RX_SetCallback(myUart1, myRxCallback);
UART_RX_Start(myUart1);
```
