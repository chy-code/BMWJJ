#ifndef  _ISO14443_
#define  _ISO14443_

#define   IBLOCK  0X02
#define   RBLOCK  0XA2
#define   SBLOCK  0XC2

#define   IBLOCK_CHAIN   0x01
#define   IBLOCK_NOCHAIN 0x02
#define   RBLOCK_NAK     0x03
#define   RBLOCK_ACK     0x04
#define   SBLOCK_WTX     0x05
#define   SBLOCK_DESELECT  0x06
#define   ERROR_PCB      0x07


#define MAX_FS			(256)
#define PCD_FSDI		(0x08)
#define CPU_CID			(0x00)



#define PCD_OK 0
#define PCD_PROTOCOL_ERR  2

#define ACK_RETRY_COUNT 3
#define PCD_RECEIVE_ERR 1
#define PCD_NO_RESPONSE 5
#define PCD_UNKOWN 6
#define PCD_RETRY_ERR 7

typedef enum tag_SLOT_NUMBER {
    SLOT_1 = 0x00,
    SLOT_2 = 0x01,
    SLOT_4 = 0x02,
    SLOT_8 = 0x03,
    SLOT_16 = 0x04
}SLOT_NUMBER;

typedef enum tagProtType {
    NOCARD = 0,
    TYPE_A,
    TYPE_B
}ProtType;

typedef enum tagCardType {
    UNKOWN = 0,
    CPUCARD,
    MIFARE,
    THR1064
}CardType;

typedef struct {
    CardType        Type;
    ProtType        Prot;
    unsigned char   CmdTtype;
    unsigned char      CardStatus;
    unsigned char      TypeAFlag;
    unsigned char      TypeBFlag;
    unsigned char       CIDFlag;
    unsigned char       CID;
    unsigned char       UIDLen;
    unsigned char       UID[16];

    unsigned char       SFGI;
    unsigned char       FWI;
    unsigned char       FSCI;
    unsigned char       bFSDI;
    unsigned char       Block;
    unsigned char       BPS_PCDCurr;
    unsigned char       BPS_PICCCurr;

    unsigned char       BPS_PCD;
    unsigned char       BPS_PICC;
}ContextInfo;

extern ContextInfo ReaderISOPare;

#endif
