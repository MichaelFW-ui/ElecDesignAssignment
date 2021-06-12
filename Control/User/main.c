//#include "stm32f10x.h"
#include "adc.h"
#include "delay.h"
#include "nixie.h"
#include "nrf24l01.h"
#include "debug.h"
#include "stdio.h"

u16 ADCValue1, ADCValue2, i = 0;
Remote_DataStructure FuncData;
Remote_ACKDataStructure FuncData_S;

int main(void) {
    delay_init();           //延迟初始化
    adc_init();             //模数转换器1(ADC1)初始化
    adc_gpio_init();        //模数转换器1(ADC1)通道0的IO端口初始化
    Debug_USART_Init();
    Nixie_Init();

    SPI_NRF_Init();


    if (!NRF_Check()) {
        Nixie_SetNumber(1234);
        delay_s(5000);
    }

    while (1) {
        NRF_TX_Mode();
        ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1,
                                 ADC_SampleTime_239Cycles5);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET)
            ;
        ADCValue1 = ADC_GetConversionValue(ADC1);

        ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1,
                                 ADC_SampleTime_239Cycles5);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
            ;
        ADCValue2 = ADC_GetConversionValue(ADC1);

        FuncData.Acceleration = (s16)(18.99 * (ADCValue2 - 1580));
        //向前为正，最高24345，向后为负，最低-29985
        FuncData.Direction = (s16)(20.5 * (ADCValue1 - 1305));
        //向右为正，最高28536，向左为负，最低-26752
        // adc会存在正负一千范围内的抖动（对于a.acc和a.dir），可以增大多项式的次数减小抖动影响
        FuncData.Status = (i++);
        Nixie_SetNumber(i);
        delay_s(1);
        printf("RRR");
        NRF_TX_Mode();
        NRF_Tx_Dat((uint8_t*)&FuncData);
        delay_s(20);
    }
}
