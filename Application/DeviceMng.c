
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "RTC_PCF8563.h"
#include "DeviceMng.h"
#include "Display.h"
#include "DbgPrint.h"
#include "FileSysUtil.h"

/*------------------------------------------------------------------
* 常量定义
*------------------------------------------------------------------*/
#define DIR_GENERAL		"general"
#define DIR_ADMIN		"admin"

#define FN_SENDER		"sender.dat"
#define FN_RECEIVER		"receiver.dat"
#define FN_DEVICE		"device.dat"
#define FN_PASSWORD		"pwd.dat"

#define FN_LOGDATA		"logdata.dat"
#define FN_LOGCNT		"logcnt.dat"

#define FN_LOCDATA	"gpsdata.dat"


// 一些错误描述
#define USER_NOT_EXIST		"用户不存在"
#define ADMIN_NOT_EXIST		"管理员不存在"

#define PWD_ADMIN_DEFAULT	"666666"
#define PWD_UNLOCK_DEFAULT	"666666"


/*------------------------------------------------------------------
* 私有变量定义
*------------------------------------------------------------------*/

static char filePathTemp[128]; // 临时变量
static char patternTemp[32];	// 临时变量
static fsFileInfo fileInfoTemp; // 临时变量

static uint8_t _devStat = 0;
static uid_t _senderID;
static uid_t _receiverID;
static PWDSet _passwords;

char devmng_lastError[128];


/*------------------------------------------------------------------
* 辅助函数定义
*------------------------------------------------------------------*/

static __forceinline
const char* GetUserFileName(
    const uid_t userID,
    const char *dir)
{
    sprintf(filePathTemp, "%s\\%s.dat", dir, userID);
    return filePathTemp;
}


/*------------------------------------------------------------------
* 初始化设备管理
*------------------------------------------------------------------*/

int DevMng_Init(void)
{
    int ret;
    fsStatus status;
    UserInfo userInfo;;

    status = FileSys_ReadBinary(FN_PASSWORD,
                                &_passwords,
                                sizeof(PWDSet));
    if (status != fsOK)
    {
        if (status != fsFileNotFound)
        {
            strcpy(devmng_lastError, FileSys_StrError(status));
            return ERROR_FILE_SYSTEM;
        }

        // 设置默认密码
        sprintf(_passwords.admin, PWD_ADMIN_DEFAULT);
        sprintf(_passwords.unlock, PWD_UNLOCK_DEFAULT);

        status = FileSys_WriteBinary(FN_PASSWORD,
                                     &_passwords,
                                     sizeof(PWDSet));
        if (status != fsOK)
        {
            strcpy(devmng_lastError, FileSys_StrError(status));
            return ERROR_FILE_SYSTEM;
        }
    }

    ret = DevMng_GetSenderInfo(&userInfo);
    if (ret == 0)
    {
        _devStat |= DEVICE_SENDER_SPEC;
        strcpy(_senderID, userInfo.UID);
    }

    ret = DevMng_GetReceiverInfo(&userInfo);
    if (ret == 0)
    {
        _devStat |= DEVICE_RECEIVER_SPEC;
        strcpy(_receiverID, userInfo.UID);
    }

    return 0;
}



int DevMng_GetStatus(void)
{
    return _devStat;
}


int DevMng_AddUpdateGeneralUserInfo(const UserInfo *userInfo)
{
    const char *filename = GetUserFileName(userInfo->UID, DIR_GENERAL);
    fsStatus status =  FileSys_WriteBinary(filename, userInfo, sizeof(UserInfo));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}


