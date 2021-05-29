#include "debug.h"
#include "stdio.h"

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

void DEBUG_USART_IRQHandler(void) {
    u8 tmp;
    if (USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET) {
        tmp = USART_ReceiveData(DEBUG_USARTx);
        Debug_CommandHandler(tmp);
       USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_RXNE);
    }
}

// Incomming commands handler, process until no more data coming in.
void Debug_CommandHandler(u8 cmd) {
    /*
    **              TODO: Completion
    */
    switch (cmd) {
        case 'O':
            Debug_OnCommand_O();
            break;
        default:
            // ignored
            printf("Unexpected\r\n");
            break;
    }
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


void Debug_OnCommand_O(void) {
    /*
    **          TODO:
    */
}
