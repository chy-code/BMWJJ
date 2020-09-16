
/*******************************************************************
* 文件名: MPUtil.h
* 用途	: 消息处理辅助函数定义.
********************************************************************/

#ifndef _MP_UTIL_H
#define _MP_UTIL_H

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

bool SkipNextString(const char *str);
bool GetName(char *nameBuf, int nameLenMax);
bool GetNumber(char *numBuf, int numLenMax);
bool GetStringValue(char *sbuf, int strLenMax, char term);
bool GetBytes(uint8_t *buf, int bytesNum);

void SetResponseV(const char *fmt, va_list args);
void SetResponse(const char *fmt, ...);
void FlushResponse(void);

#endif
