

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cmsis_os2.h"
#include "MPUtil.h"
#include "StringUtil.h"
#include "User_HID_0.h"


/*------------------------------------------------------------------
* 检查 HID 输入缓冲区中的下一个字符串是否与指定的字符串相同.
* 如果相同就跳过.
*
* 参数:
	str		需要匹配的字符串
* 返回值:
*	true	下一个字符串与指定的串匹配
*	false	下一个字符串与指定的串 不 匹配
*------------------------------------------------------------------*/

bool SkipNextString(const char *str)
{
    uint8_t b;
    uint16_t bytesRead = 0;
    bool result = true;
    uint32_t t0 = osKernelGetTickCount();

    while (*str && result)
    {
        if (isspace(*str))
        {
            str++;
            continue;
        }

        if (BSP_HID0_Available())
        {
            BSP_HID0_Read(&b, 1);
            bytesRead++;

            if (!isspace(b))
            {
                if (b != *str++)
                    result = false;
            }
        }

        if (osKernelGetTickCount() - t0 > 500)
            result = false;
    }

    if (!result)
        BSP_HID0_Unread(bytesRead);

    return result;
}


/*------------------------------------------------------------------
* 从 HID 输入缓冲区中获取一个命令(名称).
*
* 参数:
*	[out] nameBuf	命令名称缓冲区, 返回值为 true 时有效.
*	[in ] nameLenMax	缓冲区的大小
* 返回值:
*	true	成功
*	false	失败
*------------------------------------------------------------------*/

static bool IsNamePart(char c);

bool GetName(char *nameBuf, int nameLenMax)
{
    //----- 跳过无效字符 -----//
    uint8_t b;
    while (BSP_HID0_Available())
    {
        BSP_HID0_Read(&b, 1);
        if (isprint(b))
        {
            BSP_HID0_Unread(1);
            break;
        }
    }

    int len = 0;
    uint32_t t0 = osKernelGetTickCount();

    while (len < nameLenMax)
    {
        if (BSP_HID0_Available())
        {
            BSP_HID0_Read((uint8_t*)&nameBuf[len], 1);
            if (!IsNamePart(nameBuf[len]))
                break;
            len++;
        }

        if (osKernelGetTickCount() - t0 > 500)
            return false;
    }

    if (len == 0)
        return false;

    BSP_HID0_Unread(1);
    nameBuf[len] = 0;

    return true;
}


/*------------------------------------------------------------------
* 确认指定的字符是否是命令名称的一部分.
* 返回值:
*	true	是
*	false	否
*------------------------------------------------------------------*/
static bool IsNamePart(char c)
{
    if (c >= 'a' && c <= 'z')
        return true;

    if (c == '_')
        return true;

    return false;
}


/*------------------------------------------------------------------
* 从 HID 输入缓冲区中获取一个数值.
* 例如 000011, 112233 ...
*
* 参数:
*	[out] numBuf 	保存数值的缓冲区, 返回值为 true 时有效.
*	[in]  numLenMax	数值的最大长度 (缓冲区的大小)
* 返回值:
*	true	成功
*	false	失败
*------------------------------------------------------------------*/
bool GetNumber(char *numBuf, int numLenMax)
{
    int len = 0;
    uint32_t t0 = osKernelGetTickCount();

    while (1)
    {
        if (BSP_HID0_Available())
        {
            BSP_HID0_Read((uint8_t*) &numBuf[len], 1);
            if (! isdigit(numBuf[len]) )
                break;

            if (len < numLenMax)
                len++;
        }

        if (osKernelGetTickCount() - t0 > 500)
            return false;
    }

    if (len == 0)
        return false;

    BSP_HID0_Unread(1);
    numBuf[len] = 0;

    return true;
}


/*------------------------------------------------------------------
* 从 HID 输入缓冲区中获取一个字符串. 直到遇到指定的字符 term.

* 参数:
*	[out] sbuf 		保存字符串的缓冲区, 返回值为 true 时有效.
*	[in]  strLenMax	字符串的最大长度 (缓冲区的大小)
*	[in]  term		终结符
* 返回值:
*	true	成功
*	false	失败
*------------------------------------------------------------------*/
bool GetStringValue(char *sbuf, int strLenMax, char term)
{
    int len = 0;
    uint32_t t0 = osKernelGetTickCount();

    while (len < strLenMax)
    {
        if (BSP_HID0_Available())
        {
            BSP_HID0_Read((uint8_t*) &sbuf[len], 1);
            if (sbuf[len] == term)
                break;

            len++;
        }

        if (osKernelGetTickCount() - t0 > 500)
            return false;
    }

    if (len == 0)
        return false;

    sbuf[len] = 0;

    return true;
}


/*------------------------------------------------------------------
* 从 HID 输入缓冲区中获取一些字节. 这些字节是用十六进制字符串表示的.
*
* 参数:
*	[out] buf 		保存获取的数据, 返回值为 true 时有效.
*	[in]  bytesNum	要读取的字节数
* 返回值:
*	true	成功
*	false	失败
*------------------------------------------------------------------*/

bool GetBytes(uint8_t *buf, int bytesNum)
{
    char cbuf[3];
    int ccnt = 0;
    uint32_t timeout = bytesNum * 100;
    uint32_t t0 = osKernelGetTickCount();

    while (bytesNum > 0)
    {
        if (BSP_HID0_Available())
        {
            BSP_HID0_Read((uint8_t*) &cbuf[ccnt], 1);
            if (isspace(cbuf[ccnt]))
                continue;

            if (isxdigit(cbuf[ccnt]) )
            {
                if (++ccnt == 2)
                {
                    HexStringToBytes(cbuf, 2, buf);
                    buf++;
                    ccnt = 0;
                    bytesNum--;
                }
            }
            else
            {
                BSP_HID0_Unread(1);
                return false;
            }
        }

        if (osKernelGetTickCount() - t0 > timeout)
            return false;
    }

    return true;
}


/*------------------------------------------------------------------
* 向主机端发送数据.
* 此函数类似 printf , 但设计的格式暂时不用%, 且只支持i和s (类%i, %s)
*
* 参数:
*	[fmt] fmt 		格式化字符串
*	[in]  args		可变参数表
*------------------------------------------------------------------*/

void SetResponseV(const char *fmt, va_list args)
{
    int iv;
    char ch, ibuf[10];
    const char *strv;
    int len;

    while (*fmt)
    {
        switch (ch = *fmt++)
        {
        case 's':
            strv = va_arg(args, const char*);
            BSP_HID0_Write((uint8_t*)"\"", 1);
            BSP_HID0_Write((uint8_t*)strv, strlen(strv));
            BSP_HID0_Write((uint8_t*)"\"", 1);
            break;

        case 'i':
            iv = va_arg(args, int);
            len = sprintf(ibuf, "%d", iv);
            BSP_HID0_Write((uint8_t*)ibuf, len);
            break;

        default:
            BSP_HID0_Write((uint8_t*)&ch, 1);
            break;
        }
    }
}


/*------------------------------------------------------------------
* 向主机端发送数据. (调用 SetResponseV)
*
* 参数:
*	[fmt] fmt 		格式化字符串
*------------------------------------------------------------------*/

void SetResponse(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SetResponseV(fmt, args);
    va_end(args);
}


void FlushResponse(void)
{
    BSP_HID0_Flush();
}
