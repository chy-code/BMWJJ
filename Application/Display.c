
#include "cmsis_os2.h"
#include "BQ34Z100.h"
#include "Keypad.h"
#include "LCD_TJC4024.h"
#include "RTC_PCF8563.h"
#include "User_HID_0.h"

#include "DbgPrint.h"
#include "DeviceMng.h"
#include "Display.h"
#include "FileSysUtil.h"
#include "StringUtil.h"
#include "Unlock.h"


PageID disp_curPage = PAGE_01;	// 当前页面ID

int disp_activeItem_01_01_02 = 1;	// 页面 01-01-02 中激活的菜单项
int disp_activeItem_01_01_02_01 = 1; // 页面 01-01-02-01 中激活的菜单项
int disp_activeItem_01_01_02_02 = 1; // 页面 01-01-02-02 中激活的菜单项
int disp_activeItem_01_01_02_03 = 1; // 页面 01-01-02-03 中激活的菜单项

bool disp_isTurnedOn = false;		// 显示屏是否已点亮
uint32_t disp_t0 = 0;	// 显示屏点亮时的时间(滴答数)


void Disp_TurnOn(void)
{
    BSP_LCD_TurnOn();
    BSP_Keypad_OpenBacklight();

    disp_t0 = osKernelGetTickCount();
    disp_isTurnedOn = true;
}


void Disp_TurnOff(void)
{
    BSP_LCD_TurnOff();
    BSP_Keypad_CloseBacklight();
    disp_isTurnedOn = false;
}


void Disp_UpdateStatusBar(void)
{
    RTC_Time time;
    uint8_t percent;

    if (BSP_RTC_GetTime(&time))
    {
        BSP_LCD_CommandV("n0.val=%d", time.Year);
        BSP_LCD_CommandV("n1.val=%d", time.Month);
        BSP_LCD_CommandV("n2.val=%d", time.MDay);

        BSP_LCD_CommandV("n3.val=%d", time.Hour);
        BSP_LCD_CommandV("n4.val=%d", time.Minute);
    }

    if (BSP_BQ34Z100_RemainingCapacityPercent(&percent))
    {
        BSP_LCD_CommandV("n5.val=%d", percent);
    }

    BSP_LCD_CommandV("vis p1,0");
}


void Disp_UpdatePasswordBox(const char *text)
{
    BSP_LCD_CommandV("t3.txt=\"%s\"", text);
}


void Disp_UpdateUIDBox(const char *text)
{
    BSP_LCD_CommandV("t3.txt=\"%s\"", text);
}


void Disp_OnIdentifyFailed(void)
{
    BSP_LCD_CommandV("vis t0,1");
}


void Disp_OnSetReceiverFailed(void)
{
    BSP_LCD_CommandV("vis t0,1");
}


void Disp_OnPasswordInputError(void)
{
    BSP_LCD_CommandV("t3.txt=\"\"");
    BSP_LCD_CommandV("vis t1,1");
}


void Disp_OnUIDInputError(void)
{
    BSP_LCD_CommandV("t3.txt=\"\"");
    BSP_LCD_CommandV("vis t1,1");
}


void GoPage_01(void)
{
    int devStat = DevMng_GetStatus();

    if (devStat & DEVICE_SENDER_SPEC)
    {
        BSP_LCD_CommandV("page main2");
        BSP_LCD_CommandV("t3.txt=\"%s\"", DevMng_GetSenderID());

        if (devStat & DEVICE_RECEIVER_SPEC)
        {
            BSP_LCD_CommandV("page main3");
            BSP_LCD_CommandV("t0.txt=\"%s\"", DevMng_GetSenderID());
            BSP_LCD_CommandV("t3.txt=\"%s\"", DevMng_GetReceiverID());
        }
    }
    else
    {
        BSP_LCD_CommandV("page main1");
    }

    disp_activeItem_01_01_02 = 1;
    disp_activeItem_01_01_02_01 = 1;
    disp_activeItem_01_01_02_02 = 1;
    disp_activeItem_01_01_02_03 = 1;
    disp_curPage = PAGE_01;

    Disp_TurnOn();

    DbgPrint("current page: PAGE_01\r\n");
}


void GoPage_01_01(void)
{
    BSP_LCD_CommandV("page sub1");
    disp_curPage = PAGE_01_01;

    DbgPrint("current page: PAGE_01_01\r\n");
}


