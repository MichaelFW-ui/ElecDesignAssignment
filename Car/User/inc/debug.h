/**
 * @file debug.h
 * @author Michael Francis Williams (GitHub:Michael-ui)
 * @brief 
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __DEBUG_H
#define __DEBUG_H

#include "stm32f10x.h"

/*
**
**  USART:
**  8N1    9600
**  USART1
**  TX A9
**  RX A10
*/

/*          USART1 Definition        */

#define DEBUG_USARTx           USART1
#define DEBUG_USART_CLK        RCC_APB2Periph_USART1
#define DEBUG_USART_APBxClkCmd RCC_APB2PeriphClockCmd
#define DEBUG_USART_BAUDRATE   9600

// GPIO
#define DEBUG_USART_GPIO_CLK        RCC_APB2Periph_GPIOA
#define DEBUG_USART_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd
#define DEBUG_USART_TX_GPIO_PORT    GPIOA
#define DEBUG_USART_TX_GPIO_PIN     GPIO_Pin_9

#define DEBUG_USART_RX_GPIO_PORT    GPIOA
#define DEBUG_USART_RX_GPIO_PIN     GPIO_Pin_10
#define DEBUG_USART_IRQ             USART1_IRQn
#define DEBUG_USART_IRQHandler      USART1_IRQHandler


#define DEBUG_USART_BUFFER_SIZE         100

#define Debug_USART_BufferPush(x)                       \
    { Debug_USART_CommandBuffer[Debug_USART_BufferCur++] = x; } \
    while (0)

#define Debug_USART_BufferClear() \
    { Debug_USART_BufferCur = 0; } \
    while (0)

/*      END OF USART1 Definition           */

void Debug_USART_Init(void);

void Debug_USART_SendByte(u8 byte);

void Debug_CommandHandler(void);


#endif
