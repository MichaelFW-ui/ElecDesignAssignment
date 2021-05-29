//#include "stm32f10x.h"
#include "delay.h"
#include "nixie.h"
#include "debug.h"
#include "stdio.h"

int cycle = 0;

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    delay_init();
    Nixie_Init();
    Nixie_SetNumber(8888);
    Nixie_SetPoint(0x0f);
    Debug_USART_Init();

    while (1) {
        delay_s(500);        //延时500ms
        cycle++;
        Nixie_SetNumber(cycle);
        printf("%d\r\n", cycle);
        if (cycle == 1000) {
            cycle = 0;
        }
    }
}
