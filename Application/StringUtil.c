
#include <stdio.h>
#include <ctype.h>
#include "StringUtil.h"


void BytesToHexString(
    const uint8_t *Data,
    uint16_t DataLen,
    char *buf)
{
    uint8_t d;
    char c;
    int bi = 0;

    for (uint16_t i = 0; i < DataLen; i++)
    {
        d = Data[i] >> 4;
        c = (d <= 9) ? '0' + d : 'A' + (d - 10);
        buf[bi++] = c;

        d = Data[i] & 0xF;
        c = (d <= 9) ? '0' + d : 'A' + (d - 10);
        buf[bi++] = c;
    }

    buf[bi] = 0;
}



void HexStringToBytes(
    const char *Str,
    uint16_t StrLen,
    uint8_t *Buf)
{
    uint8_t b;
    int bi = 0;
    const char *cp = Str;

    while (*cp)
    {
        b = isdigit(*cp) ?
            *cp - '0' : (*cp | 0x20) - 'a' + 10;
        b <<= 4;
        cp++;

        b += isdigit(*cp) ?
             *cp - '0' : (*cp | 0x20) - 'a' + 10;
        cp++;

        Buf[bi++] = b;
    }
}


const char * TimeToString(
    const RTC_Time *time)
{
    static char timeBuf[20];

    sprintf(timeBuf, "%04d-%02d-%02d %02d:%02d:%02d",
            time->Year, time->Month, time->MDay,
            time->Hour, time->Minute, time->Second);

    return timeBuf;
}


bool StringToTime(
    const char *str,
    RTC_Time *time)
{
    int ret = sscanf(str, "%04d-%02d-%02d %02d:%02d:%02d",
                     (int*)&time->Year, (int*)&time->Month, (int*)&time->MDay,
                     (int*)&time->Hour, (int*)&time->Minute, (int*)&time->Second);
    if (ret != 6)
        return false;

    return true;
}

