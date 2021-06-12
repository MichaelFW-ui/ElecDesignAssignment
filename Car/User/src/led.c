#include "stm32f10x.h"

// LED端口初始化函数
// LED1连接PC14端口，LED2连接PC15端口
//初始化PC14、PC15两个IO口的状态
void led_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    //使能PC端口时钟

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_14 | GPIO_Pin_15;        // 选择LED端口 LED1-->PC14 LED2-->PC15
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
    GPIO_InitStructure.GPIO_Speed =
        GPIO_Speed_50MHz;        // IO口输出速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //根据结构体(GPIO_InitStructure)设定参数初始化PC14、PC15端口
}

//点亮LED1
void LED1_ON(void) {
    GPIO_SetBits(GPIOC, GPIO_Pin_14);        //将PC14端口置高电平
}

//熄灭LED1
void LED1_OFF(void) {
    GPIO_ResetBits(GPIOC, GPIO_Pin_14);        //将PC14置低电平
}

//点亮LED2
void LED2_ON(void) {
    GPIO_SetBits(GPIOC, GPIO_Pin_15);        //将PC15置高电平
}

//熄灭LED2
void LED2_OFF(void) {
    GPIO_ResetBits(GPIOC, GPIO_Pin_15);        //将PC15置低电平
}
