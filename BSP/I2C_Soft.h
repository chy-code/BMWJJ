/*******************************************************************
* 文件名: I2C_Soft.h
* 用途	: 定义软件I2C相关的函数
********************************************************************/

#ifndef _I2C_SOFT_H
#define _I2C_SOFT_H

#include <stdbool.h>
#include "stm32f10x.h"

// 软件 I2C 类型定义
typedef struct
{
    GPIO_TypeDef *SDA_Port;
    uint16_t SDA_Pin;
    GPIO_TypeDef *SCL_Port;
    uint16_t SCL_Pin;
} I2C_S_TypeDef;


void I2C_S_Init(I2C_S_TypeDef *I2C);

void I2C_S_Start(I2C_S_TypeDef *I2C);
void I2C_S_Stop(I2C_S_TypeDef *I2C);

bool I2C_S_SendData(I2C_S_TypeDef *I2C, uint8_t b);
uint8_t I2C_S_ReceiveData(I2C_S_TypeDef *I2C, bool continuous);

#endif
