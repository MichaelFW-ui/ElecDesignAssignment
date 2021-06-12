#include "stm32f10x.h"
#include "delay.h"

//模数转换器1(ADC1)初始化函数
void adc_init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//使能模数转换器1(ADC1)时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);					//配置ADC工作时钟为主频的六分之一，即72÷6=12MHz，最大不能超过14MHz
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;					//ADC工作模式：独立工作模式
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;							//ADC扫描模式：使能
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;					//ADC连续模式：失能
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;	//ADC外部触发模式：不使用外部触发；即通过软件设置开始转换。
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;				//ADC数据对齐模式：右对齐
	ADC_InitStructure.ADC_NbrOfChannel=1;								//ADC转换数目：1次
	ADC_Init(ADC1, &ADC_InitStructure);									//将结构体(ADC_InitStructure)的设定写入ADC1控制寄存器	
	ADC_Cmd(ADC1, ENABLE);	//使能ADC1
	delay_us(1);			//等待ADC1使能完成
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1))
		;
	ADC_StartCalibration(ADC1);        //开始校准ADC1
	while (ADC_GetCalibrationStatus(ADC1) != RESET)
		;        //等待ADC1校准完成
}

//模数转换器1(ADC1)通道0的IO端口初始化函数
void adc_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_AIN;        // IO端口工作模式：模拟输入

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
