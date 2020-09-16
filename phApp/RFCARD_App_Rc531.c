#include "RFCARD_App_Rc531.h"


/**************************************************/
/*NXP射频库头文件*/
#include <ph_NxpBuild.h>
#include <ph_Status.h>
#include <phpalI14443p3a.h>
#include <phpalI14443p3b.h>
#include <phpalI14443p4.h>
#include <phpalI14443p4a.h>
#include <phpalI14443p4_Sw.h>

#include <phpalI18092mPI.h>
#include <phalMful.h>
#include <phalMfc.h>
#include <phKeyStore.h>
#include <phpalFelica.h>

#include <phacDiscLoop.h>
#include <phOsal.h>

#include "phhalHw_Rc523.h"
#include "phacDiscLoop_Sw_Int.h"
#include "phacDiscLoop_Sw.h"

#include <iso14443-4.h>
#include <phalMfc_Int.h>
#include <phalMfc_Sw.h>
/**************************************************/


enum
{
  FALSE = 0, TRUE  = !FALSE
};

ContextInfo ReaderISOPare;//读写器参数


uint8_t bHalBufferReaderTx[512];
uint8_t bHalBufferReaderRx[1024];

uint8_t bAtr_Res[128];                              /**< ATR response holder */
uint8_t bAtr_ResLength;                             /**< ATR response length */
uint8_t bGtLen;                                     /**< Gt length */


uint8_t  cryptoEnc[8];
uint8_t  cryptoRng[8];
phbalReg_Stub_DataParams_t balReader;
phhalHw_Rc523_DataParams_t halReader;
phpalI14443p3a_Sw_DataParams_t  I14443p3a;
phpalI14443p3b_Sw_DataParams_t  I14443p3b;
phpalI14443p4a_Sw_DataParams_t  I14443p4a;

phpalI14443p4_Sw_DataParams_t I14443p4;


phpalMifare_Sw_DataParams_t palMifare;

phalMful_Sw_DataParams_t alMful;

phpalI18092mPI_Sw_DataParams_t   I18092mPI;    /**< PAL MPI component */
phpalFelica_Sw_DataParams_t      Felica;        /**< PAL Felica component */

phalMfc_Sw_DataParams_t alMfc;
/*软钥*/
phKeyStore_Sw_DataParams_t       SwkeyStore;
phKeyStore_Sw_KeyEntry_t         pKeyEntries;    
phKeyStore_Sw_KeyVersionPair_t   pKeyVersionPairs;  
phKeyStore_Sw_KUCEntry_t         pKUCEntries;

/*硬密钥*/
//phKeyStore_Rc663_DataParams_t Rc663keyStore;


extern unsigned short ST_WORD(unsigned short i,unsigned char *data);



//phOsal_Lpc12xx_DataParams_t         osal;            /**< OSAL component holder for RaspberryPi */

phacDiscLoop_Sw_DataParams_t     discLoop;        /**< Discovery loop component */

//phKeyStore_Rc663_DataParams_t Rc663keyStore;
phalMful_Sw_DataParams_t alMful;
uint8_t ** ppRxBuff = NULL;
uint8_t *pRxBuffer;
void *pHal;

/*==================================================
 ** GI information */
const uint8_t GI[] = 
{
    0x46,0x66,0x6D,      /* LLCP magic numbers */
    0x01,0x01,0x10,      /*VERSION*/
    0x03,0x02,0x00,0x01, /*WKS*/
    0x04,0x01,0xF1       /*LTO*/
};


unsigned char bInterfaceRFIC;    //定义接口IC
unsigned char bRfMode;           //0--ISO模式；1--EMV模式


CardAtr_STR stRfCardRet;
SlotInf_STR SlotInfor[6];

/***************************************
//延时us
***************************************/
static void DelayUs(unsigned int delayus){
    unsigned int i,j;
    for (i=0; i<delayus; i++)
    {
        for (j = 0;j<11;j++) ;
    }
    return ;
}

/***************************************
//延时ms
***************************************/
static void DelayMs(unsigned int delayms){
    DelayUs(delayms*1000);
    return ; 
}

/***************************************
//延时ms
***************************************/
static void Delay1Ms(unsigned int delayms){
    DelayUs(delayms*1000);
    return ; 
}



