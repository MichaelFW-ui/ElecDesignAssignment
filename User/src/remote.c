#include "remote.h"

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
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    // CPOL = 0
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    // CPHA = 0
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    // 高位在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    // invalid because it won't be used 
    SPI_Init(REMOTE_SPIx, &SPI_InitStructure);
    SPI_Cmd(REMOTE_SPIx, ENABLE);
}

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

u8 Remote_SPI_ReceiveByte(void) {
    return (Remote_SPI_SendByte(InvalidByte));
}

u8 Remote_SPI_Callback(u16 ErrorCode) {
    /*
                TODO : Completion
    */
    return 0;
}
