
#include "stm32f10x.h"
#include "Sensor.h"


#define SEN_PORT	GPIOB
#define SEN_PIN		GPIO_Pin_5

void BSP_Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStruct.GPIO_Pin = SEN_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SEN_PORT, &GPIO_InitStruct);
}


uint8_t BSP_Sensor_GetStatus(void)
{
    return GPIO_ReadInputDataBit(SEN_PORT, SEN_PIN);
}