void GoPage_01_02(void)
{
    BSP_LCD_CommandV("page sub2");
    disp_curPage = PAGE_01_02;

    DbgPrint("current page: PAGE_01_02\r\n");
}


void GoPage_01_03(void)
{
    BSP_LCD_CommandV("page sub3");
    disp_curPage = PAGE_01_03;

    DbgPrint("current page: PAGE_01_03\r\n");
}


void GoPage_01_04(void)
{
    BSP_LCD_CommandV("page sub4");
    disp_curPage = PAGE_01_04;
    DbgPrint("current page: PAGE_01_04\r\n");
}


void GoPage_01_05(void)
{
    BSP_LCD_CommandV("page sub5");
    BSP_LCD_CommandV("vis t0,0");

    disp_curPage = PAGE_01_05;
    DbgPrint("current page: PAGE_01_05\r\n");
}


void GoPage_01_06(void)
{
    BSP_LCD_CommandV("page sub1");

    disp_curPage = PAGE_01_06;

    DbgPrint("current page: PAGE_01_06\r\n");
}


void GoPage_01_01_01(void)
{
    BSP_LCD_CommandV("page third10");

    disp_curPage = PAGE_01_01_01;

    DbgPrint("current page: PAGE_01_01_01\r\n");
}


void GoPage_01_01_02(void)
{
    BSP_LCD_CommandV("page third1%d", disp_activeItem_01_01_02);

    disp_curPage = PAGE_01_01_02;

    DbgPrint("current page: PAGE_01_01_02\r\n");
}


void GoPage_01_02_01(const char *UID)
{
    BSP_LCD_CommandV("page third21");
    BSP_LCD_CommandV("t3.txt=\"%s\"", UID);

    disp_curPage = PAGE_01_02_01;

    DbgPrint("current page: PAGE_01_02_01\r\n");

    SF_Unlock(UID);
}


void GoPage_01_02_02(const char *UID)
{
    BSP_LCD_CommandV("page third22");
    BSP_LCD_CommandV("t3.txt=\"%s\"", UID);

    disp_curPage = PAGE_01_02_02;

    DbgPrint("current page: PAGE_01_02_02\r\n");

    SF_Unlock(UID);
}


void GoPage_SenderUnlock(const char *UID)
{
    BSP_LCD_CommandV("page third23");
    BSP_LCD_CommandV("t3.txt=\"%s\"", UID);

    disp_curPage = PAGE_SENDER_UNLOCK;

    DbgPrint("current page: PAGE_SENDER_UNLOCK\r\n");

    SF_Unlock(UID);
}


void GoPage_01_02_03(const char *UID)
{
    BSP_LCD_CommandV("page third24");
    BSP_LCD_CommandV("t3.txt=\"%s\"", UID);

    disp_curPage = PAGE_01_02_03;

    DbgPrint("current page: PAGE_01_02_03\r\n");
}


void GoPage_01_02_04(void)
{
    BSP_LCD_CommandV("page third25");

    disp_curPage = PAGE_01_02_04;

    DbgPrint("current page: PAGE_01_02_04\r\n");
}


void GoPage_01_03_01(void)
{
    BSP_LCD_CommandV("page third31");

    disp_curPage = PAGE_01_03_01;
    DbgPrint("current page: PAGE_01_03_01\r\n");

    SF_Unlock(NULL);
}


void GoPage_01_03_02(void)
{
    BSP_LCD_CommandV("page third33");

    disp_curPage = PAGE_01_03_02;

    DbgPrint("current page: PAGE_01_03_02\r\n");
}


void GoPage_01_05_01(void)
{
    BSP_LCD_CommandV("page third51");

    disp_curPage = PAGE_01_05_01;

    DbgPrint("current page: PAGE_01_05_01\r\n");
}


void GoPage_01_01_02_01(void)
{
    BSP_LCD_CommandV("page fourth11%d", disp_activeItem_01_01_02_01);

    disp_curPage = PAGE_01_01_02_01;

    DbgPrint("current page: PAGE_01_01_02_01\r\n");
}


void GoPage_01_01_02_02(void)
{
    BSP_LCD_CommandV("page fourth12%d", disp_activeItem_01_01_02_02);

    disp_curPage = PAGE_01_01_02_02;

    DbgPrint("current page: PAGE_01_01_02_02\r\n");
}


