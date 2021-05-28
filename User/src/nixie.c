#include "nixie.h"
#include "stm32f10x.h"

#define NIXIE_OVERFLOW 10000

u16 static Nixie_NumberPins[(10 + 1)] = {0};
u16 static volatile Current = 0;
u8 static volatile count = 0;
u8 static volatile Point = 0;
u16 static __NumberPins[] = {NIXIE_GPIO_PIN_0, NIXIE_GPIO_PIN_1,  NIXIE_GPIO_PIN_2,
                    NIXIE_GPIO_PIN_3, NIXIE_GPIO_PIN_4,  NIXIE_GPIO_PIN_5,
                    NIXIE_GPIO_PIN_6, NIXIE_GPIO_PIN_7,  NIXIE_GPIO_PIN_8,
                    NIXIE_GPIO_PIN_9, NIXIE_GPIO_PIN_10, NIXIE_GPIO_PIN_11};

/*
** Init the Pin list:
** 0 - 9 for ten numbers
** 10 for dash only
*/
void Nixie_Init() {
    Nixie_GPIO_Init();
    Nixie_TIM_Init();
    Nixie_IRQ_Init();
    u8 Digits[11][8] = {
        0, 0, 0, 0, 0, 0, 1, 0,
        1, 0, 0, 1, 1, 1, 1, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 1, 0, 0,
        1, 0, 0, 1, 1, 0, 0, 0,
        0, 1, 0, 0, 1, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 0, 0
    };
    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 8; ++j) {
            Nixie_NumberPins[i] |= (Digits[i][j]) ? __NumberPins[j] : 0;
        }
    }
}

void Nixie_GPIO_Init() {
    GPIO_InitTypeDef GPIO_InitStruture;
    NIXIE_GPIO_APBxClkCmd(NIXIE_GPIO_CLK, ENABLE);
    GPIO_InitStruture.GPIO_Pin = NIXIE_GPIO_PINS;
    GPIO_InitStruture.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(NIXIE_GPIO_PORT, &GPIO_InitStruture);
}
// 1ms a period
void Nixie_TIM_Init() {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    NIXIE_TIM_APBxClkCmd(NIXIE_TIM_CLK, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseInit(NIXIE_TIM, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(NIXIE_TIM, TIM_IT_Update, ENABLE);
    TIM_Cmd(NIXIE_TIM, ENABLE);
}

void Nixie_IRQ_Init() {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = NIXIE_TIM_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void NIXIE_TIM_IRQHandler() {
    if (TIM_GetITStatus(NIXIE_TIM, TIM_IT_Update) != RESET) {
        Nixie_Display(count = (count + 1) % 4);
        TIM_ClearITPendingBit(NIXIE_TIM, TIM_IT_Update);
    }
}


void Nixie_SetNumber(u16 display) {
    if (display >= NIXIE_OVERFLOW) Current = NIXIE_OVERFLOW;
    else Current = display;
}

// ordinal means which position of digit will display the dot
// For example, 0x0003 (0011b) will put the dots at 0 and 1th digit.
void Nixie_SetPoint(u8 ordinal) {
    Point = ordinal;
}

void Nixie_Display(u8 ordinal) {
    u16 digit = 0;
    u16 base = 1;
    if (ordinal > 4) return;
    if (Current == NIXIE_OVERFLOW)
        digit = 10;
    else {
        for (int i = 0; i < ordinal; ++i) base *= 10;
        digit = (Current / base) % 10;
    }
    u16 pins = Nixie_NumberPins[digit] |
               ((Point & (1 << ordinal)) ? 0 : __NumberPins[7]) |
               __NumberPins[8 + ordinal];
    GPIO_ResetBits(NIXIE_GPIO_PORT, NIXIE_GPIO_PINS);
    GPIO_SetBits(NIXIE_GPIO_PORT, pins);
}
