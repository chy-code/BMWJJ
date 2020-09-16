
#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"
#include "rl_usbd.h"
#include "../RTE/USB/USBD_Config_HID_0.h"


#define RXB_SZ_MAX	1024
#define RXB_SZ_MOD	(RXB_SZ_MAX - 1)


static uint8_t _RXBuf[RXB_SZ_MAX];
static uint16_t _RXPos = 0;
static uint16_t _readPos = 0;

static uint8_t _TXBuf[USBD_HID0_OUT_REPORT_MAX_SZ];
static uint8_t _TXPos = 0;

static uint8_t _feat[USBD_HID0_FEAT_REPORT_MAX_SZ];


void BSP_HID0_Init(void)
{
    USBD_Initialize (0);       // USB Device 0 初始化
    USBD_Connect    (0);       // USB Device 0 连接
}


uint16_t BSP_HID0_Available(void)
{
    return (_RXPos - _readPos) & RXB_SZ_MOD;
}


void BSP_HID0_Read(uint8_t *buf, uint16_t bytesToRead)
{
    for (uint16_t i = 0; i < bytesToRead; i++)
    {
        buf[i] = _RXBuf[_readPos++];
        _readPos &= RXB_SZ_MOD;
    }
}


void BSP_HID0_Unread(uint16_t count)
{
    _readPos -= count;
    _readPos &= RXB_SZ_MOD;
}


/*------------------------------------------------------------------
* 发送数据到主机端. 数据大小没有限制. 如果数据大小
* 不是 USBD_HID0_FEAT_REPORT_MAX_SZ的整数倍, 需调用
* BSP_HID0_Flush 强制发送剩余的数据.
*------------------------------------------------------------------*/
void BSP_HID0_Write(uint8_t *data, uint16_t dataLen)
{
    uint8_t n;

    while (dataLen > 0)
    {
        n = USBD_HID0_OUT_REPORT_MAX_SZ - _TXPos;
        if (n > dataLen)
            n = dataLen;

        if (n > 0)
        {
            memcpy(&_TXBuf[_TXPos], data, n);
            _TXPos += n;

            data += n;
            dataLen -= n;
        }

        if (_TXPos >= USBD_HID0_OUT_REPORT_MAX_SZ)
        {
            USBD_HID_GetReportTrigger(0, 0,
                                      _TXBuf, USBD_HID0_OUT_REPORT_MAX_SZ);

            _TXPos = 0;
            osDelay(20);
        }
    }
}


/*------------------------------------------------------------------
* 发送缓冲区中数据 (如果有).
*------------------------------------------------------------------*/
void BSP_HID0_Flush(void)
{
    uint8_t n;

    if (_TXPos > 0)
    {
        n = USBD_HID0_OUT_REPORT_MAX_SZ - _TXPos;
        if (n > 0)
            memset(&_TXBuf[_TXPos], 0, n);

        USBD_HID_GetReportTrigger(0, 0,
                                  _TXBuf, USBD_HID0_OUT_REPORT_MAX_SZ);
        _TXPos = 0;

        osDelay(20);
    }
}



// 回调函数, 由USB库调用。

int32_t USBD_HID0_GetReport (
    uint8_t rtype,
    uint8_t req,
    uint8_t rid,
    uint8_t *buf)
{
    switch (rtype)
    {
    case HID_REPORT_FEATURE:
        memcpy(buf, _feat, USBD_HID0_FEAT_REPORT_MAX_SZ);
        return USBD_HID0_FEAT_REPORT_MAX_SZ;
    }

    return 0;
}


// 回调函数, 由USB库调用。

bool USBD_HID0_SetReport (
    uint8_t rtype,
    uint8_t req,
    uint8_t rid,
    const uint8_t *buf,
    int32_t len)
{
    switch (rtype)
    {
    case HID_REPORT_OUTPUT:
        // 把来自主机端的报告保存到接收缓冲区
        for (int32_t i = 0; i < len; i++)
        {
            _RXBuf[_RXPos++] = buf[i];
            _RXPos &= RXB_SZ_MOD;
        }
        break;

    case HID_REPORT_FEATURE:
        memcpy(_feat, buf, len);
        break;
    }

    return true;
}
