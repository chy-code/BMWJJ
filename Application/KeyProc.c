
#include <string.h>
#include "cmsis_os2.h"

#include "FP_Reader.h"
#include "LED.h"
#include "Keypad.h"
#include "LCD_TJC4024.h"
#include "RFID_Reader.h"

#include "AddFP.h"
#include "AddCard.h"
#include "DeviceMng.h"
#include "Display.h"
#include "DbgPrint.h"


/*------------------------------------------------------------------
* 私有变量定义.
*------------------------------------------------------------------*/

#define INB_SIZE	10 	// 输入缓冲区大小

static char _inBuf[INB_SIZE+1]; // 输入缓冲区, 保存当前输入的密码或UID
static int _inLen = 0; // 输入缓冲区有效字符的长度

static PageID _lastPage = PAGE_01; // 当前页之前的一页


/*------------------------------------------------------------------
* 私有函数声明.
*------------------------------------------------------------------*/

static void HandleKeyEvent(const KeyCode key, bool shortPress);
static void HandleInputedPassword(void);
static bool IsNumKey(const KeyCode key, char *val);
static const char* GetKeyName(const KeyCode key);

static void DeleteInBufferLast(void);
static bool AddNumToInBuffer(KeyCode key, int inLenMax);
static void ClearInBuffer(void);


/*------------------------------------------------------------------
* 按键处理线程
*------------------------------------------------------------------*/

void KeyProc(void *arg)
{
    KeyCode key;
    KpdStatus kpdStat;
    bool longp;	// true: 长按, false: 短按
    bool ignore = false; // 是否忽略当前的按键
    uint32_t t0, t1;
    PageID pageid;

    while (1)
    {
        kpdStat = BSP_Keypad_ScanKey(&key);

        switch (kpdStat)
        {
        case kpdKeyDown:
            ignore = !disp_isTurnedOn; // 忽略打开显示时的第一次按键
            if (!disp_isTurnedOn)
                Disp_TurnOn(); // 打开显示

            t0 = osKernelGetTickCount();
            DbgPrint("key down: \"%s\"\r\n", GetKeyName(key));
            break;

        case kpdKeyUp:
            if (!ignore) // 需要处理当前的按键
            {
                t1 = osKernelGetTickCount();
                longp = (t1 - t0) > 500;
                pageid = disp_curPage;

                HandleKeyEvent(key, longp); // 处理当前的按键

                if (pageid != disp_curPage)
                    _lastPage = pageid;
            }
            break;

        default:
            t1 = osKernelGetTickCount();
            if (t1 - disp_t0 > 10000
                    && disp_curPage == PAGE_01
                    && disp_isTurnedOn)
            {
                // 10 秒内无按键动作, 关闭显示
                Disp_TurnOff();
            }

            break;
        }

        osDelay(10);
    }
}



/*------------------------------------------------------------------
* 按键事件处理.
*------------------------------------------------------------------*/

