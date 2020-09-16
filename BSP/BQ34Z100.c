
#include "cmsis_os2.h"
#include "stm32f10x.h"
#include "I2C_Soft.h"


#define BQ34Z100_ADDR_WR	0xAA
#define BQ34Z100_ADDR_RD	0xAB


static I2C_S_TypeDef I2C_BQ34Z100 =
{
    .SDA_Port = GPIOE,
    .SDA_Pin = GPIO_Pin_14,
    .SCL_Port = GPIOE,
    .SCL_Pin = GPIO_Pin_13
};


// BQ34Z100 命令

static bool BQ34Z100_Read(uint8_t cmd, uint8_t *val);


/*-------------------------------------------------------
* 初始化电源管理.
* 参数:
*	无
* 返回值:
*	无
*--------------------------------------------------------*/

void BSP_BQ34Z100_Init(void)
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);
    I2C_S_Init(&I2C_BQ34Z100);
}


/*-------------------------------------------------------
* 获取锂电池的剩余电量百分比.
* 参数:
*	[out] percent	电量百分比 (0~100)
* 返回值:
*	true	成功
*	false	失败
*--------------------------------------------------------*/
bool BSP_BQ34Z100_RemainingCapacityPercent(uint8_t *percent)
{
    return BQ34Z100_Read(0x02, percent);
}


bool BQ34Z100_Read(uint8_t cmd, uint8_t *val)
{
    bool result = false;

    I2C_S_Start(&I2C_BQ34Z100);

    result = I2C_S_SendData(&I2C_BQ34Z100, BQ34Z100_ADDR_WR);
    if (!result) goto END;

    result = I2C_S_SendData(&I2C_BQ34Z100, cmd);
    if (!result) goto END;

    I2C_S_Start(&I2C_BQ34Z100);

    result = I2C_S_SendData(&I2C_BQ34Z100, BQ34Z100_ADDR_RD);
    if (!result) goto END;

    *val = I2C_S_ReceiveData(&I2C_BQ34Z100, false);

END:
    I2C_S_Stop(&I2C_BQ34Z100);

    return result;
}

