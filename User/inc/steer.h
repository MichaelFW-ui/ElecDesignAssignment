/**
 * @file steer.h
 * @author Michael Francis Williams (GitHub:Michael-ui)
 * @brief 
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __STEER_H
#define __STEER_H

#include "remote.h"

#include "stm32f10x.h"

/*                        Definition of GPIO                                  */

// Output A
#define STEER_PWM_A_APBxClk_Fn RCC_APB2PeriphClockCmd
#define STEER_PWM_A_CLK RCC_APB2Periph_GPIOB
#define STEER_PWM_A_PORT GPIOB
#define STEER_PWM_A_PIN GPIO_Pin_8

// Output B
#define STEER_PWM_B_APBxClk_Fn RCC_APB2PeriphClockCmd
#define STEER_PWM_B_CLK RCC_APB2Periph_GPIOB
#define STEER_PWM_B_PORT GPIOB
#define STEER_PWM_B_PIN GPIO_Pin_9

// ADC1 Channel8 PB0
#define STEER_ADC_APBxClk_Fn RCC_APB2PeriphClockCmd
#define STEER_ADC_CLK RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB
#define STEER_ADC_PORT GPIOB
#define STEER_ADC_PIN GPIO_Pin_0
#define STEER_ADCx ADC1
#define STEER_ADC_CHANNEL ADC_Channel_8
#define STEER_ADC_IRQ   ADC1_2_IRQn

#define Steer_ADC_IRQHandler ADC1_2_IRQHandler

/*                        END OF GPIO Definition                              */

/*                        Definition of TIM                                   */
#define STEER_TIM TIM5
#define STEER_TIM_APBxClkCmd RCC_APB1PeriphClockCmd
#define STEER_TIM_CLK RCC_APB1Periph_TIM5
#define STEER_TIM_IRQ TIM5_IRQn

#define Steer_TIM_IRQHandler TIM5_IRQHandler

/*                        END OF TIM Definition                               */
//          PWM frequency: Same as motor

void Steer_OnCommandLine(Remote_DataStructure *data);

void Steer_TIM_Init(void);

void Steer_Init(void);

void Steer_GPIO_ADC_Init(void);

void Steer_TIM_IRQ_Init(void);

void Steer_PWM_SetDutyCycle(u8 DutyCycle);

void Steer_ADC_NVIC_Init(void);

void Steer_PID_Operate(void);

void Steer_GetPosition(void);

void Steer_PlainOperate(void);


/*                   PID TRY                                                  */

typedef struct {
    double Kp;
    double Ki;
    double Kd;
    s32 SetPoint;
    s32 LastError;
    s32 IntegralError;
} Steer_PID_TypeDef;

#endif
