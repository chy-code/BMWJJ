/*
*         Copyright (c), NXP Semiconductors Gratkorn / Austria
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is 
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

/** \file
* Software ISO14443-3A Component of Reader Library Framework.
* $Author: santosh.d.araballi $
* $Revision: 1.5 $
* $Date: Fri Apr 15 09:27:53 2011 $
*
* History:
*  CHu: Generated 19. May 2009
*
*/

#include <ph_Status.h>
#include <phhalHw.h>
#include <phpalI14443p3a.h>
#include <ph_RefDefs.h>
#include "ISO14443-4.h"

//#include "define.h"

/**************************************************************
* 
* 移除卡片
***************************************************************/
phStatus_t  phpal14443p3aEMV_SW_RemoveCard(ContextInfo *ReaderContextInfo , void * pDataParams);


#ifdef NXPBUILD__PHPAL_I14443P3A_SW

    #include "phpalI14443p3a_Sw.h"
    #include "phpalI14443p3a_Sw_Int.h"

//#include "phpalI14443p3b.h"
//#include "phpalI14443p3b_Sw.h"
//   #include "phpalI14443p3b_Sw_Int.h"



phStatus_t phpalI14443p3a_Sw_Init(
                                 phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                 uint16_t wSizeOfDataParams,
                                 void * pHalDataParams
                                 )
{
    if (sizeof(phpalI14443p3a_Sw_DataParams_t) != wSizeOfDataParams) {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_PAL_ISO14443P3A);
    }
    PH_ASSERT_NULL (pDataParams);
    PH_ASSERT_NULL (pHalDataParams);

    /* init private data */
    pDataParams->wId            = PH_COMP_PAL_ISO14443P3A | PHPAL_I14443P3A_SW_ID;
    pDataParams->pHalDataParams = pHalDataParams;
    pDataParams->bUidLength     = 0;
    pDataParams->bUidComplete   = 0;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P3A);
}

phStatus_t phpalI14443p3a_Sw_RequestA(
                                     phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                     uint8_t * pAtqa
                                     )
{
    return phpalI14443p3a_Sw_RequestAEx(pDataParams, PHPAL_I14443P3A_REQUEST_CMD, pAtqa);
}

phStatus_t phpalI14443p3a_Sw_WakeUpA(
                                    phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                    uint8_t * pAtqa
                                    )
{
    return phpalI14443p3a_Sw_RequestAEx(pDataParams, PHPAL_I14443P3A_WAKEUP_CMD, pAtqa);
}

phStatus_t phpalI14443p3a_Sw_HaltA(
                                  phpalI14443p3a_Sw_DataParams_t * pDataParams
                                  )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM cmd[2];
    uint8_t *   PH_MEMLOC_REM pResp;
    uint16_t    PH_MEMLOC_REM wRespLength;

    /* Set halt timeout */
//  PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                   pDataParams->pHalDataParams,
//                                                   PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,
//                                                   PHPAL_I14443P3A_HALT_TIME_US + PHPAL_I14443P3A_EXT_TIME_US));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                     pDataParams->pHalDataParams,
                                                     PHHAL_HW_TIME_MICROSECONDS,
                                                     1900));
    //1888));

    /* 禁止CRC检验 */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));

    //禁止数据错误处理
//    ((phhalHw_Rc663_DataParams_t*)(pDataParams->pHalDataParams))->bDateFliter = PH_OFF;

    /* Send HltA command */
    cmd[0] = PHPAL_I14443P3A_HALT_CMD;
    cmd[1] = 0x00;
    status = phhalHw_Exchange(pDataParams->pHalDataParams, PH_EXCHANGE_DEFAULT, cmd, 2, &pResp, &wRespLength);

    switch (status & PH_ERR_MASK) {
    /* HltA command should timeout -> success */
    case PH_ERR_IO_TIMEOUT:
        return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P3A);
        /* Return protocol error */
    case PH_ERR_SUCCESS:
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
        /* Return other errors */
    default:
        return status;
    }
}

phStatus_t phpalI14443p3a_Sw_Anticollision(
                                          phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                          uint8_t bCascadeLevel,
                                          uint8_t * pUidIn,
                                          uint8_t bNvbUidIn,
                                          uint8_t * pUidOut,
                                          uint8_t * pNvbUidOut
                                          )

{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIsSelect;
    uint8_t     PH_MEMLOC_REM bCmdBuffer[7];
    uint8_t *   PH_MEMLOC_REM pRcvBuffer;
    uint16_t    PH_MEMLOC_REM wSndBytes;
    uint16_t    PH_MEMLOC_REM wRcvBytes;
    uint16_t    PH_MEMLOC_REM wRcvBits;
    uint8_t     PH_MEMLOC_REM bUidStartIndex;
    uint8_t     PH_MEMLOC_REM bBitCount;

    /* Check for invalid bNvbUidIn parameter */
    if ((bNvbUidIn > 0x40) || ((bNvbUidIn & 0x0F) > 0x07)) {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P3A);
    }

//    pDataParams->pHalDataParams->bDisableDF  = 1;//禁止过虑
    /* Check for invalid bCascadeIndex parameter */
    switch (bCascadeLevel) {
    case PHPAL_I14443P3A_CASCADE_LEVEL_1:
        /* Reset UID length */
        pDataParams->bUidLength = 0;
    case PHPAL_I14443P3A_CASCADE_LEVEL_2:
    case PHPAL_I14443P3A_CASCADE_LEVEL_3:
        break;
    default:
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P3A);
    }

    /* Reset UID complete flag */
    pDataParams->bUidComplete = 0;

    //禁止数据错误处理
