
#include <string.h>
#include "cmsis_os2.h"
#include "RFID_Reader.h"

#include "AddCard.h"
#include "DeviceMng.h"
#include "Display.h"
#include "DbgPrint.h"


#define FLAG_STOP	1


/*------------------------------------------------------------------
* 私有变量定义
*------------------------------------------------------------------*/

static bool _major = false;
static uid_t _userID;
static osThreadId_t _tid = 0;


/*------------------------------------------------------------------
* 私有函数声明
*------------------------------------------------------------------*/
static void ReadCardThread(void *arg);
static void OnReadComplete(cardno_t cardNO);


/*------------------------------------------------------------------
* 启动读卡线程
*------------------------------------------------------------------*/

bool StartAddCard(bool major, const char *UID)
{
    StopAddCard();

    _major = major;
    strcpy(_userID, UID);

    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.attr_bits = osThreadJoinable;
    attr.priority = osPriorityHigh;

    _tid = osThreadNew(ReadCardThread, NULL, &attr);
    if (_tid == NULL)
    {
        DbgPrint("!!! StartAddCard: osThreadNew FAIL\r\n");
        return false;
    }

    return true;
}


/*------------------------------------------------------------------
* 停止读卡线程
*------------------------------------------------------------------*/
void StopAddCard(void)
{
    if (_tid == 0)
        return;

    DbgPrint("StopAddCard begin\r\n");

    osThreadFlagsSet(_tid, FLAG_STOP);

    osStatus_t status = osThreadJoin(_tid);
    if (status == osOK)
    {
        osThreadDetach(_tid);
        _tid = 0;
    }

    DbgPrint("StopAddCard end: _tid=%d\r\n", _tid);
}


/*------------------------------------------------------------------
* 读卡线程
*------------------------------------------------------------------*/
void ReadCardThread(void *arg)
{
    int ret;
    uint32_t flag;
    cardno_t cardNO;
    bool interrupted = false;

    DbgPrint("*** ReadCardThread Started ***\r\n");
    GoPage_ReadCard();

    while (!interrupted)
    {
        ret = BSP_RFID_GetCardNO(cardNO);
        if (ret == 0)
        {
            OnReadComplete(cardNO);
            break;
        }

        flag = osThreadFlagsWait(FLAG_STOP, osFlagsWaitAny, 500);
        if (flag & FLAG_STOP)
            interrupted = true;
    }

    DbgPrint("*** ReadCardThread Stopped ***\r\n");

    if (!interrupted)
    {
        osThreadDetach(_tid);
        _tid = 0;
    }
    else
    {
        osThreadExit();
    }
}


void OnReadComplete(cardno_t cardNO)
{
    UserInfo userInfo = { 0 };
    int ret = DevMng_GetGeneralUserInfo(_userID, &userInfo);
    if (ret != 0)
        strcpy(userInfo.UID, _userID);

    if (_major)
    {
        strcpy(userInfo.MajorCardNO, cardNO);
        userInfo.flags |= UF_MAJOR_CARD_VALID;
    }
    else
    {
        strcpy(userInfo.MinorCardNO, cardNO);
        userInfo.flags |= UF_MINOR_CARD_VALID;
    }

    ret = DevMng_AddUpdateGeneralUserInfo(&userInfo);
    if (ret != 0)
        GoPage_AddCardFail();
    else
        GoPage_AddCardOK(_userID);
}
