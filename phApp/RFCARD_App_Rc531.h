#ifndef  RFCARD_App_Rc531_H
#define  RFCARD_App_Rc531_H



//A类标签
#define PICC_TagA_Unkown    0x0000 //PICC未知的卡类型
#define PICC_TagA_S50       0x0001
#define PICC_TagA_S70       0x0002
#define PICC_TagA_UL        0x0003
#define PICC_TagA_P2P       0x0010
#define PICC_TagA_CPU       0x0040
                         
//B类标签 
#define PICC_TagB_Unkown    0x0100//PICC未知的卡类型
#define PICC_TagB_CPU       0x0140
                                   
//F类标签
#define PICC_TagF_Unkown    0Xf000
#define PICC_TagF_Felic     0xf011//
#define PICC_TagF_P2P       0xf020

#define PICC_Tag_Multi      0xfffe//存在多个卡片
#define PICC_Tag_NoCard     0xffff//无卡


extern unsigned char bInterfaceRFIC;    //定义接口IC
extern unsigned char bRfMode;           //0--ISO模式；1--EMV模式


#define ATR_BUF_LEN 64

/*卡片ATR返回结构体*/
typedef struct tagRfCardRet_STR {
    /*接触式，非接触式CPU卡共有，卡片类型*/
    unsigned short ucCardType;


    /*射频卡特有*/
    unsigned char  aucATQA[2];
    unsigned char  ucUidLen;
    unsigned char  aucUID[8];
    unsigned char  ucSAK;


    //对于ISO14443-3A的卡片，返回的是UID
    //对于ISO14443-4B \F标签，返回的是ATQ

    /*ATR长度*/  //如果不是CPU卡，那么返回的是SAK ，UID 等信息
    unsigned char ucAtrLen;
    unsigned char aucATR[ATR_BUF_LEN]; 
}CardAtr_STR;

extern CardAtr_STR stRfCardRet;



typedef struct tagSlotInf_STR 
{
	/*卡槽状态，有卡，无卡*/
	unsigned char ucSlotStat;  //有卡、无卡

	/*卡片类型 */
	unsigned short ucCardType;
	/*卡片状态 激活，未激活*/
	unsigned char ucCardStat;  //0--激活；1--有卡，但是没有激活；2--无卡

	/*ATR长度*/
	unsigned char ucAtrLen;
	
	/*Atr内容，只有当卡片激活时内容才有效*/
	unsigned char aucATR[ATR_BUF_LEN]; 
}SlotInf_STR;

extern SlotInf_STR SlotInfor[6];
extern SlotInf_STR astMapSlotInf;

#define RF_CARD_SLOT        0x00         //RF卡的数组位置


// Table 6.2-3 card Status register ICCD: Table 6.1-8 Bitmap for bStatus field
#define ICC_BS_PRESENT_ACTIVATED     0x00    // 卡片在位而且激活
#define ICC_BS_PRESENT_NOTACTIVATED  0x01    // 卡片在位但是没有激活
#define ICC_BS_NOTPRESENT            0x02    // 没有卡片
#define ICC_NOT_PRESENT              0x00
#define ICC_PRESENT                  0x01


void PICC_Init(void);                                       //射频卡初始化   
unsigned short PiccTest(unsigned char *CardType);           //识别射频卡
unsigned short ISO14443_3_ActiveABApp(void);                //激活
unsigned char RFDeactive(void);                             //射频下电
unsigned char RFCPUApdu(unsigned char *T_Apdu,unsigned short int T_Apdu_Len,unsigned char *R_Apdu,unsigned short int * R_Apdu_Len);         //射频卡CPU卡数据交互
unsigned char PiccPollingMFC_App(void);                     //寻卡片S50,S70
unsigned char PiccPollUL_App(void);                         //寻卡UL卡 
unsigned char PiccPollCPU_App(unsigned char *buffer);       //CPU卡激活
unsigned short PiccGetCIDMFC_App(unsigned char *uid);       //获取卡片序列号
unsigned char RFMifareActive(void);                                                                                                         //mifare卡激活
unsigned char RFMifareAuthenticationKey(unsigned char mode,unsigned char section_number,unsigned char* key_data);                           //mifare卡验证密码
unsigned char RFMifareReadSector(unsigned short int bBlockNum,unsigned char *read_data);                                                    //mifare读取数据  
unsigned char RFMifareWriteSector(unsigned short int bBlockNum,unsigned char *write_data);                                                  //mifare写入数据 
unsigned char RFMifareValue(unsigned char mode,unsigned char source_block,unsigned char destination_block,unsigned char *value);            //mifare值操作
unsigned char RFMifareULReadSector(unsigned short int bBlockNum,unsigned char *read_data);                                                  //mifareUL读取扇区
unsigned char RFMifareULWriteSector(unsigned short int bBlockNum,unsigned char *write_data);                                                //mifareUL写入扇区

#endif