//    ((phhalHw_Rc663_DataParams_t*)(pDataParams->pHalDataParams))->bDateFliter = PH_OFF;

    /* ANTICOLLISION: Disable CRC */
    if (bNvbUidIn != 0x40) {
        bIsSelect = 0;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_OFF));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_OFF));
    }
    /* SELECT: Enable CRC */
    else {
        bIsSelect = 1;
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));
    }

    /* Init. command buffer */
    bCmdBuffer[0] = bCascadeLevel;
    memset(&bCmdBuffer[2], 0x00, 5);  /* PRQA S 3200 */

    /* Copy valid UID bits */
    wSndBytes = (uint16_t)(((bNvbUidIn & 0xF0) >> 4) + ((bNvbUidIn & 0x0F) ? 1 : 0));
    memcpy(&bCmdBuffer[2], pUidIn, (size_t)wSndBytes);  /* PRQA S 3200 */
    wSndBytes += 2;

    /* SELECT: Add BCC */
    if (bIsSelect) {
        bNvbUidIn = 0x50;
        bCmdBuffer[6] = pUidIn[0] ^ pUidIn[1] ^ pUidIn[2] ^ pUidIn[3];
        ++wSndBytes;
    }

    /* Encode NVB */
    bCmdBuffer[1] = bNvbUidIn + 0x20;

    /* Adjust Rx-Align */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXALIGN, bNvbUidIn & 0x07));

    /* Adjust TxBits */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXLASTBITS, bNvbUidIn & 0x07));

    /* Enable Clearing of bits after collision */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_CLEARBITSAFTERCOLL, PH_ON));

    /* Send the ANTICOLLISION command */
    status = phhalHw_Exchange(pDataParams->pHalDataParams, PH_EXCHANGE_DEFAULT, bCmdBuffer, wSndBytes, &pRcvBuffer, &wRcvBytes);

    /* Disable Clearing of bits after collision */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_CLEARBITSAFTERCOLL, PH_OFF));

    /* Reset RxAlignment */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXALIGN, 0));

    /* Check status, Collision is allowed for anticollision command */
    if ((bIsSelect == 0) && ((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)) {
        /* Retrieve number of valid bits of last byte */
        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXLASTBITS, &wRcvBits));
    } else {
        /* Check for protocol error */
        if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS_INCOMPLETE_BYTE) {
            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
        }
        /* Return on other errors */
        else {
            PH_CHECK_SUCCESS(status);
        }

        /* whole byte valid */
        wRcvBits = 0;
    }

    /* Add received data to UID */
    if (bIsSelect == 0) {
        /* Retrieve byte-starting-index of received Uid */
        bUidStartIndex = (uint8_t)((bNvbUidIn & 0xF0) >> 4);

        /* Add new bitcount */
        bBitCount = (uint8_t)(((bNvbUidIn >> 4) << 3) + (wRcvBytes << 3) + wRcvBits);

        /* Last incomplete byte is added to wRcvBytes, so remove that again */
        if (wRcvBits > 0) {
            bBitCount -= 8;
        }

        /* Convert bitcount to NVB format */
        *pNvbUidOut = (uint8_t)(((bBitCount >> 3) << 4) + (bBitCount & 0x07));

        /* We do not tolerate more than (5 * 8 =)40 bits because it would lead to buffer overflows */
        if (*pNvbUidOut > 0x50) {
            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
        }

        /* After successfull reception, the UID must be exact 40 bits */
        if (((status & PH_ERR_MASK) == PH_ERR_SUCCESS) && (*pNvbUidOut != 0x50)) {
            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
        }

        /* Copy received bytes to uid */
        if (wRcvBytes > 0) {
            /* Incomplete byte sent: Merge Rx-Aligned first byte */
            if (bNvbUidIn & 0x07) {
                bCmdBuffer[2 + bUidStartIndex] |= pRcvBuffer[0];
            }
            /* Else just copy the first byte */
            else {
                bCmdBuffer[2 + bUidStartIndex] = pRcvBuffer[0];
            }

            /* Add the rest of the uid bytes */
            memcpy(&bCmdBuffer[2 + bUidStartIndex + 1], &pRcvBuffer[1], (size_t)(wRcvBytes - 1));  /* PRQA S 3200 */
        }

        /* Anticollision finished */
        if (*pNvbUidOut > 0x40) {
            /* Collision in BCC byte can never happen */
            if (*pNvbUidOut < 0x50) {
                return PH_ADD_COMPCODE(PH_ERR_FRAMING_ERROR, PH_COMP_PAL_ISO14443P3A);
            }

            /* Remove BCC from NvbUidOut */
            *pNvbUidOut = 0x40;
            --wRcvBytes;

            /* BCC Check */
            if ((bCmdBuffer[2] ^ bCmdBuffer[3] ^ bCmdBuffer[4] ^ bCmdBuffer[5]) != bCmdBuffer[6]) {
                return PH_ADD_COMPCODE(PH_ERR_FRAMING_ERROR, PH_COMP_PAL_ISO14443P3A);
            }
        }

        /* Copy UID */
        memcpy(pUidOut, &bCmdBuffer[2], bUidStartIndex + wRcvBytes);  /* PRQA S 3200 */
    }
    /* Return SAK instead of the UID */
    else {
        /* only one byte allowed */
        if (wRcvBytes != 1) {
            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
        }

        /* Cascade Bit is set */
        if (pRcvBuffer[0] & 0x04) {
            /* If additional cascade levels are impossible -> protocol error */
            if (bCascadeLevel == PHPAL_I14443P3A_CASCADE_LEVEL_3) {
                return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
            }

            /* Cascade tag does not match -> protocol error */
            if (pUidIn[0] != PHPAL_I14443P3A_CASCADE_TAG) {
                return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
            }

            /* Ignore Cascade Tag */
            memcpy(&pDataParams->abUid[pDataParams->bUidLength], &pUidIn[1], 3);  /* PRQA S 3200 */

            /* Increment Uid length */
            pDataParams->bUidLength += 3;
        }
        /* Cascade Bit is cleared -> no further cascade levels */
        else {
            /* Cascade tag does not match -> protocol error */
            if (pUidIn[0] == PHPAL_I14443P3A_CASCADE_TAG) {
                return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
            }

            /* Copy all uid bytes except BCC */
            memcpy(&pDataParams->abUid[pDataParams->bUidLength], &pUidIn[0], 4);  /* PRQA S 3200 */

            /* Increment Uid length */
            pDataParams->bUidLength += 4;

            /* Set UID complete flag */
            pDataParams->bUidComplete = 1;

            /* set default card timeout */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                             pDataParams->pHalDataParams,
                                                             PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,
                                                             PHPAL_I14443P3A_TIMEOUT_DEFAULT_MS));
        }

        /* Copy SAK */
        pUidOut[0] = pRcvBuffer[0];
    }

    return PH_ADD_COMPCODE(status, PH_COMP_PAL_ISO14443P3A);
}
/**************************************************************
*说明：bNvbUidIn是0x40的时候，执行sel命令的时候，返回的是SAK,否则就返回UID 
*      这个是标志决定的 
***************************************************************/
phStatus_t phpalI14443p3aEMV_Sw_Anticollision(
                                             phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                             uint8_t bCascadeLevel,
                                             uint8_t * pUidIn,
                                             uint8_t bNvbUidIn,
                                             uint8_t * pUidOut,
                                             uint8_t * pNvbUidOut
                                             )
{
    return  phpalI14443p3a_Sw_Anticollision(pDataParams, bCascadeLevel,pUidIn,bNvbUidIn,pUidOut,pNvbUidOut);

}
//{
//    phStatus_t  PH_MEMLOC_REM status;
//    phStatus_t  PH_MEMLOC_REM statusTmp;
//    uint8_t     PH_MEMLOC_REM bIsSelect;
//    uint8_t     PH_MEMLOC_REM bCmdBuffer[7];
//    uint8_t *   PH_MEMLOC_REM pRcvBuffer;
//    uint16_t    PH_MEMLOC_REM wSndBytes;
//    uint16_t    PH_MEMLOC_REM wRcvBytes;
//    uint16_t    PH_MEMLOC_REM wRcvBits;
//    uint8_t     PH_MEMLOC_REM bUidStartIndex;
//    uint8_t     PH_MEMLOC_REM bBitCount;
//    uint32_t    dTimeus = 0;
//
//    /* Check for invalid bNvbUidIn parameter */
//    if ((bNvbUidIn > 0x40) || ((bNvbUidIn & 0x0F) > 0x07)) {
//        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P3A);
//    }
//    // pDataParams->pHalDataParams->bDisableDF  = 1;//禁止过虑
//
//    /* Check for invalid bCascadeIndex parameter */
//    switch (bCascadeLevel) {
//    case PHPAL_I14443P3A_CASCADE_LEVEL_1:
//        /* Reset UID length */
//        pDataParams->bUidLength = 0;
//    case PHPAL_I14443P3A_CASCADE_LEVEL_2:
//    case PHPAL_I14443P3A_CASCADE_LEVEL_3:
//        break;
//    default:
//        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P3A);
//    }
//
//    /* Reset UID complete flag */
//    pDataParams->bUidComplete = 0;
//
//    /* ANTICOLLISION: Disable CRC */
//    if (bNvbUidIn != 0x40) {//如果是完整的4字节，那么就CRC使能，此时
//        bIsSelect = 0;
//        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_OFF));
//        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_OFF));
//    }
//    /* SELECT: Enable CRC */
//    else {
//        bIsSelect = 1;//当提供完整的UID的时候，会返回SAK，二不是UID，这个是
//        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_ON));
//        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_ON));
//    }
//
//    /* Init. command buffer */
//    bCmdBuffer[0] = bCascadeLevel;
//    memset(&bCmdBuffer[2], 0x00, 5);  /* PRQA S 3200 */
//
//    /* Copy valid UID bits */
//    wSndBytes = (uint16_t)(((bNvbUidIn & 0xF0) >> 4) + ((bNvbUidIn & 0x0F) ? 1 : 0));
//    memcpy(&bCmdBuffer[2], pUidIn, (size_t)wSndBytes);  /* PRQA S 3200 */
//    wSndBytes += 2;
//
//    /* SELECT: Add BCC */
//    if (bIsSelect) {
//        bNvbUidIn = 0x50;
//        bCmdBuffer[6] = pUidIn[0] ^ pUidIn[1] ^ pUidIn[2] ^ pUidIn[3];
//        ++wSndBytes;
//    }
//    /*  设置帧的等待时间   */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                     pDataParams->pHalDataParams,
//                                                     PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,
//                                                     1000));
//
//
//    /* Encode NVB */
//    bCmdBuffer[1] = bNvbUidIn + 0x20;
//
//    /* Adjust Rx-Align */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXALIGN, bNvbUidIn & 0x07));
//
//    /* Adjust TxBits */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXLASTBITS, bNvbUidIn & 0x07));
//
//    /* Enable Clearing of bits after collision */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_CLEARBITSAFTERCOLL, PH_ON));
//
//    /* Send the ANTICOLLISION command */
//    status = phhalHw_Exchange(pDataParams->pHalDataParams, PH_EXCHANGE_DEFAULT, bCmdBuffer, wSndBytes, &pRcvBuffer, &wRcvBytes);
//
//    if ((status & PH_ERR_MASK)==PH_ERR_SUCCESS) {//TA302_04
//        phhalHw_Rc663_GetFdt(pDataParams->pHalDataParams, status, &dTimeus);
//        if ((unsigned int)(dTimeus/9.44f)<9) {
//            printf("接收帧的时间为= %dETU,非法\n\r",(dTimeus/9.44f));
//            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//        }
//    }
//
//    /* Disable Clearing of bits after collision */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_CLEARBITSAFTERCOLL, PH_OFF));
//
//    /* Reset RxAlignment */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXALIGN, 0));
//
//    /* Check status, Collision is allowed for anticollision command */
//    if ((bIsSelect == 0) && ((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR)) {
//        /* Retrieve number of valid bits of last byte */
//        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXLASTBITS, &wRcvBits));
//    } else {
//        /* Check for protocol error */
//        if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS_INCOMPLETE_BYTE) {
//            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//        }
//        /* Return on other errors */
//        else {
//            PH_CHECK_SUCCESS(status);
//        }
//
//        /* whole byte valid */
//        wRcvBits = 0;
//    }
//
//    /* Add received data to UID */
//    if (bIsSelect == 0) {
//        /* Retrieve byte-starting-index of received Uid */
//        bUidStartIndex = (uint8_t)((bNvbUidIn & 0xF0) >> 4);
//
//        /* Add new bitcount */
//        bBitCount = (uint8_t)(((bNvbUidIn >> 4) << 3) + (wRcvBytes << 3) + wRcvBits);
//
//        /* Last incomplete byte is added to wRcvBytes, so remove that again */
//        if (wRcvBits > 0) {
//            bBitCount -= 8;
//        }
//
//        /* Convert bitcount to NVB format */
//        *pNvbUidOut = (uint8_t)(((bBitCount >> 3) << 4) + (bBitCount & 0x07));
//
//        /* We do not tolerate more than (5 * 8 =)40 bits because it would lead to buffer overflows */
//        if (*pNvbUidOut > 0x50) {
//            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//        }
//
//        /* After successfull reception, the UID must be exact 40 bits */
//        if (((status & PH_ERR_MASK) == PH_ERR_SUCCESS) && (*pNvbUidOut != 0x50)) {
//            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//        }
//
//        /* Copy received bytes to uid */
//        if (wRcvBytes > 0) {
//            /* Incomplete byte sent: Merge Rx-Aligned first byte */
//            if (bNvbUidIn & 0x07) {
//                bCmdBuffer[2 + bUidStartIndex] |= pRcvBuffer[0];
//            }
//            /* Else just copy the first byte */
//            else {
//                bCmdBuffer[2 + bUidStartIndex] = pRcvBuffer[0];
//            }
//
//            /* Add the rest of the uid bytes */
//            memcpy(&bCmdBuffer[2 + bUidStartIndex + 1], &pRcvBuffer[1], (size_t)(wRcvBytes - 1));  /* PRQA S 3200 */
//        }
//
//        /* Anticollision finished */
//        if (*pNvbUidOut > 0x40) {
//            /* Collision in BCC byte can never happen */
//            if (*pNvbUidOut < 0x50) {
//                return PH_ADD_COMPCODE(PH_ERR_FRAMING_ERROR, PH_COMP_PAL_ISO14443P3A);
//            }
//
//            /* Remove BCC from NvbUidOut */
//            *pNvbUidOut = 0x40;
//            --wRcvBytes;
//
//            /* BCC Check */
//            if ((bCmdBuffer[2] ^ bCmdBuffer[3] ^ bCmdBuffer[4] ^ bCmdBuffer[5]) != bCmdBuffer[6]) {
//                printf("BCC 错误\n\r");
//                return PH_ADD_COMPCODE(PH_ERR_FRAMING_ERROR, PH_COMP_PAL_ISO14443P3A);
//            }
//        }
//
//        /* Copy UID */
//        memcpy(pUidOut, &bCmdBuffer[2], bUidStartIndex + wRcvBytes);  /* PRQA S 3200 */
//    }
//    /* Return SAK instead of the UID */
//    else {
//        /* only one byte allowed */
//        if (wRcvBytes != 1) {
//            return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//        }
//
//        /* Cascade Bit is set */
//        if (pRcvBuffer[0] & 0x04) {//提示UID不完整
//            /* If additional cascade levels are impossible -> protocol error */
//            if (bCascadeLevel == PHPAL_I14443P3A_CASCADE_LEVEL_3) {//但是级别已经到了最后
//                return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//            }
//
//            /* Cascade tag does not match -> protocol error */
//            if (pUidIn[0] != PHPAL_I14443P3A_CASCADE_TAG) {//UID不完整但是标示不是0X88
//                return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//            }
//
//            /* Ignore Cascade Tag */
//            memcpy(&pDataParams->abUid[pDataParams->bUidLength], &pUidIn[1], 3);  /* PRQA S 3200 */
//
//            /* Increment Uid length */
//            pDataParams->bUidLength += 3;
//        }
//        /* Cascade Bit is cleared -> no further cascade levels */
//        else {//级别已经结束，但是还提示有效标示
//            /* Cascade tag does not match -> protocol error */
//            if (pUidIn[0] == PHPAL_I14443P3A_CASCADE_TAG) {
//                return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//            }
//
//            /* Copy all uid bytes except BCC */
//            memcpy(&pDataParams->abUid[pDataParams->bUidLength], &pUidIn[0], 4);  /* PRQA S 3200 */
//
//            /* Increment Uid length */
//            pDataParams->bUidLength += 4;
//
//            /* Set UID complete flag */
//            pDataParams->bUidComplete = 1;
//
//            /* set default card timeout */
//            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                             pDataParams->pHalDataParams,
//                                                             PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,
//                                                             PHPAL_I14443P3A_TIMEOUT_DEFAULT_MS));
//        }
//        /* Copy SAK */
//        pUidOut[0] = pRcvBuffer[0];
//    }
//
//    return PH_ADD_COMPCODE(status, PH_COMP_PAL_ISO14443P3A);
//}

