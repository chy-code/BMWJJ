#include "stm32f10x.h"


#define LED_PORT	GPIOD
#define LED_PIN		GPIO_Pin_0


void BSP_LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = LED_PIN;
    GPIO_Init(LED_PORT, &GPIO_InitStruct);
}


void BSP_LED_On(void)
{
    GPIO_SetBits(LED_PORT, LED_PIN);
}


void BSP_LED_Off(void)
{
    GPIO_ResetBits(LED_PORT, LED_PIN);
}


void BSP_LED_ToggleState(void)
{
    LED_PORT->ODR ^= LED_PIN;
}
