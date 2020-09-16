
#include "cmsis_os2.h"
#include "stm32f10x.h"
#include "I2C_Soft.h"
#include "Keypad.h"
#include "DbgPrint.h"


// 硬件配置

#define TTY6952_SDA_PORT	GPIOD
#define TTY6952_SDA_PIN		GPIO_Pin_15

#define TTY6952_SCL_PORT	GPIOD
#define TTY6952_SCL_PIN		GPIO_Pin_14

#define TTY6952_INT_PORT	GPIOD
#define TTY6952_INT_PIN		GPIO_Pin_13

#define TTY6952_INT_PORT_SOURCE	GPIO_PortSourceGPIOD
#define TTY6952_INT_PIN_SOURCE	GPIO_PinSource13

#define TTY6952_INT_EXTI_LINE		EXTI_Line13

#define LIGHT_EN_PORT		GPIOC
#define LIGHT_EN_PIN		GPIO_Pin_6

#define TTY6952_ADDR_W		0xA6
#define TTY6952_ADDR_R		0xA7


static I2C_S_TypeDef I2C_TTY6952 =
{
    .SDA_Port = TTY6952_SDA_PORT,
    .SDA_Pin = TTY6952_SDA_PIN,
    .SCL_Port = TTY6952_SCL_PORT,
    .SCL_Pin = TTY6952_SCL_PIN
};


#define KEY(code, value) { code, value },
static struct
{
    KeyCode code;
    uint16_t value;
} _cKeys[NUM_KEYS] =
{
#include "keys.h"
};


#define KB_SZ_MAX	128
#define KB_SZ_MOD	(KB_SZ_MAX - 1)

static uint32_t _keyBuf[KB_SZ_MAX]; // 按键缓冲区
static uint8_t _bpos = 0;	// 向缓冲区缓存按键值的位置
static uint8_t _readPos = 0; // 从缓冲中读取按键的位置


static bool TTY6952_Setup(void);

static bool TTY6952_Write(uint8_t b1, uint8_t b2, uint8_t b3);
static bool TTY6952_Read(uint8_t *b1, uint8_t *b2, uint8_t *b3);


/*-------------------------------------------------------
* 初始化键盘.
* 参数:
*	无
* 返回值:
*	无
*--------------------------------------------------------*/
void BSP_Keypad_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC
                           | RCC_APB2Periph_GPIOD
                           | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = LIGHT_EN_PIN;
    GPIO_Init(LIGHT_EN_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = TTY6952_INT_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(TTY6952_INT_PORT, &GPIO_InitStruct);

    GPIO_EXTILineConfig(TTY6952_INT_PORT_SOURCE, TTY6952_INT_PIN_SOURCE);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_EnableIRQ(EXTI15_10_IRQn);

    I2C_S_Init(&I2C_TTY6952);
    TTY6952_Setup();

    BSP_Keypad_OpenBacklight();
}


/*-------------------------------------------------------
* 打开键盘背光灯.
* 参数:
*	无
* 返回值:
*	无
*--------------------------------------------------------*/

void BSP_Keypad_OpenBacklight(void)
{
    GPIO_SetBits(LIGHT_EN_PORT, LIGHT_EN_PIN);
}


/*-------------------------------------------------------
* 关闭键盘背光灯.
* 参数:
*	无
* 返回值:
*	无
*--------------------------------------------------------*/

void BSP_Keypad_CloseBacklight(void)
{
    GPIO_ResetBits(LIGHT_EN_PORT, LIGHT_EN_PIN);
}


/*-------------------------------------------------------
* 读取用户输入的按键.
* 参数:
*	[out] key	用户输入的按键.
* 返回值:
*	true	有按键输入
*	false	无按键输入
*--------------------------------------------------------*/

KpdStatus BSP_Keypad_ScanKey(KeyCode *keyCode)
{
    uint8_t cnt = (_bpos - _readPos) & KB_SZ_MOD;
    if (cnt == 0)
        return kpdNoEvent;

    uint16_t value = _keyBuf[_readPos++];
    _readPos &= KB_SZ_MOD;

    if (value == 0)
        return kpdKeyUp;

    for (int i = 0; i < NUM_KEYS; i++)
    {
        if (_cKeys[i].value == value)
        {
            *keyCode = _cKeys[i].code;
            return kpdKeyDown;
        }
    }

    return kpdNoEvent;
}



/*-------------------------------------------------------
* 配置 TTY6952.
* 参数:
*	无
* 返回值:
*	无
*--------------------------------------------------------*/

bool TTY6952_Setup(void)
{
    //----- Write Settings -----//
    if (!TTY6952_Write(0xB3, 0xF8, 0))
        return false;

    //----- 设置按键灵敏度 -----//
    for (uint8_t i = 0; i < NUM_KEYS; i++)
    {
        if (!TTY6952_Write(0xC0 + i, 0x10, 0))
            return false;
    }

    return true;
}


bool TTY6952_Write(uint8_t b1, uint8_t b2, uint8_t b3)
{
    bool ret = false;

    I2C_S_Start(&I2C_TTY6952);

    ret = I2C_S_SendData(&I2C_TTY6952, TTY6952_ADDR_W);
    if (!ret) goto END;

    ret = I2C_S_SendData(&I2C_TTY6952, b1);
    if (!ret) goto END;

    ret = I2C_S_SendData(&I2C_TTY6952, b2);
    if (!ret) goto END;

    ret = I2C_S_SendData(&I2C_TTY6952, b3);
    if (!ret) goto END;

END:
    I2C_S_Stop(&I2C_TTY6952);

    return ret;
}


bool TTY6952_Read(uint8_t *b1, uint8_t *b2, uint8_t *b3)
{
    bool ret = false;

    I2C_S_Start(&I2C_TTY6952);

    ret = I2C_S_SendData(&I2C_TTY6952, TTY6952_ADDR_R);
    if (!ret) goto END;

    *b1 = I2C_S_ReceiveData(&I2C_TTY6952, true);
    *b2 = I2C_S_ReceiveData(&I2C_TTY6952, true);
    *b3 = I2C_S_ReceiveData(&I2C_TTY6952, false);

END:
    I2C_S_Stop(&I2C_TTY6952);

    return ret;
}


void EXTI15_10_IRQHandler(void)
{
    uint8_t bb[3];
    uint32_t value;
    ITStatus stat;

    stat = EXTI_GetITStatus(TTY6952_INT_EXTI_LINE);
    if (stat != RESET)
    {
        if (TTY6952_Read(&bb[0], &bb[1], &bb[2]))
        {
            value = (bb[0] << 16) + (bb[1] << 8) + bb[2];
            _keyBuf[_bpos++] = value;
            _bpos &= KB_SZ_MOD;
        }

        EXTI_ClearITPendingBit(TTY6952_INT_EXTI_LINE);
    }
}