phStatus_t phpalI14443p3a_Sw_Select(
                                   phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                   uint8_t bCascadeLevel,
                                   uint8_t * pUidIn,
                                   uint8_t * pSak
                                   )
{
    uint8_t PH_MEMLOC_REM bDummy;
    return phpalI14443p3aEMV_Sw_Anticollision(pDataParams, bCascadeLevel, pUidIn, 0x40, pSak, &bDummy);
}

phStatus_t phpalI14443p3aEMV_Sw_Select(
                                      phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                      uint8_t bCascadeLevel,
                                      uint8_t * pUidIn,
                                      uint8_t * pSak
                                      )
{
    uint8_t PH_MEMLOC_REM bDummy;
    return phpalI14443p3aEMV_Sw_Anticollision(pDataParams, bCascadeLevel, pUidIn, 0x40, pSak, &bDummy);
}



phStatus_t phpalI14443p3a_Sw_ActivateCard(
                                         phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                         uint8_t * pUidIn,
                                         uint8_t bLenUidIn,
                                         uint8_t * pUidOut,
                                         uint8_t * pLenUidOut,
                                         uint8_t * pSak,
                                         uint8_t * pMoreCardsAvailable,
                                         uint8_t * pAtq
                                         )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bCascadeLevel;
    uint8_t     PH_MEMLOC_COUNT bCascadeIndex;
    uint8_t     PH_MEMLOC_REM bUidIndex;
    uint8_t     PH_MEMLOC_REM bNvbUid;
    uint8_t     PH_MEMLOC_REM bAtqa[2];
    uint8_t     PH_MEMLOC_REM bUid[4];

    /* Parameter check */
    if ((bLenUidIn != 0) &&
        (bLenUidIn != 4) &&
        (bLenUidIn != 7) &&
        (bLenUidIn != 10)) {
        /* Given UID length is invalid, return error */
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P3A);
    }

    /* Clear Uid */
    memset(bUid, 0x00, 4);  /* PRQA S 3200 */

    /* Clear Uid lengths */
    bUidIndex = 0;
    *pLenUidOut = 0;

    /* Clear the more cards available flag */
    *pMoreCardsAvailable = PH_OFF;

    /* UidIn is not given -> ReqA */
    if (bLenUidIn == 0) {
        status = phpalI14443p3a_Sw_RequestA(pDataParams, pAtq);
    }
    /* UidIn is given -> WupA */
    else {
        status = phpalI14443p3a_Sw_WakeUpA(pDataParams, pAtq);
    }

    /* Collision error may happen */
    if ((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR) {
        /* Set the more cards available flag */
        *pMoreCardsAvailable = PH_ON;
    }
    /* Status check */
    else {
        PH_CHECK_SUCCESS(status);
    }

    /* Go through all cascade levels */
    for (bCascadeIndex = 0; bCascadeIndex < 3; bCascadeIndex++) {
        /* Set cascade level tags */
        switch (bCascadeIndex) {
        case 0:
            bCascadeLevel = PHPAL_I14443P3A_CASCADE_LEVEL_1;
            break;
        case 1:
            bCascadeLevel = PHPAL_I14443P3A_CASCADE_LEVEL_2;
            break;
        case 2:
            bCascadeLevel = PHPAL_I14443P3A_CASCADE_LEVEL_3;
            break;
        default:
            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P3A);
        }

        /* Copy know Uid part if neccessary */
        if (bLenUidIn == (bUidIndex + 4)) {
            /* Copy whole Uid if this is the only cascade level */
            memcpy(&bUid[0], &pUidIn[bUidIndex], 4);  /* PRQA S 3200 */

            /* [Incrementing Uid index would have no effect] */

            /* All bits except BCC are valid */
            bNvbUid = 0x40;
        } else if (bLenUidIn > (bUidIndex + 4)) {
            /* Prepend cascade tag if we expect more cascade levels*/
            bUid[0] = PHPAL_I14443P3A_CASCADE_TAG;

            /* Only three more uid bytes are valid for this level */
            memcpy(&bUid[1], &pUidIn[bUidIndex], 3);  /* PRQA S 3200 */

            /* Increment Uid index */
            bUidIndex += 3;

            /* All bits except BCC are valid */
            bNvbUid = 0x40;
        } else {
            /* No Uid given */
            bNvbUid = 0;
        }

        /* Anticollision Loop */
        while (bNvbUid != 0x40) {//一个级别是4个字节
            /* Perform single anticollision command */
            status = phpalI14443p3aEMV_Sw_Anticollision(pDataParams, bCascadeLevel, bUid, bNvbUid, bUid, &bNvbUid);

            /* Collision error occured, add one (zero-)bit to Uid */
            if ((status & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR) {
                /* Set the more cards available flag */
                *pMoreCardsAvailable = PH_ON;

                /* Increment NvbUid by one bit */
                if ((bNvbUid & 0x07) < 7) {
                    bNvbUid++;
                }
                /* This is more difficult if we have to increment the bytecount */
                else {
                    bNvbUid = (uint8_t)((((bNvbUid & 0xF0) >> 4) + 1) << 4);
                }
            } else {
                /* Check success */
                PH_CHECK_SUCCESS(status);
            }
        }

        /* Select cascade level */
        PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p3a_Sw_Select(pDataParams, bCascadeLevel, bUid, pSak));

        /* Cascade Bit is cleared -> no further cascade levels */
        if (!(pSak[0] & 0x04)) {
            break;
        }
    }

    /* Return UID */
    memcpy(pUidOut, pDataParams->abUid, pDataParams->bUidLength);  /* PRQA S 3200 */
    *pLenUidOut = pDataParams->bUidLength;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P3A);
}

