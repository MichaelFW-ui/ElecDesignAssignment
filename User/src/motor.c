#include "motor.h"
#include "stm32f10x.h"
#include "remote.h"

void Motor_OnCommandLine(Remote_DataStructure *data) {
    /*
    **
    **
    **
    **              TODO: Completion
    **
    **
    **
    */
}


void Motor_Init() {
    Motor_TIM_Init();
    Motor_TIM_IRQ_Init();

    MOTOR_PWM_A_APBxClk_Fn(MOTOR_PWM_A_CLK, ENABLE);
    MOTOR_PWM_B_APBxClk_Fn(MOTOR_PWM_B_CLK, ENABLE);
    MOTOR_PWM_EN_APBxClk_Fn(MOTOR_PWM_EN_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // Pin A
    GPIO_InitStructure.GPIO_Pin = MOTOR_PWM_A_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MOTOR_PWM_A_PORT, &GPIO_InitStructure);

    // Pin B
    GPIO_InitStructure.GPIO_Pin = MOTOR_PWM_B_PIN;
    GPIO_Init(MOTOR_PWM_B_PORT, &GPIO_InitStructure);

    // Pin EN
    GPIO_InitStructure.GPIO_Pin = MOTOR_PWM_EN_PIN;
    GPIO_Init(MOTOR_PWM_EN_PORT, &GPIO_InitStructure);
}

// 100Hz / 2 = 50Hz, because APB1 is 36MHz
void Motor_TIM_Init() {
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;

   MOTOR_TIM_APBxClkCmd(MOTOR_TIM_CLK, ENABLE);
   TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
   TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;
   TIM_TimeBaseInit(MOTOR_TIM, &TIM_TimeBaseInitStructure);

   TIM_ITConfig(MOTOR_TIM, TIM_IT_Update, ENABLE);
   TIM_Cmd(MOTOR_TIM, ENABLE);
}

void Motor_TIM_IRQ_Init() {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = MOTOR_TIM_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Motor_TIM_IRQHandler() {
    if (TIM_GetITStatus(MOTOR_TIM, TIM_IT_Update) != RESET) {
        /*
        **
        **
        **
        **      TODO: Completion
        **
        **
        */
        TIM_ClearITPendingBit(MOTOR_TIM, TIM_IT_Update);
    }
}
