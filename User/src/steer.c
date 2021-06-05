#include "steer.h"

#include "stm32f10x.h"

u16 Steer_ADC_Converted = 0;

void Steer_OnCommandLine(Remote_DataStructure *data) {
    /*
    **
    **
    **
    **      TODO:Completion
    **
    **
    **
    */
}

/**
 * @brief Set TIM as 100us
 * 
 */
void Steer_TIM_Init() {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    STEER_TIM_APBxClkCmd(STEER_TIM_CLK, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_Period        = 100 - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(STEER_TIM, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(STEER_TIM, TIM_IT_Update, ENABLE);
    TIM_Cmd(STEER_TIM, ENABLE);
}

// 100Hz
void Steer_TIM_IRQ_Init() {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                   = STEER_TIM_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

u8 static volatile SteerCounter   = 0;
u16 static volatile SteerPosition = 0;
u8 static volatile SteerDirection = 0;
u8 static volatile SteerDutyCycle = 0;
// 0 for A and 1 for B

// PID
// read position to pretend it from broken
// direction, careful
/**
 * @brief PWM Control
 *       PWM - 0 : A output
 *       0 - PWM : B output
 *       0 -     : Stop
 * @attention NonPID Control Flow
 */
void Steer_TIM_IRQHandler() {
    if (TIM_GetITStatus(STEER_TIM, TIM_IT_Update) != RESET) {
        if (SteerCounter < SteerDutyCycle) {
            if (SteerDirection) {
                GPIO_SetBits(STEER_PWM_B_PORT, STEER_PWM_B_PIN);
                GPIO_ResetBits(STEER_PWM_A_PORT, STEER_PWM_A_PIN);
            } else {
                GPIO_SetBits(STEER_PWM_A_PORT, STEER_PWM_A_PIN);
                GPIO_ResetBits(STEER_PWM_B_PORT, STEER_PWM_B_PIN);
            }
        } else {
            GPIO_ResetBits(STEER_PWM_A_PORT, STEER_PWM_A_PIN);
            GPIO_ResetBits(STEER_PWM_B_PORT, STEER_PWM_B_PIN);
        }
        SteerCounter = (SteerCounter + 1) % 100;
        /*
        **
        **
        **
        ** TODO:Comletion for PID Control Flow version
        **
        **
        **
        */
        TIM_ClearITPendingBit(STEER_TIM, TIM_IT_Update);
    }
}

void Steer_Init() {
    Steer_GPIO_ADC_Init();
    Steer_TIM_Init();
    Steer_TIM_IRQ_Init();
    Steer_ADC_NVIC_Init();
}

void Steer_GPIO_ADC_Init() {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Output A B8
    STEER_PWM_A_APBxClk_Fn(STEER_PWM_A_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = STEER_PWM_A_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(STEER_PWM_A_PORT, &GPIO_InitStructure);

    // Output B B9
    GPIO_InitStructure.GPIO_Pin = STEER_PWM_B_PIN;
    STEER_PWM_B_APBxClk_Fn(STEER_PWM_B_CLK, ENABLE);
    GPIO_Init(STEER_PWM_B_PORT, &GPIO_InitStructure);

    // ADC1 Channel8 PB0

    GPIO_InitStructure.GPIO_Pin = STEER_ADC_PIN;
    STEER_ADC_APBxClk_Fn(STEER_ADC_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(STEER_ADC_PORT, &GPIO_InitStructure);

    // ADC initialize
    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode       = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = 1;
    ADC_Init(STEER_ADCx, &ADC_InitStructure);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    // 72 / 8 = 9MHz
    ADC_RegularChannelConfig(STEER_ADCx, STEER_ADC_CHANNEL, 1,
                             ADC_SampleTime_239Cycles5);
    // sampling every 28us

    ADC_ITConfig(STEER_ADCx, ADC_IT_EOC, ENABLE);
    ADC_Cmd(STEER_ADCx, ENABLE);

    ADC_ResetCalibration(STEER_ADCx);
    while (ADC_GetResetCalibrationStatus(STEER_ADCx))
        ;
    ADC_StartCalibration(STEER_ADCx);
    while (ADC_GetResetCalibrationStatus(STEER_ADCx))
        ;

    ADC_SoftwareStartConvCmd(STEER_ADCx, ENABLE);
}

void Steer_ADC_NVIC_Init() {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                   = STEER_ADC_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}


void Steer_ADC_IRQHandler() {
    if (ADC_GetITStatus(STEER_ADCx, ADC_IT_EOC) == SET) {
        Steer_ADC_Converted = ADC_GetConversionValue(STEER_ADCx);
        ADC_ClearITPendingBit(STEER_ADCx, ADC_IT_EOC);
    }
}


/*                  PID TRY                                                   */

Steer_PID_TypeDef Steer_PID;

void Steer_PID_Init() {
    /*
    ** TODO:
    */
}
