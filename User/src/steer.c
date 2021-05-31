#include "steer.h"
#include "stm32f10x.h"

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

void Steer_TIM_Init() {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    STEER_TIM_APBxClkCmd(STEER_TIM_CLK, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(STEER_TIM, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(STEER_TIM, TIM_IT_Update, ENABLE);
    TIM_Cmd(STEER_TIM, ENABLE);
}

// 100Hz
void Steer_TIM_IRQ_Init() {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = STEER_TIM_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

u8 static volatile SteerCounter = 0;
u16 static volatile SteerPosition = 0;
u8 static volatile SteerDirection = 0;
// 0 for A and 1 for B

// PID
// read position to pretend it from broken
// direction, careful
void Steer_TIM_IRQHandler() {
    if (TIM_GetITStatus(STEER_TIM, TIM_IT_Update) != RESET) {
        /*
        **
        **
        **
        ** TODO:Comletion
        **
        **
        **
        */
        TIM_ClearITPendingBit(STEER_TIM, TIM_IT_Update);
    }
}


