//#include "stm32f10x.h"
#include "delay.h"
#include "nixie.h"

int cycle = 0;

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    delay_init();
    Nixie_Init();
    Nixie_SetNumber(1234);
    Nixie_SetPoint((1 << 3) | (1 << 1));

    while (1) {
        delay_s(500);        //延时500ms
        delay_s(500);        //延时500ms
        cycle++;
        Nixie_SetNumber(cycle);
        if (cycle == 1000) {
            cycle = 0;
        }
    }
}