/*==============================================================================================
 * \brief      Initializes the discover loop component
 *     phacDiscLoop_Sw_DataParams_t * pDataParams   <[In]  DataParams representing the discovery loop
 *
 ---------------------------------------------------------------------------------------------*/
phStatus_t DiscLoopInit( phacDiscLoop_Sw_DataParams_t * pDataParams )
{
	phStatus_t status;

	/* Set for poll and listen mode */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_MODE,
																	PHAC_DISCLOOP_SET_POLL_MODE | PHAC_DISCLOOP_SET_PAUSE_MODE);

	/* Set for detection of TypeA, TypeB and Type F tags */
	status = phacDiscLoop_SetConfig(
																 pDataParams,
																 PHAC_DISCLOOP_CONFIG_DETECT_TAGS,
																 PHAC_DISCLOOP_CON_POLL_A| PHAC_DISCLOOP_CON_POLL_B | PHAC_DISCLOOP_CON_POLL_F
																 );
	status = phacDiscLoop_SetConfig(//时能激活应用程序
																	pDataParams,
																	PHAC_DISCLOOP_CONFIG_BAIL_OUT,
																	PHAC_DISCLOOP_CON_BAIL_OUT_A| PHAC_DISCLOOP_CON_BAIL_OUT_B);



	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_PAUSE_PERIOD_MS, 1000);


	/* Set number of polling loops to 5 */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_NUM_POLL_LOOPS, 1);//1次或者3次

	/* Configure felica discovery */
	/* Set the system code to 0xffff */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_SYSTEM_CODE, 0xffff);
	/* Set the maximum number of Type F tags to be detected to 3 */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_DEVICE_LIMIT, 3);
	/* Set the polling limit for Type F tags to 5 */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_POLL_LIMIT, 3);
	/* Set the slot number to 3 */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_TIME_SLOT, 3);
	/* Set LRI to 3 */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_LRI, 3);
	/* Set DID to 3 */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_DID, 0);
	/* Disable NAD */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD_ENABLE, PH_OFF);
	/* Clear NAD info */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD, 0);
	/* Assign the GI */
	discLoop.sTypeFTargetInfo.sTypeF_P2P.pGi = (uint8_t *)GI;
	/* Set the length of GI */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_GI_LEN, sizeof(GI));
	/* Assign ATR response */
	discLoop.sTypeFTargetInfo.sTypeF_P2P.pAtrRes = bAtr_Res;
	/* Set ATR response length */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEF_P2P_ATR_RES_LEN, bAtr_ResLength);

	/* Configure Type B tag discovery */
	/* Set slot coding number to 0 */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEB_NCODING_SLOT, 0);
	/* Set AFI to 0, Let all TypeB tags in field respond */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEB_AFI_REQ, 0);
	/* Disable extended ATQB response */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEB_EXTATQB, 0);
	/* Set poll limit for Type B tags to 5 */
	status = phacDiscLoop_SetConfig(pDataParams, PHAC_DISCLOOP_CONFIG_TYPEB_POLL_LIMIT, 1);

	return PH_ERR_SUCCESS;
}


