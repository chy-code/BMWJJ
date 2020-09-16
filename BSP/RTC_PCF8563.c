
#include "cmsis_os2.h"
#include "stm32f10x.h"
#include "I2C_Soft.h"
#include "RTC_PCF8563.h"


// PCF8563 硬件配置

#define PCF8563_SCL_PORT	GPIOA
#define PCF8563_SCL_PIN		GPIO_Pin_0

#define PCF8563_SDA_PORT	GPIOA
#define PCF8563_SDA_PIN		GPIO_Pin_1


static I2C_S_TypeDef I2C_PCF8563 =
{
    .SDA_Port = PCF8563_SDA_PORT,
    .SDA_Pin = PCF8563_SDA_PIN,
    .SCL_Port = PCF8563_SCL_PORT,
    .SCL_Pin = PCF8563_SCL_PIN
};



static __forceinline  uint8_t BCDToHex(uint8_t b)
{
    return (b >> 4) * 10 + (b & 0xf);
}


static __forceinline  uint8_t HexToBCD(uint8_t b)
{
    return ((b / 10) << 4) + (b % 10);
}


void BSP_RTC_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    I2C_S_Init(&I2C_PCF8563);
}


bool BSP_RTC_SetTime(const RTC_Time *time)
{
    bool result = false;

    /* Send a START condition */
    I2C_S_Start(&I2C_PCF8563);

    do
    {
        /* Send the slave address for read (A3h) */
        if (! I2C_S_SendData(&I2C_PCF8563, 0xA2))
            break;

        /* Set the address pointer to 2 (VL_seconds) by sending 02h */
        if (! I2C_S_SendData(&I2C_PCF8563, 0x02))
            break;

        /* Write VL_seconds */
        if (! I2C_S_SendData(&I2C_PCF8563, HexToBCD(time->Second)))
            break;

        /* Write Minutes */
        if (! I2C_S_SendData(&I2C_PCF8563, HexToBCD(time->Minute)))
            break;

        /* Write Hours */
        if (! I2C_S_SendData(&I2C_PCF8563, HexToBCD(time->Hour)))
            break;

        /* Write Days */
        if (! I2C_S_SendData(&I2C_PCF8563, HexToBCD(time->MDay)))
            break;

        /* Write Weekdays */
        if (! I2C_S_SendData(&I2C_PCF8563, HexToBCD(time->Weekday)))
            break;

        /* Write Century_months */
        if (! I2C_S_SendData(&I2C_PCF8563, HexToBCD(time->Month)))
            break;

        /* Write Years */
        if (! I2C_S_SendData(&I2C_PCF8563, HexToBCD(time->Year - 2000)))
            break;

        result = true;
    } while (0);

    /* Send a STOP condition */
    I2C_S_Stop(&I2C_PCF8563);

    return result;
}


bool BSP_RTC_GetTime(RTC_Time *time)
{
    uint8_t b;
    bool result = false;

    /* Send a START condition */
    I2C_S_Start(&I2C_PCF8563);

    do
    {
        /* Send the slave address for write (A2h) */
        if (! I2C_S_SendData(&I2C_PCF8563, 0xA2))
            break;

        /* Set the address pointer to 2 (VL_seconds) by sending 02h */
        if (! I2C_S_SendData(&I2C_PCF8563, 0x02))
            break;

        /* Send a RESTART condition */
        I2C_S_Start(&I2C_PCF8563);

        /* Send the slave address for read (A3h) */
        if (! I2C_S_SendData(&I2C_PCF8563, 0xA3))
            break;

        /* Read VL_seconds */
        b = I2C_S_ReceiveData(&I2C_PCF8563, true);
        time->Second = BCDToHex(b & 0x7f);

        /* Read Minutes */
        b = I2C_S_ReceiveData(&I2C_PCF8563, true);
        time->Minute = BCDToHex(b & 0x7f);

        /* Read Hours */
        b = I2C_S_ReceiveData(&I2C_PCF8563, true);
        time->Hour = BCDToHex(b & 0x3f);

        /* Read Days */
        b = I2C_S_ReceiveData(&I2C_PCF8563, true);
        time->MDay = BCDToHex(b & 0x3f);

        /* Read Weekdays */
        b = I2C_S_ReceiveData(&I2C_PCF8563, true);
        time->Weekday = BCDToHex(b & 0x07);

        /* Read Century_months */
        b = I2C_S_ReceiveData(&I2C_PCF8563, true);
        time->Month = BCDToHex(b & 0x1f);

        /* Read Years */
        b = I2C_S_ReceiveData(&I2C_PCF8563, false);
        time->Year = BCDToHex(b) + 2000;

        result = true;
    } while (0);

    /* Send a STOP condition */
    I2C_S_Stop(&I2C_PCF8563);

    return result;
}

