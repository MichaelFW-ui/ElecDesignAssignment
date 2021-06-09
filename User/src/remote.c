/**
 * @file remote.c
 * @author Michael Francis Williams (GitHub:Michael-ui)
 * @brief Remote Control Module NRF24L01
* Params are fixed.
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "remote.h"
#include "stdio.h"
#include "delay.h"
#include "steer.h"
#include "motor.h"

// Init the SPI part, not including the nrf chip.
void Remote_SPI_Init() {
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /*          GPIO init           */
    REMOTE_SPI_APBxClk_Fn(REMOTE_SPI_CLK, ENABLE);
    REMOTE_SPI_CS_APBxClk_Fn(REMOTE_SPI_CS_CLK, ENABLE);
    REMOTE_SPI_SCK_APBxClk_Fn(REMOTE_SPI_SCK_CLK, ENABLE);
    REMOTE_SPI_MISO_APBxClk_Fn(REMOTE_SPI_MISO_CLK, ENABLE);
    REMOTE_SPI_MOSI_APBxClk_Fn(REMOTE_SPI_MOSI_CLK, ENABLE);

    // CS
    GPIO_InitStructure.GPIO_Pin   = REMOTE_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(REMOTE_SPI_CS_PORT, &GPIO_InitStructure);

    // SCK
    GPIO_InitStructure.GPIO_Pin  = REMOTE_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(REMOTE_SPI_SCK_PORT, &GPIO_InitStructure);
    // MISO
    GPIO_InitStructure.GPIO_Pin = REMOTE_SPI_MISO_PIN;
    GPIO_Init(REMOTE_SPI_MISO_PORT, &GPIO_InitStructure);
    // MOSI
    GPIO_InitStructure.GPIO_Pin = REMOTE_SPI_MOSI_PIN;
    GPIO_Init(REMOTE_SPI_MOSI_PORT, &GPIO_InitStructure);

    // CS high
    REMOTE_SPI_CS_HIGH();

    /*          End of GPIO init           */

    /*          SPI init                    */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
    // CPOL = 0
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    // CPHA = 0
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    // 高位在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    // invalid because it won't be used
    SPI_Init(REMOTE_SPIx, &SPI_InitStructure);
    SPI_Cmd(REMOTE_SPIx, ENABLE);
}

// Use when the CS is LOW
u8 Remote_SPI_SendByte(u8 byte) {
    u32 SPITimeout = REMOTE_SPI_TIMEOUT;

    // Wait for TXE
    while (SPI_I2S_GetFlagStatus(REMOTE_SPIx, SPI_I2S_FLAG_TXE) == RESET) {
        if ((SPITimeout--) == 0)
            return Remote_SPI_Callback(REMOTE_SPI_FLAG_TIMEOUT_TXE);
    }

    SPI_I2S_SendData(REMOTE_SPIx, byte);

    SPITimeout = REMOTE_SPI_TIMEOUT;

    // Wait for RXNE
    while (SPI_I2S_GetFlagStatus(REMOTE_SPIx, SPI_I2S_FLAG_RXNE) == RESET) {
        if ((SPITimeout--) == 0)
            return Remote_SPI_Callback(REMOTE_SPI_FLAG_TIMEOUT_RXNE);
    }

    return SPI_I2S_ReceiveData(REMOTE_SPIx);
}

u8 Remote_SPI_ReceiveByte(void) { return (Remote_SPI_SendByte(InvalidByte)); }

u8 Remote_SPI_Callback(u16 ErrorCode) {
    return printf("Error %x\r\n", ErrorCode);
}