/************************************************************************************
//射频卡初始化
*************************************************************************************/
void PICC_Init(void)
{
	unsigned char atq[4]; 
	unsigned char sak[4]; 
	unsigned char uid[20]; 
	unsigned char uid_len;
	unsigned char aSW[12];
	unsigned char bDRI;
	unsigned char bDetectLoop;
	unsigned short  bBlockNum,i;
	unsigned char ucSector;
	signed char status;
	static unsigned short wRApduLen,wApduLen;

	//rc663相关的参数
	phStatus_t bstatus;
	uint8_t bMoreCardsAvailable;

	uint8_t pAts[128];
	uint16_t wAtsLen,wCardType;


  if (bInterfaceRFIC==0xff) return ;//射频卡接口无效

  ppRxBuff = &pRxBuffer;



	phbalReg_Stub_Init(&balReader, sizeof(phbalReg_Stub_DataParams_t));
	status = phhalHw_Rc523_Init(&halReader,sizeof(phhalHw_Rc523_DataParams_t),&balReader, 0,
															bHalBufferReaderTx,sizeof(bHalBufferReaderTx),
															bHalBufferReaderRx,sizeof(bHalBufferReaderRx));
	halReader.bBalConnectionType = PHHAL_HW_BAL_CONNECTION_SPI;
	pHal = &halReader;
	phpalI14443p3a_Sw_Init(&I14443p3a,sizeof(phpalI14443p3a_Sw_DataParams_t), pHal);

	phpalI14443p3b_Sw_Init(&I14443p3b,sizeof(phpalI14443p3b_Sw_DataParams_t), pHal);

	phpalI14443p4a_Sw_Init(&I14443p4a, sizeof(phpalI14443p4a_Sw_DataParams_t),pHal );

	phpalI14443p4_Sw_Init(&I14443p4,sizeof(phpalI14443p4_Sw_DataParams_t), pHal);

	phpalMifare_Sw_Init(&palMifare,sizeof(phpalMifare_Sw_DataParams_t), pHal, &I14443p4);

	phpalI18092mPI_Sw_Init(&I18092mPI, sizeof(phpalI18092mPI_Sw_DataParams_t), pHal);

	phpalFelica_Sw_Init(&Felica, sizeof(phpalFelica_Sw_DataParams_t), pHal);

	phalMfc_Sw_Init(&alMfc,sizeof(phalMfc_Sw_DataParams_t), &palMifare,&SwkeyStore);

	phalMful_Sw_Init(&alMful,sizeof(phalMful_Sw_DataParams_t), &palMifare,NULL,NULL,NULL);

	status = phacDiscLoop_Sw_Init(&discLoop, sizeof(phacDiscLoop_Sw_DataParams_t), pHal, NULL);

	phhalHw_Rc523_Cmd_SoftReset(pHal);
	phhalHw_Rc523_FieldReset(pHal)/* Reset the RF field */;


	discLoop.pPal1443p3aDataParams = &I14443p3a;
	discLoop.pPal1443p3bDataParams = &I14443p3b;
	discLoop.pPal1443p4aDataParams = &I14443p4a;
	discLoop.pPal18092mPIDataParams = &I18092mPI;
	discLoop.pPal1443p4DataParams = &I14443p4;
	discLoop.pPalFelicaDataParams = &Felica;
	discLoop.pHalDataParams = pHal;
	discLoop.pOsal = NULL;


	phKeyStore_Sw_Init(&SwkeyStore, sizeof(phKeyStore_Sw_DataParams_t),
										 &pKeyEntries, 1,
										 &pKeyVersionPairs, 1, 
										 &pKUCEntries, 1);

	DiscLoopInit(&discLoop);

  phhalHw_FieldOff(discLoop.pHalDataParams);//关闭电磁场
}



/*************************************
//保存卡片信息
*************************************/
void SaveAtrInf(unsigned char Slot)
{
	SlotInfor[Slot].ucCardType=stRfCardRet.ucCardType;
	SlotInfor[Slot].ucCardStat=ICC_BS_PRESENT_ACTIVATED;
	SlotInfor[Slot].ucSlotStat=ICC_PRESENT;

	if ((stRfCardRet.ucCardType==PICC_TagA_S50)||(stRfCardRet.ucCardType==PICC_TagA_S70)||
			(stRfCardRet.ucCardType==PICC_TagA_UL)||(stRfCardRet.ucCardType==PICC_TagA_Unkown)||
			(stRfCardRet.ucCardType==PICC_TagA_P2P))
	{
			SlotInfor[Slot].ucAtrLen=stRfCardRet.ucUidLen;
			memcpy(&SlotInfor[Slot].aucATR,&stRfCardRet.aucUID,SlotInfor[Slot].ucAtrLen);
	} 
	else
	{
			SlotInfor[Slot].ucAtrLen=stRfCardRet.ucAtrLen;
			memcpy(&SlotInfor[Slot].aucATR,&stRfCardRet.aucATR,SlotInfor[Slot].ucAtrLen);
	}
}


