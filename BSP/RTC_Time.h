
#ifndef _RTC_TIME_H
#define _RTC_TIME_H

#include <stdint.h>

typedef struct 
{
	uint16_t Year;
	uint8_t Month;
	uint8_t MDay;
	uint8_t Weekday;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
} RTC_Time;

#endif
