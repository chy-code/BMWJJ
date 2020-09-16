
#include <stdio.h>
#include <stdint.h>
#include "ph_Status.h"
#include "RFCard_App_Rc531.h"
#include "RFCard_Init.h"
#include "RFID_Reader.h"
#include "DbgPrint.h"


void BSP_RFID_Init(void)
{
    RC523_Init();
    RC523_Identify();
    PICC_Init();
}


int BSP_RFID_GetCardNO(char *cardNO)
{
    static uint8_t key_data[6]= {0xff,0xff,0xff,0xff,0xff,0xff};
    unsigned char cardtype[1]= {0xff};
    uint8_t ret = 0;

    RC523_Identify();
    PICC_Init();
    ret = PiccTest(cardtype);
    if(ret ==1)
    {
        if((cardtype[0]==0x30)||(cardtype[0]==0x31))  //S50,S70¿¨Æ¬
        {
            ret = RFMifareActive();   //¼¤»î¿¨Æ¬
            ret = RFMifareAuthenticationKey(0x60,0,key_data);//Ð£ÑéÃÜÂë
            ret = RFMifareReadSector(1, (uint8_t*)cardNO);
            RFDeactive();  //ÏÂµç

            return 0;
        }
        else if((cardtype[0]==0x34)||(cardtype[0]==0x35))   //CPU¿¨
        {
            return RFID_ERR_UNSUPP_CARD;
        }
        else
        {
            return RFID_ERR_UNSUPP_CARD;
        }
    }

    return RFID_ERR_UNSUPP_CARD;
}


const char * BSP_RFID_StrError(int ret)
{
    switch (ret)
    {
//    case PH_ERR_SUCCESS:
//        return "operator successful.";
//    case PH_ERR_SUCCESS_CHAINING:
//        return "Rx chaining is not complete, further action needed.";
//    case PH_ERR_SUCCESS_INCOMPLETE_BYTE:
//        return "An incomplete byte was received.";
//    case PH_ERR_IO_TIMEOUT:
//        return "No reply received, e.g. PICC removal.";
//    case PH_ERR_INTEGRITY_ERROR:
//        return "Wrong CRC or parity detected.";
//    case PH_ERR_COLLISION_ERROR:
//        return "A collision occured.";
//    case PH_ERR_BUFFER_OVERFLOW:
//        return "Attempt to write beyond buffer size.";
//    case PH_ERR_FRAMING_ERROR:
//        return "Invalid frame format.";
//    case PH_ERR_PROTOCOL_ERROR:
//        return "Received response violates protocol.";
//    case PH_ERR_AUTH_ERROR:
//        return "Authentication error.";
//    case PH_ERR_READ_WRITE_ERROR:
//        return "A Read or Write error occured in RAM/ROM or Flash.";
//    case PH_ERR_TEMPERATURE_ERROR:
//        return "The RC sensors signal overheating.";
//    case PH_ERR_RF_ERROR:
//        return "Error on RF-Interface.";
//    case PH_ERR_INTERFACE_ERROR:
//        return "An error occured in RC communication.";
//    case PH_ERR_LENGTH_ERROR:
//        return "A length error occured.";
//    case PH_ERR_INTERNAL_ERROR:
//        return "An internal error occured.";
//    case PH_ERR_RESOURCE_ERROR:
//        return "An resource error.";
//    case PH_ERR_INVALID_DATA_PARAMS:
//        return "Invalid data parameters supplied (layer id check failed).";
//    case PH_ERR_INVALID_PARAMETER:
//        return "Invalid parameter supplied.";
//    case PH_ERR_PARAMETER_OVERFLOW :
//        return "Reading/Writing a parameter would produce an overflow.";
//    case PH_ERR_UNSUPPORTED_PARAMETER:
//        return "Parameter not supported.";
//    case PH_ERR_UNSUPPORTED_COMMAND:
//        return "Command not supported.";
//    case PH_ERR_USE_CONDITION:
//        return "Condition of use not satisfied.";
//    case PH_ERR_KEY:
//        return "A key error occured.";

    case RFID_ERR_NO_CARD:
        return "no card";
    case RFID_ERR_ACTIVATION:
        return "activate error";
    case RFID_ERR_UNSUPP_CARD:
        return "unsupportd card";
    default:
        return "unknown error";
    }
}
