#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"
#include "remote.h"

/*                    Defination of GPIO                                      */
// the first pin
#define MOTOR_PWM_A_APBxClk_Fn RCC_APB2PeriphClockCmd
#define MOTOR_PWM_A_CLK RCC_APB2Periph_GPIOA
#define MOTOR_PWM_A_PORT GPIOA
#define MOTOR_PWM_A_PIN  GPIO_Pin_3

// the second pin
#define MOTOR_PWM_B_APBxClk_Fn RCC_APB2PeriphClockCmd
#define MOTOR_PWM_B_CLK RCC_APB2Periph_GPIOA
#define MOTOR_PWM_B_PORT GPIOA
#define MOTOR_PWM_B_PIN  GPIO_Pin_4

// enable signal.
#define MOTOR_PWM_EN_APBxClk_Fn RCC_APB2PeriphClockCmd
#define MOTOR_PWM_EN_CLK RCC_APB2Periph_GPIOB
#define MOTOR_PWM_EN_PORT GPIOB
#define MOTOR_PWM_EN_PIN GPIO_Pin_7
// 0 for enable.



/*                        END OF of GPIO Defination                           */


/*                        TIM DEFINATION                                      */
#define MOTOR_TIM TIM7
#define MOTOR_TIM_APBxClkCmd RCC_APB1PeriphClockCmd
#define MOTOR_TIM_CLK RCC_APB1Periph_TIM7
#define MOTOR_TIM_IRQ TIM7_IRQn
#define MOTOR_TIM_IRQHandler TIM7_IRQHandler
/*                 END OF TIM DEFINATION                                      */


//          PWM frequency: 100Hz, that is 
//          10ms every round
//          APB1 is doubled for TIM

void Motor_OnCommandLine(Remote_DataStructure *data);

void Motor_TIM_Init(void);

void Motor_TIM_IRQ_Init(void);

void Motor_Init(void);

void Motor_PWM_SetDutyCycle(u8 DutyCycle);

#endif
