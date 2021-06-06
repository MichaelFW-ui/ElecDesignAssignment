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


u8 Debug_USART_CommandBuffer[DEBUG_USART_BUFFER_SIZE] = {0};
u16 Debug_USART_BufferCur = 0;


void DEBUG_USART_IRQHandler(void) {
    u8 tmp;
    if (USART_GetITStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_RXNE);
        Debug_USART_BufferPush(tmp = USART_ReceiveData(DEBUG_USARTx));
        if (tmp == '\n') Debug_CommandHandler();
        // tmp = USART_ReceiveData(DEBUG_USARTx);
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
   u16 cur = Debug_USART_BufferCur;
    /*        Modify this for your commands                                   */
   for (int i = 0; i < cur; ++i) {
       printf("%c", *(ptrBuffer + i));
   }
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