phStatus_t phpalI14443p3a_Sw_Exchange(
                                     phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                     uint16_t wOption,
                                     uint8_t * pTxBuffer,
                                     uint16_t wTxLength,
                                     uint8_t ** ppRxBuffer,
                                     uint16_t * pRxLength
                                     )
{
    /* direct mapping of hal exchange function */
    return phhalHw_Exchange(
                           pDataParams->pHalDataParams,
                           wOption,
                           pTxBuffer,
                           wTxLength,
                           ppRxBuffer,
                           pRxLength);
}

phStatus_t phpalI14443p3a_Sw_RequestAEx(
                                       phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                       uint8_t bReqCode,
                                       uint8_t * pAtqa
                                       )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM cmd[1];
    uint8_t *   PH_MEMLOC_REM pResp;
    uint16_t    PH_MEMLOC_REM wRespLength;


//    pDataParams->pHalDataParams->bDisableDF  = 1;//禁止过虑

    /* Disable MIFARE(R) Crypto1 */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                     pDataParams->pHalDataParams,
                                                     PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1,
                                                     PH_ON));

    /* 设置比特率 */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                     pDataParams->pHalDataParams,
                                                     PHHAL_HW_CONFIG_TXDATARATE,
                                                     PHHAL_HW_RF_DATARATE_106));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                     pDataParams->pHalDataParams,
                                                     PHHAL_HW_CONFIG_RXDATARATE,
                                                     PHHAL_HW_RF_DATARATE_106));


    /*  设置帧的等待时间   */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                     pDataParams->pHalDataParams,
                                                     PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,
                                                     PHPAL_I14443P3A_SELECTION_TIME_US + PHPAL_I14443P3A_EXT_TIME_US));

    /*设置帧的保护时间  8个ETU*/
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXDEAFBITS, 8));

    /* 禁止CRC检验 */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_OFF));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_OFF));

    /* 设置最后字节的数据长度位 */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXLASTBITS, 7));

    /* Send ReqA command */
    cmd[0] = bReqCode;
   // PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Rc663_ExchangeISO(pDataParams->pHalDataParams, PH_EXCHANGE_DEFAULT, cmd, 1, &pResp, &wRespLength));
    phhalHw_Exchange(pDataParams->pHalDataParams, PH_EXCHANGE_DEFAULT, cmd, 1, &pResp, &wRespLength);

    /* Check and copy ATQA */
    if (wRespLength == PHPAL_I14443P3A_ATQA_LENGTH) {
        memcpy(pAtqa, pResp, PHPAL_I14443P3A_ATQA_LENGTH);  /* PRQA S 3200 */
    } else {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P3A);
}



