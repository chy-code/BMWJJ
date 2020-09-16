#include "stm32f10x.h"
#include "IWDG.h"


void BSP_IWDG_Init(uint8_t prescaler, uint16_t reload)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(prescaler);
    IWDG_SetReload(reload);
    IWDG_ReloadCounter();
    IWDG_Enable();
}
