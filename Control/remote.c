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

    return;
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
    return;


    /*
    **          TODO:Command lines Completion
    */

    u8 tx;
    u8 rx;

    printf("Ready to config nrf in 5 seconds\r\n");
    delay_s(5000);
    printf("Begin configuration\r\n");

    
    REMOTE_SPI_CS_LOW();
    tx = READ_RG | CONFIG;         // read CONFIG
    rx = Remote_SPI_SendByte(tx);
    rx = Remote_SPI_ReceiveByte();
    REMOTE_SPI_CS_HIGH();
    REMOTE_DELAY();


    tx = ResetBitAt(rx, 0);           // PTX
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

    Remote_NRF_WriteReg(EN_RXADDR, 0, 1);
    REMOTE_DELAY();

    Remote_NRF_WriteReg(EN_AA, 0, 1);
    REMOTE_DELAY();

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

    // Transmit PRX address : PRX_ADDR:0xf0f0f0f0f0
    // LSB!!
    reset = 0;
    set = 0;
    REMOTE_SPI_CS_LOW();
    tx = WRITE_RG | TX_ADDR;
    rx = Remote_SPI_SendByte(tx);
    for (int i = 0; i < 5; ++i) {
        tx = (PRX_ADDR >> (i * 8)) % 0x00000100;
        Remote_SPI_SendByte(tx);
    }
    REMOTE_SPI_CS_HIGH();
    REMOTE_DELAY();

    REMOTE_SPI_CS_LOW();
    Remote_SPI_SendByte(FLUSH_TX);
    REMOTE_SPI_CS_HIGH();
    REMOTE_DELAY();
    
    REMOTE_SPI_CS_LOW();
    Remote_SPI_SendByte(FLUSH_RX);
    REMOTE_SPI_CS_HIGH();
    REMOTE_DELAY();

    Remote_NRF_WriteReg(STATUS, 0, 0);
    REMOTE_DELAY();

    Remote_NRF_WriteReg(SETUP_RETR, 0xf0, 0x03);
    REMOTE_DELAY();
    // PWR_UP set high
    reset = 0;
    set = SetBitAt(0, 1) | SetBitAt(0, 5);
    Remote_NRF_WriteReg(CONFIG, reset, set);
    REMOTE_DELAY();

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
        EXTI_ClearITPendingBit(NRF_IRQ_EXTI_LINE);
        return;
        int rx, tx;
        tx = NOP;
        REMOTE_SPI_CS_LOW();
        rx = Remote_SPI_SendByte(tx);
        REMOTE_SPI_CS_HIGH();
        REMOTE_DELAY();
        if (rx & (1 << 6)) {

            printf("Rx");

            // ACknowledgement received
            REMOTE_SPI_CS_LOW();

            tx = 0xa8;        // W_ACK_PAYLOAD,  PPP = 000b
            rx = Remote_SPI_SendByte(tx);

            u8 *ptr = (u8 *)&RxACKData;
            (void)ptr;
            for (int i = 0; i < REMOTE_ACKDATA_SIZE; ++i) {
                *((u8 *)&RxACKData + i) = Remote_SPI_SendByte(NOP);
            }

            printf("%x\r\n", RxACKData.Status);

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

        } else {
            Remote_NRF_WriteReg(STATUS, 0, 0);
            REMOTE_DELAY();
        }
        EXTI_ClearITPendingBit(NRF_IRQ_EXTI_LINE);
    }
}

void Remote_NRF_SendData(Remote_DataStructure *data) {
    u8 tx, rx;

    printf("%d %d\r\n", data->Acceleration, data->Direction);

    REMOTE_SPI_CS_LOW();
    tx = WR_TX_PLOAD;
    rx = Remote_SPI_SendByte(tx);
    for (int i = 0; i < REMOTE_ACKDATA_SIZE; ++i) {
        Remote_SPI_SendByte(*((u8 *)data + i));
    }

    REMOTE_SPI_CS_HIGH();
    REMOTE_NRF_CE_HIGH();
    delay_us(20);
    REMOTE_NRF_CE_LOW();
    printf("Sent?\r\n");
}


/******************************************************************************/
#define IRQ_TX (GPIO_ReadInputDataBit(REMOTE_NRF_IRQ_PORT, REMOTE_NRF_IRQ_PIN))
#define IRQ_RX (GPIO_ReadInputDataBit(REMOTE_NRF_IRQ_PORT, REMOTE_NRF_IRQ_PIN))
uint8_t DATAWIDTH=REMOTE_DATA_SIZE;
uint8_t TX_ADDRESS[5] = {0X3E, 0x7E, 0x7E, 0x7E, 0x7E};
/*NRF24LO1指令定义*/
 
#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define DUMMY      0x00
 
#define RX_PAYLOAD	0X61
#define TX_PAYLOAD	0XA0

//读写字节
static uint8_t SPI_RW_BYTE(uint8_t wdata)//MSB
{
	// uint8_t i;
	uint8_t rdata;
	//发送数据
    rdata = Remote_SPI_SendByte(rdata);
	// for(i=0;i<8;i++)
	// {
	// 	SCK_L;
	// 	delay_us(5);
	// 	((wdata<<i)&0x80)? MOSI_H : MOSI_L;
	// 	delay_us(5);
		
	// 	//接收数据
	// 	rdata<<=1;
	// 	SCK_TX;
	// 	rdata|=MISO;
	// 	delay_us(5);
	// 	SCK_L;
	// }
	return rdata;
}
 