void HandleKeyEvent(const KeyCode key, bool longPress)
{
    int ret;
    UserInfo userInfo;

    switch (disp_curPage)
    {
    case PAGE_01:
    {
        if (key == KEY_SWITCH)
        {
            Disp_TurnOff();
        }
        else if (key == KEY_SET)
        {
            GoPage_01_01();
        }
        else if (key == KEY_LOCK)
        {
            if (DevMng_GetStatus() & DEVICE_RECEIVER_SPEC)
            {
                GoPage_01_04();
            }
            else
            {
                if (! longPress)
                {
                    GoPage_01_05();
                }
                else
                {
                    GoPage_01_06();
                }
            }
        }
        else if (key == KEY_UNLOCK)
        {
            if (! longPress)
            {
                if (DevMng_GetStatus() & DEVICE_RECEIVER_SPEC)
                {
                    GoPage_01_02();
                }
            }
            else
            {
                GoPage_01_03();
            }
        }
        break;
    }

    case PAGE_01_01:
    case PAGE_01_03:
    case PAGE_01_06:
    case PAGE_01_01_01:
    {
        if (key == KEY_SWITCH || key == KEY_BACK)
        {
            GoPage_01();
            ClearInBuffer();
        }
        else if (key == KEY_SET)
        {
            DeleteInBufferLast();
            Disp_UpdatePasswordBox(_inBuf);
        }
        else if (key == KEY_OK)
        {
            DbgPrint("input: %s\r\n", _inBuf);

            HandleInputedPassword();
            ClearInBuffer();
        }
        else
        {
            if (AddNumToInBuffer(key, PWD_SZ_MAX))
                Disp_UpdatePasswordBox(_inBuf);
        }
        break;
    }

    case PAGE_01_02:
    {
        if (key == KEY_SWITCH || key == KEY_BACK)
        {
            GoPage_01();
        }
        break;
    }

    case PAGE_01_04:
    {
        if (key == KEY_SWITCH || key == KEY_BACK)
        {
            GoPage_01();
        }
        else if (key == KEY_UNLOCK)
        {
            if (! longPress)
            {
                GoPage_01_02();
            }
            else
            {
                GoPage_01_03();
            }
        }
        else if (key == KEY_BACK)
        {
            GoPage_01();
        }
        break;
    }

    case PAGE_01_05:
    {
        if (key == KEY_SWITCH || key == KEY_BACK)
        {
            GoPage_01();
        }
        break;
    }

    case PAGE_01_01_02:
    {
        if (key == KEY_SWITCH || key == KEY_BACK)
        {
            GoPage_01();
        }
        else if (key == KEY_SET)
        {
            if (++disp_activeItem_01_01_02 > 6)
                disp_activeItem_01_01_02 = 1;

            GoPage_01_01_02();
        }
        else if (key == KEY_OK)
        {
            switch (disp_activeItem_01_01_02)
            {
            case 1:
                GoPage_01_01_02_01();
                break;
            case 2:
                GoPage_01_01_02_02();
                break;
            case 3:
                GoPage_01_01_02_03();
                break;
            case 4:
                GoPage_01_01_02_04();
                break;
            case 5:
                GoPage_01_01_02_05();
                break;
            case 6:
                GoPage_01_01_02_06();
                break;
            }
        }
    }

    case PAGE_01_02_01:
    {
        break;
    }

    case PAGE_01_02_02:
    {
        break;
    }

    case PAGE_SENDER_UNLOCK:
    {
        break;
    }

    case PAGE_01_02_03:
    {
        if (key == KEY_SWITCH
                || key == KEY_BACK)
            GoPage_01();
        break;;
    }

    case PAGE_01_02_04:
    {
        if (key == KEY_SWITCH
                || key == KEY_BACK)
            GoPage_01();
        break;
    }

    case PAGE_01_03_01:
    {
        break;
    }

    case PAGE_01_03_02:
    {
        if (key == KEY_SWITCH || key == KEY_BACK)
            GoPage_01();
        break;
    }

    case PAGE_01_05_01:
    {
        if (key == KEY_SWITCH || key == KEY_BACK)
        {
            GoPage_01();
            ClearInBuffer();
        }
        else if (key == KEY_SET)
        {
            DeleteInBufferLast();
            Disp_UpdateUIDBox(_inBuf);
        }
        else if (key == KEY_OK)
        {
            DbgPrint("input: %s\r\n", _inBuf);

            ret = DevMng_GetGeneralUserInfo(_inBuf, &userInfo);
            if (ret == 0)
            {
                ret = DevMng_SetReceiverID(userInfo.UID);
                if (ret == 0)
                {
                    DevMng_AddLog(NULL, "set receiver %s", userInfo.UID);
                    GoPage_01_05_01_01(userInfo.UID);
                }
                else
                {
                    Disp_OnSetReceiverFailed();
                }
            }
            else
            {
                Disp_OnSetReceiverFailed();
            }
            ClearInBuffer();
            Disp_UpdateUIDBox(_inBuf);
        }
        else
        {
            if (AddNumToInBuffer(key, UID_SZ_MAX))
                Disp_UpdateUIDBox(_inBuf);
        }
        break;
    }

    case PAGE_01_01_02_01:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02();
        }
        else if (key == KEY_SET)
        {
            if (++disp_activeItem_01_01_02_01 > 2)
                disp_activeItem_01_01_02_01 = 1;

            GoPage_01_01_02_01();
        }
        else if (key == KEY_OK)
        {
            switch (disp_activeItem_01_01_02_01)
            {
            case 1:
                GoPage_01_01_02_01_01();
                break;
            case 2:
                GoPage_01_01_02_01_02();
                break;
            default:
                break;
            }
        }
        break;
    }

    case PAGE_01_01_02_02:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02();
        }
        else if (key == KEY_SET)
        {
            if (++disp_activeItem_01_01_02_02 > 2)
                disp_activeItem_01_01_02_02 = 1;

            GoPage_01_01_02_02();
        }
        else if (key == KEY_OK)
        {
            switch (disp_activeItem_01_01_02_02)
            {
            case 1:
                GoPage_01_01_02_02_01();
                break;
            case 2:
                GoPage_01_01_02_02_02();
                break;
            default:
                break;
            }
        }
        break;
    }

    case PAGE_01_01_02_03:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02();
        }
        else if (key == KEY_SET)
        {
            if (++disp_activeItem_01_01_02_03 > 4)
                disp_activeItem_01_01_02_03 = 1;

            GoPage_01_01_02_03();
        }
        else if (key == KEY_OK)
        {
            switch (disp_activeItem_01_01_02_03)
            {
            case 1:
                GoPage_01_01_02_03_01();
                break;
            case 2:
                GoPage_01_01_02_03_02();
                break;
            case 3:
                GoPage_01_01_02_03_03();
                break;
            case 4:
                GoPage_01_01_02_03_04();
                break;
            default:
                break;
            }
        }
        break;
    }

    case PAGE_01_01_02_04:
    {
        if (key == KEY_SWITCH)
            GoPage_01();
        else if (key == KEY_BACK)
            GoPage_01_01_02();

        break;
    }

    case PAGE_01_01_02_05:
    {
        if (key == KEY_SWITCH)
            GoPage_01();
        else if (key == KEY_BACK)
            GoPage_01_01_02();
        break;
    }

    case PAGE_01_01_02_06:
    {
        if (key == KEY_SWITCH)
            GoPage_01();
        else if (key == KEY_BACK)
            GoPage_01_01_02();
        break;
    }

    case PAGE_01_02_01_01:
    {
        if (key == KEY_SWITCH
                || key == KEY_BACK)
            GoPage_01();
        break;
    }

    case PAGE_01_05_01_01:
    {
        if (key == KEY_SWITCH
                || key == KEY_BACK)
            GoPage_01();
        break;
    }

    case PAGE_01_01_02_01_01:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
            ClearInBuffer();
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02_01();
            ClearInBuffer();
        }
        else if (key == KEY_OK)
        {
            DbgPrint("input: %s\r\n", _inBuf);

            if (_inLen >= 4)
            {
                ret = DevMng_UpdateUnlockPassword(_inBuf);
                if (ret != 0)
                {
                    Disp_OnPasswordInputError();
                }
                else
                {
                    GoPage_01_01_02_01();
                }
            }
            else
            {
                Disp_OnPasswordInputError();
            }

            ClearInBuffer();
        }
        else
        {
            if (AddNumToInBuffer(key, PWD_SZ_MAX))
                Disp_UpdatePasswordBox(_inBuf);
        }
        break;
    }

    case PAGE_01_01_02_01_02:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
            ClearInBuffer();
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02_01();
            ClearInBuffer();
        }
        else if (key == KEY_OK)
        {
            DbgPrint("input: %s\r\n", _inBuf);

            if (_inLen >= 4)
            {
                ret = DevMng_UpdateAdminPassword(_inBuf);
                if (ret != 0)
                    Disp_OnPasswordInputError();
                else
                    GoPage_01_01_02_01();
            }
            else
            {
                Disp_OnPasswordInputError();
            }
            ClearInBuffer();
        }
        else
        {
            if (AddNumToInBuffer(key, PWD_SZ_MAX))
                Disp_UpdatePasswordBox(_inBuf);
        }
        break;
    }

    case PAGE_01_01_02_02_01:
    case PAGE_01_01_02_02_02:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
            ClearInBuffer();
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02_02();
            ClearInBuffer();
        }
        else if (key == KEY_SET)
        {
            DeleteInBufferLast();
            Disp_UpdateUIDBox(_inBuf);
        }
        else if (key == KEY_OK)
        {
            DbgPrint("input: %s\r\n", _inBuf);

            ret = DevMng_GetGeneralUserInfo(_inBuf, &userInfo);
            if (ret != 0)
            {
                Disp_OnUIDInputError();
            }
            else
            {
                if (disp_curPage == PAGE_01_01_02_02_01)
                {
                    StartAddFP(_inBuf);
                }
                else
                {
                    BSP_FP_WakeUp();
                    ret = BSP_FP_DeleteFP(userInfo.FPID);
                    BSP_FP_Sleep();

                    if (ret == 0)
                        ret = DevMng_InvalidateUserFP(&userInfo);

                    if (ret != 0)
                        GoPage_DeleteFPFail();
                    else
                        GoPage_DeleteFPOk();
                }
            }
            ClearInBuffer();
        }
        else
        {
            if (AddNumToInBuffer(key, UID_SZ_MAX))
                Disp_UpdateUIDBox(_inBuf);
        }
        break;
    }

    case PAGE_READ_FP_STEP1:
    case PAGE_READ_FP_STEP2:
    case PAGE_READ_FP_STEP3:
    {
        if (key == KEY_SWITCH)
        {
            StopAddFP();
            GoPage_01();

        }
        else if (key == KEY_BACK)
        {
            StopAddFP();
            GoPage_01_01_02_02();
        }
        break;
    }

    case PAGE_ADD_FP_OK:
    case PAGE_ADD_FP_FAIL:
    case PAGE_DELETE_FP_OK:
    case PAGE_DELETE_FP_FAIL:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02_02(); // 返回
        }
        break;
    }

    case PAGE_01_01_02_03_01:
    case PAGE_01_01_02_03_02:
    case PAGE_01_01_02_03_03:
    case PAGE_01_01_02_03_04:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
            ClearInBuffer();
        }
        else if (key == KEY_SET)
        {
            DeleteInBufferLast();
            Disp_UpdateUIDBox(_inBuf);
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02_03();
            ClearInBuffer();
        }
        else if (key == KEY_OK)
        {
            DbgPrint("input: %s\r\n", _inBuf);

            ret = DevMng_GetGeneralUserInfo(_inBuf, &userInfo);
            if (ret != 0)
            {
                Disp_OnUIDInputError();
            }
            else
            {
                if (disp_curPage == PAGE_01_01_02_03_01)
                {
                    StartAddCard(true, _inBuf);
                }
                else if (disp_curPage == PAGE_01_01_02_03_03)
                {
                    StartAddCard(false, _inBuf);
                }
                else if (disp_curPage == PAGE_01_01_02_03_02)
                {
                    ret = DevMng_InvalidateUserMajorCardNO(&userInfo);
                    if (ret != 0)
                        GoPage_DeleteCardFail();
                    else
                        GoPage_DeleteCardOK();
                }
                else if (disp_curPage == PAGE_01_01_02_03_04)
                {
                    ret = DevMng_InvalidateUserMinorCardNO(&userInfo);
                    if (ret != 0)
                        GoPage_DeleteCardFail();
                    else
                        GoPage_DeleteCardOK();
                }
            }

            ClearInBuffer();
        }
        else
        {
            if (AddNumToInBuffer(key, UID_SZ_MAX))
                Disp_UpdateUIDBox(_inBuf);
        }
        break;
    }

    case PAGE_READ_CARD:
    {
        if (key == KEY_SWITCH)
        {
            StopAddCard();
            GoPage_01();
        }
        else if (key == KEY_BACK)
        {
            StopAddCard();
            GoPage_01_01_02_03(); // 返回上一页
        }
        break;
    }

    case PAGE_ADD_CARD_OK:
    case PAGE_ADD_CARD_FAIL:
    case PAGE_DELETE_CARD_OK:
    case PAGE_DELETE_CARD_FAIL:
    {
        if (key == KEY_SWITCH)
        {
            GoPage_01();
        }
        else if (key == KEY_BACK)
        {
            GoPage_01_01_02_03(); // 返回上一页
        }
        break;
    }

    default:
    {
        break;
    }

    }
}


