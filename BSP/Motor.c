
#include "stm32f10x.h"
#include "Motor.h"

#define MX612E_IN_PORT	GPIOB

#define MX612E_INB_PIN	GPIO_Pin_15
#define MX612E_INA_PIN	GPIO_Pin_14


void BSP_Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MX612E_INB_PIN | MX612E_INA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MX612E_IN_PORT, &GPIO_InitStructure);

    //----- 待机 -----//
    GPIO_ResetBits(MX612E_IN_PORT, MX612E_INA_PIN);
    GPIO_ResetBits(MX612E_IN_PORT, MX612E_INB_PIN);
}


/*------------------------------------------------------------------
* 启动电机.
*
* 参数:
*	[in ] CW	true 正转, false 反转
*------------------------------------------------------------------*/
void BSP_Motor_Start(bool CW)
{
    if (CW)
    {
        GPIO_SetBits(MX612E_IN_PORT, MX612E_INA_PIN);
        GPIO_ResetBits(MX612E_IN_PORT, MX612E_INB_PIN);
    }
    else
    {
        GPIO_ResetBits(MX612E_IN_PORT, MX612E_INA_PIN);
        GPIO_SetBits(MX612E_IN_PORT, MX612E_INB_PIN);
    }
}


/*------------------------------------------------------------------
* 停止转动.
*
* 参数:
*	[in ] CW	true 正转, false 反转
*------------------------------------------------------------------*/
void BSP_Motor_Stop(void)
{
    //----- 刹车 -----//
    GPIO_SetBits(MX612E_IN_PORT, MX612E_INA_PIN);
    GPIO_SetBits(MX612E_IN_PORT, MX612E_INB_PIN);
}