/**************************************************************
*次函数额外使用了1.5MS的时间,现在就是200us
***************************************************************/
//phStatus_t phpalI14443p3aEMV_Sw_RequestAExAddTime(
//                                                 phpalI14443p3a_Sw_DataParams_t * pDataParams,
//                                                 uint8_t bReqCode,
//                                                 uint8_t * pAtqa,
//                                                 uint16_t  wUnit,
//                                                 uint16_t  wValue
//                                                 )
//{
//    phStatus_t  PH_MEMLOC_REM statusTmp;
//    uint8_t     PH_MEMLOC_REM cmd[1];
//    uint8_t *   PH_MEMLOC_REM pResp;
//    uint8_t  Resp[10];
//
//    uint16_t    PH_MEMLOC_REM wRespLength;
//
//
//
//    //禁止数据错误处理
//    // ((phhalHw_Rc663_DataParams_t*)(pDataParams->pHalDataParams))->bDateFliter = PH_OFF;
//
//    ((phhalHw_Rc663_DataParams_t*)(pDataParams->pHalDataParams))->bDateFliter = PH_OFF;
//
//    /* Disable MIFARE(R) Crypto1 */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                     pDataParams->pHalDataParams,
//                                                     PHHAL_HW_CONFIG_DISABLE_MF_CRYPTO1,
//                                                     PH_ON));
//
//
//
//    /* 设置比特率   这两个函数使用了1MS时间,现在使用300us
//
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                     pDataParams->pHalDataParams,
//                                                     PHHAL_HW_CONFIG_TXDATARATE,
//                                                     PHHAL_HW_RF_DATARATE_106));
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                     pDataParams->pHalDataParams,
//                                                     PHHAL_HW_CONFIG_RXDATARATE,
//                                                     PHHAL_HW_RF_DATARATE_106));
//    */
//
//
//
//    /*  设置帧的等待时间    */
//    if (wUnit==PHHAL_HW_TIME_MICROSECONDS) {
//        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                         pDataParams->pHalDataParams,
//                                                         PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,
//                                                         wValue));
//    } else {
//        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                         pDataParams->pHalDataParams,
//                                                         PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS,
//                                                         wValue));
//    }
//    // PHPAL_I14443P3A_SELECTION_TIME_US + PHPAL_I14443P3A_EXT_TIME_US));
//
//    /*设置帧的保护时间  8个ETU*/
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXDEAFBITS, 5));
//
//    /* 禁止CRC检验 */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXCRC, PH_OFF));
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXCRC, PH_OFF));
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_PARITY, PH_ON));
//
//
//    /* 设置最后字节的数据长度位 */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXLASTBITS, 7));
//
//
//    /* Send ReqA command */
//    cmd[0] = bReqCode;
//
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(pDataParams->pHalDataParams, PH_EXCHANGE_DEFAULT, cmd, 1, &pResp, &wRespLength));
//
//    // printf("wRespLength = %02x\n\r",wRespLength);
//
//    /* Check and copy ATQA */
//    if (wRespLength == PHPAL_I14443P3A_ATQA_LENGTH) {
//        memcpy(pAtqa, pResp, PHPAL_I14443P3A_ATQA_LENGTH);  /* PRQA S 3200 */
//    } else {
//        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P3A);
//    }
//
//    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P3A);
//}


