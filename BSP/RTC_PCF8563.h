#ifndef _RTC_PCF8563_H
#define _RTC_PCF8563_H

#include <stdbool.h>
#include "RTC_Time.h"

void BSP_RTC_Init(void);

bool BSP_RTC_SetTime(const RTC_Time *time);
bool BSP_RTC_GetTime(RTC_Time *time);

#endif