void GoPage_01_01_02_03(void)
{
    BSP_LCD_CommandV("page fourth13%d", disp_activeItem_01_01_02_03);

    disp_curPage = PAGE_01_01_02_03;

    DbgPrint("current page: PAGE_01_01_02_03\r\n");
}


void GoPage_01_01_02_04(void)
{
    int ret;
    UserInfo userInfo;
    char buf[128];

    BSP_LCD_CommandV("page fourth141");

    ret = DevMng_GetSenderInfo(&userInfo);
    if (ret == 0)
        sprintf(buf, "%s\r\nMajor Card NO: %s\r\nMinor Card NO: %s\r\n",
                userInfo.UID, userInfo.MajorCardNO, userInfo.MinorCardNO);
    else
        sprintf(buf, "Not specified");

    BSP_LCD_CommandV("t0.txt=t0.txt+\"%s\"", buf);


    ret = DevMng_GetReceiverInfo(&userInfo);
    if (ret == 0)
        sprintf(buf, "%s\r\nMajor Card NO: %s\r\nMinor Card NO: %s\r\n",
                userInfo.UID, userInfo.MajorCardNO, userInfo.MinorCardNO);
    else
        sprintf(buf, "Not specified");

    BSP_LCD_CommandV("t1.txt=t1.txt+\"%s\"", buf);


    disp_curPage = PAGE_01_01_02_04;

    DbgPrint("current page: PAGE_01_01_02_04\r\n");
}


void GoPage_01_01_02_05(void)
{
    int ret;
    FILE *stream;
    LogEntry entry;
    char buf[128];

    BSP_LCD_CommandV("page fourth151");

    ret = DevMng_GetLogDataStream(&stream);
    if (ret == 0)
    {
        BSP_LCD_CommandV("t2.txt=\"\"");

        while (DevMng_GetNextLogEntry(stream, &entry))
        {
            sprintf(buf, "%s, %s, %s\r\n",
                    TimeToString(&entry.time),
                    entry.operatorID,
                    entry.opt);
            BSP_LCD_CommandV("t2.txt=t2.txt+\"%s\"", buf);
        }

        fclose(stream);
    }

    disp_curPage = PAGE_01_01_02_05;

    DbgPrint("current page: PAGE_01_01_02_05\r\n");
}


void GoPage_01_01_02_06(void)
{
    int capacity = 0;
    int free = 0;
    int userCout = 0;
    int logEntCount = 0;
    double percent;

    FileSys_GetDriveInfo(&capacity, &free);
    percent = free / (double)capacity;

    DevMng_GetUserCount(&userCout);
    DevMng_GetLogEntryCount(&logEntCount);

    BSP_LCD_CommandV("page fourth161");
    BSP_LCD_CommandV("n8.val=%d", logEntCount);
    BSP_LCD_CommandV("n6.val=%d", free);
    BSP_LCD_CommandV("n7.val=%d", capacity);
    BSP_LCD_CommandV("n9.val=%d", userCout);
    BSP_LCD_CommandV("j0.val=%d", 100 - (int)(percent * 100));

    disp_curPage = PAGE_01_01_02_06;

    DbgPrint("current page: PAGE_01_01_02_06\r\n");
}


void GoPage_01_02_01_01(void)
{
    BSP_LCD_CommandV("page pagelock");
    disp_curPage = PAGE_01_02_01_01;

    DbgPrint("current page: PAGE_01_02_01_01\r\n");
}


void GoPage_01_05_01_01(const char *UID)
{
    BSP_LCD_CommandV("page fourth51");
    BSP_LCD_CommandV("t3.txt=\"%s\"", UID);
    disp_curPage = PAGE_01_05_01_01;

    DbgPrint("current page: PAGE_01_05_01_01\r\n");
}


void GoPage_01_01_02_01_01(void)
{
    BSP_LCD_CommandV("page fifth11");
    disp_curPage = PAGE_01_01_02_01_01;

    DbgPrint("current page: PAGE_01_01_02_01_01\r\n");
}


void GoPage_01_01_02_01_02(void)
{
    BSP_LCD_CommandV("page fifth11");
    disp_curPage = PAGE_01_01_02_01_02;

    DbgPrint("current page: PAGE_01_01_02_01_02\r\n");
}


void GoPage_01_01_02_02_01(void)
{
    BSP_LCD_CommandV("page fifth121");

    disp_curPage = PAGE_01_01_02_02_01;
    DbgPrint("current page: PAGE_01_01_02_02_01\r\n");
}


