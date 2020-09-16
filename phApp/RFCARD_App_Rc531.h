#ifndef  RFCARD_App_Rc531_H
#define  RFCARD_App_Rc531_H



//A���ǩ
#define PICC_TagA_Unkown    0x0000 //PICCδ֪�Ŀ�����
#define PICC_TagA_S50       0x0001
#define PICC_TagA_S70       0x0002
#define PICC_TagA_UL        0x0003
#define PICC_TagA_P2P       0x0010
#define PICC_TagA_CPU       0x0040
                         
//B���ǩ 
#define PICC_TagB_Unkown    0x0100//PICCδ֪�Ŀ�����
#define PICC_TagB_CPU       0x0140
                                   
//F���ǩ
#define PICC_TagF_Unkown    0Xf000
#define PICC_TagF_Felic     0xf011//
#define PICC_TagF_P2P       0xf020

#define PICC_Tag_Multi      0xfffe//���ڶ����Ƭ
#define PICC_Tag_NoCard     0xffff//�޿�


extern unsigned char bInterfaceRFIC;    //����ӿ�IC
extern unsigned char bRfMode;           //0--ISOģʽ��1--EMVģʽ


#define ATR_BUF_LEN 64

/*��ƬATR���ؽṹ��*/
typedef struct tagRfCardRet_STR {
    /*�Ӵ�ʽ���ǽӴ�ʽCPU�����У���Ƭ����*/
    unsigned short ucCardType;


    /*��Ƶ������*/
    unsigned char  aucATQA[2];
    unsigned char  ucUidLen;
    unsigned char  aucUID[8];
    unsigned char  ucSAK;


    //����ISO14443-3A�Ŀ�Ƭ�����ص���UID
    //����ISO14443-4B \F��ǩ�����ص���ATQ

    /*ATR����*/  //�������CPU������ô���ص���SAK ��UID ����Ϣ
    unsigned char ucAtrLen;
    unsigned char aucATR[ATR_BUF_LEN]; 
}CardAtr_STR;

extern CardAtr_STR stRfCardRet;



typedef struct tagSlotInf_STR 
{
	/*����״̬���п����޿�*/
	unsigned char ucSlotStat;  //�п����޿�

	/*��Ƭ���� */
	unsigned short ucCardType;
	/*��Ƭ״̬ ���δ����*/
	unsigned char ucCardStat;  //0--���1--�п�������û�м��2--�޿�

	/*ATR����*/
	unsigned char ucAtrLen;
	
	/*Atr���ݣ�ֻ�е���Ƭ����ʱ���ݲ���Ч*/
	unsigned char aucATR[ATR_BUF_LEN]; 
}SlotInf_STR;

extern SlotInf_STR SlotInfor[6];
extern SlotInf_STR astMapSlotInf;

#define RF_CARD_SLOT        0x00         //RF��������λ��


// Table 6.2-3 card Status register ICCD: Table 6.1-8 Bitmap for bStatus field
#define ICC_BS_PRESENT_ACTIVATED     0x00    // ��Ƭ��λ���Ҽ���
#define ICC_BS_PRESENT_NOTACTIVATED  0x01    // ��Ƭ��λ����û�м���
#define ICC_BS_NOTPRESENT            0x02    // û�п�Ƭ
#define ICC_NOT_PRESENT              0x00
#define ICC_PRESENT                  0x01


void PICC_Init(void);                                       //��Ƶ����ʼ��   
unsigned short PiccTest(unsigned char *CardType);           //ʶ����Ƶ��
unsigned short ISO14443_3_ActiveABApp(void);                //����
unsigned char RFDeactive(void);                             //��Ƶ�µ�
unsigned char RFCPUApdu(unsigned char *T_Apdu,unsigned short int T_Apdu_Len,unsigned char *R_Apdu,unsigned short int * R_Apdu_Len);         //��Ƶ��CPU�����ݽ���
unsigned char PiccPollingMFC_App(void);                     //Ѱ��ƬS50,S70
unsigned char PiccPollUL_App(void);                         //Ѱ��UL�� 
unsigned char PiccPollCPU_App(unsigned char *buffer);       //CPU������
unsigned short PiccGetCIDMFC_App(unsigned char *uid);       //��ȡ��Ƭ���к�
unsigned char RFMifareActive(void);                                                                                                         //mifare������
unsigned char RFMifareAuthenticationKey(unsigned char mode,unsigned char section_number,unsigned char* key_data);                           //mifare����֤����
unsigned char RFMifareReadSector(unsigned short int bBlockNum,unsigned char *read_data);                                                    //mifare��ȡ����  
unsigned char RFMifareWriteSector(unsigned short int bBlockNum,unsigned char *write_data);                                                  //mifareд������ 
unsigned char RFMifareValue(unsigned char mode,unsigned char source_block,unsigned char destination_block,unsigned char *value);            //mifareֵ����
unsigned char RFMifareULReadSector(unsigned short int bBlockNum,unsigned char *read_data);                                                  //mifareUL��ȡ����
unsigned char RFMifareULWriteSector(unsigned short int bBlockNum,unsigned char *write_data);                                                //mifareULд������

#endif