/***********************************************
//激活
***********************************************/
unsigned short ISO14443_3_ActiveABApp(void)
{
	unsigned char  atq[4]; 
	unsigned char  sak[4]; 
	unsigned char  uid[20]; 
	unsigned char  uid_len;
	unsigned char  aSW[12];
	unsigned char  bDRI;
	unsigned char  bDetectLoop;
	unsigned short bBlockNum,i;
	unsigned char  ucSector;
	signed char status;
	//rc663相关的参数
	phStatus_t bstatus;
	uint8_t bMoreCardsAvailable;
	unsigned char abApdu[512] ;
	uint8_t pAts[128];
	uint16_t wAtsLen,wCardType;

	if (bInterfaceRFIC==0xff) return FALSE;//射频卡接口无效

	phacDiscLoop_SetConfig(&discLoop,PHAC_DISCLOOP_CONFIG_DETECT_TAGS,
												 PHAC_DISCLOOP_CON_POLL_A | PHAC_DISCLOOP_CON_POLL_B | PHAC_DISCLOOP_CON_POLL_F);
	phacDiscLoop_SetConfig(&discLoop,PHAC_DISCLOOP_CONFIG_BAIL_OUT,0x00); //禁止应用程序激活命令
	for (bDetectLoop = 0;bDetectLoop<2; bDetectLoop++)
	{
		phacDiscLoop_Sw_Int_ClearDataParams(&discLoop);//清除参数
		bstatus =  phacDiscLoop_Sw_Start(&discLoop);
		phacDiscLoop_Sw_GetAts(&discLoop,pAts,&wAtsLen,&wCardType);
		stRfCardRet.ucCardType = wCardType;
		if (pAts[0]==0)
		{
			phhalHw_FieldOff(discLoop.pHalDataParams);
			return FALSE;
		} 
		else if (pAts[0]==1)      /*  一张卡  */
		{
			//使能能激活应用程序
			phacDiscLoop_SetConfig(&discLoop,PHAC_DISCLOOP_CONFIG_BAIL_OUT, PHAC_DISCLOOP_CON_BAIL_OUT_A| PHAC_DISCLOOP_CON_BAIL_OUT_B);
			switch (wCardType)
			{
			  case PICC_TagA_CPU://CPU卡
			  case PICC_TagA_S50:
			  case PICC_TagA_S70:
			  case PICC_TagA_UL:
			  case PICC_TagA_Unkown:
					  phacDiscLoop_SetConfig(&discLoop,PHAC_DISCLOOP_CONFIG_DETECT_TAGS, PHAC_DISCLOOP_CON_POLL_A );
					  memcpy(stRfCardRet.aucATQA,&pAts[2],2);
					  stRfCardRet.ucSAK = pAts[4];
					  stRfCardRet.ucUidLen =  pAts[5];
					  memcpy(stRfCardRet.aucUID,&pAts[6],stRfCardRet.ucUidLen);
					  stRfCardRet.ucAtrLen = pAts[6+stRfCardRet.ucUidLen];
					  memcpy(stRfCardRet.aucATR,&pAts[7+stRfCardRet.ucUidLen],stRfCardRet.ucAtrLen);
					  break;
			  case PICC_TagB_CPU:
			  case PICC_TagB_Unkown:
					  phacDiscLoop_SetConfig(&discLoop,PHAC_DISCLOOP_CONFIG_DETECT_TAGS, PHAC_DISCLOOP_CON_POLL_B );
					  stRfCardRet.ucAtrLen = pAts[2];
				  	memcpy(stRfCardRet.aucATR,&pAts[3],stRfCardRet.ucAtrLen);
					  break;
			  case PICC_TagF_P2P:
			  case PICC_TagF_Felic:
			  case PICC_TagF_Unkown:
					  bDetectLoop = 1;
					  stRfCardRet.ucAtrLen = pAts[3];
					  memcpy(stRfCardRet.aucATR,&pAts[4],stRfCardRet.ucAtrLen);
					  break;
			}
		}
		else/********多张卡片********/
		{
				stRfCardRet.ucCardType = wCardType;
				stRfCardRet.ucAtrLen = wAtsLen;
				memcpy(stRfCardRet.aucATR,&pAts[4],wAtsLen);
		}
	}
	if (pAts[0]==0x00)
	{
		status = phhalHw_FieldOff(discLoop.pHalDataParams);
		return FALSE;
	} 
	else
	{
		SaveAtrInf(RF_CARD_SLOT);    //保存信息
		return TRUE;
	}
}


