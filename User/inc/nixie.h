#include "stm32f10x.h"

#ifndef __NIXIE_H
#define __NIXIE_H

#define NIXIE_TIM TIM6
#define NIXIE_TIM_APBxClkCmd RCC_APB1PeriphClockCmd
#define NIXIE_TIM_CLK RCC_APB1Periph_TIM6
#define NIXIE_TIM_IRQ TIM6_IRQn
#define NIXIE_TIM_IRQHandler TIM6_IRQHandler

#define NIXIE_GPIO_CLK RCC_APB2Periph_GPIOC
#define NIXIE_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd
#define NIXIE_GPIO_PORT GPIOC
#define NIXIE_GPIO_PIN_0 GPIO_Pin_0
#define NIXIE_GPIO_PIN_1 GPIO_Pin_1
#define NIXIE_GPIO_PIN_2 GPIO_Pin_2
#define NIXIE_GPIO_PIN_3 GPIO_Pin_3
#define NIXIE_GPIO_PIN_4 GPIO_Pin_4
#define NIXIE_GPIO_PIN_5 GPIO_Pin_5
#define NIXIE_GPIO_PIN_6 GPIO_Pin_6
#define NIXIE_GPIO_PIN_7 GPIO_Pin_7
#define NIXIE_GPIO_PIN_8 GPIO_Pin_8
#define NIXIE_GPIO_PIN_9 GPIO_Pin_9
#define NIXIE_GPIO_PIN_10 GPIO_Pin_10
#define NIXIE_GPIO_PIN_11 GPIO_Pin_11

#define NIXIE_GPIO_PINS                                       \
    (NIXIE_GPIO_PIN_0 | NIXIE_GPIO_PIN_1 | NIXIE_GPIO_PIN_2 | \
     NIXIE_GPIO_PIN_3 | NIXIE_GPIO_PIN_4 | NIXIE_GPIO_PIN_5 | \
     NIXIE_GPIO_PIN_6 | NIXIE_GPIO_PIN_7 | NIXIE_GPIO_PIN_8 | \
     NIXIE_GPIO_PIN_9 | NIXIE_GPIO_PIN_10 | NIXIE_GPIO_PIN_11)

void Nixie_Init(void);

void Nixie_IRQ_Init(void);

void Nixie_GPIO_Init(void);

void Nixie_TIM_Init(void);

void Nixie_SetNumber(u16 display);

void Nixie_SetPoint(u8 ordinal);

void Nixie_Display(u8 ordinal);

#endif
