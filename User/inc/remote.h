#ifndef __REMOTE_H
#define __REMOTE_H

#include "stm32f10x.h"

/*SPI1*/
#define REMOTE_SPIx           SPI1
#define REMOTE_SPI_APBxClk_Fn RCC_APB2PeriphClockCmd
#define REMOTE_SPI_CLK        RCC_APB2Periph_SPI1
// CS
#define REMOTE_SPI_CS_APBxClk_Fn RCC_APB2PeriphClockCmd
#define REMOTE_SPI_CS_CLK        RCC_APB2Periph_GPIOA
#define REMOTE_SPI_CS_PORT       GPIOA
#define REMOTE_SPI_CS_PIN        GPIO_Pin_0
// SCK
#define REMOTE_SPI_SCK_APBxClk_Fn RCC_APB2PeriphClockCmd
#define REMOTE_SPI_SCK_CLK        RCC_APB2Periph_GPIOA
#define REMOTE_SPI_SCK_PORT       GPIOA
#define REMOTE_SPI_SCK_PIN        GPIO_Pin_5
// MISO
#define REMOTE_SPI_MISO_APBxClk_Fn RCC_APB2PeriphClockCmd
#define REMOTE_SPI_MISO_CLK        RCC_APB2Periph_GPIOA
#define REMOTE_SPI_MISO_PORT       GPIOA
#define REMOTE_SPI_MISO_PIN        GPIO_Pin_6
// MOSI
#define REMOTE_SPI_MOSI_APBxClk_Fn RCC_APB2PeriphClockCmd
#define REMOTE_SPI_MOSI_CLK        RCC_APB2Periph_GPIOA
#define REMOTE_SPI_MOSI_PORT       GPIOA
#define REMOTE_SPI_MOSI_PIN        GPIO_Pin_7

// CS Function
#define REMOTE_SPI_CS_LOW() \
    GPIO_ResetBits(REMOTE_SPI_CS_PORT, REMOTE_SPI_CS_PIN)
#define REMOTE_SPI_CS_HIGH() \
    GPIO_SetBits(REMOTE_SPI_CS_PORT, REMOTE_SPI_CS_PIN)

#define REMOTE_SPI_TIMEOUT      (0x1000)
#define REMOTE_SPI_LONG_TIMEOUT (REMOTE_SPI_TIMEOUT << 2)

#define REMOTE_SPI_FLAG_TIMEOUT_TXE  (0x0001)
#define REMOTE_SPI_FLAG_TIMEOUT_RXNE (0x0001)
#define REMOTE_SPI_FLAG_CLEAR        (0x0000)

#define InvalidByte 0xFF

/*END OF SPI DEFINATION*/

/*
**          Data of the SPI from NRF chip:
**          CPHA = 0
**          CPOL = 0
**          Use 4.5Mbits/s, i.e. f_sck =  f_pclk / 16 = 72 / 16 = 4.5
*/

void Remote_SPI_Init(void);

void Remote_Chip_Init(void);

u8 Remote_SPI_SendByte(u8 byte);

u8 Remote_SPI_ReceiveByte(void);

u8 Remote_SPI_Callback(u16 ErrorCode);

#endif
