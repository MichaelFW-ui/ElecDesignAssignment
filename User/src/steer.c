/**
 * @file steer.c
 * @author Michael Francis Williams (GitHub:Michael-ui)
 * @brief the module functions under a 10ms control data flow.
 * You have to input the desired position regularly.
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "steer.h"
#include "stdio.h"
#include "stm32f10x.h"

u16 Steer_ADC_Converted = 0;

// u16 static volatile SteerPosition = 0;
// u16 static volatile SteerDestinationPos = 0;

// u8 static volatile SteerDirection = 0;
// u8 static volatile SteerDestinationDir = 0;
// // 0 for positive
// u8 static volatile SteerAccelDirection = 0;
// 0 for A PWM, 1 for B PWM


u8 static volatile SteerCounter   = 0;
u8 static volatile SteerDutyCycle = 0;

static vs32 SteerPosition       = 0;
static vs32 SteerDestinationPos = 0;
static vs8 SteerAccelDirection  = 0;

Steer_PID_TypeDef Steer_PID;

#define STEER_MID_RES 2047
// Mid point resistence

/**
 * @brief Get Current Position
 * 
 */
void Steer_GetPosition(void) {
    u16 tmp = Steer_ADC_Converted;
    SteerPosition = (tmp - STEER_MID_RES) << 2;
}

/**
 * @brief Operate the Steer when command comes
 * 
 * @param data Received command data
 */
void Steer_OnCommandLine(Remote_DataStructure *data) {
    Steer_GetPosition();
    SteerDestinationPos = data->Direction;
    // Steer_PID_Operate();
    Steer_PlainOperate();
}

/**
 * @brief NonPID operation
 * According to the report, it is not okey to treat the steering motor as 
 * an engine. So PID is Not Okay.
 * 
 * @param None
 * @retval None
 */
void Steer_PlainOperate() {
    SteerAccelDirection = ((SteerDestinationPos > 0) ? 1 : 0);

    u16 output = (SteerAccelDirection > 0) ? SteerAccelDirection / 300
                                           : -SteerAccelDirection / 300;
    if (output > 100) output = 100;
    if (output < 10) output = 0;
    Steer_PWM_SetDutyCycle(output);
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

// 0 for A and 1 for B

// PID
// read position to pretend it from broken
// direction, careful
/**
 * @brief PWM Control
 *  PWM - 0 : A output
 *  0 - PWM : B output
 *  0 -     : Stop
 * @attention NonPID Control Flow
 */
void Steer_TIM_IRQHandler() {
    if (TIM_GetITStatus(STEER_TIM, TIM_IT_Update) != RESET) {
        if (SteerCounter++ < SteerDutyCycle) {
            if (SteerAccelDirection) {
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
        SteerCounter %= 100;
        if (!SteerCounter) {
            Steer_PID_Operate();
        }
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


void Steer_PID_Init() {
    /*
    ** TODO:
    */

   // Initialize
    Steer_PID.Kd            = 0;
    Steer_PID.Ki            = 0;
    Steer_PID.Kp            = 0;
    Steer_PID.LastError     = 0;
    Steer_PID.SetPoint      = 0;
    Steer_PID.IntegralError = 0;
}

#define STEER_PID_END_OPT 20
// End of pid how to deal with the output?
// Make it ok for the pwm
#define STEER_PID_ERR 5
// less than this will be considered no force
#define STEER_PID_INTEGRAL_OVERFLOW 300
// Prevent integral overflow

#define __DEBUG_PWM

/**
 * @brief PID Control Flow
 * 
 */
void Steer_PID_Operate() {
    s32 Error  = 0;
    s32 Output = 0;

    Error = SteerPosition - SteerDestinationPos;

    SteerAccelDirection = ((Error > 0) ? 1 : 0);
    if (Steer_PID.IntegralError + Error < STEER_PID_INTEGRAL_OVERFLOW &&
        Steer_PID.IntegralError + Error > -STEER_PID_INTEGRAL_OVERFLOW)
        Steer_PID.IntegralError += Error;
    // Integral overflow prevention
    // modify the parameter later
    Output = Steer_PID.Kd * Error +
             Steer_PID.Kp * (Error - Steer_PID.LastError) +
             Steer_PID.Ki * (Steer_PID.IntegralError);
    Steer_PID.LastError = Error;
    Output /= STEER_PID_END_OPT;
    if (Output < 0) Output = ~Output + 1;        // Equals -Output
    if (Output > 100) Output = 100;
    if (Output < STEER_PID_ERR) Output = 0;
#ifdef __DEBUG_PWM

    printf("Steer %d %d\r\n", SteerAccelDirection, Output);

#else

    Steer_PWM_SetDutyCycle((u8)Output);

#endif // __DEBUG_PWM
}

/**
 * @brief Set PWM DutyCycle
 * 
 * @param DutyCycle 
 */
void Steer_PWM_SetDutyCycle(u8 DutyCycle) {
    SteerDutyCycle = DutyCycle;
}
