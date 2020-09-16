
#include "cmsis_os2.h"
#include "stm32f10x.h"
#include "Buzzer.h"


#define BUZ_PORT	GPIOE
#define BUZ_PIN		GPIO_Pin_0


void BSP_Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = BUZ_PIN;
    GPIO_Init(BUZ_PORT, &GPIO_InitStruct);

    BSP_Buzzer_Off();
}


void BSP_Buzzer_On(void)
{
    GPIO_SetBits(BUZ_PORT, BUZ_PIN);
}


void BSP_Buzzer_Off(void)
{
    GPIO_ResetBits(BUZ_PORT, BUZ_PIN);
}
