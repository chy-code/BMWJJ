
#ifndef _DEVICE_DATA_H
#define _DEVICE_DATA_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "RTC_Time.h"

//#define ERROR_SUCCESS			0
#define ERROR_FILE_SYSTEM		-1	// 文件系统错误
#define ERROR_USER_NOT_EXISTS	-2	// 用户不存在


// 设备状态标志
#define DEVICE_SENDER_SPEC		0x1 // 表示已指定发件人
#define DEVICE_RECEIVER_SPEC	0x2	// 表示已指定收件人


// 宏定义
#define UID_SZ_MAX		6	// 用户ID的最大长度
#define CARDNO_SZ_MAX	10	// 卡号的最大长度
#define PWD_SZ_MAX		6	// 密码最大长度


// 自定义数据类型
typedef char uid_t[UID_SZ_MAX + 1];	// 用户ID
typedef char cardno_t[CARDNO_SZ_MAX+1];	// 卡号
typedef char pwd_t[PWD_SZ_MAX + 1];	// 密码

// 用户信息标志
#define UF_FP_VALID				0x01	// UserInfo 中的 FPID 有效
#define UF_MAJOR_CARD_VALID		0x02	// UserInfo 中的 MajorCardNO 有效
#define UF_MINOR_CARD_VALID		0x04	// UserInfo 中的 MinorCardNO 有效
#define UF_ALL_VALID			0x07	// 以上三者有效

// 用户信息
typedef struct
{
    uid_t UID;			// 用户编号
    cardno_t MajorCardNO;	// 主卡号
    cardno_t MinorCardNO;	// 副卡号
    uint16_t FPID;			// 指纹编号, 由指纹阅读器得到
    uint8_t flags;	// 标志
} UserInfo;


// 设备信息
typedef struct
{
    char dev_code[20];
    RTC_Time time;
    char investor[20]; // 投递者名字
    char location[20];
} DeviceInfo;


// 密码集
typedef struct
{
    pwd_t admin;	// 管理员密码
    pwd_t unlock;	// 解锁密码
} PWDSet;


// 日志项
typedef struct
{
    RTC_Time time;
    uid_t operatorID;
    char opt[64];
} LogEntry;


// 地理位置信息
typedef struct
{
    RTC_Time time;	// 时间
    float longitude;	// 经度
    float latitude;	// 纬度
} GeoLoc;


extern char devmng_lastError[128];

int DevMng_Init(void);

int DevMng_GetStatus(void);

int DevMng_AddUpdateGeneralUserInfo(const UserInfo *userInfo);
int DevMng_GetGeneralUserInfo(const uid_t userID, UserInfo *userInfo);

int DevMng_AddUpdateAdminInfo(const UserInfo *userInfo);
int DevMng_GetAdminInfo(const uid_t userID, UserInfo *userInfo);
int DevMng_GetAdminInfoByCardNO(const cardno_t cardNO, UserInfo *userInfo);
int DevMng_GetAdminInfoByFP(int FPID, UserInfo *userInfo);

const char* DevMng_GetSenderID(void);
const char* DevMng_GetReceiverID(void);

int DevMng_SetSenderID(const uid_t senderID);
int DevMng_GetSenderInfo(UserInfo *senderInfo);

int DevMng_SetReceiverID(const uid_t receiverID);
int DevMng_GetReceiverInfo(UserInfo *receiverInfo);
int DevMng_UnsetSenderAndReceiver(void);

int DevMng_InvalidateUserFP(UserInfo *userInfo);
int DevMng_InvalidateUserMajorCardNO(UserInfo *userInfo);
int DevMng_InvalidateUserMinorCardNO(UserInfo *userInfo);

int DevMng_DeleteAdmin(const uid_t userID);
int DevMng_DeleteAllUsers(void);

int DevMng_SetDeviceInfo(const DeviceInfo *devInfo);
int DevMng_GetDeviceInfo(DeviceInfo *devInfo);

bool DevMng_IsAdminPassword(const char *text);
bool DevMng_IsUnlockPassword(const char *text);

int DevMng_UpdateAdminPassword(const pwd_t pwd);
int DevMng_UpdateUnlockPassword(const pwd_t pwd);

int DevMng_GetUserCount(int *count);

void DevMng_AddLog(const uid_t operatorID, const char *format, ...);
int DevMng_DeleteLogData(void);

int DevMng_GetLogDataStream(FILE **stream);
bool DevMng_GetNextLogEntry(FILE *stream, LogEntry *entry);
int DevMng_GetLogEntryCount(int *count);

void DevMng_AddGeoLoc(float longitude, float latitude);
int DevMng_DeleteGeoLocData(void);

int DevMng_GetGeoLocDataStream(FILE **stream);
bool DevMng_GetNextGeoLoc(FILE *stream, GeoLoc *loc);

#endif