/**********************************************
//自动检测射频卡类型
//输入
//unsigned char *CardType-----保存射频卡的类型
//PICC_TagA_S50
//PICC_TagA_S70
//PICC_TagA_UL
//PICC_TagA_CPU
//PICC_TagB_CPU
//PICC_TagF_P2P
//PICC_TagA_P2P
//返回
//TRUE------操作成功
//FALSE-----操作失败
**********************************************/
unsigned short PiccTest(unsigned char *CardType)
{
	unsigned short ApduLen,RApduLen;
	signed char status;
	status = ISO14443_3_ActiveABApp();        
	if (status==TRUE)
	{
		switch (stRfCardRet.ucCardType)
		{
			case PICC_TagA_S50:
					*CardType= '0';
					break;
			case PICC_TagA_S70:
					*CardType = '1';
					break;
			case PICC_TagA_UL:
					*CardType = '2';
					break;
			case PICC_TagA_CPU:
					*CardType = '4';
					break;
			case PICC_TagB_CPU:
					*CardType = '5';
					break;
			case PICC_TagF_P2P:
			case PICC_TagA_P2P:
					*CardType = '6';
					break;

			default:
					*CardType = '9';
					break;
		}
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}



/***********************************************
//寻卡S50,S70
//返回
//0----操作成功
//1----操作失败
************************************************/
unsigned char PiccPollingMFC_App(void)
{
	phStatus_t  phstatus;  
	unsigned char bUid[20],bSak[8],bAtq[4];
	unsigned char bLength,bMoreCardsAvailable;

	phhalHw_FieldReset(&halReader);

	phstatus = phhalHw_ApplyProtocolSettings(&halReader, PHHAL_HW_CARDTYPE_ISO14443A);
	phstatus = phpalI14443p3a_Sw_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength,
																						bSak, &bMoreCardsAvailable,bAtq);

	if (((phstatus==0)&&((bAtq[0]==0x04)&&(bAtq[1]==0x00))&&(bSak[0]==0x08))||
			((phstatus==0)&&((bAtq[0]==0x02)&&(bAtq[1]==0x00))&&(bSak[0]==0x18)))
	{
		return 0x00;
	} 
	else
	{
		return 0x01;
	}   
}


/***********************************************
//寻卡UL
//返回
//0----操作成功
//1----操作失败
************************************************/
unsigned char PiccPollUL_App(void)
{
	phStatus_t  phstatus;  
	unsigned char bUid[20],bSak[8],bAtq[4];
	unsigned char bLength,bMoreCardsAvailable;

	phhalHw_FieldReset(&halReader);

	phstatus = phhalHw_ApplyProtocolSettings(&halReader, PHHAL_HW_CARDTYPE_ISO14443A);
	phstatus = phpalI14443p3a_Sw_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength,
																						bSak, &bMoreCardsAvailable,bAtq);
	if ((phstatus==0)&&((bAtq[0]==0x00)&&(bAtq[1]==0x44)))
	{
		return 0x00;
	} 
	else
	{
		return 0x01;
	}	
}

/***********************************************
//激活cpu
//返回
//0----操作成功
//1----操作失败
************************************************/
unsigned char PiccPollCPU_App(unsigned char *buffer)
{
	phStatus_t           phstatus;  
	unsigned char        bSak[8],bAtq[4];
	static unsigned char bUid[20],bLength;
	unsigned char        bMoreCardsAvailable;
	unsigned char        bReqCode;

	unsigned char        bBlockNum;
	static unsigned char aAtqb[16];
	static unsigned char bAtqbLen = 0;	
	
	phhalHw_FieldReset(I14443p3a.pHalDataParams);
	Delay1Ms(10);
	phstatus = phhalHw_ApplyProtocolSettings(I14443p3a.pHalDataParams, PHHAL_HW_CARDTYPE_ISO14443A);
	if (PH_ERR_SUCCESS==phstatus) {

			phstatus = phpalI14443p3a_Sw_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength,
																								bSak, &bMoreCardsAvailable,bAtq);
	}
	if (PH_ERR_SUCCESS!=phstatus)
			phstatus = phpalI14443p3a_Sw_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength,
																								bSak, &bMoreCardsAvailable,bAtq);


	if (PH_ERR_SUCCESS==phstatus) {

			phstatus = phpalI14443p4a_Sw_ActivateCard(
																							 &I14443p4a,
																							 8,
																							 0,
																							 PHPAL_I14443P4A_DATARATE_106,
																							 PHPAL_I14443P4A_DATARATE_106,
																							 buffer);
	}
	if (PH_ERR_SUCCESS==phstatus)
			phpalI14443p4_Sw_SetProtocol( &I14443p4,0,0,0,0,
																		I14443p4a.bFwi,
																		I14443p4a.bFsdi,
																		I14443p4a.bFsci);

	if (PH_ERR_SUCCESS==phstatus)
	{
    return 0x00;
  }
	else
	{
    return 0x01;
  }	
}	


