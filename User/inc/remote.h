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

// Delay between the commands
#define REMOTE_DELAY()              delay_us(1)

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

/*                               NRF24L01                                     */


// GPIO
#define REMOTE_NRF_CE_APBxClk_Fn RCC_APB2PeriphClockCmd
#define REMOTE_NRF_CE_CLK        RCC_APB2Periph_GPIOA
#define REMOTE_NRF_CE_PORT       GPIOA
#define REMOTE_NRF_CE_PIN        GPIO_Pin_1

#define REMOTE_NRF_CE_HIGH() \
    GPIO_SetBits(REMOTE_NRF_CE_PORT, REMOTE_NRF_CE_PIN)
#define REMOTE_NRF_CE_LOW() \
    GPIO_ResetBits(REMOTE_NRF_CE_PORT, REMOTE_NRF_CE_PIN)

#define REMOTE_NRF_IRQ_APBxClk_Fn RCC_APB2PeriphClockCmd
#define REMOTE_NRF_IRQ_CLK        (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)
#define REMOTE_NRF_IRQ_PORT       GPIOA
#define REMOTE_NRF_IRQ_PIN        GPIO_Pin_2

//EXTI for IRQ
#define NRF_IRQ_EXTI_PORTSOURCE GPIO_PortSourceGPIOA
#define NRF_IRQ_EXTI_PINSOURCE  GPIO_PinSource2
#define NRF_IRQ_EXTI_LINE       EXTI_Line2
#define NRF_IRQ_EXTI_IRQ        EXTI2_IRQn
#define NRF_IRQHandler          EXTI2_IRQHandler

// Address
#define READ_RG     0x00        // Read register
#define WRITE_RG    0x20        // write register
#define RD_RX_PLOAD 0x61        // Read RX load
#define WR_TX_PLOAD 0xA0        // write TX load
#define FLUSH_TX    0xE1        // clear TX FIFO when transmitting
#define FLUSH_RX    0xE2        // clear RX FIXO when receiving
#define REUSE_TX_PL 0xE3        // Reuse last package when ce is low
#define NOP         0xFF        // empty operator
/*                            END OF NRF24L01                                 */

void Remote_SPI_Init(void);

void Remote_NRF_GPIO_Init(void);

void Remote_NRF_NVIC_Init(void);

// NRF configuration
void Remote_NRF_Init(void);

u8 Remote_SPI_SendByte(u8 byte);

u8 Remote_SPI_ReceiveByte(void);

u8 Remote_SPI_Callback(u16 ErrorCode);

#endif