phStatus_t phpalI14443p3a_Sw_GetSerialNo(
                                        phpalI14443p3a_Sw_DataParams_t * pDataParams,
                                        uint8_t * pUidOut,
                                        uint8_t * pLenUidOut
                                        )
{
    /* Return an error if UID is not complete */
    if (!pDataParams->bUidComplete) {
        return PH_ADD_COMPCODE(PH_ERR_USE_CONDITION, PH_COMP_PAL_ISO14443P3A);
    }

    /* Copy UID */
    memcpy(pUidOut, pDataParams->abUid, pDataParams->bUidLength);  /* PRQA S 3200 */
    *pLenUidOut = pDataParams->bUidLength;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P3A);
}





/**************************************************************
* 
* 激活卡
***************************************************************/
//phStatus_t  phpal14443p3aEMV_SW_ActiveCard(ContextInfo *ReaderContextInfo,
//                                           phpalI14443p3a_Sw_DataParams_t * pDataParams,
//                                           uint8_t * pUidIn,
//                                           uint8_t bLenUidIn,
//                                           uint8_t * pUidOut,
//                                           uint8_t * pLenUidOut,
//                                           uint8_t * pSak,
//                                           uint8_t * pMoreCardsAvailable,
//                                           uint8_t * pAtq
//                                          ){
//    phStatus_t  PH_MEMLOC_REM status;
//    phStatus_t  PH_MEMLOC_REM statusTmp;
//    uint8_t     PH_MEMLOC_REM bCascadeLevel;
//    uint8_t     PH_MEMLOC_COUNT bCascadeIndex;
//    uint8_t     PH_MEMLOC_REM bUidIndex;
//    uint8_t     PH_MEMLOC_REM bNvbUid;
//    //static uint8_t     PH_MEMLOC_REM pAtq[2];
//    static uint8_t     PH_MEMLOC_REM bUid[4];
//    uint32_t    dTimeus = 0;
//    uint8_t     cnt = 0;
//    uint8_t     bCascAnticollLoop = 0,bSeltctLoop = 0;
//
//
//
//    /* Parameter check */
//    if ((bLenUidIn != 0) &&
//        (bLenUidIn != 4) &&
//        (bLenUidIn != 7) &&
//        (bLenUidIn != 10)) {
//        /* Given UID length is invalid, return error */
//        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P3A);
//    }
//
//    /* Clear Uid */
//    memset(bUid, 0x00, 4);  /* PRQA S 3200 */
//
//    /* Clear Uid lengths */
//    bUidIndex = 0;
//    *pLenUidOut = 0;
//
//    /* Clear the more cards available flag */
//    *pMoreCardsAvailable = PH_OFF;
//
//
//
//    if ((ReaderContextInfo->TypeAFlag==0)||(ReaderContextInfo->TypeAFlag==1)) {
//        DelayUs(5000);//延时4.5ms
//        if (ReaderContextInfo->TypeAFlag==0) {
//            //时间为200个ETU)9.44 = 1888,禁止CRC，使能parity
//            status = phpalI14443p3aEMV_Sw_RequestAExAddTime(pDataParams,
//                                                            PHPAL_I14443P3A_WAKEUP_CMD,
//                                                            pAtq,
//                                                            0,
//                                                            1900);
//            if ((status & PH_ERR_MASK)!=PH_ERR_IO_TIMEOUT) {
//                //PCD帧的延时时间，最小是6780/f = 500us
//                phhalHw_Wait(pDataParams->pHalDataParams,PHHAL_HW_TIME_MICROSECONDS,600);
//                phpalI14443p3a_Sw_HaltA(pDataParams);
//                //如果A B卡同时存在，就场复位 6MS
//                if (ReaderContextInfo->TypeBFlag) {//TA303
//
//                    status =  phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                    if (DebugPinErr()) {
//                        printf("polling A卡的时候，发现已经存在B卡\n\r");
//                    }
//                    ReaderContextInfo->TypeAFlag = 0;
//                    ReaderContextInfo->TypeBFlag = 0;
//                } else {
//                    ReaderContextInfo->TypeAFlag = 1;
//                }
//                if (DebugPinErr())
//                    printf("phpalI14443p3aEMV_Sw_RequestAExAddTime= %02x\n\r",(status & PH_ERR_MASK));
//                return status;
//            } else {
//
//                PH_CHECK_SUCCESS(status);
//            }
//        } else {
//            //时间为5.5ms,禁止CRC，使能parity
//            pAtq[0] = 0;
//            pAtq[1] = 0;
//            cnt = 0;
//            while (1) {
//
//                status = phpalI14443p3aEMV_Sw_RequestAExAddTime(pDataParams,
//                                                                PHPAL_I14443P3A_WAKEUP_CMD,
//                                                                pAtq,
//                                                                PHHAL_HW_TIME_MICROSECONDS,
//                                                                5500);
//                phhalHw_Rc663_GetFdt(pDataParams->pHalDataParams, status, &dTimeus);
//                //协议错误就退出
//                if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS) {//TA304
//                    if (((pAtq[0] & 0xc0)==0xc0)||
//                        (((pAtq[0] & 0x1F)!=1)&&((pAtq[0] & 0x1F)!=2)&&((pAtq[0] & 0x1F)!=4)
//                         &&((pAtq[0] & 0x1F)!=8)&&((pAtq[0] & 0x1F)!=16))||// check lower 5 bits, for tag-type//比特防冲突有多个1的话
//                        (((unsigned int)(dTimeus/9.44f))<9)) {//FWT不能小于9ETU
//                        //FWT不对，应该==9
//                        if (DebugPinErr()) {
//                            printf("tow ATQA返回的ATQA不复合规范 status = %x;atq[0] = %02x;atq[1] = %02x;ETU = %d\n\r",
//                                   (signed char)status,pAtq[0],pAtq[1],(dTimeus/9.44f));
//                        }
//                        ReaderContextInfo->TypeAFlag = 0;
//                        status =  phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                        return status;
//                    } else {
//                        // if (DebugPinRetry())
//                        {
//                            printf("ATQA TOW  ETU = %d\n\r",(unsigned int)(dTimeus/9.44));
//                        }
//                        ReaderContextInfo->TypeAFlag = 2;
//
//                        break;
//                    }
//                    //超时就重发4次
//                } else if ((status & PH_ERR_MASK)==PH_ERR_IO_TIMEOUT) {//如果超时没有回应，就继续 TA311
//                    cnt++;
//                    if (cnt>=4) {
//                        ReaderContextInfo->TypeAFlag = 0;
//                        phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                        return status;
//                    }
//                } else {                 //其他的一切错误，不进行容错
//
//                    if (DebugPinErr())
//                        printf("phpalI14443p3aEMV_Sw_RequestAExAddTime status = %02X\n\r",(status & PH_ERR_MASK));
//                    ReaderContextInfo->TypeAFlag = 0;
//                    phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                    return status;
//                }
//            }
//        }
//    }
//
//    if (ReaderContextInfo->TypeAFlag==2) {
//        bNvbUid = 0;
//        /* Go through all cascade levels */
//        for (bCascadeIndex = 0; bCascadeIndex < 3; bCascadeIndex++) {
//
//            /* Set cascade level tags */
//            switch (bCascadeIndex) {
//            case 0:
//                bCascadeLevel = PHPAL_I14443P3A_CASCADE_LEVEL_1;
//                break;
//            case 1:
//                bCascadeLevel = PHPAL_I14443P3A_CASCADE_LEVEL_2;
//                break;
//            case 2:
//                bCascadeLevel = PHPAL_I14443P3A_CASCADE_LEVEL_3;
//                break;
//            default:
//                return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P3A);
//            }
//            //printf("bCascadeLevel = %02x\n\r",bCascadeLevel);
//
//
//            /*放冲突选择，超时重发三次*/
//            for (bCascAnticollLoop = 0; bCascAnticollLoop < 3; bCascAnticollLoop++) {
//                //帧等待时间
//                phhalHw_Wait(pDataParams->pHalDataParams,PHHAL_HW_TIME_MICROSECONDS,600);
//                status = phpalI14443p3aEMV_Sw_Anticollision(pDataParams, bCascadeLevel, bUid, bNvbUid, bUid, &bNvbUid);
//                //如果ATQA返回的只有一个级别，那么返回88，就错误  TA302
//                if ((status & PH_ERR_MASK)==PH_ERR_SUCCESS) {
//                    if ((((pAtq[0]>>6)&0x03)==0)&&(bUid[bUidIndex]==0x88)) {
//                        ReaderContextInfo->TypeAFlag = 0;
//                        phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                        if (DebugPinErr())
//                            printf("ATQA返回的只有一个级别,都是返回有CT=0X88级联标志\n\r");
//                        return status;
//                    } else {
//                        //超时重发4次
//                        for (bSeltctLoop = 0; bSeltctLoop < 4; bSeltctLoop++) {
//                            phhalHw_Wait(pDataParams->pHalDataParams,PHHAL_HW_TIME_MICROSECONDS,600);
//                            statusTmp = phpalI14443p3aEMV_Sw_Select(pDataParams, bCascadeLevel, bUid, pSak);
//                            if ((statusTmp & PH_ERR_MASK)==PH_ERR_SUCCESS) {
//                                phhalHw_Rc663_GetFdt(pDataParams->pHalDataParams, statusTmp, &dTimeus);
//                                //协议错误，复位电磁场
//                                if (((unsigned int)(dTimeus/9.44f))<9) {
//                                    ReaderContextInfo->TypeAFlag = 0;
//                                    phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                                    printf("phpalI14443p3aEMV_Sw_Select错误  FWT = %dETU\n\r",((unsigned int)(dTimeus/9.44f)));
//                                    return statusTmp;
//                                }
//                                /* Copy know Uid part if neccessary */
//                                if (bLenUidIn == (bUidIndex + 4)) {
//                                    memcpy(&bUid[0], &pUidIn[bUidIndex], 4);  /* PRQA S 3200 */
//                                    bNvbUid = 0x40;
//                                } else if (bLenUidIn > (bUidIndex + 4)) {
//                                    bUid[0] = PHPAL_I14443P3A_CASCADE_TAG;
//                                    memcpy(&bUid[1], &pUidIn[bUidIndex], 3);  /* PRQA S 3200 */
//                                    bUidIndex += 3;
//                                    bNvbUid = 0x40;
//                                } else {
//                                    bNvbUid = 0;
//                                }
//                                break;
//
//
//                                /*非超时错误，复位电磁场*/
//                            } else if ((statusTmp & PH_ERR_MASK)!=PH_ERR_IO_TIMEOUT) {
//                                ReaderContextInfo->TypeAFlag = 0;
//                                phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                                if (DebugPinErr())
//                                    printf("ATQA返回的只有一个级别,都是返回有CT=0X88级联标志\n\r");
//                                return statusTmp;
//                            } else {
//                                if (DebugPinRetry())
//                                    printf("phpalI14443p3aEMV_Sw_Select 超时次数 = d;\n\r",bCascAnticollLoop);
//                            }
//                        }
//
//                        if ( (pSak[0] & 0x04) &&(bCascadeIndex<(pAtq[0]>>6))) {//如果提示不完整而且级别没有到
//                            //printf("有级链\n\r");
//                            break; ;
//                        } else if (((pSak[0]&0x24) == 0x20)&&(bCascadeIndex==(pAtq[0]>>6))) {//UID完整而且级别已到
//                            ReaderContextInfo->TypeAFlag = 3;
////                          printf("phpalI14443p3aEMV_Sw_Select 协议错误pSak = %02x;bCascadeIndex = %02x"
////                                 "pAtqa(bit6-7) = %02x;pDataParams->bUidLength = %02x\n\r",pSak[0],bCascadeIndex,(pAtq[0]>>6),pDataParams->bUidLength);
//                            break;
//                        } else {//其他的协议错误，复位电磁场
//                            // ReaderContextInfo->TypeAFlag = 0;
//                            // phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                            //if (DebugPinErr())
//                            // printf("phpalI14443p3aEMV_Sw_Select 协议错误pSak = %02x;bCascadeIndex = %02x"
//                            //       "pAtqa(bit6-7) = %02x\n\r",pSak[0],bCascadeIndex,(pAtq[0]>>6));
//                            return statusTmp;
//                        }
//                        if (bSeltctLoop==4) {
//                            ReaderContextInfo->TypeAFlag = 0;
//                            phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                            if (DebugPinErr())
//                                printf("phpalI14443p3aEMV_Sw_Select 超时退出 \n\r");
//                            return statusTmp;
//                        }
//
//                        break;
//                    }
//                } else if ((status & PH_ERR_MASK)==PH_ERR_IO_TIMEOUT) {
//                    if (DebugPinRetry())
//                        printf("Anticollision 超时\n\r");
//                } else {
//                    ReaderContextInfo->TypeAFlag = 0;
//                    phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                    if (DebugPinErr())
//                        printf("Anticollision 有非超时错误，代码:%02x\n\r",status);
//                    return status;
//
//                }
//            }
//            if (bCascAnticollLoop==3) {
//                ReaderContextInfo->TypeAFlag = 0;
//                phhalHw_Rc663_FieldReset(pDataParams->pHalDataParams);
//                if (DebugPinErr())
//                    printf("Anticollision 超时 复位\n\r");
//                return status;
//            }
//            if (ReaderContextInfo->TypeAFlag==3) {
//
//                //phpal14443p3aEMV_SW_RemoveCard(ReaderContextInfo,pDataParams);
//                break;
//            }
//        }
//        /* Return UID */
//        memcpy(pUidOut, pDataParams->abUid, pDataParams->bUidLength);  /* PRQA S 3200 */
//        *pLenUidOut = pDataParams->bUidLength;
//    }
//
//    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P3A);
//}


