#ifndef _USER_HID_0_H
#define _USER_HID_0_H

#include <stdbool.h>
#include <stdint.h>

void BSP_HID0_Init(void);

uint16_t BSP_HID0_Available(void);

void BSP_HID0_Read(uint8_t *buf, uint16_t bytesToRead);
void BSP_HID0_Unread(uint16_t count);
void BSP_HID0_Write(uint8_t *buf, uint16_t bytesToWrite);
void BSP_HID0_Flush(void);

#endif
