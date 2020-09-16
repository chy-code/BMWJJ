#ifndef _BQ34Z100_H
#define _BQ34Z100_H

#include <stdint.h>
#include <stdbool.h>

void BSP_BQ34Z100_Init(void);
bool BSP_BQ34Z100_RemainingCapacityPercent(uint8_t *percent);

#endif