void GoPage_01_01_02_02_02(void)
{
    BSP_LCD_CommandV("page fifth122");

    disp_curPage = PAGE_01_01_02_02_02;
    DbgPrint("current page: PAGE_01_01_02_02_02\r\n");
}


void GoPage_01_01_02_03_01(void)
{
    BSP_LCD_CommandV("page fifth131");
    disp_curPage = PAGE_01_01_02_03_01;
    DbgPrint("current page: PAGE_01_01_02_03_01\r\n");
}


void GoPage_01_01_02_03_02(void)
{
    BSP_LCD_CommandV("page fifth132");
    disp_curPage = PAGE_01_01_02_03_02;
    DbgPrint("current page: PAGE_01_01_02_03_02\r\n");
}

void GoPage_01_01_02_03_03(void)
{
    BSP_LCD_CommandV("page fifth131");
    disp_curPage = PAGE_01_01_02_03_03;
    DbgPrint("current page: PAGE_01_01_02_03_03\r\n");
}


void GoPage_01_01_02_03_04(void)
{
    BSP_LCD_CommandV("page fifth132");

    disp_curPage = PAGE_01_01_02_03_04;
    DbgPrint("current page: PAGE_01_01_02_03_04\r\n");
}


void GoPage_ReadFPStep(int step)
{
    BSP_LCD_CommandV("page sixth121%d", step);

    if (step == 1)
    {
        disp_curPage = PAGE_READ_FP_STEP1;
        DbgPrint("current page: PAGE_READ_FP_STEP1\r\n");
    }
    else if (step == 2)
    {
        disp_curPage = PAGE_READ_FP_STEP2;
        DbgPrint("current page: PAGE_READ_FP_STEP2\r\n");
    }
    else if (step == 3)
    {
        disp_curPage = PAGE_READ_FP_STEP3;
        DbgPrint("current page: PAGE_READ_FP_STEP3\r\n");
    }
}


void GoPage_AddFPOk(const char *userID)
{
    BSP_LCD_CommandV("page sixth1214");
    BSP_LCD_CommandV("t3.txt=\"%s\"", userID);

    disp_curPage = PAGE_ADD_FP_OK;
    DbgPrint("current page: PAGE_ADD_FP_OK\r\n");
}


void GoPage_AddFPFail(void)
{
    BSP_LCD_CommandV("page fail_addfp");
    disp_curPage = PAGE_ADD_FP_FAIL;
    DbgPrint("current page: PAGE_ADD_FP_FAIL\r\n");
}


void GoPage_DeleteFPOk(void)
{
    BSP_LCD_CommandV("page sixth1221");
    disp_curPage = PAGE_DELETE_FP_OK;
    DbgPrint("current page: PAGE_DELETE_FP_OK\r\n");
}


void GoPage_DeleteFPFail(void)
{
    BSP_LCD_CommandV("page fail_delfp");
    disp_curPage = PAGE_DELETE_FP_FAIL;

    DbgPrint("current page: PAGE_DELETE_FP_FAIL\r\n");
}


void GoPage_ReadCard(void)
{
    BSP_LCD_CommandV("page sixth_readcard");

    disp_curPage = PAGE_READ_CARD;

    DbgPrint("current page: PAGE_READ_CARD\r\n");
}


void GoPage_AddCardOK(const char *userID)
{
    BSP_LCD_CommandV("page sixth131");
    BSP_LCD_CommandV("t3.txt=\"%s\"", userID);

    disp_curPage = PAGE_ADD_CARD_OK;

    DbgPrint("current page: PAGE_ADD_CARD_OK\r\n");
}


void GoPage_AddCardFail(void)
{
    BSP_LCD_CommandV("page fail_addcard");

    disp_curPage = PAGE_ADD_CARD_FAIL;

    DbgPrint("current page: PAGE_ADD_CARD_FAIL\r\n");
}


void GoPage_DeleteCardOK(void)
{
    BSP_LCD_CommandV("page sixth1220");

    disp_curPage = PAGE_DELETE_CARD_OK;

    DbgPrint("current page: PAGE_DELETE_CARD_OK\r\n");
}


void GoPage_DeleteCardFail(void)
{
    BSP_LCD_CommandV("page fail_delcard");

    disp_curPage = PAGE_DELETE_CARD_FAIL;

    DbgPrint("current page: PAGE_DELETE_CARD_FAIL\r\n");
}