void HandleInputedPassword(void)
{
    bool inputError = false;

    if (disp_curPage == PAGE_01_01
            || (_lastPage == PAGE_01_01
                && disp_curPage == PAGE_01_01_01))
    {
        if (DevMng_IsAdminPassword(_inBuf))
        {
            GoPage_01_01_02();
            DevMng_AddLog("", "admin logined");
        }
        else
        {
            inputError = true;
        }
    }
    else if (disp_curPage == PAGE_01_03
             || (_lastPage == PAGE_01_03
                 && disp_curPage == PAGE_01_01_01))
    {
        if (DevMng_IsUnlockPassword(_inBuf))
        {
            GoPage_01_03_01();
        }
        else
        {
            inputError = true;
        }
    }
    else if (disp_curPage == PAGE_01_06
             || (_lastPage == PAGE_01_06
                 && disp_curPage == PAGE_01_01_01))
    {
        if (DevMng_IsAdminPassword(_inBuf))
        {
            GoPage_01_05_01();
        }
        else
        {
            inputError = true;
        }
    }

    if (inputError)
    {
        GoPage_01_01_01();
        Disp_OnPasswordInputError();
    }
}


bool IsNumKey(const KeyCode key, char *val)
{
    switch (key)
    {
    case KEY_NUM0:
        *val = '0';
        return true;

    case KEY_NUM1:
        *val = '1';
        return true;

    case KEY_NUM2:
        *val = '2';
        return true;

    case KEY_NUM3:
        *val = '3';
        return true;

    case KEY_NUM4:
        *val = '4';
        return true;

    case KEY_NUM5:
        *val = '5';
        return true;

    case KEY_NUM6:
        *val = '6';
        return true;

    case KEY_NUM7:
        *val = '7';
        return true;

    case KEY_NUM8:
        *val = '8';
        return true;

    case KEY_NUM9:
        *val = '9';
        return true;

    default:
        return false;
    }
}


