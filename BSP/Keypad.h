#ifndef _KEYPAD_H
#define _KEYPAD_H

#include <stdint.h>
#include <stdbool.h>


// 键盘状态
typedef enum
{
	kpdNoEvent,
	kpdKeyDown,
	kpdKeyUp
} KpdStatus;


// 按键代码
#define KEY(code, value) code,
typedef enum
{
#include "keys.h"
	NUM_KEYS
} KeyCode;


void BSP_Keypad_Init(void);

void BSP_Keypad_OpenBacklight(void);
void BSP_Keypad_CloseBacklight(void);

KpdStatus BSP_Keypad_ScanKey(KeyCode *keyCode);

#endif
