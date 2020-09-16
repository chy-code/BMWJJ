#ifndef _FP_READER_H
#define _FP_READER_H

#include <stdint.h>
#include <stdbool.h>

// 函数返回值定义
#define FP_E_SUCCESS		0
#define FP_E_RECEPTION		-1
#define FP_E_NO_RESPONSE	-2

void BSP_FP_Init(void);

void BSP_FP_WakeUp(void);
void BSP_FP_Sleep(void);

int BSP_FP_GetFingerStatus(bool *isTouched);
int BSP_FP_DeleteFP(uint16_t id);
int BSP_FP_DeleteAllFPs(void);
int BSP_FP_RegisterFPStep(int step, uint16_t *id);
int BSP_FP_Match(uint16_t *id);

const char* BSP_FP_StrError(int ret);

#endif
