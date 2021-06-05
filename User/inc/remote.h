/**
 * @file remote.h
 * @author Michael Francis Williams (GitHub:Michael-ui)
 * @brief 
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
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

/*END OF SPI Definition*/

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

// Operators
#define READ_RG     0x00        // Read register
#define WRITE_RG    0x20        // write register
#define RD_RX_PLOAD 0x61        // Read RX load
#define WR_TX_PLOAD 0xA0        // write TX load
#define FLUSH_TX    0xE1        // clear TX FIFO when transmitting
#define FLUSH_RX    0xE2        // clear RX FIXO when receiving
#define REUSE_TX_PL 0xE3        // Reuse last package when ce is low
#define NOP         0xFF        // empty operator
// End of operators

// Address

#define CONFIG     0x00        // configuration register
#define EN_AA      0x01        // Enable Auto Acknowledgement
#define EN_RXADDR  0x02        // Enable RX Addresses
#define SETUP_AW   0x03        // Setup of Address Widths
#define SETUP_RETR 0x04        // Setup of Automatic Retransmission
#define RF_CH      0x05        // RF Channel
#define RF_SETUP   0x06        // RF Setup Register
#define STATUS     0x07
#define OBSERVE_TX 0x08
#define RPD        0x09

#define RX_ADDR_P0 0x0a
#define RX_ADDR_P1 0x0b
#define RX_ADDR_P2 0x0c
#define RX_ADDR_P3 0x0d
#define RX_ADDR_P4 0x0e
#define RX_ADDR_P5 0x0f        // RX addresses

#define TX_ADDR 0x10

#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16        // Number of bytes in data pipe_x

#define FIFO_STATUS 0x17
#define DYNPD       0x1c        // Enable dynamic payload length
// End of Addresses

#define PRX_ADDR 0xf0f0f0f0f0

/*                            END OF NRF24L01                                 */


/*                            Bit operation                                   */
#define SetBitAt(value, position) (value | (1 << position))
#define ResetBitAt(value, position) (value & ~(1 << position))
/*                     END OF Bit operation                                   */

/*                        Remote Ctrl Data structure                          */
typedef enum { ACK = 0, NACK = !0 } DataStatus;
typedef struct {
    s16 Acceleration;         // -32768 ~ 32767, USE -30000 ~ 30000 ONLY
    s16 Direction;            // -32768 ~ 32767, USE -30000 ~ 30000 ONLY
    DataStatus Status;        // ACK: ACK data received
} Remote_DataStructure;

typedef struct {
    s16 CurrentSpeed;
    DataStatus Status;        // Reserved for ACK.
} Remote_ACKDataStructure;

#define REMOTE_DATA_SIZE (sizeof (Remote_DataStructure))
#define REMOTE_ACKDATA_SIZE (sizeof (Remote_ACKDataStructure))

/*                 END OF Remote Ctrl Data structure                          */

void Remote_SPI_Init(void);

void Remote_NRF_GPIO_Init(void);

void Remote_NRF_NVIC_Init(void);

// NRF configuration
void Remote_NRF_Init(void);

u8 Remote_SPI_SendByte(u8 byte);

u8 Remote_SPI_ReceiveByte(void);

u8 Remote_SPI_Callback(u16 ErrorCode);

u8 Remote_NRF_WriteReg(u8 reg, u8 Reset, u8 Set);
#endif
