
#include <stdio.h>
#include "cmsis_os2.h"
#include "USART_Util.h"
#include "FP_Reader.h"
#include "DbgPrint.h"


/*------------------------------------------------------------------
* 硬件配置
*------------------------------------------------------------------*/

#define FPR_USART			USART1

#define FPR_TOUCH_OUT_PORT	GPIOC
#define FPR_TOUCH_OUT_PIN	GPIO_Pin_9

#define FPR_VCC_PORT		GPIOD
#define FPR_VCC_PIN			GPIO_Pin_8


/*------------------------------------------------------------------
* 私有变量定义
*------------------------------------------------------------------*/

#define IOB_SIZE	1024 	// I/O 缓冲区大小

static uint8_t _iobuf[IOB_SIZE];	 // I/O 缓冲区, 用于接收和发送数据
static osMutexId_t _mutexId = 0;


/*------------------------------------------------------------------
* 辅助函数声明
*------------------------------------------------------------------*/
static void FP_GPIO_Init(void);
static void FP_SendCommand(uint8_t CC, uint8_t P1, uint8_t P2, uint8_t P3);
static int FP_RecvResponse(bool moreThan8, uint32_t timeout);
static uint8_t FP_CalcBCC(uint8_t InitValue, uint8_t *Data, uint16_t DataLen);



/*------------------------------------------------------------------
* 初始化指纹阅读器.
*------------------------------------------------------------------*/

void BSP_FP_Init(void)
{
    MY_USART_Init(FPR_USART, 115200, USART_Parity_No);
    MY_USART_EnableDMA(FPR_USART);
    USART_Cmd(FPR_USART, ENABLE);

    FP_GPIO_Init();
    BSP_FP_Sleep();

    _mutexId = osMutexNew(NULL);
}


/*------------------------------------------------------------------
* 唤醒指纹阅读器. 在调用其它函数之前调用.
*------------------------------------------------------------------*/

void BSP_FP_WakeUp(void)
{
    osStatus_t stat = osMutexAcquire(_mutexId, 60000);
    if (stat != osOK)
        return;

    GPIO_SetBits(FPR_VCC_PORT, FPR_VCC_PIN);

    osDelay(500);
}


/*------------------------------------------------------------------
* 使指纹阅读器进入休眠状态 (节能).
*------------------------------------------------------------------*/

void BSP_FP_Sleep(void)
{
    GPIO_ResetBits(FPR_VCC_PORT, FPR_VCC_PIN);

    osMutexRelease(_mutexId);
}


/*------------------------------------------------------------------
* 获取指纹阅读器上是否有手指按压.
* 返回值
*		0: 成功, 其它值: 失败.
*------------------------------------------------------------------*/
int BSP_FP_GetFingerStatus(bool *isTouched)
{
    int ret;
    *isTouched = false;

    FP_SendCommand(0x30, 0x0, 0x0, 0x0);
    ret = FP_RecvResponse(false, 5000);
    if (ret != 0)
    {
        if (ret != 0x01)
            return ret;
    }
    else
    {
        *isTouched = true;
    }

    return 0;
}


/*------------------------------------------------------------------
* 删除指定的指纹.
* 返回值
*		0: 成功, 其它值: 失败.
*------------------------------------------------------------------*/
int BSP_FP_DeleteFP(uint16_t id)
{
    int ret;
    FP_SendCommand(0x04, (id >> 8), (id & 0xff), 0x0);
    ret = FP_RecvResponse(false, 10000);
    if (ret != 0)
    {
        if (ret != 0x5)
            return ret;
    }

    return 0;
}


/*------------------------------------------------------------------
* 删除指纹阅读器内已登记的所有指纹信息.
* 返回值
*		0: 成功, 其它值: 失败.
*------------------------------------------------------------------*/

int BSP_FP_DeleteAllFPs(void)
{
    int ret;
    FP_SendCommand(0x05, 0x0, 0x0, 0x0);
    ret = FP_RecvResponse(false, 10000);
    if (ret != 0)
        return ret;

    return 0;
}


/*------------------------------------------------------------------
* 使用3C3R方式注册指纹. 采集3次, 最后一次返回指纹ID.
* 参数
*	[in ] step	序号. 取值范围 1~3.
*	[out] id	指纹ID. 函数成功时有效.
* 返回值
*	0 表示成功
*	其它值失败.
*------------------------------------------------------------------*/