/***********************************************
//获取卡片序列号
//返回
//0----操作成功
//1----操作失败
//uid--存储卡片序列号，首位存储序列号的长度
************************************************/
unsigned short PiccGetCIDMFC_App(unsigned char *uid)
{
	phStatus_t  phstatus;  
	unsigned char bUid[20],bSak[8],bAtq[4];
	unsigned char bLength,bMoreCardsAvailable;
	phhalHw_FieldReset(&halReader);

	phstatus = phhalHw_ApplyProtocolSettings(&halReader, PHHAL_HW_CARDTYPE_ISO14443A);
	phstatus = phpalI14443p3a_Sw_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength,
																						bSak, &bMoreCardsAvailable,bAtq);
	if (((phstatus==0)&&((bAtq[0]==0x04)&&(bAtq[1]==0x00))&&(bSak[0]==0x08))||
			((phstatus==0)&&((bAtq[0]==0x02)&&(bAtq[1]==0x00))&&(bSak[0]==0x18)))
	{
		unsigned char i;
		for(i=0;i<bLength;i++)
		{
      uid[i+1]=bUid[i];
    }
		uid[0]=bLength;
		return 0x00;
	} 
	else
	{
	  return 1;
	}
}

/***********************************************
//射频卡下电
************************************************/
unsigned char RFDeactive(void)
{
	unsigned short RApduLen;
	phStatus_t     bstatus;
	signed char    status;
	
	status = phhalHw_FieldOff(discLoop.pHalDataParams);
	DelayMs(10);
	phhalHw_FieldOff(discLoop.pHalDataParams);
	
  return 0x00;
}



/***********************************************
//射频卡CPU卡片数据交互
//返回0x00-------表示操作成功
      0x01-------表示操作失败
//输入参数
      T_Apdu-----需要发送的APDU数据区
      T_Apdu_Len-需要发送的APDU数据长度
      R_Apdu-----接收APDU数据返回的缓冲区
      R_Apdu_Len-接收到的APDU数据长度
************************************************/
unsigned char RFCPUApdu(unsigned char *T_Apdu,unsigned short int T_Apdu_Len,unsigned char *R_Apdu,unsigned short int *R_Apdu_Len)
{
	phStatus_t bstatus;
	
	phhalHw_FieldOn(discLoop.pHalDataParams);
	bstatus =  phpalI14443p4_Sw_Exchange( &I14443p4, 0x00,T_Apdu,
																				T_Apdu_Len,ppRxBuff,R_Apdu_Len);
	if (bstatus==PH_ERR_SUCCESS)
	{
		unsigned short int i;
		for(i=0;i<(*R_Apdu_Len);i++)
		{
      R_Apdu[i]=(ppRxBuff[0])[i];
    }
	  return 0x00;
	}
	else
	{
	  return 1;
	}
}


/***********************************************
//射频卡Mifare激活
************************************************/
unsigned char RFMifareActive(void)
{
	phStatus_t  phstatus;  
	unsigned char bUid[20],bSak[8],bAtq[4];
	unsigned char bLength,bMoreCardsAvailable;

	phhalHw_FieldReset(&halReader);

	phstatus = phhalHw_ApplyProtocolSettings(&halReader, PHHAL_HW_CARDTYPE_ISO14443A);
	phstatus = phpalI14443p3a_Sw_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength,
																						bSak, &bMoreCardsAvailable,bAtq);


	if (((phstatus==0)&&((bAtq[0]==0x04)&&(bAtq[1]==0x00))&&(bSak[0]==0x08))||
			((phstatus==0)&&((bAtq[0]==0x02)&&(bAtq[1]==0x00))&&(bSak[0]==0x18)))
	{
		return 0x00;
	} 
	else
	{
		return 0x01;
	}	
}


