/**
 * @file motor.c
 * @author Michael Francis Williams (GitHub:Michael-ui)
 * @brief Motor Module
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "motor.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "nrf24l01.h"



u8 static volatile MotorCounter = 0;
u8 static volatile MotorDirection = 0;
u8 static volatile MotorDutyCycle = 0;
u8 static volatile MotorChangingDir = 10;
// 0 for A and 1 for B
s16 static volatile LastSpeed = 0;

#define MOTOR_CHANGING_WAIT 50
// wait for about 1 second

// #define __DEBUG_PWM

/**
 * @brief Operate the accelerate of motor depending on the command
 *  Operation time on a 10ms basis.
 * @param data 
 */
void Motor_OnCommandLine(Remote_DataStructure *data) {
    s16 speed = data->Acceleration;

    speed = -speed;
                // if Currently changing direction,
                // to prevent the motor from being damaged,
                // you have to wait for some time.
                // Actually how long it should be is unknown.
    // printf("%d %d %d\r\n", speed, LastSpeed, MotorChangingDir);
    if (speed > -3000 && speed < 3000) {
        speed = 0;
    }
    if (((speed > 0) ? 1 : 0) ^ ((LastSpeed > 0) ? 1 : 0)) {
        MotorChangingDir = MOTOR_CHANGING_WAIT;
        Motor_PWM_SetDutyCycle(0);
        LastSpeed = speed;
        return;
    } else {
        LastSpeed = speed;
    }
    if (MotorChangingDir) {
        --MotorChangingDir;
        printf("changing\r\n");
        return;
    }
    MotorDirection = ((speed > 0) ? 1 : 0);
    speed /= MotorDirection ? 300 : -300;
    // No more. It was unwise to transport so many digits
    speed /= 3;
    // printf("Motor %d %d\r\n", MotorDirection, speed);

#ifdef __DEBUG_PWM

    printf("Motor %d %d\r\n", MotorDirection, speed);

#else

    Motor_PWM_SetDutyCycle((u8)speed);

#endif // __DEBUG_PWM
}


void Motor_Init() {
    Motor_TIM_Init();
    Motor_TIM_IRQ_Init();

    MOTOR_PWM_A_APBxClk_Fn(MOTOR_PWM_A_CLK, ENABLE);
    MOTOR_PWM_B_APBxClk_Fn(MOTOR_PWM_B_CLK, ENABLE);
    MOTOR_PWM_EN_APBxClk_Fn(MOTOR_PWM_EN_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // Pin A
    GPIO_InitStructure.GPIO_Pin   = MOTOR_PWM_A_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(MOTOR_PWM_A_PORT, &GPIO_InitStructure);

    // Pin B
    GPIO_InitStructure.GPIO_Pin = MOTOR_PWM_B_PIN;
    GPIO_Init(MOTOR_PWM_B_PORT, &GPIO_InitStructure);

    // Pin EN
    GPIO_InitStructure.GPIO_Pin = MOTOR_PWM_EN_PIN;
    GPIO_Init(MOTOR_PWM_EN_PORT, &GPIO_InitStructure);
}

// 100Hz
// TIM is doubled.
void Motor_TIM_Init() {
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;

   MOTOR_TIM_APBxClkCmd(MOTOR_TIM_CLK, ENABLE);
   TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
   TIM_TimeBaseInitStructure.TIM_Period    = 100 - 1;
   TIM_TimeBaseInit(MOTOR_TIM, &TIM_TimeBaseInitStructure);

   TIM_ITConfig(MOTOR_TIM, TIM_IT_Update, ENABLE);
   TIM_Cmd(MOTOR_TIM, ENABLE);
}

void Motor_TIM_IRQ_Init() {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                   = MOTOR_TIM_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void MOTOR_TIM_IRQHandler() {
    if (TIM_GetITStatus(MOTOR_TIM, TIM_IT_Update) != RESET) {
        if (MotorCounter++ < MotorDutyCycle) {
            GPIO_ResetBits(MOTOR_PWM_EN_PORT, MOTOR_PWM_EN_PIN);
            if (MotorDirection) {
                GPIO_SetBits(MOTOR_PWM_A_PORT, MOTOR_PWM_A_PIN);
                GPIO_ResetBits(MOTOR_PWM_B_PORT, MOTOR_PWM_B_PIN);
            } else {
                GPIO_ResetBits(MOTOR_PWM_A_PORT, MOTOR_PWM_A_PIN);
                GPIO_SetBits(MOTOR_PWM_B_PORT, MOTOR_PWM_B_PIN);
            }
        } else {
            GPIO_ResetBits(MOTOR_PWM_A_PORT, MOTOR_PWM_A_PIN);
            GPIO_ResetBits(MOTOR_PWM_B_PORT, MOTOR_PWM_B_PIN);
            GPIO_SetBits(MOTOR_PWM_EN_PORT, MOTOR_PWM_EN_PIN);
        }
        MotorCounter %= 100;
        TIM_ClearITPendingBit(MOTOR_TIM, TIM_IT_Update);
    }
}

void Motor_PWM_SetDutyCycle(u8 DutyCycle) {
    MotorDutyCycle = DutyCycle;
}
