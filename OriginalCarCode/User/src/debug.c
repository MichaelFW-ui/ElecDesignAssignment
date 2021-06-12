/**
 * @file debug.c
 * @author Michael Francis Williams (GitHub:Michael-ui)
 * @brief Debug function based on SPL
 * @version 0.1
 * @date 2021-06-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "debug.h"
#include "stdio.h"
#include "remote.h"
#include "motor.h"
#include "delay.h"
#include "steer.h"

void Debug_USART_Init() {
    // NVIC configuration
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                   = DEBUG_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    //GPIO configuration
    GPIO_InitTypeDef GPIO_InitStructure;

    DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    // USART configuration
    // 8N1 9600
    USART_InitTypeDef USART_InitStructure;

    DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

    USART_InitStructure.USART_BaudRate   = DEBUG_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(DEBUG_USARTx, &USART_InitStructure);


    //Command
    USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);
    USART_Cmd(DEBUG_USARTx, ENABLE);
}

void Debug_USART_SendByte(u8 byte) {
    USART_SendData(DEBUG_USARTx, byte);

    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET)
        ;
}


u8 Debug_USART_CommandBuffer[DEBUG_USART_BUFFER_SIZE] = {0};
u16 Debug_USART_BufferCur = 0;


void DEBUG_USART_IRQHandler(void) {
    u8 tmp;
    if (USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_RXNE);
        Debug_USART_BufferPush(tmp = USART_ReceiveData(DEBUG_USARTx));
        if (tmp == '\n') Debug_CommandHandler();
    }
    // https://blog.csdn.net/origin333/article/details/49992383
    // 检查 ORE 标志
    if (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_ORE) == SET) {
        USART_ClearFlag(DEBUG_USARTx, USART_FLAG_ORE);
        USART_ReceiveData(DEBUG_USARTx);
    }
    // before the ORE is cleared, never do printf.
    // You have to clear the ORE.
}

// Incomming commands handler, process until no more data coming in.
void Debug_CommandHandler() {
    /*
    **              TODO: Completion
    */
    u8 *ptrBuffer = Debug_USART_CommandBuffer;
    u16 cur       = Debug_USART_BufferCur;
    /*        Modify this for your commands                                   */
#define SHOW_CMD 0
    if (SHOW_CMD) {
        for (int i = 0; i < cur; ++i) {
            printf("0x%X ", *(ptrBuffer + i));
        }
    }
    printf("\r\n");
    if (ptrBuffer[0] == 'E') {
        u8 tx, rx;
        tx = NOP;
        REMOTE_SPI_CS_LOW();
        rx = Remote_SPI_SendByte(tx);
        REMOTE_SPI_CS_HIGH();
        printf("\r\nStatus : 0x%X", rx);
        REMOTE_DELAY();
    } else if (ptrBuffer[0] == 'M') {
        u8 data = ptrBuffer[1] - '0';
        printf("\r\nCommand:Set Motor at %d0", data * 10);
        Motor_PWM_SetDutyCycle(10 * data);
    } else if (ptrBuffer[0] == 'S') {
        u8 data = ptrBuffer[1] - '0';
        printf("\r\nCommand:Set Steer at %d0", data * 10);
        Steer_PWM_SetDutyCycle(10 * data);
    } else if (ptrBuffer[0] == 'W') {
        printf("\r\nCommand:Module On going, standing by.");
    }
    printf("\r\n");

    /*       End of modification                                              */

    Debug_USART_BufferClear();
}

int fputc(int ch, FILE *f) {
    Debug_USART_SendByte(ch);
    return ch;
}

int fgetc(FILE *f) {
    while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET)
        ;
    return (int)USART_ReceiveData(DEBUG_USARTx);
}

