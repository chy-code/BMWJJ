
#include <string.h>
#include "cmsis_os2.h"
#include "Buzzer.h"
#include "FP_Reader.h"
#include "RFID_Reader.h"
#include "RTC_PCF8563.h"

#include "AlertUtil.h"
#include "StringUtil.h"
#include "FileSysUtil.h"
#include "MPUtil.h"
#include "DeviceMng.h"
#include "Display.h"
#include "DbgPrint.h"


// 消息处理状态
typedef enum
{
    msgHandled,	// 命令消息已处理
    msgInvParam	// 命令消息中包含有无效参数
} MPStatus;


/*------------------------------------------------------------------
* 宏定义
*------------------------------------------------------------------*/
#define RESULT_OK		0
#define RESULT_NOPERM	300
#define RESULT_ERROR	500

#define K_CODE			"code"
#define K_MESSAGE		"message"
#define K_DATA			"data"
#define K_DEV_CODE		"dev_code"
#define K_TIME			"time"
#define K_INVESTOR		"investor"
#define K_LOCATION		"location"
#define K_LOG			"log"
#define K_GPS			"gps"
#define K_TIME			"time"
#define K_USER_CODE		"user_code"
#define K_OPT			"opt"

#define S_OK			"OK"
#define S_INV_PARAM		"invalid parameter"


