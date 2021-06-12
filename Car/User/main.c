//#include "stm32f10x.h"
#include "delay.h"
#include "nixie.h"
#include "debug.h"
#include "stdio.h"
#include "nrf24l01.h"
#include "motor.h"
#include "steer.h"

int cycle = 0;
int cnt = 0;
Remote_DataStructure RxData;
Remote_ACKDataStructure RxACKData;

extern u16 Steer_ADC_Converted;

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    delay_init();
    Nixie_Init();
    Debug_USART_Init();
    Motor_Init();
    Steer_Init();
    Steer_PID_Init();
    SPI_NRF_Init();

    Nixie_SetNumber(8888);
    Nixie_SetPoint(0x0f);
    delay_s(1000);
    Nixie_SetPoint(0);

    if (!NRF_Check()) {
        Nixie_SetNumber(1234);
        delay_s(5000);
    }

    while (1) {
        NRF_RX_Mode();
        NRF_Rx_Dat((uint8_t*)&RxData);
        Nixie_SetNumber(RxData.Status % 10000);
        Steer_OnCommandLine(&RxData);
        Motor_OnCommandLine(&RxData);
        // printf("input %d %d\r\n", RxData.Acceleration, RxData.Direction);
        // printf("%d\r\n", Steer_ADC_Converted);
        Steer_GetPosition();
        delay_ms(20);
    }
}
