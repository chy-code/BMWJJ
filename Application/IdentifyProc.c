
#include <string.h>
#include "cmsis_os2.h"

#include "RFID_Reader.h"
#include "FP_Reader.h"

#include "DeviceMng.h"
#include "Display.h"
#include "DbgPrint.h"


/*------------------------------------------------------------------
* 私有函数声明.
*------------------------------------------------------------------*/
static void OnReadCardComplete(const cardno_t cardNO);
static void OnReadFPComplete(int FPID);


/*------------------------------------------------------------------
* 身份识别任务.
*------------------------------------------------------------------*/
void IdentifyProc(void *arg)
{
    int ret;
    static cardno_t cardNO;
    uint16_t FPID;
    bool touched;
    bool read = false;

    while (1)
    {
        if (disp_curPage == PAGE_01_02
                || disp_curPage == PAGE_01_05
                || (disp_curPage == PAGE_01
                    && disp_isTurnedOn
                    && (DevMng_GetStatus() & DEVICE_RECEIVER_SPEC)))
        {
            if (!read)
            {
                read = true;
                DbgPrint("*** IdentifyProc Start Read ***\r\n");
            }
        }
        else if (read)
        {
            DbgPrint("*** IdentifyProc Stop Read ***\r\n");
            read = false;
        }

        if (read)
        {
            BSP_FP_WakeUp();

            ret = BSP_FP_GetFingerStatus(&touched);
            if (ret == 0) // 检查手指状态成功
            {
                DbgPrint("手指: %d\r\n", touched);

                if (touched) // 手指在阅读器上
                {
                    ret = BSP_FP_Match(&FPID); // 采集指纹并与指纹库对比
                    DbgPrint("BSP_FP_Match 返回 %d\n", ret);

                    if (ret == 0) // 匹配指纹库中的某个ID
                    {
                        DbgPrint("指纹匹配成功: FPID=%d\r\n", FPID);
                        OnReadFPComplete(FPID);
                    }
                }
            }

            BSP_FP_Sleep();

            ret = BSP_RFID_GetCardNO(cardNO);
            if (ret == 0)
            {
                DbgPrint("读卡号成功: %s\r\n", cardNO);
                OnReadCardComplete(cardNO);
            }
        }

        osDelay(500);
    }
}


void OnReadCardComplete(const cardno_t cardNO)
{
    int ret;
    UserInfo userInfo;

    if (disp_curPage == PAGE_01_02
            || disp_curPage == PAGE_01)
    {
        ret = DevMng_GetReceiverInfo(&userInfo);
        if (ret == 0)
        {
            if ((userInfo.flags & UF_MAJOR_CARD_VALID)
                    && strcmp(cardNO, userInfo.MajorCardNO) == 0)
            {
                GoPage_01_02_02(userInfo.UID);
                return;
            }

            if ((userInfo.flags & UF_MINOR_CARD_VALID)
                    && strcmp(cardNO, userInfo.MinorCardNO) == 0)
            {
                GoPage_01_02_02(userInfo.UID);
                return;
            }
        }

        ret = DevMng_GetSenderInfo(&userInfo);
        if (ret == 0)
        {
            if ((userInfo.flags & UF_MAJOR_CARD_VALID)
                    && strcmp(cardNO, userInfo.MajorCardNO) == 0)
            {
                GoPage_SenderUnlock(userInfo.UID);
                return;
            }

            if ((userInfo.flags & UF_MINOR_CARD_VALID)
                    && strcmp(cardNO, userInfo.MinorCardNO) == 0)
            {
                GoPage_SenderUnlock(userInfo.UID);
                return;
            }
        }

        ret = DevMng_GetAdminInfoByCardNO(cardNO, &userInfo);
        if (ret == 0)
        {
            GoPage_01_02_01(userInfo.UID);
            return;
        }

        GoPage_01_02_04();
    }
    else if (disp_curPage == PAGE_01_05)
    {
        ret = DevMng_GetAdminInfoByCardNO(cardNO, &userInfo);
        if (ret == 0)
        {
            GoPage_01_05_01();
            return;
        }

        Disp_OnIdentifyFailed();
    }
}


void OnReadFPComplete(int FPID)
{
    int ret;
    UserInfo userInfo;

    if (disp_curPage == PAGE_01_02
            || disp_curPage == PAGE_01)
    {
        ret = DevMng_GetReceiverInfo(&userInfo);
        if (ret == 0
                && (userInfo.flags & UF_FP_VALID)
                && userInfo.FPID == FPID)
        {
            GoPage_01_02_02(userInfo.UID);
            return;
        }

        ret = DevMng_GetSenderInfo(&userInfo);
        if (ret == 0
                && (userInfo.flags & UF_FP_VALID)
                && userInfo.FPID == FPID)
        {
            GoPage_SenderUnlock(userInfo.UID);
            return;
        }

        ret = DevMng_GetAdminInfoByFP(FPID, &userInfo); // 通过指纹ID查找用户
        if (ret == 0)
        {
            GoPage_01_02_01(userInfo.UID);
            return;
        }

        GoPage_01_02_04();
    }
    else if (disp_curPage == PAGE_01_05)
    {
        ret = DevMng_GetAdminInfoByFP(FPID, &userInfo);
        if (ret == 0)
        {
            GoPage_01_05_01();
            return;
        }

        Disp_OnIdentifyFailed();
    }
}
