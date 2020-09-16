

#ifndef RFCARD_Init_H
#define RFCARD_Init_H

void RC523_Init(void);
void RC523_Identify(void);

uint8_t RC523_ReadByte(const uint8_t addr);
void RC523_WriteByte(const uint8_t addr, uint8_t data);

#endif