void Remote_NRF_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    REMOTE_NRF_CE_APBxClk_Fn(REMOTE_NRF_CE_CLK, ENABLE);
    REMOTE_NRF_IRQ_APBxClk_Fn(REMOTE_NRF_IRQ_CLK, ENABLE);

    // CE
    GPIO_InitStructure.GPIO_Pin   = REMOTE_NRF_CE_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(REMOTE_NRF_CE_PORT, &GPIO_InitStructure);

    // IRQ and GPIO
    GPIO_InitStructure.GPIO_Pin  = REMOTE_NRF_IRQ_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(REMOTE_NRF_IRQ_PORT, &GPIO_InitStructure);

    // IRQ and EXTI
    GPIO_EXTILineConfig(NRF_IRQ_EXTI_PORTSOURCE, NRF_IRQ_EXTI_PINSOURCE);
    EXTI_InitStructure.EXTI_Line    = NRF_IRQ_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void Remote_NRF_NVIC_Init(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                   = NRF_IRQ_EXTI_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// Init the NRF chip
void Remote_NRF_Init() {
    Remote_NRF_NVIC_Init();
    Remote_NRF_GPIO_Init();


    /*
    **          TODO:Command lines Completion
    */

    u8 tx;
    u8 rx;

    REMOTE_SPI_CS_LOW();
    tx = READ_RG | CONFIG;         // read CONFIG
    rx = Remote_SPI_SendByte(tx);
    rx = Remote_SPI_ReceiveByte();
    REMOTE_SPI_CS_HIGH();
    REMOTE_DELAY();

    tx = SetBitAt(rx, 0);           // PRX
    tx = SetBitAt(tx, 3);           // enable CRC
    tx = SetBitAt(tx, 2);
    tx = ResetBitAt(tx, 1);
    REMOTE_SPI_CS_LOW();
    rx = Remote_SPI_SendByte(WRITE_RG | CONFIG);
    rx = Remote_SPI_SendByte(tx);
    REMOTE_SPI_CS_HIGH();
    REMOTE_DELAY();

    u8 reset = 0;
    u8 set = 0;

    //  Address width 5 bytes
    set = SetBitAt(0, 0) | SetBitAt(0, 1);
    reset = 0;
    Remote_NRF_WriteReg(SETUP_AW, reset, set);
    REMOTE_DELAY();

    // 2400 + 100 = 2500MHz
    set = 100;
    reset = 0;
    Remote_NRF_WriteReg(RF_CH, reset, set);
    REMOTE_DELAY();

    // 1Mbps, 0dBm
    reset = SetBitAt(0, 3);
    set = 0;
    Remote_NRF_WriteReg(RF_SETUP, reset, set);
    REMOTE_DELAY();

    // Send PRX address : PRX_ADDR:0xf0f0f0f0f0
    // LSB!!
    reset = 0;
    set = 0;
    REMOTE_SPI_CS_LOW();
    tx = WRITE_RG | RX_ADDR_P0;
    rx = Remote_SPI_SendByte(tx);
    for (int i = 0; i < 5; ++i) {
        tx = (PRX_ADDR >> (i * 8)) % 0x00000100;
        Remote_SPI_SendByte(tx);
    }
    REMOTE_SPI_CS_HIGH();
    REMOTE_DELAY();

    // PWR_UP set high
    reset = 0;
    set = SetBitAt(0, 1);
    Remote_NRF_WriteReg(CONFIG, reset, set);
    REMOTE_DELAY();

    REMOTE_NRF_CE_HIGH();

    return;
}


u8 Remote_NRF_WriteReg(u8 reg, u8 reset, u8 set) {
    u8 tx;
    u8 rx;

    // read reg first
    tx = READ_RG | reg;
    REMOTE_SPI_CS_LOW();
    rx = Remote_SPI_SendByte(tx);
    rx = Remote_SPI_ReceiveByte();
    REMOTE_SPI_CS_HIGH();
    REMOTE_DELAY();

    // Set the bytes and write them
    tx = WRITE_RG | reg;
    REMOTE_SPI_CS_LOW();
    Remote_SPI_SendByte(tx);
    tx = rx & ~reset;
    tx = tx | set;
    rx = Remote_SPI_SendByte(tx);
    REMOTE_SPI_CS_HIGH();
    return rx;
}

Remote_DataStructure RxData       = {0};
Remote_ACKDataStructure RxACKData = {0};
// I suppose this will take more than 500us. Careful.
void NRF_IRQHandler() {
    if (EXTI_GetITStatus(NRF_IRQ_EXTI_LINE) != RESET) {
        int rx, tx;
        tx = NOP;
        REMOTE_SPI_CS_LOW();
        rx = Remote_SPI_SendByte(tx);
        REMOTE_SPI_CS_HIGH();
        REMOTE_DELAY();
        if (rx & (1 << 6)) {
            // ACknowledgement
            REMOTE_SPI_CS_LOW();

            tx = 0xa8;        // W_ACK_PAYLOAD,  PPP = 000b
            rx = Remote_SPI_SendByte(tx);

            u8 *ptr = (u8 *)&RxACKData;
            for (int i = 0; i < REMOTE_ACKDATA_SIZE; ++i) {
                tx = *((u8 *)&RxACKData + i);
                Remote_SPI_SendByte(tx);
            }
            REMOTE_SPI_CS_HIGH();
            REMOTE_DELAY();

            // Read data
            REMOTE_SPI_CS_LOW();
            tx  = RD_RX_PLOAD;
            rx  = Remote_SPI_SendByte(tx);
            tx  = NOP;
            ptr = (u8 *)&RxData;
            for (int i = 0; i < REMOTE_DATA_SIZE; ++i) {
                *(ptr + i) = Remote_SPI_SendByte(tx);
            }
            REMOTE_SPI_CS_HIGH();
            REMOTE_DELAY();

            // Clear the flag
            REMOTE_SPI_CS_LOW();
            tx = WRITE_RG | STATUS;
            rx = Remote_SPI_SendByte(tx);
            tx = SetBitAt(rx, 6);
            rx = Remote_SPI_SendByte(tx);
            REMOTE_SPI_CS_HIGH();
            REMOTE_DELAY();
            /*
            **
            **
            **      TODO:Completion
            **      How to use the command lines?
            **
            */
            Steer_OnCommandLine(&RxData);
            Motor_OnCommandLine(&RxData);
        }
        EXTI_ClearITPendingBit(NRF_IRQ_EXTI_LINE);
    }
}
