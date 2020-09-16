
#include <string.h>
#include "cmsis_os2.h"
#include "FP_Reader.h"

#include "AddFP.h"
#include "AlertUtil.h"
#include "DeviceMng.h"
#include "Display.h"
#include "DbgPrint.h"


#define FLAG_STOP	1

/*------------------------------------------------------------------
* 私有变量定义
*------------------------------------------------------------------*/
static uid_t _userID;
static osThreadId_t _tid = 0;


/*------------------------------------------------------------------
* 私有函数声明
*------------------------------------------------------------------*/
static void ReadFPThread(void *arg);
static void OnReadOK(uint16_t FPID);
static void OnReadError(void);

/*------------------------------------------------------------------
* 启动读线纹线程
*------------------------------------------------------------------*/
bool StartAddFP(const char *UID)
{
    StopAddFP();
    strcpy(_userID, UID);

    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.attr_bits = osThreadJoinable;
    attr.priority = osPriorityAboveNormal;

    _tid = osThreadNew(ReadFPThread, NULL, &attr);
    if (_tid == NULL)
    {
        DbgPrint("!!! StartAddFP: osThreadNew FAIL\r\n");
        return false;
    }

    return true;
}


/*------------------------------------------------------------------
* 停止读指纹线程
*------------------------------------------------------------------*/
void StopAddFP(void)
{
    if (_tid == 0)
        return;

    DbgPrint("StopAddFP begin\r\n");

    osThreadFlagsSet(_tid, FLAG_STOP);

    osStatus_t status = osThreadJoin(_tid);
    if (status == osOK)
    {
        osThreadDetach(_tid);
        _tid = 0;
    }

    DbgPrint("StopAddFP end: _tid=%d\r\n", _tid);
}


/*------------------------------------------------------------------
* 读指纹线程
*------------------------------------------------------------------*/
void ReadFPThread(void *arg)
{
    int ret;
    int step = 1;
    uint16_t FPID;
    uint32_t flag;
    bool touched;
    bool interrupted = false;

    DbgPrint("*** ReadFPThread Started ***\r\n");

    BSP_FP_WakeUp();
    Alert_OnStartReadFinger();

    while (1)
    {
        GoPage_ReadFPStep(step);

        // 等待手指按压
        while (!interrupted)
        {
            ret = BSP_FP_GetFingerStatus(&touched);
            if (ret != 0)
                break;

            if (touched)
                break;

            flag = osThreadFlagsWait(FLAG_STOP, osFlagsWaitAny, 100);
            if (flag & FLAG_STOP)
            {
                interrupted = true;
                goto END;
            }
        }

        ret = BSP_FP_RegisterFPStep(step, &FPID);
        if (ret != 0)
        {
            OnReadError();
            break;
        }

        Alert_OnReadFingerOK();

        if (++step > 3)
        {
            OnReadOK(FPID);
            break;
        }

        // 等待手指离开
        while (!interrupted)
        {
            ret = BSP_FP_GetFingerStatus(&touched);
            if (ret != 0)
                break;

            if (!touched)
                break;

            flag = osThreadFlagsWait(FLAG_STOP, osFlagsWaitAny, 100);
            if (flag & FLAG_STOP)
            {
                interrupted = true;
                goto END;
            }
        }
    }

END:
    BSP_FP_Sleep();

    DbgPrint("*** ReadFPThread Stopped ***\r\n");

    if (!interrupted) // 正常退出
    {
        osThreadDetach(_tid); //  释放资源
        _tid = 0;
    }
    else
    {
        osThreadExit();
        // 等待 join
    }
}


void OnReadOK(uint16_t FPID)
{
    UserInfo userInfo = { 0 };
    int ret = DevMng_GetGeneralUserInfo(_userID, &userInfo);
    if (ret != 0)
    {
        strcpy(userInfo.UID, _userID);
    }
    else if (userInfo.flags & UF_FP_VALID)
    {
        ret = BSP_FP_DeleteFP(userInfo.FPID);
        if (ret != 0)
        {
            GoPage_AddFPFail();
            return;
        }
    }

    userInfo.FPID = FPID;
    userInfo.flags |= UF_FP_VALID;

    ret = DevMng_AddUpdateGeneralUserInfo(&userInfo);
    if (ret != 0)
    {
        BSP_FP_DeleteFP(FPID);
        GoPage_AddFPFail();
    }
    else
    {
        GoPage_AddFPOk(_userID);
        DbgPrint("AddFPOk: UID=%s, FPID=%d\r\n", _userID, FPID);
    }
}


void OnReadError(void)
{
    GoPage_AddFPFail();
}