const char* GetKeyName(const KeyCode key)
{
    switch (key)
    {
    case KEY_NUM0:
        return "0";
    case KEY_NUM1:
        return "1";
    case KEY_NUM2:
        return "2";
    case KEY_NUM3:
        return "3";
    case KEY_NUM4:
        return "4";
    case KEY_NUM5:
        return "5";
    case KEY_NUM6:
        return "6";
    case KEY_NUM7:
        return "7";
    case KEY_NUM8:
        return "8";
    case KEY_NUM9:
        return "9";
    case KEY_BACK:
        return "返回";
    case KEY_LOCK:
        return "锁定";
    case KEY_OK:
        return "确定";
    case KEY_SET:
        return "设置";
    case KEY_SWITCH:
        return "开关";
    case KEY_UNLOCK:
        return "解锁";
    default:
        return "";
    }
}


void DeleteInBufferLast(void)
{
    if (_inLen > 0)
        _inBuf[--_inLen] = 0;
}


bool AddNumToInBuffer(KeyCode key, int lenMax)
{
    if (IsNumKey(key, &_inBuf[_inLen])
            && _inLen < lenMax)
    {
        _inBuf[++_inLen] = 0;
        return true;
    }

    return false;
}


void ClearInBuffer(void)
{
    _inBuf[0] = 0;
    _inLen = 0;
}
