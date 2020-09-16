
#include <string.h>
#include "cmsis_os2.h"
#include "stm32f10x.h"

#include "BQ34Z100.h"
#include "Buzzer.h"
#include "FP_Reader.h"
#include "Keypad.h"
#include "LCD_TJC4024.h"
#include "LED.h"
#include "Motor.h"
#include "RTC_PCF8563.h"
#include "RFID_Reader.h"
#include "Sensor.h"
#include "User_HID_0.h"

#include "DeviceMng.h"
#include "Display.h"
#include "DbgPrint.h"


extern void MessageProc(void *arg);
extern void KeyProc(void *arg);
extern void IdentifyProc(void *arg);


static void app_main(void *arg)
{
    /*----- BSP 配置-----*/
    BSP_Buzzer_Init();
    BSP_BQ34Z100_Init();
    BSP_FP_Init();
    BSP_LCD_Init();
    BSP_LED_Init();
    BSP_Motor_Init();
    BSP_RTC_Init();
    BSP_RFID_Init();
    BSP_Sensor_Init();
    BSP_Keypad_Init();
    BSP_HID0_Init();

    /*----- App -----*/
    DevMng_Init();
    GoPage_01();

    static osThreadAttr_t attr = { 0 };
    attr.priority = osPriorityLow2;
    osThreadNew(MessageProc, NULL, &attr);
    attr.priority = osPriorityNormal;
    osThreadNew(IdentifyProc, NULL, &attr);
    attr.priority = osPriorityAboveNormal;
    osThreadNew(KeyProc, NULL, &attr);

    while (1)
    {
        if (disp_isTurnedOn)
            Disp_UpdateStatusBar();

        if (BSP_Sensor_GetStatus())
            BSP_Buzzer_On();

        BSP_LED_ToggleState();
        osDelay(500);
    }
}



__NO_RETURN int main()
{
    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.priority = osPriorityLow;

    osKernelInitialize();
    osThreadNew(app_main, NULL, &attr);
    osKernelStart();

    while(1);
}
