
#ifdef USE_DBG_PRINT

#include <stdio.h>
#include <stdarg.h>

#include "rl_usb.h"
#include "User_HID_0.h"

void DbgPrint(const char *format, ...)
{
    static char buf[1024];
    int len;

    va_list args;
    va_start(args, format);
    len = vsprintf(buf, format, args);
    va_end(args);

    BSP_HID0_Write((uint8_t*)buf, len);
    BSP_HID0_Flush();
}

#endif
