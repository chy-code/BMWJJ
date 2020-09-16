
#include <string.h>
#include "cmsis_os2.h"
#include "Motor.h"
#include "DeviceMng.h"
#include "Display.h"
#include "DbgPrint.h"


void SF_Unlock(const char * userID)
{
    DbgPrint("unlocking...\r\n");

    if (userID != NULL)
        DevMng_AddLog(userID, "unlock");
    else
        DevMng_AddLog("", "unlock");

    BSP_Motor_Start(true);
    osDelay(3000);
    BSP_Motor_Stop();

    BSP_Motor_Start(false);
    osDelay(500);
    BSP_Motor_Stop();

    GoPage_01();
}
