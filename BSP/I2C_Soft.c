#include "I2C_Soft.h"


static __forceinline
void Set_SCL_High(I2C_S_TypeDef *I2C)
{
    GPIO_SetBits(I2C->SCL_Port, I2C->SCL_Pin);
}

static __forceinline
void Set_SCL_Low(I2C_S_TypeDef *I2C)
{
    GPIO_ResetBits(I2C->SCL_Port, I2C->SCL_Pin);
}

static __forceinline
void Wait_SCL_High(I2C_S_TypeDef *I2C)
{
    while (! GPIO_ReadInputDataBit(I2C->SCL_Port, I2C->SCL_Pin));
}


static __forceinline
void Set_SDA_High(I2C_S_TypeDef *I2C)
{
    GPIO_SetBits(I2C->SDA_Port, I2C->SDA_Pin);
}

static __forceinline
void Set_SDA_Low(I2C_S_TypeDef *I2C)
{
    GPIO_ResetBits(I2C->SDA_Port, I2C->SDA_Pin);
}


static __forceinline
void Set_SDA_In(I2C_S_TypeDef *I2C)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = I2C->SDA_Pin;
    GPIO_Init(I2C->SDA_Port, &GPIO_InitStruct);
}


static __forceinline
void Set_SDA_Out(I2C_S_TypeDef *I2C)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = I2C->SDA_Pin;
    GPIO_Init(I2C->SDA_Port, &GPIO_InitStruct);;
}


static void Set_SCL_In(I2C_S_TypeDef *I2C)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = I2C->SCL_Pin;
    GPIO_Init(I2C->SCL_Port, &GPIO_InitStruct);
}


static void Set_SCL_Out(I2C_S_TypeDef *I2C)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = I2C->SCL_Pin;
    GPIO_Init(I2C->SCL_Port, &GPIO_InitStruct);;
}


static __forceinline uint8_t Read_SDA(I2C_S_TypeDef *I2C)
{
    return GPIO_ReadInputDataBit(I2C->SDA_Port, I2C->SDA_Pin);
}


static __forceinline void Delay(void)
{
    for (int i = 0; i < 300; i++);
}


void I2C_S_Init(I2C_S_TypeDef *I2C)
{
    Set_SCL_Out(I2C);
    Set_SDA_Out(I2C);

    Set_SCL_High(I2C);
    Set_SDA_High(I2C);
}


// 产生起始条件
void I2C_S_Start(I2C_S_TypeDef *I2C)
{
    Set_SDA_High(I2C);
    Set_SCL_High(I2C);
    Delay();

    Set_SCL_In(I2C);
    Wait_SCL_High(I2C);
    Set_SCL_Out(I2C);

    Set_SDA_Low(I2C);
    Delay();

    Set_SCL_Low(I2C);
    Delay();
}


// 产生停止条件
void I2C_S_Stop(I2C_S_TypeDef *I2C)
{
    Set_SDA_Low(I2C);
    Delay();

    Set_SCL_High(I2C);
    Set_SCL_In(I2C);
    Wait_SCL_High(I2C);
    Delay();

    Set_SDA_High(I2C);
    Delay();
}


bool I2C_S_SendData(I2C_S_TypeDef *I2C, uint8_t Data)
{
    int cnt = 0;
    bool ret = true;

    for (int i = 0; i < 8; i++)
    {
        Set_SCL_Low(I2C);
        Delay();

        if (Data & 0x80)
            Set_SDA_High(I2C);
        else
            Set_SDA_Low(I2C);
        Data <<= 1;

        Set_SCL_High(I2C);
        Delay();

        Set_SCL_In(I2C);
        Wait_SCL_High(I2C);
        Set_SCL_Out(I2C);
    }

    //----- 读应答 -----//
    Set_SCL_Low(I2C);
    Delay();
    Set_SDA_High(I2C); // 释放 SDA 线 (开漏模式有效)
    Delay();
    Set_SCL_High(I2C);
    Delay();

    Set_SDA_In(I2C);	// 设置 SDA 输入模式
    while (Read_SDA(I2C))
    {
        if (++cnt >= 200)
        {
            ret = false;
            break;
        }
    }

    Set_SDA_Out(I2C);	// 重设 SDA 为输出模式
    Set_SCL_Low(I2C);
    Delay();

    return ret;
}


uint8_t I2C_S_ReceiveData(I2C_S_TypeDef *I2C, bool continuous)
{
    uint8_t Data = 0;

    Set_SCL_Low(I2C);
    Delay();

    Set_SDA_High(I2C);	// 释放 SDA 线 (开漏模式有效)
    Set_SDA_In(I2C);

    for (int i = 0; i < 8; i++)
    {
        Set_SCL_High(I2C);
        Delay();

        Data <<= 1;
        Data += Read_SDA(I2C);

        Set_SCL_Low(I2C);
        Delay();
    }

    Set_SDA_Out(I2C);

    if (continuous)
        Set_SDA_Low(I2C);
    else
        Set_SDA_High(I2C);
    Delay();

    Set_SCL_High(I2C);
    Delay();
    Set_SCL_Low(I2C);
    Delay();

    return Data;
}