//指令读写
static uint8_t SPI_RW_REG(uint16_t cmd,uint16_t val)
{	
	uint8_t status;
	REMOTE_SPI_CS_LOW();
	status=SPI_RW_BYTE(cmd);
	SPI_RW_BYTE(val);
	REMOTE_SPI_CS_HIGH();
	return status;
}
 
//读寄存器
static uint8_t SPI_R_REG(uint16_t reg)
{	
	uint8_t status;
	REMOTE_SPI_CS_LOW();
	SPI_RW_BYTE(reg);
	status=SPI_RW_BYTE(DUMMY);
	REMOTE_SPI_CS_HIGH();
	return status;
}
 
//写数据
static uint8_t SPI_WRITE_DATA(uint8_t cmd,uint8_t *wdata,uint8_t wnum)
{
	uint8_t status;
	REMOTE_SPI_CS_LOW();
	status=SPI_RW_BYTE(cmd);
	while(wnum)
	{
		SPI_RW_BYTE(*wdata++);
		//wdata++;
		wnum--;
	}
	REMOTE_SPI_CS_HIGH();
	return status;
}
 
//读数据
static uint8_t SPI_READ_DATA_RX(uint16_t cmd,uint8_t *rdat,uint8_t wnum)
{
	uint8_t status;
	REMOTE_SPI_CS_LOW();
	status=SPI_RW_BYTE(cmd);	
	while(wnum)
	{
		*rdat=SPI_RW_BYTE(DUMMY);
		rdat++;
		wnum--;
	}
	REMOTE_SPI_CS_HIGH();
	return status;
}

 
//发送模式配置，ACK
void NRF24_RX_MODE(void)
{
	REMOTE_SPI_CS_LOW();
	
	SPI_WRITE_DATA(W_REGISTER+RX_ADDR_P0,TX_ADDRESS,5);//写入接收通道0地址，5字节
	
	SPI_RW_REG(W_REGISTER+FLUSH_RX,0xFF);//清空RX_FIFO
	SPI_RW_REG(W_REGISTER+EN_AA,0X01);//使能自动确认
	SPI_RW_REG(W_REGISTER+EN_RXADDR,0X01);//使能接收通道0
	SPI_RW_REG(W_REGISTER+RF_CH,1000);//射频通道40
	SPI_RW_REG(W_REGISTER+RX_PW_P0,DATAWIDTH);//设置接收通道0，与发送字节相同
	SPI_RW_REG(W_REGISTER+RF_SETUP,0X0F);//速率2Mbps，功率7dBm
	SPI_RW_REG(W_REGISTER+CONFIG,0X0F);//接收送模式，2 Byte CRC
	
	REMOTE_NRF_CE_HIGH();
}
 
//接收数据
uint8_t RXPACKET(uint8_t *rx)
{
	uint8_t status,rdata[32];
	
	while(IRQ_RX);
	
	status=SPI_R_REG(STATUS);
	SPI_RW_REG(W_REGISTER+STATUS,status);//清除标志位
	printf("status=0x%x\n",status);
	
	if(status&0x40)
	{
		SPI_READ_DATA_RX(RD_RX_PLOAD,rdata,32);		
		SPI_RW_REG(W_REGISTER+FLUSH_RX,0xFF);
		printf("Input data:0x%x\n",rdata[0]);
		return 1;		
	}
	else 
		printf("Data unreceived\n");
		return 0;	
}

 
//发送模式配置，ACK
void NRF24_TX_MODE(void)
{
	REMOTE_NRF_CE_LOW();
		
	SPI_WRITE_DATA(W_REGISTER+TX_ADDR,TX_ADDRESS,5);//写入发送地址，5字节
	SPI_WRITE_DATA(W_REGISTER+RX_ADDR_P0,TX_ADDRESS,5);//写入接收通道0地址，5字节

    SPI_RW_REG(W_REGISTER + FLUSH_TX, 0xFF);         //清空TX_FIFO
    SPI_RW_REG(W_REGISTER + EN_AA, 0X01);            //使能自动确认
    SPI_RW_REG(W_REGISTER + EN_RXADDR, 0X01);        //使能接收通道0
    SPI_RW_REG(W_REGISTER + SETUP_RETR,
               0X25);        // 750us自动重发延时，自动重发5次
    SPI_RW_REG(W_REGISTER + RF_CH, 1000);           //射频通道40
    SPI_RW_REG(W_REGISTER + RF_SETUP, 0X0F);        //速率2Mbps，功率7dBm
    SPI_RW_REG(W_REGISTER + CONFIG, 0X0E);          //发送模式，2 Byte CRC

    REMOTE_NRF_CE_HIGH();
	delay_ms(1);
}
 

//发送数据包
uint8_t TXPACKET(uint8_t *tx)
{
	uint8_t status;
	
	SPI_RW_REG(FLUSH_TX,DUMMY);
	
	REMOTE_NRF_CE_LOW();
	SPI_WRITE_DATA(WR_TX_PLOAD,tx,DATAWIDTH);
	REMOTE_NRF_CE_HIGH();

    while (IRQ_TX)
        printf("W");
    
    GPIO_ReadInputDataBit(REMOTE_NRF_IRQ_PORT, REMOTE_NRF_IRQ_PIN);
	status=SPI_R_REG(STATUS);
	SPI_RW_REG(W_REGISTER+STATUS,status);//清除标志位
	printf("status=0x%x\n",status);
    if (status & 0x10) {
        SPI_RW_REG(W_REGISTER + FLUSH_TX, 0xFF);
        printf("Data untransmitted\n");
        return 0;
    }
    if (status & 0x20) {
        SPI_RW_REG(W_REGISTER + FLUSH_TX, 0xFF);
        printf("Data transmitted\n");
        return 1;
    }
    return 0;
}

