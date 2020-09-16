#ifndef _MOTOR_H
#define _MOTOR_H

#include <stdbool.h>

void BSP_Motor_Init(void);
void BSP_Motor_Start(bool CW);
void BSP_Motor_Stop(void);

#endif