/*------------------------------------------------------------------
* 处理命令 read_state
*------------------------------------------------------------------*/
static MPStatus ReadState(void)
{
    DbgPrint("=========== read_state\r\n");
    SetResponse("{\ns:i,\ns:s,\n}",
                K_CODE, RESULT_OK,
                K_MESSAGE, S_OK);

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 read_description
*------------------------------------------------------------------*/
static MPStatus ReadDescription(void)
{
    int ret;
    DeviceInfo devInfo;

    DbgPrint("=========== read_description\r\n");

    ret = DevMng_GetDeviceInfo(&devInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\ns:{\ns:s,\ns:s,\ns:s,\ns:s\n}\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK,
                    K_DATA,
                    K_DEV_CODE, devInfo.dev_code,
                    K_TIME, TimeToString(&devInfo.time),
                    K_INVESTOR, devInfo.investor,
                    K_LOCATION, devInfo.location);
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 read_card
*------------------------------------------------------------------*/
static MPStatus ReadCard(void)
{
    int ret;
    cardno_t cardNO;

    DbgPrint("=========== read_card\r\n");

    ret = BSP_RFID_GetCardNO(cardNO);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, BSP_RFID_StrError(ret));
    }
    else
    {
        Alert_OnReadCardOK();

        SetResponse("{\ns:i,\ns:s,\ns:s\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK,
                    K_DATA, cardNO);
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 read_finger
*------------------------------------------------------------------*/
static MPStatus ReadFinger(void)
{
    int ret = 0;
    uint16_t fpid;
    bool touched;
    char idbuf[10];

    DbgPrint("=========== read_finger\r\n");

    BSP_FP_WakeUp();
    Alert_OnStartReadFinger();

    for (int step = 1; step <= 3; step++)
    {
        ret = BSP_FP_RegisterFPStep(step, &fpid);
        if (ret != 0)
            break;

        Alert_OnReadFingerOK();

        while (1)
        {
            ret = BSP_FP_GetFingerStatus(&touched);
            if (ret != 0)
                break;

            if (!touched)
                break;

            osDelay(100);
        }
    }

    BSP_FP_Sleep();

    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, BSP_FP_StrError(ret));
    }
    else
    {
        sprintf(idbuf, "%04X", fpid);

        SetResponse("{\ns:i,\ns:s,\ns:s\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK,
                    K_DATA, idbuf);
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 write_admin
*------------------------------------------------------------------*/
static MPStatus WriteAdmin(void)
{
    int ret;
    uint8_t idbuf[2];
    UserInfo userInfo;

    DbgPrint("=========== write_admin\r\n");

    if (! SkipNextString("&data={user_code:"))
        return msgInvParam;
    if (! GetNumber(userInfo.UID, UID_SZ_MAX))
        return msgInvParam;

    if (! SkipNextString(",card:"))
        return msgInvParam;
    if (! GetNumber(userInfo.MajorCardNO, CARDNO_SZ_MAX))
        return msgInvParam;

    if (! SkipNextString(",secard:"))
        return msgInvParam;
    if (! GetNumber(userInfo.MinorCardNO, CARDNO_SZ_MAX))
        return msgInvParam;

    if (! SkipNextString(",finger:"))
        return msgInvParam;
    if (! GetBytes(idbuf, 2))
        return msgInvParam;

    userInfo.FPID = (idbuf[0] << 8) + idbuf[1];
    userInfo.flags = UF_ALL_VALID;

    if (! SkipNextString("}"))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tuser_code: %s\r\n", userInfo.UID);
    DbgPrint("\tcard: %s\r\n", userInfo.MajorCardNO);
    DbgPrint("\tsecard: %s\r\n", userInfo.MinorCardNO);
    DbgPrint("\tfinger: %d\r\n", userInfo.FPID);

    ret = DevMng_AddUpdateAdminInfo(&userInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK);
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 write_addrbook
*------------------------------------------------------------------*/
static MPStatus WriteAddrBook(void)
{
    static UserInfo users[100];
    int userCnt = 0;
    int i, ret;
    UserInfo adminInfo;
    uid_t adminID;
    uint8_t idbuf[2];

    DbgPrint("=========== write_addrbook\r\n");

    if (! SkipNextString("&data=["))
        return msgInvParam;

    while (1)
    {
        if (! SkipNextString("{user_code:"))
            return msgInvParam;
        if (! GetNumber(users[userCnt].UID, UID_SZ_MAX))
            return msgInvParam;

        if (! SkipNextString(",card:"))
            return msgInvParam;
        if (! GetNumber(users[userCnt].MajorCardNO, CARDNO_SZ_MAX))
            return msgInvParam;

        if (! SkipNextString(",secard:"))
            return msgInvParam;
        if (! GetNumber(users[userCnt].MinorCardNO, CARDNO_SZ_MAX))
            return msgInvParam;

        if (! SkipNextString(",finger:"))
            return msgInvParam;
        if (! GetBytes(idbuf, 2))
            return msgInvParam;

        users[userCnt].FPID = (idbuf[0] << 8) + idbuf[1];
        users[userCnt].flags = UF_ALL_VALID;

        DbgPrint("\tparameter %d: \r\n", userCnt);
        DbgPrint("\tuser_code: %s\r\n", users[userCnt].UID);
        DbgPrint("\tcard: %s\r\n", users[userCnt].MajorCardNO);
        DbgPrint("\tsecard: %s\r\n", users[userCnt].MinorCardNO);
        DbgPrint("\tfinger: %d\r\n", users[userCnt].FPID);

        userCnt++;

        if (SkipNextString("}]"))
            break;

        if (!SkipNextString("},"))
            return msgInvParam;

        if (userCnt >= 100)
            return msgInvParam;
    }

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("\tadmin_code:%s\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    for (i = 0; i < userCnt; i++)
    {
        ret = DevMng_AddUpdateGeneralUserInfo(&users[i]);
        if (ret != 0)
        {
            SetResponse("{\ns:i,\ns:s,\n}",
                        K_CODE, RESULT_ERROR,
                        K_MESSAGE, devmng_lastError);
            return msgHandled;
        }
    }

    SetResponse("{\ns:i,\ns:s,\n}",
                K_CODE, RESULT_OK,
                K_MESSAGE, S_OK);

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 write_description
*------------------------------------------------------------------*/
static MPStatus WriteDescription(void)
{
    int ret;
    char tmstr[20];
    uid_t adminID;
    UserInfo adminInfo;
    DeviceInfo devinfo;

    DbgPrint("=========== write_description\r\n");

    if (! SkipNextString("&data={dev_code:"))
        return msgInvParam;

    if (! GetStringValue(devinfo.dev_code, sizeof(devinfo.dev_code), ','))
        return msgInvParam;

    if (! SkipNextString("time:"))
        return msgInvParam;
    if (! GetStringValue(tmstr, sizeof(tmstr), ','))
        return msgInvParam;

    if (!StringToTime(tmstr, &devinfo.time))
        return msgInvParam;

    if (! SkipNextString("investor:"))
        return msgInvParam;
    if (! GetStringValue(devinfo.investor, sizeof(devinfo.investor), ','))
        return msgInvParam;

    if (! SkipNextString("location:"))
        return msgInvParam;
    if (! GetStringValue(devinfo.location, sizeof(devinfo.location), '}'))
        return msgInvParam;

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;


    DbgPrint("parameters: \r\n");
    DbgPrint("\tdev_code: %s\r\n", devinfo.dev_code);
    DbgPrint("\ttime: %s\r\n", tmstr);
    DbgPrint("\tinvestor: %s\r\n", devinfo.investor);
    DbgPrint("\tlocation: %d\r\n", devinfo.location);
    DbgPrint("\tadmin_code: %d\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    BSP_RTC_SetTime(&devinfo.time);

    ret = DevMng_SetDeviceInfo(&devinfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK);
    }

    return msgHandled;
}



/*------------------------------------------------------------------
* 处理命令 auth_sender
*------------------------------------------------------------------*/
static MPStatus AuthSender(void)
{
    int ret;
    uid_t senderID;
    uid_t adminID;
    UserInfo userInfo;

    DbgPrint("=========== auth_sender\r\n");

    if (! SkipNextString("&user_code:"))
        return msgInvParam;
    if (! GetNumber(senderID, UID_SZ_MAX))
        return msgInvParam;

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tuser_code: %s\r\n", senderID);
    DbgPrint("\tadmin_code: %s\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &userInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    ret = DevMng_GetGeneralUserInfo(senderID, &userInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }


    ret = DevMng_SetSenderID(senderID);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK);

        DevMng_AddLog(adminID, "set sender %s", senderID);
        GoPage_01();
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 auth_receiver
*------------------------------------------------------------------*/
static MPStatus AuthReceiver(void)
{
    int ret;
    uid_t receiverID;
    uid_t adminID;
    UserInfo userInfo;

    DbgPrint("=========== auth_receiver\r\n");

    if (! SkipNextString("&user_code:"))
        return msgInvParam;
    if (! GetNumber(receiverID, UID_SZ_MAX))
        return msgInvParam;

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tuser_code: %s\r\n", receiverID);
    DbgPrint("\tadmin_code: %s\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &userInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    ret = DevMng_GetGeneralUserInfo(receiverID, &userInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    ret = DevMng_SetReceiverID(receiverID);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK);

        DevMng_AddLog(adminID, "set receiver %s", receiverID);
        GoPage_01();
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 auth_remove
*------------------------------------------------------------------*/
static MPStatus AuthRemove(void)
{
    int ret;
    uid_t adminID;
    UserInfo adminInfo;

    DbgPrint("=========== auth_remove\r\n");

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tadmin_code: %s\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    ret = DevMng_UnsetSenderAndReceiver();
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK);

        DevMng_AddLog(adminID, "unset sender and receiver");
        GoPage_01();
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 output_gps
*------------------------------------------------------------------*/
static MPStatus OutputGPS(void)
{
    int ret;
    uid_t adminID;
    UserInfo adminInfo;
    DeviceInfo devInfo;
    static char xval[16];
    static char yval[16];

    DbgPrint("=========== output_gps\r\n");

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tadmin_code: %s\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    FILE *stream;
    ret = DevMng_GetGeoLocDataStream(&stream);
    if (ret == 0)
    {
        DevMng_GetDeviceInfo(&devInfo);

        SetResponse("{\ns:i,\ns:s,\ns:{\ns:s,\ns:[\n",
                    K_CODE, 0,
                    K_MESSAGE, S_OK,
                    K_DATA,
                    K_DEV_CODE, devInfo.dev_code,
                    K_GPS);

        GeoLoc loc;
        while (DevMng_GetNextGeoLoc(stream, &loc))
        {
            sprintf(xval, "%f", loc.longitude);
            sprintf(yval, "%f", loc.latitude);
            SetResponse("{\ns:s,\ns:s,\ns:s\n}\n,",
                        "x", xval, "y", yval,
                        K_TIME, TimeToString(&loc.time));
        }
        SetResponse("]\n}\n}\n");
        fclose(stream);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 output_log
*------------------------------------------------------------------*/
static MPStatus OutputLog(void)
{
    int ret;
    uid_t adminID;
    UserInfo adminInfo;

    DbgPrint("=========== output_log\r\n");

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tadmin_code: %s\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    FILE *stream;
    ret = DevMng_GetLogDataStream(&stream);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    DeviceInfo devInfo;
    DevMng_GetDeviceInfo(&devInfo);

    SetResponse("{\ns:i,\ns:s,\ns:{\ns:s,\ns:[\n",
                K_CODE, 0,
                K_MESSAGE, S_OK,
                K_DATA,
                K_DEV_CODE, devInfo.dev_code,
                K_LOG);
    LogEntry entry;
    while (DevMng_GetNextLogEntry(stream, &entry))
    {
        SetResponse("{\ns:s,\ns:s,\ns:s\n}\n,",
                    K_TIME, TimeToString(&entry.time),
                    K_USER_CODE, entry.operatorID,
                    K_OPT, entry.opt);
    }
    SetResponse("]\n}\n}\n");
    fclose(stream);

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 clear_gps
*------------------------------------------------------------------*/
static MPStatus ClearGPS(void)
{
    int ret;
    uid_t adminID;
    UserInfo adminInfo;

    DbgPrint("=========== clear_gps\r\n");

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    ret = DevMng_DeleteGeoLocData();
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK);
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 clear_log
*------------------------------------------------------------------*/
static MPStatus ClearLog(void)
{
    int ret;
    uid_t adminID;
    UserInfo adminInfo;

    DbgPrint("=========== clear_log\r\n");

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tadmin_code: %s\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    ret = DevMng_DeleteLogData();
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK);
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 clear_admin
*------------------------------------------------------------------*/
static MPStatus ClearAdmin(void)
{
    int ret;
    uid_t userID;
    uid_t adminID;
    UserInfo adminInfo;

    DbgPrint("=========== clear_admin\r\n");

    if (! SkipNextString("&user_code="))
        return msgInvParam;
    if (! GetNumber(userID, UID_SZ_MAX))
        return msgInvParam;

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tuser_code: %s\r\n", userID);
    DbgPrint("\tadmin_code: %s\r\n", adminID);

    if (strcmp(userID, adminID) == 0)
        return msgInvParam;

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    ret = DevMng_DeleteAdmin(userID);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, S_OK);

        DevMng_AddLog(adminID, "delete admin %s", userID);
    }

    return msgHandled;
}


/*------------------------------------------------------------------
* 处理命令 clear_addrbook
*------------------------------------------------------------------*/
static MPStatus ClearAddrBook(void)
{
    int ret;
    uid_t adminID;
    UserInfo adminInfo;

    DbgPrint("=========== clear_addrbook\r\n");

    if (! SkipNextString("&admin_code="))
        return msgInvParam;
    if (! GetNumber(adminID, UID_SZ_MAX))
        return msgInvParam;

    DbgPrint("parameters: \r\n");
    DbgPrint("\tadmin_code: %s\r\n", adminID);

    ret = DevMng_GetAdminInfo(adminID, &adminInfo);
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_ERROR,
                    K_MESSAGE, devmng_lastError);
        return msgHandled;
    }

    ret = DevMng_DeleteAllUsers();
    if (ret != 0)
    {
        SetResponse("{\ns:i,\ns:s,\n}",
                    K_CODE, RESULT_OK,
                    K_MESSAGE, devmng_lastError);
    }
    else
    {
        BSP_FP_WakeUp();
        ret = BSP_FP_DeleteAllFPs();
        BSP_FP_Sleep();

        if (ret != 0)
        {
            SetResponse("{\ns:i,\ns:s,\n}",
                        K_CODE, RESULT_OK,
                        K_MESSAGE, BSP_FP_StrError(ret));
        }
        else
        {
            SetResponse("{\ns:i,\ns:s,\n}",
                        K_CODE, RESULT_OK,
                        K_MESSAGE, S_OK);
        }

        DevMng_AddLog(adminID, "delete all users");
        GoPage_01();
    }

    return msgHandled;
}



/*------------------------------------------------------------------
* 消息处理线程. 用于从 HID0 获取命令并执行.
*------------------------------------------------------------------*/

void MessageProc(void *arg)
{
    static char name[30];
    MPStatus status;

    while (1)
    {
        if (! GetName(name, sizeof(name)))
        {
            osDelay(500);
            continue;
        }

        if (strcmp(name, "read_state") == 0)
        {
            status = ReadState();
        }
        else if (strcmp(name, "read_description") == 0)
        {
            status = ReadDescription();
        }
        else if (strcmp(name, "read_card") == 0)
        {
            status = ReadCard();
        }
        else if (strcmp(name, "read_finger") == 0)
        {
            status = ReadFinger();
        }
        else if (strcmp(name, "write_admin") == 0)
        {
            status = WriteAdmin();
        }
        else if (strcmp(name, "write_addrbook") == 0)
        {
            status = WriteAddrBook();
        }
        else if (strcmp(name, "write_description") == 0)
        {
            status = WriteDescription();
        }
        else if (strcmp(name, "auth_sender") == 0)
        {
            status = AuthSender();
        }
        else if (strcmp(name, "auth_receiver") == 0)
        {
            status = AuthReceiver();
        }
        else if (strcmp(name, "auth_remove") == 0)
        {
            status = AuthRemove();
        }
        else if (strcmp(name, "output_gps") == 0)
        {
            status = OutputGPS();
        }
        else if (strcmp(name, "output_log") == 0)
        {
            status = OutputLog();
        }
        else if (strcmp(name, "clear_gps") == 0)
        {
            status = ClearGPS();
        }
        else if (strcmp(name, "clear_log") == 0)
        {
            status = ClearLog();
        }
        else if (strcmp(name, "clear_admin") == 0)
        {
            status = ClearAdmin();
        }
        else if (strcmp(name, "clear_addrbook") == 0)
        {
            status = ClearAddrBook();
        }
        else
        {
            continue;
        }

        switch (status)
        {
        case msgInvParam:
            SetResponse("{\ns:i,\ns:s,\n}",
                        K_CODE, RESULT_ERROR,
                        K_MESSAGE, S_INV_PARAM);
            break;
        default:
            break;
        }

        FlushResponse();
    }
}

