#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    PAGE_01,
    PAGE_01_01,
    PAGE_01_02,
    PAGE_01_03,
    PAGE_01_04,
    PAGE_01_05,
    PAGE_01_06,
    PAGE_01_01_01,
    PAGE_01_01_02,
    PAGE_01_02_01,
    PAGE_01_02_02,
    PAGE_01_02_03,
    PAGE_01_02_04,
    PAGE_01_03_01,
    PAGE_01_03_02,
    PAGE_01_05_01,
    PAGE_01_01_02_01,
    PAGE_01_01_02_02,
    PAGE_01_01_02_03,
    PAGE_01_01_02_04,
    PAGE_01_01_02_05,
    PAGE_01_01_02_06,
    PAGE_01_02_01_01,
    PAGE_01_05_01_01,
    PAGE_01_01_02_01_01,
    PAGE_01_01_02_01_02,
    PAGE_01_01_02_02_01,
    PAGE_01_01_02_02_02,
    PAGE_01_01_02_03_01,
    PAGE_01_01_02_03_02,
    PAGE_01_01_02_03_03,
    PAGE_01_01_02_03_04,

    PAGE_READ_FP_STEP1,
    PAGE_READ_FP_STEP2,
    PAGE_READ_FP_STEP3,
    PAGE_ADD_FP_OK,
    PAGE_ADD_FP_FAIL,
    PAGE_DELETE_FP_OK,
    PAGE_DELETE_FP_FAIL,

    PAGE_READ_CARD,
    PAGE_ADD_CARD_OK,
    PAGE_ADD_CARD_FAIL,
    PAGE_DELETE_CARD_OK,
    PAGE_DELETE_CARD_FAIL,

    PAGE_SENDER_UNLOCK
} PageID;


extern PageID disp_curPage;

extern int disp_activeItem_01_01_02;
extern int disp_activeItem_01_01_02_01;
extern int disp_activeItem_01_01_02_02;
extern int disp_activeItem_01_01_02_03;

extern bool disp_isTurnedOn;
extern uint32_t disp_t0;


void Disp_TurnOn(void);
void Disp_TurnOff(void);

void Disp_UpdateStatusBar(void);
void Disp_UpdatePasswordBox(const char *text);
void Disp_UpdateUIDBox(const char *text);

void Disp_OnIdentifyFailed(void);
void Disp_OnSetReceiverFailed(void);
void Disp_OnPasswordInputError(void);
void Disp_OnUIDInputError(void);

void GoPage_01(void);

void GoPage_01_01(void);
void GoPage_01_02(void);
void GoPage_01_03(void);
void GoPage_01_04(void);
void GoPage_01_05(void);

void GoPage_01_06(void);

void GoPage_01_01_01(void);
void GoPage_01_01_02(void);

void GoPage_01_02_01(const char *UID);
void GoPage_01_02_02(const char *UID);
void GoPage_SenderUnlock(const char *UID);
void GoPage_01_02_03(const char *UID);
void GoPage_01_02_04(void);

void GoPage_01_03_01(void);
void GoPage_01_03_02(void);
void GoPage_01_05_01(void);

void GoPage_01_01_02_01(void);
void GoPage_01_01_02_02(void);
void GoPage_01_01_02_03(void);
void GoPage_01_01_02_04(void);
void GoPage_01_01_02_05(void);
void GoPage_01_01_02_06(void);

void GoPage_01_02_01_01(void);
void GoPage_01_05_01_01(const char *UID);

void GoPage_01_01_02_01_01(void);
void GoPage_01_01_02_01_02(void);
void GoPage_01_01_02_02_01(void);
void GoPage_01_01_02_02_02(void);
void GoPage_01_01_02_03_01(void);
void GoPage_01_01_02_03_02(void);
void GoPage_01_01_02_03_03(void);
void GoPage_01_01_02_03_04(void);

void GoPage_ReadFPStep(int step);
void GoPage_AddFPOk(const char *userID);
void GoPage_AddFPFail(void);
void GoPage_DeleteFPOk(void);
void GoPage_DeleteFPFail(void);

void GoPage_ReadCard(void);
void GoPage_AddCardOK(const char *userID);
void GoPage_AddCardFail(void);
void GoPage_DeleteCardOK(void);
void GoPage_DeleteCardFail(void);

#endif
