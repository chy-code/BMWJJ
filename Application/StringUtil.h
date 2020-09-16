
#ifndef _STRING_UTIL_H
#define _STRING_UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include "RTC_Time.h"

void BytesToHexString(
    const uint8_t *Data,
    uint16_t DataLen,
    char *Str);

void HexStringToBytes(
    const char *Str,
    uint16_t StrLen,
    uint8_t *Buf);

const char * TimeToString(
    const RTC_Time *time);

bool StringToTime(
    const char *Str,
    RTC_Time *time);

#endif

