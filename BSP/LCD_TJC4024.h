#ifndef _LCD_TJC4024_H
#define _LCD_TJC4024_H

#include <stdint.h>
#include <stdbool.h>

void BSP_LCD_Init(void);

void BSP_LCD_CommandV(const char *format, ...);


__forceinline
void BSP_LCD_TurnOn(void)
{
	BSP_LCD_CommandV("sleep=0");
}

__forceinline 
void BSP_LCD_TurnOff(void)
{
	BSP_LCD_CommandV("sleep=1");
}


#endif