/**************************************************************
* 
* 移除卡片
***************************************************************/
//phStatus_t  phpal14443p3aEMV_SW_RemoveCard(ContextInfo *ReaderContextInfo , void * pDataParams)
//{
//    phStatus_t  PH_MEMLOC_REM status;
//    unsigned char  bAtqa[2];
//    unsigned char loop;
//
//    if (ReaderContextInfo->TypeAFlag) {
//        phhalHw_Rc663_FieldOff( ((phpalI14443p3a_Sw_DataParams_t *)pDataParams)->pHalDataParams);
//        DelayMs(6);
//        phhalHw_Rc663_FieldOn( ((phpalI14443p3a_Sw_DataParams_t *)pDataParams)->pHalDataParams);
//        DelayMs(6);
//
//        loop=1;
//        for ( ;loop<=3;loop++) {
//            if (loop<=2) {
//                status = phpalI14443p3aEMV_Sw_RequestAExAddTime(pDataParams,
//                                                                PHPAL_I14443P3A_WAKEUP_CMD,
//                                                                bAtqa,
//                                                                PHHAL_HW_TIME_MICROSECONDS,
//                                                                //1900); // 禁止CRC,使能奇偶校验
//                                                                1900); // 禁止CRC,使能奇偶校验
//                if ((status & PH_ERR_MASK)!=PH_ERR_IO_TIMEOUT) {
//                    //printf("RequestAEx:status = %02x\n\r",status)  ;
//                    //PCD帧的延时时间，最小是6780/f = 500us  64
//                    phhalHw_Rc663_Wait(((phpalI14443p3a_Sw_DataParams_t *)pDataParams)->pHalDataParams,PHHAL_HW_TIME_MICROSECONDS,600);
//                    phpalI14443p3a_Sw_HaltA(pDataParams);
//                    //printf("RequestAEx:status = %02x\n\r",status)  ;
//                    DelayMs(6);
//                    loop = 0;
//                } else
//                    DelayMs(6);
//
//            } else {
//                status = phpalI14443p3aEMV_Sw_RequestAExAddTime((phpalI14443p3a_Sw_DataParams_t *)pDataParams,
//                                                                PHPAL_I14443P3A_WAKEUP_CMD,
//                                                                bAtqa,
//                                                                PHHAL_HW_TIME_MICROSECONDS,
//                                                                300);
//                if ((status & PH_ERR_MASK)!=PH_ERR_IO_TIMEOUT) {
//                    //PCD帧的延时时间，最小是6780/f = 500us
//                    phhalHw_Rc663_Wait(((phpalI14443p3a_Sw_DataParams_t *)pDataParams)->pHalDataParams,PHHAL_HW_TIME_MICROSECONDS,600);
//                    phpalI14443p3a_Sw_HaltA((phpalI14443p3a_Sw_DataParams_t *)pDataParams);
//                    DelayMs(6);
//                    loop = 0;
//                }
//            }
//        }
//        if (loop==4) {
//            phhalHw_Rc663_FieldOff( ((phpalI14443p3a_Sw_DataParams_t *)pDataParams)->pHalDataParams);
//            DelayMs(6);
//            phhalHw_Rc663_FieldOn( ((phpalI14443p3a_Sw_DataParams_t *)pDataParams)->pHalDataParams);
//            DelayMs(1);
//            printf("移除成功\n\r");
//        }
//        ReaderContextInfo->TypeAFlag = 0;//空闲
//    }
//    return  0;
//
//}






#endif /* NXPBUILD__PHPAL_I14443P3A_SW */
