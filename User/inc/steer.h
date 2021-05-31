#ifndef __STEER_H
#define __STEER_H

#include "remote.h"

#include "stm32f10x.h"

/*                        Defination of GPIO                                  */

/*                        END OF GPIO Defination                              */

/*                        Defination of TIM                                   */
#define STEER_TIM TIM5
#define STEER_TIM_APBxClkCmd RCC_APB1PeriphClockCmd
#define STEER_TIM_CLK RCC_APB1Periph_TIM5
#define STEER_TIM_IRQ TIM5_IRQn
#define STEER_TIM_IRQHandler TIM5_IRQHandler
/*                        END OF TIM Defination                               */
//          PWM frequency: Same as motor

void Steer_OnCommandLine(Remote_DataStructure *data);

void Steer_TIM_Init(void);

void Steer_Init(void);

void Steer_TIM_IRQ_Init(void);

void Steer_PWM_SetDutyCycle(u8 DutyCycle);

#endif