int DevMng_GetGeneralUserInfo(const uid_t userID, UserInfo *userInfo)
{
    const char *filename = GetUserFileName(userID, DIR_GENERAL);
    fsStatus status = FileSys_ReadBinary(filename, userInfo, sizeof(UserInfo));
    if (status != fsOK)
    {
        memset(userInfo, 0, sizeof(UserInfo));

        if (status == fsFileNotFound)
        {
            strcpy(devmng_lastError, USER_NOT_EXIST);
            return ERROR_USER_NOT_EXISTS;
        }

        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}


int DevMng_AddUpdateAdminInfo(const UserInfo *userInfo)
{
    const char *filename = GetUserFileName(userInfo->UID, DIR_ADMIN);
    fsStatus status = FileSys_WriteBinary(filename, userInfo, sizeof(UserInfo));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}


int DevMng_GetAdminInfo(const uid_t userID, UserInfo *userInfo)
{
    const char *filename = GetUserFileName(userID, DIR_ADMIN);
    fsStatus status = FileSys_ReadBinary(filename, userInfo, sizeof(UserInfo));
    if (status != fsOK)
    {
        memset(userInfo, 0, sizeof(UserInfo));

        if (status == fsFileNotFound)
        {
            strcpy(devmng_lastError, ADMIN_NOT_EXIST);
            return ERROR_USER_NOT_EXISTS;
        }

        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}


int DevMng_GetAdminInfoByCardNO(const cardno_t cardNO, UserInfo *userInfo)
{
    fsStatus status;
    sprintf(patternTemp, "%s\\*.dat", DIR_ADMIN);
    memset(&fileInfoTemp, 0, sizeof(fsFileInfo));

    while (ffind (patternTemp, &fileInfoTemp) == fsOK)
    {
        sprintf(filePathTemp, "%s\\%s", DIR_ADMIN, fileInfoTemp.name);
        status = FileSys_ReadBinary(filePathTemp, userInfo, sizeof(UserInfo));
        if (status != fsOK)
        {
            strcpy(devmng_lastError, FileSys_StrError(status));
            return ERROR_FILE_SYSTEM;
        }

        if ( (userInfo->flags & UF_MAJOR_CARD_VALID)
                && strcmp(userInfo->MajorCardNO, cardNO) == 0)
        {
            return 0;
        }

        if ( (userInfo->flags & UF_MINOR_CARD_VALID)
                && strcmp(userInfo->MinorCardNO, cardNO) == 0)
        {
            return 0;
        }
    }

    strcpy(devmng_lastError, USER_NOT_EXIST);

    return ERROR_USER_NOT_EXISTS;
}


int DevMng_GetAdminInfoByFP(int FPID, UserInfo *userInfo)
{
    fsStatus status;
    sprintf(patternTemp, "%s\\*.dat", DIR_ADMIN);
    memset(&fileInfoTemp, 0, sizeof(fsFileInfo));

    while (FileSys_Find(patternTemp, &fileInfoTemp) == fsOK)
    {
        sprintf(filePathTemp, "%s\\%s", DIR_ADMIN, fileInfoTemp.name);
        status = FileSys_ReadBinary(filePathTemp, userInfo, sizeof(UserInfo));
        if (status != fsOK)
        {
            strcpy(devmng_lastError, FileSys_StrError(status));
            return ERROR_FILE_SYSTEM;
        }

        if ( (userInfo->flags & UF_FP_VALID)
                && userInfo->FPID == FPID)
        {
            return 0;
        }
    }

    strcpy(devmng_lastError, USER_NOT_EXIST);
    return ERROR_USER_NOT_EXISTS;
}


const char* DevMng_GetSenderID(void)
{
    return _senderID;
}


const char* DevMng_GetReceiverID(void)
{
    return _receiverID;
}


int DevMng_SetSenderID(const uid_t senderID)
{
    fsStatus status = FileSys_WriteBinary(FN_SENDER, senderID, sizeof(uid_t));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    _devStat |= DEVICE_SENDER_SPEC;
    strcpy(_senderID, senderID);

    return 0;
}


int DevMng_GetSenderInfo(UserInfo *senderInfo)
{
    uid_t userID;
    fsStatus status = FileSys_ReadBinary(FN_SENDER, userID, sizeof(uid_t));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return DevMng_GetGeneralUserInfo(userID, senderInfo);
}


int DevMng_SetReceiverID(const uid_t receiverID)
{
    fsStatus status = FileSys_WriteBinary(FN_RECEIVER, receiverID, sizeof(uid_t));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    _devStat |= DEVICE_RECEIVER_SPEC;
    strcpy(_receiverID, receiverID);

    return 0;
}


int DevMng_GetReceiverInfo(UserInfo *receiverInfo)
{
    uid_t userID;
    fsStatus status = FileSys_ReadBinary(FN_RECEIVER, userID, sizeof(uid_t));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return DevMng_GetGeneralUserInfo(userID, receiverInfo);
}


int DevMng_UnsetSenderAndReceiver(void)
{
    fsStatus status = FileSys_DeleteFile(FN_SENDER);
    if (status == fsOK)
        FileSys_DeleteFile(FN_RECEIVER);

    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    _devStat = 0;

    return 0;
}


int DevMng_InvalidateUserFP(UserInfo *userInfo)
{
    userInfo->flags &= ~UF_FP_VALID;
    userInfo->FPID = 0;
    return DevMng_AddUpdateGeneralUserInfo(userInfo);
}


int DevMng_InvalidateUserMajorCardNO(UserInfo *userInfo)
{
    userInfo->flags &= ~UF_MAJOR_CARD_VALID;
    userInfo->MajorCardNO[0] = 0;
    return DevMng_AddUpdateGeneralUserInfo(userInfo);
}


int DevMng_InvalidateUserMinorCardNO(UserInfo *userInfo)
{
    userInfo->flags &= ~UF_MINOR_CARD_VALID;
    userInfo->MinorCardNO[0] = 0;
    return DevMng_AddUpdateGeneralUserInfo(userInfo);
}


int DevMng_DeleteAdmin(const uid_t userID)
{
    const char *filename = GetUserFileName(userID, DIR_ADMIN);
    fsStatus status = FileSys_DeleteFile(filename);
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}



int DevMng_DeleteAllUsers(void)
{
    fsStatus status = FileSys_DeleteFile(FN_SENDER);
    if (status == fsOK)
    {
        status = FileSys_DeleteFile(FN_RECEIVER);
        if (status == fsOK)
        {
            status = FileSys_DeleteDirectory(DIR_GENERAL);
            if (status == fsOK)
                status = FileSys_DeleteDirectory(DIR_ADMIN);
        }
    }

    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    _devStat = 0;

    return 0;
}



int DevMng_SetDeviceInfo(const DeviceInfo *devInfo)
{
    fsStatus status = FileSys_WriteBinary(FN_DEVICE, devInfo, sizeof(DeviceInfo));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}



int DevMng_GetDeviceInfo(DeviceInfo *devInfo)
{
    memset(devInfo, 0, sizeof(DeviceInfo));
    fsStatus status =  FileSys_ReadBinary(FN_DEVICE, devInfo, sizeof(DeviceInfo));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}


bool DevMng_IsAdminPassword(const char *text)
{
    return strcmp(_passwords.admin, text) == 0;
}


bool DevMng_IsUnlockPassword(const char *text)
{
    return strcmp(_passwords.unlock, text) == 0;
}


int DevMng_UpdateAdminPassword(const pwd_t pwd)
{
    strcpy(_passwords.admin, pwd);

    fsStatus status = FileSys_WriteBinary(FN_PASSWORD,
                                          &_passwords, sizeof(PWDSet));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}



int DevMng_UpdateUnlockPassword(const pwd_t pwd)
{
    strcpy(_passwords.unlock, pwd);

    fsStatus status = FileSys_WriteBinary(FN_PASSWORD,
                                          &_passwords, sizeof(PWDSet));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}


int DevMng_GetUserCount(int *count)
{
    *count = 0;

    sprintf(patternTemp, "%s\\*.dat", DIR_GENERAL);
    fileInfoTemp.fileID = 0;
    while (ffind (patternTemp, &fileInfoTemp) == fsOK)
        (*count)++;

    sprintf(patternTemp, "%s\\*.dat", DIR_ADMIN);
    fileInfoTemp.fileID = 0;
    while (ffind (patternTemp, &fileInfoTemp) == fsOK)
        (*count)++;

    return 0;
}


void DevMng_AddLog(const uid_t operatorID, const char *format, ...)
{
    static LogEntry entry;
    fsStatus status;
    int ret, count;

    if (operatorID != NULL)
        strcpy(entry.operatorID, operatorID);
    else
        entry.operatorID[0] = 0;

    BSP_RTC_GetTime(&entry.time);

    va_list args;
    va_start(args, format);
    vsprintf(entry.opt, format, args);
    va_end(args);

    status = FileSys_AppendBinary(FN_LOGDATA, &entry, sizeof(LogEntry));
    if (status != fsOK)
        return;

    ret = DevMng_GetLogEntryCount(&count);
    if (ret == 0)
    {
        count++;
        FileSys_WriteBinary(FN_LOGCNT, &count, sizeof(int));
    }
}


int DevMng_DeleteLogData(void)
{
    fsStatus status = FileSys_DeleteFile(FN_LOGDATA);
    if (status == fsOK)
        status = FileSys_DeleteFile(FN_LOGCNT);

    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}


int DevMng_GetLogDataStream(FILE **stream)
{
    fsStatus status = FileSys_OpenFile(FN_LOGDATA, "rb", stream);
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }
    return 0;
}


bool DevMng_GetNextLogEntry(FILE *stream, LogEntry *entry)
{
    if (feof(stream))
        return false;

    size_t nb = fread(entry, sizeof(LogEntry), 1, stream);
    if (nb != 1)
        return false;

    return true;
}


int DevMng_GetLogEntryCount(int *count)
{
    fsStatus status;
    *count = 0;
    status = FileSys_ReadBinary(FN_LOGCNT, count, sizeof(int));
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }
    return 0;
}


void DevMng_AddGeoLoc(float longitude, float latitude)
{
    static GeoLoc loc;

    BSP_RTC_GetTime(&loc.time);
    loc.longitude = longitude;
    loc.latitude = latitude;

    FileSys_AppendBinary(FN_LOCDATA, &loc, sizeof(GeoLoc));
}


int DevMng_DeleteGeoLocData(void)
{
    fsStatus status;

    status = FileSys_DeleteFile(FN_LOCDATA);
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }

    return 0;
}


int DevMng_GetGeoLocDataStream(FILE **stream)
{
    fsStatus status = FileSys_OpenFile(FN_LOCDATA, "rb", stream);
    if (status != fsOK)
    {
        strcpy(devmng_lastError, FileSys_StrError(status));
        return ERROR_FILE_SYSTEM;
    }
    return 0;
}


bool DevMng_GetNextGeoLoc(FILE *stream, GeoLoc *loc)
{
    if (feof(stream))
        return false;

    size_t nb = fread(loc, sizeof(GeoLoc), 1, stream);
    if (nb != 1)
        return false;

    return true;
}
