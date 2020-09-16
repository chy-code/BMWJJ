
#include <stdio.h>
#include <stdarg.h>
#include "cmsis_os2.h"
#include "stm32f10x.h"
#include "USART_Util.h"
#include "LCD_TJC4024.h"


/*------------------------------------------------------------------
* 常量定义
*------------------------------------------------------------------*/
#define LCD_USART	USART2


/*------------------------------------------------------------------
* 初始化 LCD
*------------------------------------------------------------------*/
void BSP_LCD_Init(void)
{
    MY_USART_Init(LCD_USART, 115200, USART_Parity_No);
    USART_Cmd(LCD_USART, ENABLE);
}


void BSP_LCD_CommandV(const char *format, ...)
{
    static char cmdbuf[1024];
    uint16_t cmdlen;

    va_list args;
    va_start(args, format);
    cmdlen = vsprintf(cmdbuf, format, args);
    va_end(args);

    MY_USART_Transmit(LCD_USART, (uint8_t*)cmdbuf, cmdlen);
    MY_USART_Transmit(LCD_USART, (uint8_t*)"\xff\xff\xff", 3);

    osDelay(100);
}
