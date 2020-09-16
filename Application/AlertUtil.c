
#include "cmsis_os2.h"
#include "Buzzer.h"


static void Buzzer_ShortAlternate(int cnt)
{
    for (int i = 0; i < cnt; i++)
    {
        BSP_Buzzer_On();
        osDelay(100);
        BSP_Buzzer_Off();
        osDelay(100);
    }
}

static void Buzzer_LongAlternate(int cnt)
{
    for (int i = 0; i < cnt; i++)
    {
        BSP_Buzzer_On();
        osDelay(200);
        BSP_Buzzer_Off();
        osDelay(200);
    }
}


void Alert_OnStartReadFinger(void)
{
    Buzzer_LongAlternate(1);
}


void Alert_OnReadFingerOK(void)
{
    Buzzer_ShortAlternate(2);
}


void Alert_OnReadCardOK(void)
{
    Buzzer_ShortAlternate(1);
}

