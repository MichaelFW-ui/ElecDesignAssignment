#ifndef __STEER_H
#define __STEER_H

#include "stm32f10x.h"

/*                        Defination of GPIO                                  */





/*                            END OF of GPIO Defination                       */


void Steer_OnCommandLine(Remote_DataStructure *data);

void Steer_TIM_Init(void);

void Steer_PWM_SetDutyCycle(u8 DutyCycle);

void Steer_Init(void);

#endif