int BSP_FP_RegisterFPStep(int step, uint16_t *id)
{
    int ret;
    uint8_t role = 1;
    if (step > 1) role = 0;

    FP_SendCommand((uint8_t)(step), 0x0, 0x0, role);
    ret = FP_RecvResponse(false, 10000);
    if (ret != 0)
        return ret;

    if (step == 3)
        *id = (_iobuf[2] << 8) | _iobuf[3];

    return 0;
}


int BSP_FP_Match(uint16_t *id)
{
    int ret;
    FP_SendCommand(0x0C, 0x0, 0x0, 0x0);
    ret = FP_RecvResponse(false, 10000);
    if (ret < 0)
        return ret;

    *id = (_iobuf[2] << 8) | _iobuf[3];

    return 0;
}


const char* BSP_FP_StrError(int ret)
{
    static char msg[128];
    switch (ret)
    {
    case 0:
        sprintf(msg, "FP: 操作成功");
        break;
    case FP_E_RECEPTION:
        sprintf(msg, "FP: 接收到的数据不正确");
        break;
    case FP_E_NO_RESPONSE:
        sprintf(msg, "FP: 无应答");
        break;
    case 0x1:
        sprintf(msg, "FP: 命令失败");
        break;
    case 0x4:
        sprintf(msg, "FP: 数据库满");
        break;
    case 0x5:
        sprintf(msg, "FP: 用户不存在");
        break;
    case 0x7:
        sprintf(msg, "FP: 用户已存在");
        break;
    case 0x8:
        sprintf(msg, "FP: 图像采集超时");
        break;
    case 0xA:
        sprintf(msg, "FP: 硬件错误");
        break;
    case 0x10:
        sprintf(msg, "FP: 图像错误");
        break;
    case 0x18:
        sprintf(msg, "FP: 命令中止");
        break;
    case 0x11:
        sprintf(msg, "FP: 贴模攻击检测");
        break;
    case 0x12:
        sprintf(msg, "FP: 同源性校验错误");
        break;
    default:
        sprintf(msg, "FP: 未知错误 %02X", ret);
        break;
    }
    return msg;
}


void FP_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC
                           | RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = FPR_TOUCH_OUT_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FPR_TOUCH_OUT_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = FPR_VCC_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(FPR_VCC_PORT, &GPIO_InitStruct);

    GPIO_ResetBits(FPR_VCC_PORT, FPR_VCC_PIN); // 休眠
}


void FP_SendCommand(uint8_t CC, uint8_t P1, uint8_t P2, uint8_t P3)
{
    _iobuf[0] = 0xF5;
    _iobuf[1] = CC;
    _iobuf[2] = P1;
    _iobuf[3] = P2;
    _iobuf[4] = P3;
    _iobuf[5] = 0;
    _iobuf[6] = FP_CalcBCC(0, _iobuf + 1, 5);
    _iobuf[7] = 0xF5;

    MY_USART_Transmit(FPR_USART, _iobuf, 8);
}


int FP_RecvResponse(bool moreThan8, uint32_t timeout)
{
    uint32_t t0 = osKernelGetTickCount();

    //----- 读数据包头部 8 个字节 -----//

    MY_USART_Receive_DMA(FPR_USART, _iobuf, 8);
    while (! MY_USART_IsReceiveComplete(FPR_USART))
    {
        if (osKernelGetTickCount() - t0 > timeout)
            return FP_E_NO_RESPONSE;
    }

    //----- 检查应答状态 -----//

    uint8_t bcc = FP_CalcBCC(0, _iobuf + 1, 5);
    if (bcc != _iobuf[6])
        return FP_E_RECEPTION; // BCC 不正确

    if (_iobuf[4] != 0)
        return _iobuf[4];	// 命令失败, 返回状态码

    //----- 读取剩余的数据包(如果有) -----//

    uint16_t rlen = (_iobuf[2] << 8) + _iobuf[3];
    if (rlen > 0 && moreThan8)
    {
        MY_USART_Receive_DMA(FPR_USART, _iobuf, rlen + 6);

        while (! MY_USART_IsReceiveComplete(FPR_USART))
        {
            if (osKernelGetTickCount() - t0 > timeout)
                return FP_E_NO_RESPONSE;
        }
    }

    return 0;
}


uint8_t FP_CalcBCC(uint8_t InitValue, uint8_t *Data, uint16_t DataLen)
{
    uint8_t b = InitValue;
    for (uint16_t i = 0; i < DataLen; i++)
        b ^= Data[i];

    return b;
}