/***********************************************
//射频卡Mifare验证密码
//返回
      0x00-------------验证密码成功
      0x30-------------没有卡片
      0x31-------------扇区号错误
      0x32-------------序列号错误
      0x33-------------密码错误
      0x34-------------读数据错误 
//输入
      mode-------------模式0x60验证keyA,0x61验证keyB
      section_number---需要验证密码的扇区位置
      key_data---------密码缓冲区
************************************************/
unsigned char RFMifareAuthenticationKey(unsigned char mode,unsigned char section_number,unsigned char* key_data)
{
	unsigned short int i=0;
	unsigned char      keycoded[12]; 
  unsigned short int bBlockNum;
	phStatus_t         phstatus;  
	unsigned char      bUid[20],bSak[8],bAtq[4];
	unsigned char      bLength,bMoreCardsAvailable;
	unsigned char      bKeyTpyeData;


	ReaderISOPare.CmdTtype = 1;

	for(i=0;i<6;i++)
	{
    keycoded[i]=key_data[i];
  }
	for(i=0;i<6;i++)
	{
    keycoded[6+i]=key_data[i];
  } 

	phKeyStore_FormatKeyEntry(&SwkeyStore,0x00,PH_KEYSTORE_KEY_TYPE_MIFARE);
	phstatus = phKeyStore_SetKey(
															&SwkeyStore,
															0X00,
															0X00,
															PH_KEYSTORE_KEY_TYPE_MIFARE,
															keycoded,
															0 );

	phstatus = phalMfc_Sw_Init(&alMfc,sizeof(phalMfc_Sw_DataParams_t), 
														 &palMifare,&SwkeyStore);//给MFC注册软件密钥 Rc663keyStore

	phhalHw_FieldReset(&halReader);
	bKeyTpyeData = (mode== 0x60)?PHHAL_HW_MFC_KEYA:PHHAL_HW_MFC_KEYB;
	phstatus = phhalHw_ApplyProtocolSettings(&halReader, PHHAL_HW_CARDTYPE_ISO14443A);
	phstatus = phpalI14443p3a_Sw_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength,
																						bSak, &bMoreCardsAvailable,bAtq);
	
	if ((bAtq[1]==0x00)&&(bAtq[0]==0x04)&&(section_number>15))    //S50
	{
		return 0x31;
	} 
	else if ((bAtq[1]==0x00)&&(bAtq[0]==0x02)&&(section_number>39))//S70
	{
		return 0x31;
	}

	if (section_number<32)
	{	
		bBlockNum = section_number*4;
	}	
	else
	{	
		bBlockNum = 32*4+(section_number-32)*16;
	}	


	phstatus = phalMfc_Authenticate(&alMfc,bBlockNum,bKeyTpyeData,0,0,bUid,bLength);
	if (phstatus!=PH_ERR_SUCCESS)
	{
		phstatus = phalMfc_Authenticate(&alMfc,bBlockNum,bKeyTpyeData,0,0,bUid,bLength);
	}


	if (!phstatus)
	{
		return 0x00;
	} 
	else
	{
    return 0x33;
	}
}

/***********************************************
//射频卡Mifare读取数据
//返回
      0x00-------------验证密码成功
      0x30-------------没有卡片
      0x31-------------扇区号错误
      0x32-------------序列号错误
      0x33-------------密码错误
      0x34-------------读数据错误 
//输入
      bBlockNum--------块号
      read_data--------读取数据缓冲区
************************************************/
unsigned char RFMifareReadSector(unsigned short int bBlockNum,unsigned char *read_data)
{
	phStatus_t         phstatus;  

	//status=Mf500PiccRead(bBlockNum,&psLkPackTrs->APPPack.bBuffer[3]);
	return phalMfc_Read(&alMfc,bBlockNum,read_data); 
	if (PH_ERR_SUCCESS==phstatus)
	{
		return 0x00;
	} 
	else if (phstatus==PH_ERR_IO_TIMEOUT)           //超时提示无卡 
	{
		return 0x30;
	}
	else if(phstatus==PH_ERR_AUTH_ERROR)            //密码验证错误
	{
    return 0x33;
  }		
	else
	{
		return 0x34;
	}    
}

