//#include "stm32f10x.h"
#include "delay.h"
#include "nixie.h"
#include "debug.h"
#include "stdio.h"
#include "remote.h"
#include "motor.h"
#include "steer.h"

int cycle = 0;

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    delay_init();
    Nixie_Init();
    Debug_USART_Init();
    Remote_SPI_Init();
    Remote_NRF_Init();
    Motor_Init();
    Steer_Init();

    Nixie_SetNumber(8888);
    Nixie_SetPoint(0x0f);
    delay_s(1000);
    Nixie_SetPoint(0);

    while (1) {}

    while (1) {
        cycle++;
        Nixie_SetNumber(cycle);
        printf("Cnt:%d\r\n", cycle);
        if (cycle == 1000) {
            cycle = 0;
        }
        delay_s(1000);
    }
}
