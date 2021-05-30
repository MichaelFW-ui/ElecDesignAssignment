#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

/*                    Defination of GPIO                                      */



/*                        END OF of GPIO Defination                           */

void Motor_OnCommandLine(Remote_DataStructure *data);

void Motor_TIM_Init(void);

void Motor_Init(void);

void Motor_PWM_SetDutyCycle(u8 DutyCycle);

#endif