/***********************************************
//射频卡Mifare写入数据
//返回
      0x00-------------验证密码成功
      0x30-------------没有卡片
      0x31-------------扇区号错误
      0x32-------------序列号错误
      0x33-------------密码错误
      0x34-------------读数据错误 
//输入
      bBlockNum--------块号
      read_data--------读取数据缓冲区
************************************************/
unsigned char RFMifareWriteSector(unsigned short int bBlockNum,unsigned char *write_data)
{
	phStatus_t  phstatus;  
	
	phstatus = phalMfc_Write(&alMfc,bBlockNum,write_data);

	if (PH_ERR_SUCCESS==phstatus)
	{
    return 0x00;
	} 
	else if (phstatus==PH_ERR_IO_TIMEOUT)           //无卡错误
	{
    return 0x30;
	} 
	else if(phstatus==PH_ERR_AUTH_ERROR)            //密码验证错误
	{
    return 0x33;
  }	
	else
	{
    return 0x34;
	}
}

/***********************************************
//射频卡Mifare值操作
//返回
      0x00---------------验证密码成功
      0x30---------------没有卡片
      0x31---------------扇区号错误
      0x32---------------序列号错误
      0x33---------------密码错误
      0x34---------------读数据错误 
//输入
      mode---------------值操作模式,增值,减值.0xc1增值,0xc0减值
      source_block-------源块
      destination-block--目标块
      value--------------值缓冲区
************************************************/
unsigned char RFMifareValue(unsigned char mode,unsigned char source_block,unsigned char destination_block,unsigned char *value)
{
	phStatus_t  phstatus;

	phstatus = phalMfc_Int_Value(alMfc.pPalMifareDataParams, 
															 mode, 
															 source_block, value);
	
	phstatus = phalMfc_Sw_Transfer(&alMfc, destination_block);

	//status=Mf500PiccValue(mode,bBlockNum,&psLkPackRev->APPPack.bBuffer[2],bBlockNum);
	if (phstatus==PH_ERR_SUCCESS)
	{
    return 0x00;
	} 
	else if (phstatus==PH_ERR_IO_TIMEOUT)                     //超时提示无卡 
	{
		return 0x30;
	} 
	else
	{
    return 0x33;
	}
}


/***********************************************
//射频卡Mifare读取数据
//返回
      0x00-------------验证密码成功
      0x30-------------没有卡片
      0x31-------------扇区号错误
      0x32-------------序列号错误
      0x33-------------密码错误
      0x34-------------读数据错误 
//输入
      bBlockNum--------块号
      read_data--------读取数据缓冲区
************************************************/
unsigned char RFMifareULReadSector(unsigned short int bBlockNum,unsigned char *read_data)
{
	phStatus_t  phstatus;
	phstatus = phalMful_Read(&alMful,bBlockNum,
													 read_data);
	if (phstatus==PH_ERR_SUCCESS)
	{
    return 0x00;
	} 
	else if (phstatus==PH_ERR_IO_TIMEOUT)                     //超时提示无卡 
	{
    return 0x30;
	} 
	else
	{
    return 0x34;
	}  
}

/***********************************************
//射频卡Mifare读取数据
//返回
      0x00-------------验证密码成功
      0x30-------------没有卡片
      0x31-------------扇区号错误
      0x32-------------序列号错误
      0x33-------------密码错误
      0x34-------------读数据错误 
//输入
      bBlockNum--------块号
      write_data-------读取数据缓冲区
************************************************/
unsigned char RFMifareULWriteSector(unsigned short int bBlockNum,unsigned char *write_data)
{
	phStatus_t  phstatus;
	phstatus = phalMful_Write(&alMful,bBlockNum,
														write_data);
	if (phstatus==PH_ERR_SUCCESS)
	{
    return 0x00;
	} 
	else if (phstatus==PH_ERR_IO_TIMEOUT)                     //超时提示无卡 
	{
    return 0x30;
	} 
	else
	{
    return 0x34;
	}  
}


