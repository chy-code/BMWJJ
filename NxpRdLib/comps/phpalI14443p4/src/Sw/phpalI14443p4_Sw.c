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
* Software ISO14443-4 Component of Reader Library Framework.
* $Author: santosh.d.araballi $
* $Revision: 1.6 $
* $Date: Fri Apr 15 09:27:26 2011 $
*
* History:
*  CHu: Generated 29. May 2009
*
*/

#include <ph_Status.h>
#include <phhalHw.h>
#include <phpalI14443p4.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHPAL_I14443P4_SW

    #include "phpalI14443p4_Sw_Int.h"
    #include "phpalI14443p4_Sw.h"




static const uint16_t PH_MEMLOC_CONST_ROM bI14443p4_FsTable[9] = {16, 24, 32, 40, 48, 64, 96, 128, 256};

    #define PHPAL_I14443P4_SW_IS_BLOCKNR_EQUAL(bPcb)                                    \
    (                                                                               \
    ((((bPcb) & PHPAL_I14443P4_SW_PCB_BLOCKNR) ^ pDataParams->bPcbBlockNum) == 0)   \
    ? 1 : 0                                                                         \
    )

    #define PHPAL_I14443P4_SW_IS_ACK(bPcb)                                  \
    (                                                                   \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_NAK) == PHPAL_I14443P4_SW_PCB_ACK) \
    ? 1 : 0                                                             \
    )

    #define PHPAL_I14443P4_SW_IS_NACK(bPcb)                                  \
    (                                                                   \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_NAK) == PHPAL_I14443P4_SW_PCB_NAK) \
    ? 1 : 0                                                             \
    )

    #define PHPAL_I14443P4_SW_IS_CHAINING(bPcb)                                         \
    (                                                                               \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_CHAINING) == PHPAL_I14443P4_SW_PCB_CHAINING)   \
    ? 1 : 0                                                                         \
    )

    #define PHPAL_I14443P4_SW_IS_WTX(bPcb)                                  \
    (                                                                   \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_WTX) == PHPAL_I14443P4_SW_PCB_WTX) \
    ? 1 : 0                                                             \
    )

    #define PHPAL_I14443P4_SW_IS_DESELECT(bPcb)                                     \
    (                                                                           \
    (((bPcb) & PHPAL_I14443P4_SW_PCB_WTX) == PHPAL_I14443P4_SW_PCB_DESELECT)    \
    ? 1 : 0                                                                     \
    )

    #define PHPAL_I14443P4_SW_IS_I_BLOCK(bPcb)                                  \
    (                                                                       \
    (((bPcb) & PHPAL_I14443P4_SW_BLOCK_MASK) == PHPAL_I14443P4_SW_I_BLOCK)  \
    ? 1 : 0                                                                 \
    )

    #define PHPAL_I14443P4_SW_IS_R_BLOCK(bPcb)                                  \
    (                                                                       \
    (((bPcb) & PHPAL_I14443P4_SW_BLOCK_MASK) == PHPAL_I14443P4_SW_R_BLOCK)  \
    ? 1 : 0                                                                 \
    )

    #define PHPAL_I14443P4_SW_IS_S_BLOCK(bPcb)                                  \
    (                                                                       \
    (((bPcb) & PHPAL_I14443P4_SW_BLOCK_MASK) == PHPAL_I14443P4_SW_S_BLOCK)  \
    ? 1 : 0                                                                 \
    )

//  #define PHPAL_I14443P4_SW_IS_INVALID_BLOCK_STATUS(bStatus)                  \//ISO 标准
//  ((                                                                      \
//  (((bStatus) & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT) ||                     \
//  (((bStatus) & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR) ||                \
//  (((bStatus) & PH_ERR_MASK) == PH_ERR_FRAMING_ERROR) ||                  \
//  (((bStatus) & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR))                  \
//  ? 1 : 0                                                                 \
//  )

    #define PHPAL_I14443P4_SW_IS_INVALID_BLOCK_STATUS(bStatus)                  \
    ((                                                                      \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT) ||                     \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR) ||                \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_FRAMING_ERROR) ||                  \
    (((bStatus) & PH_ERR_MASK) == PH_ERR_COLLISION_ERROR))                  \
    ? 1 : 0                                                                 \
    )


phStatus_t phpalI14443p4_Sw_Init(
                                phpalI14443p4_Sw_DataParams_t * pDataParams, 
                                uint16_t wSizeOfDataParams,
                                void * pHalDataParams
                                )
{
    if (sizeof(phpalI14443p4_Sw_DataParams_t) != wSizeOfDataParams) {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_PAL_ISO14443P4);
    }
    PH_ASSERT_NULL (pDataParams);
    PH_ASSERT_NULL (pHalDataParams);

    /* Init private data */
    pDataParams->wId            = PH_COMP_PAL_ISO14443P4 | PHPAL_I14443P4_SW_ID;
    pDataParams->pHalDataParams = pHalDataParams;

    /* Reset protocol to defaults */
    return phpalI14443p4_Sw_ResetProtocol(pDataParams);
}

phStatus_t phpalI14443p4_Sw_SetProtocol(
                                       phpalI14443p4_Sw_DataParams_t * pDataParams,
                                       uint8_t   bCidEnable,
                                       uint8_t   bCid,
                                       uint8_t   bNadEnable,
                                       uint8_t   bNad,
                                       uint8_t   bFwi,
                                       uint8_t   bFsdi,
                                       uint8_t   bFsci
                                       )
{
    /* Rule A, ISO/IEC 14443-4:2008(E), init. Blocknumber to zero */
    pDataParams->bPcbBlockNum = 0;

    /* Check parameters */
    if ((bCid > PHPAL_I14443P4_CID_MAX) ||
        (bFwi > PHPAL_I14443P4_FWI_MAX) ||
        (bFsdi > PHPAL_I14443P4_FRAMESIZE_MAX) ||
        (bFsci > PHPAL_I14443P4_FRAMESIZE_MAX)) {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
    }

    /* Apply parameters */
    pDataParams->bCidEnabled = bCidEnable;
    pDataParams->bCid = bCid;
    pDataParams->bNadEnabled = bNadEnable;
    pDataParams->bNad = bNad;
    pDataParams->bFwi = bFwi;
    pDataParams->bFsdi = bFsdi;
    pDataParams->bFsci = bFsci;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_ResetProtocol(
                                         phpalI14443p4_Sw_DataParams_t * pDataParams
                                         )
{
    /* Set default state */
    pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_TX;

    /* Rule A, ISO/IEC 14443-4:2008(E), init. Blocknumber to zero */
    pDataParams->bPcbBlockNum = 0;

    /* Apply default parameters */
    pDataParams->bCidEnabled        = 0x00;
    pDataParams->bCid               = 0x00;
    pDataParams->bNadEnabled        = 0x00;
    pDataParams->bNad               = 0x00;
    pDataParams->bFwi               = PHPAL_I14443P4_SW_FWI_DEFAULT;
    pDataParams->bFsdi              = 0x00;
    pDataParams->bFsci              = PHPAL_I14443P4_SW_FSCI_DEFAULT;
    pDataParams->bMaxRetryCount     = PHPAL_I14443P4_SW_MAX_RETRIES_DEFAULT;

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_Deselect(
                                    phpalI14443p4_Sw_DataParams_t * pDataParams
                                    )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
    uint16_t    PH_MEMLOC_REM wIsoFrameLen;
    uint8_t *   PH_MEMLOC_REM pResp;
    uint16_t    PH_MEMLOC_REM wRespLen;
    uint8_t     PH_MEMLOC_REM bInvalidBlock;
    uint8_t     PH_MEMLOC_REM bResponseReceived;
    uint16_t    PH_MEMLOC_REM wRetries;

    /* Build S(DESELECT) frame */
    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildSBlock(
                                                                pDataParams->bCidEnabled,
                                                                pDataParams->bCid,
                                                                0,
                                                                0,
                                                                bIsoFrame,
                                                                &wIsoFrameLen));

    /* Set DESELECT timeout */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                     pDataParams->pHalDataParams,
                                                     PHHAL_HW_CONFIG_TIMEOUT_VALUE_US,
                                                     PHPAL_I14443P4_SW_FWT_DESELECT_US + PHPAL_I14443P4_EXT_TIME_US));

    /* Retrieve max. retry count */
    wRetries = (uint16_t)pDataParams->bMaxRetryCount + 1;

    /* Reset response received flag */
    bResponseReceived = 0;

    /* Do as long as invalid responses are received
    and the retry counter has not reached zero.*/
    do {
        /* Send the S(DESELECT) request */
        status = phhalHw_Exchange(
                                 pDataParams->pHalDataParams,
                                 PH_EXCHANGE_DEFAULT,
                                 bIsoFrame,
                                 wIsoFrameLen,
                                 &pResp,
                                 &wRespLen);

        /* Status --> InvalidBlock mapping */
        bInvalidBlock = (uint8_t)PHPAL_I14443P4_SW_IS_INVALID_BLOCK_STATUS(status);
        if (!bInvalidBlock) {
            /* Check for other errors */
            PH_CHECK_SUCCESS(status);

            /* Signal that we've received something */
            bResponseReceived = 1;

            /* Rule 8, ISO/IEC 14443-4:2008(E), "...the S(DESELECT) request may be re-transmitted..." */
            if (wRespLen == wIsoFrameLen) {
                /* Mask out Power-Level-Indication */
                if (pDataParams->bCidEnabled) {
                    pResp[1] &= 0x3F;
                }

                /* Verify S(DESELECT) Response */
                if (memcmp(bIsoFrame, pResp, wRespLen) != 0) {
                    status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
                }
            } else {
                status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
            }
        }
    }
    /* Retry as long as neccessary */
    while ((bInvalidBlock) && (--wRetries));

    /* Operation not successful */
    if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS) {
        /* Return ERR_RECOVERY_FAILED if some response has been received before (bMaxRetryCount = 0 suppresses the retry behaviour) */
        if ((pDataParams->bMaxRetryCount > 0) && (bResponseReceived)) {
            status = PH_ADD_COMPCODE(PHPAL_I14443P4_ERR_RECOVERY_FAILED, PH_COMP_PAL_ISO14443P4);
        }
    }

    return status;
}

phStatus_t phpalI14443p4_Sw_PresCheck(
                                     phpalI14443p4_Sw_DataParams_t * pDataParams
                                     )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
    uint8_t *   PH_MEMLOC_REM pIsoFrameResp;
    uint16_t    PH_MEMLOC_REM wIsoFrameLen;

    /* Build R(NAK) frame */
    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                                                                pDataParams->bCidEnabled,
                                                                pDataParams->bCid,
                                                                pDataParams->bPcbBlockNum,
                                                                0,
                                                                bIsoFrame,
                                                                &wIsoFrameLen));

    /* Send frame */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(
                                                    pDataParams->pHalDataParams,
                                                    PH_EXCHANGE_DEFAULT,
                                                    bIsoFrame,
                                                    wIsoFrameLen,
                                                    &pIsoFrameResp,
                                                    &wIsoFrameLen));

    /* Check if we got a valid R(ACK) response */
    /* */
    if (PHPAL_I14443P4_SW_IS_R_BLOCK(pIsoFrameResp[PHPAL_I14443P4_SW_PCB_POS]) &&
        PHPAL_I14443P4_SW_IS_ACK(pIsoFrameResp[PHPAL_I14443P4_SW_PCB_POS])) {
        PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_IsValidRBlock(
                                                                      pDataParams->bCidEnabled,
                                                                      pDataParams->bCid,
                                                                      pIsoFrameResp,
                                                                      wIsoFrameLen));
    } else {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}


phStatus_t phpalI14443p4_Sw_Exchange(
                                    phpalI14443p4_Sw_DataParams_t * pDataParams,
                                    uint16_t wOption,
                                    uint8_t * pTxBuffer,
                                    uint16_t wTxLength,
                                    uint8_t ** ppRxBuffer,
                                    uint16_t * pRxLength
                                    )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bBufferOverflow;

    /* Used to build I/R/S block frames */
    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
    uint16_t    PH_MEMLOC_REM wIsoFrameLen;
    uint8_t     PH_MEMLOC_REM bRxOverlapped[3];
    uint16_t    PH_MEMLOC_REM wRxOverlappedLen;
    uint16_t    PH_MEMLOC_REM wRxStartPos;
    uint8_t     PH_MEMLOC_REM bUseNad = 0;
    uint8_t     PH_MEMLOC_REM bForceSend;

    /* Used for Transmission */
    uint16_t    PH_MEMLOC_REM wRxBufferSize;
    uint16_t    PH_MEMLOC_REM wTxBufferSize;
    uint16_t    PH_MEMLOC_REM wTxBufferLen;
    uint16_t    PH_MEMLOC_REM wInfLength;
    uint16_t    PH_MEMLOC_REM wMaxPcdFrameSize;
    uint16_t    PH_MEMLOC_REM wMaxCardFrameSize;
    uint16_t    PH_MEMLOC_REM wPcb = 0;
    uint8_t     PH_MEMLOC_REM bRetryCountRetransmit;

    /* Used for Reception */
    uint16_t    PH_MEMLOC_REM RxLength;
    uint8_t *   PH_MEMLOC_REM pRxBuffer;

    /* Option parameter check */
    if (wOption &  (uint16_t)~(uint16_t)
        (
        PH_EXCHANGE_BUFFERED_BIT | PH_EXCHANGE_LEAVE_BUFFER_BIT |
        PH_EXCHANGE_TXCHAINING | PH_EXCHANGE_RXCHAINING | PH_EXCHANGE_RXCHAINING_BUFSIZE
        )) {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
    }

    /* Check if caller has provided valid RxBuffer */
    if (ppRxBuffer == NULL) {
        ppRxBuffer = &pRxBuffer;
    }
    if (pRxLength == NULL) {
        pRxLength = &RxLength;
    }

    /* 获取发送和接收的缓冲区大小 */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_BUFSIZE, &wRxBufferSize));
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER_BUFSIZE, &wTxBufferSize));

    /* Retrieve maximum frame sizes */
    wMaxPcdFrameSize = bI14443p4_FsTable[pDataParams->bFsdi];
    wMaxCardFrameSize = bI14443p4_FsTable[pDataParams->bFsci]-2;

    /* R(ACK) transmission in case of Rx-Chaining */
    if (((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING) ||
        ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE)) {
        pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_RX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
    }
    /* I-Block transmission */
    else {
        /* Reset to default state if not in Tx-Mode */
        if ((pDataParams->bStateNow & PH_EXCHANGE_MODE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_TX) {
            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_TX;
        }
    }

    /* Reset receive length */
    *pRxLength = 0;

    /* Reset RetryCount */
    bRetryCountRetransmit = 0;

    /* Reset BufferOverflow flag */
    bBufferOverflow = 0;

    /* ******************************** */
    /*     I-BLOCK TRANSMISSION LOOP    */
    /* ******************************** */
    do {
        /* Reset Preloaded bytes and ForceSend */
        wTxBufferLen = 0;
        bForceSend = 0;

        switch (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) {
        case PHPAL_I14443P4_SW_STATE_I_BLOCK_TX:

            /* Retrieve Number of preloaded bytes */
            if (wOption & PH_EXCHANGE_LEAVE_BUFFER_BIT) {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(
                                                                 pDataParams->pHalDataParams,
                                                                 PHHAL_HW_CONFIG_TXBUFFER_LENGTH,
                                                                 &wTxBufferLen));
            }

            /* Set initial INF length to (remaining) input data length */
            wInfLength = wTxLength;

            /* Frame has already been preloaded -> IsoFrameLen is zero */
            if (wTxBufferLen > 0) {
                /* do not generate the iso frame */
                wIsoFrameLen = 0;
            }
            /* Else evaluate IsoFrameLen*/
            else {
                /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
                if (!(pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
                    bUseNad = pDataParams->bNadEnabled;
                } else {
                    bUseNad = 0;
                }

                /* Evaluate frame overhead */
                wIsoFrameLen = 1;
                if (bUseNad) {
                    ++wIsoFrameLen;
                }
                if (pDataParams->bCidEnabled) {
                    ++wIsoFrameLen;
                }
            }

            /* Check if chaining is intended or not */
            if ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING) {
                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
            } else {
                pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
            }

            /* Force frame exchange if
            a) the maximum frame size of the card has been reached;
            */
            if ((wTxBufferLen + wIsoFrameLen + wInfLength) > wMaxCardFrameSize) {
                /* force frame exchange */
                bForceSend = 1;

                /* force chaining */
                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;

                /* limit number of bytes to send */
                wInfLength = wMaxCardFrameSize - wTxBufferLen - wIsoFrameLen;
            }

            /* Force frame exchange if
            b) the TxBuffer is full;
            */
            if ((wOption & PH_EXCHANGE_BUFFERED_BIT) &&
                ((wTxBufferLen + wIsoFrameLen + wInfLength) >= wTxBufferSize)) {
                /* force frame exchange */
                bForceSend = 1;

                /* force chaining */
                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
            }

            /* Generate / Buffer ISO frame */
            if (wIsoFrameLen > 0) {
                /* Generate I-Block frame header */
                PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildIBlock(
                                                                            pDataParams->bCidEnabled,
                                                                            pDataParams->bCid,
                                                                            bUseNad,
                                                                            pDataParams->bNad,
                                                                            pDataParams->bPcbBlockNum,
                                                                            (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT) ? 1 : 0,
                                                                            bIsoFrame,
                                                                            &wIsoFrameLen));

                /* Write Frame to HAL TxBuffer but do not preform Exchange */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(
                                                                pDataParams->pHalDataParams,
                                                                PH_EXCHANGE_BUFFER_FIRST,
                                                                bIsoFrame,
                                                                wIsoFrameLen,
                                                                NULL,
                                                                NULL));

                /* Retain the preloaded bytes from now on */
                wOption |= PH_EXCHANGE_LEAVE_BUFFER_BIT;
            }

            /* Tx-Buffering mode (and no forced exchange) */
            if ((!bForceSend) && (wOption & PH_EXCHANGE_BUFFERED_BIT)) {
                /* Preload the data into the TxBuffer */
                return phhalHw_Exchange(
                                       pDataParams->pHalDataParams,
                                       (wOption & (uint16_t)~(uint16_t)PH_EXCHANGE_MODE_MASK),
                                       pTxBuffer,
                                       wInfLength,
                                       NULL,
                                       NULL);
            }

            /* Content has been buffered before */
            if (wTxBufferLen > 0) {
                /* retrieve PCB byte */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_ADDITIONAL_INFO, 0x00));
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, &wPcb));

                /* Preloaded Data or ForceSend -> Modify PCB byte if neccessary */
                if ((bForceSend) || ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING)) {
                    /* modify PCB byte */
                    wPcb |= PHPAL_I14443P4_SW_PCB_CHAINING;
                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, wPcb));
                }
            }
            break;

        case PHPAL_I14443P4_SW_STATE_I_BLOCK_RX:

            /* Build R(ACK) frame */
            PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                                                                        pDataParams->bCidEnabled,
                                                                        pDataParams->bCid,
                                                                        pDataParams->bPcbBlockNum,
                                                                        1,
                                                                        bIsoFrame,
                                                                        &wIsoFrameLen));

            /* Write Frame to HAL TxBuffer but do not preform Exchange */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(
                                                            pDataParams->pHalDataParams,
                                                            PH_EXCHANGE_BUFFERED_BIT,
                                                            bIsoFrame,
                                                            wIsoFrameLen,
                                                            NULL,
                                                            NULL));

            /* Retain the preloaded bytes from now on */
            wOption |= PH_EXCHANGE_LEAVE_BUFFER_BIT;

            /* do not append any data */
            wInfLength = 0;
            break;

            /* Should NEVER happen! */
        default:
            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
        }

        /* Perform Exchange using complete ISO handling */
        status = phpalI14443p4_Sw_IsoHandling(
                                             pDataParams,
                                             wOption & (uint16_t)~(uint16_t)PH_EXCHANGE_BUFFERED_BIT,
                                             bRetryCountRetransmit,
                                             pTxBuffer,
                                             wInfLength,
                                             ppRxBuffer,
                                             pRxLength);

        /* Complete chaining if buffer is full */
        if (((status & PH_ERR_MASK) == PH_ERR_BUFFER_OVERFLOW) &&
            ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE)) {
            /* Indicate Buffer Overflow */
            bBufferOverflow = 1;

            /* Toggle Blocknumber */
            pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
        }
        /* Else bail out on error */
        else {
            PH_CHECK_SUCCESS(status);
        }

        /* Retransmission in progress */
        if (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT) {
            /* Increment Retransmit RetryCount */
            ++bRetryCountRetransmit;

            /* Restore internal TxBuffer. */
            /* Neccessary in case RxBuffer and TxBuffer are the same. */
            if (wTxBufferLen > 0) {
                /* restore PCB byte */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_ADDITIONAL_INFO, 0x00));
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, wPcb));

                /* restore TxBufferLen */
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                                 pDataParams->pHalDataParams,
                                                                 PHHAL_HW_CONFIG_TXBUFFER_LENGTH,
                                                                 wTxBufferLen));
            }

            /* Clear retransmission bit */
            pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT;
        }
        /* No retransmission in progress */
        else {
            /* Reset Retransmit RetryCount */
            bRetryCountRetransmit = 0;

            /* Chaining is active */
            if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
                /* Bytes to send cannot be less than sent bytes */
                if (wTxLength < wInfLength) {
                    return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
                }

                /* Remove sent bytes from TxBuffer */
                pTxBuffer += wInfLength;
                wTxLength = wTxLength - wInfLength;
            }

            /* Buffered / TxChaining mode -> finished after sending */
            if ((wTxLength == 0) &&
                (
                (wOption & PH_EXCHANGE_BUFFERED_BIT) ||
                ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING)
                )) {
                return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
            }
        }
    }
    /* Loop as long as the state does not transit to RX mode */
    while ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_RX);

    /* Overlap PCB */
    wRxOverlappedLen = 1;

    /* Overlap CID */
    if ((*ppRxBuffer)[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING) {
        wRxOverlappedLen++;
    }

    /* Overlap NAD */
    if ((*ppRxBuffer)[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING) {
        wRxOverlappedLen++;
    }

    /* Reset RxStartPos */
    wRxStartPos = 0;

    /* ******************************** */
    /*      I-BLOCK RECEPTION LOOP      */
    /* ******************************** */
    do {
        /* Only allow receive state at this point */
        if ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_RX) {
            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
        }

        /* Rule 2, ISO/IEC 14443-4:2008(E), PICC chaining */
        if (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT) {
            /* Skip overlapping / SUCCESS_CHAINING checks in case of BufferOverflow */
            if (!bBufferOverflow) {
                /* This is first chained response */
                if (wRxStartPos == 0) {
                    /* Special NAD chaining handling */
                    /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
                    if (pDataParams->bNadEnabled) {
                        --wRxOverlappedLen;
                    }
                }

                /* Backup overlapped bytes */
                memcpy(bRxOverlapped, &(*ppRxBuffer)[((*pRxLength) - wRxOverlappedLen)], wRxOverlappedLen);  /* PRQA S 3200 */

                /* Calculate RxBuffer Start Position */
                wRxStartPos = (*pRxLength) - wRxOverlappedLen;

                /* Skip SUCCESS_CHAINING check for RXCHAINING_BUFSIZE mode */
                if ((wOption & PH_EXCHANGE_MODE_MASK) != PH_EXCHANGE_RXCHAINING_BUFSIZE) {
                    /* Return with chaining status if the next chain may not fit into our buffer */
                    if ((*pRxLength + wMaxPcdFrameSize) > wRxBufferSize) {
                        /* Adjust RxBuffer position */
                        (*ppRxBuffer) += wRxOverlappedLen;
                        *pRxLength = *pRxLength - wRxOverlappedLen;

                        return PH_ADD_COMPCODE(PH_ERR_SUCCESS_CHAINING, PH_COMP_PAL_ISO14443P4);
                    }
                }
            }

            /* Set RxBuffer Start Position */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
                                                             pDataParams->pHalDataParams,
                                                             PHHAL_HW_CONFIG_RXBUFFER_STARTPOS,
                                                             wRxStartPos));

            /* Prepare R(ACK) frame */
            PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                                                                        pDataParams->bCidEnabled,
                                                                        pDataParams->bCid,
                                                                        pDataParams->bPcbBlockNum,
                                                                        1,
                                                                        bIsoFrame,
                                                                        &wIsoFrameLen));

            /* Perform Exchange using complete ISO handling */
            status = phpalI14443p4_Sw_IsoHandling(
                                                 pDataParams,
                                                 PH_EXCHANGE_DEFAULT,
                                                 0,
                                                 bIsoFrame,
                                                 wIsoFrameLen,
                                                 ppRxBuffer,
                                                 pRxLength);

            /* Complete chaining if buffer is full */
            if (((status & PH_ERR_MASK) == PH_ERR_BUFFER_OVERFLOW) &&
                ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE)) {
                /* Reset wRxStartPos */
                wRxStartPos = 0;

                /* Indicate Buffer Overflow */
                bBufferOverflow = 1;

                /* Toggle Blocknumber */
                pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
            }
            /* Default behaviour */
            else {
                /* Bail out on error */
                PH_CHECK_SUCCESS(status);

                /* Restore overlapped INF bytes */
                memcpy(&(*ppRxBuffer)[wRxStartPos], bRxOverlapped, wRxOverlappedLen);  /* PRQA S 3200 */ /* PRQA S 3354 */
            }
        }
        /* No chaining -> reception finished */
        else {
            /* Return data */
            if (!bBufferOverflow) {
                /* Special NAD chaining handling */
                /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
                if ((wRxStartPos > 0) && (pDataParams->bNadEnabled)) {
                    ++wRxOverlappedLen;
                }

                /* Do not return protocol bytes, advance to INF field */
                (*ppRxBuffer) += wRxOverlappedLen;
                *pRxLength = *pRxLength - wRxOverlappedLen;

                /* Reception successful */
                status = PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
            }
            /* do not return any data in case of Buffer Overflow */
            else {
                *pRxLength = 0;
                status = PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_PAL_ISO14443P4);
            }

            /* Reception finished */
            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_FINISHED;
        }
    }
    while (pDataParams->bStateNow != PHPAL_I14443P4_SW_STATE_FINISHED);

    return status;
}
phStatus_t phpalI14443p4_Sw_IsoHandling(
                                       phpalI14443p4_Sw_DataParams_t * pDataParams,
                                       uint16_t wOption,
                                       uint8_t bRetryCount,
                                       uint8_t * pTxBuffer,
                                       uint16_t wTxLength,
                                       uint8_t ** ppRxBuffer,
                                       uint16_t * pRxLength
                                       ){
    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
    uint8_t *   PH_MEMLOC_REM pResp;
    uint16_t    PH_MEMLOC_REM wRespLen;
    uint16_t    PH_MEMLOC_REM wRxBufferStartPos;
    uint8_t     PH_MEMLOC_REM bInvalidBlock;
    uint8_t     PH_MEMLOC_REM bResponseReceived;
    uint8_t     PH_MEMLOC_REM bWtxm = 0;
    uint8_t     PH_MEMLOC_REM bCheckNad;
    uint32_t    PH_MEMLOC_REM dwTimeout;
    uint16_t    PH_MEMLOC_REM wTimeoutPrev = 0;
    uint8_t     PH_MEMLOC_REM bTimeoutInMs = 0;

    /* Buffered Exchange is not allowed here */
    if (wOption & PH_EXCHANGE_BUFFERED_BIT) {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* Ignore custom modes */
    wOption &= (uint16_t)~(uint16_t)PH_EXCHANGE_MODE_MASK;

    /* Reset response received flag */
    bResponseReceived = 0;

    /* Loops in case of an error */
    do {
        /* WTX Timeout - set temporary FWT */
        if (bWtxm > 0) {
            /* Retrieve current timeout */
            status = phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, &wTimeoutPrev);

            /* Timeout is out of range, retrieve it in milliseconds */
            if ((status & PH_ERR_MASK) == PH_ERR_PARAMETER_OVERFLOW) {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, &wTimeoutPrev));
                bTimeoutInMs = 1;
            } else {
                PH_CHECK_SUCCESS(status);
                bTimeoutInMs = 0;
            }

            /* Calculate temporary WTX timeout */
            dwTimeout = (uint32_t)((PHPAL_I14443P4_SW_FWT_MIN_US * (1 << pDataParams->bFwi)) * bWtxm);

            /* Add extension time */
            dwTimeout = dwTimeout + PHPAL_I14443P4_EXT_TIME_US;

            /* Limit timeout to FWT max */
            if (dwTimeout > PHPAL_I14443P4_SW_FWT_MAX_US) {
                dwTimeout = PHPAL_I14443P4_SW_FWT_MAX_US;
            }

            /* Set temporary WTX timeout */
            if (dwTimeout > 0xFFFF) {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, (uint16_t)(dwTimeout / 1000)));
            } else {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, (uint16_t)dwTimeout));
            }
        }

        /* Call HAL exchange function */
        status = phhalHw_Exchange(
                                 pDataParams->pHalDataParams,
                                 wOption,
                                 pTxBuffer,
                                 wTxLength,
                                 ppRxBuffer,
                                 pRxLength);

        /* Reset Exchange Option */
        wOption = PH_EXCHANGE_DEFAULT;

        /* Reset transmission length */
        wTxLength = 0;

        /* WTX Timeout - restore previous value */
        if (bWtxm > 0) {
            /* Restore previous state and timeout after */
            if (!bTimeoutInMs) {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, wTimeoutPrev));
            } else {
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, wTimeoutPrev));
            }
        }

        /* Status --> InvalidBlock mapping */
        bInvalidBlock = (uint8_t)PHPAL_I14443P4_SW_IS_INVALID_BLOCK_STATUS(status);
        if (!bInvalidBlock) {
            /* MIFARE compliancy: force protocol error on NAK */
            if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS_INCOMPLETE_BYTE) {
                *pRxLength = 0;
                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, 0));
                return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
            } else {
                PH_CHECK_SUCCESS(status);
            }
        }

        /* Reset defaults */
        bWtxm = 0;

        /* Do not enter if we received an invalid block */
        if (!bInvalidBlock) {
            /* Signal that we've received something */
            bResponseReceived = 1;

            /* Map receive buffer pointer to current data */
            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, &wRxBufferStartPos));
            pResp = *ppRxBuffer + wRxBufferStartPos;
            wRespLen = *pRxLength - wRxBufferStartPos;

            /* I-Block handling */
            if (PHPAL_I14443P4_SW_IS_I_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
                /* Rule 2/10, ISO/IEC 14443-4:2008(E) */
                if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
                    /* Protocol violation */
                    bInvalidBlock = 1;
                } else {
                    /* Evaluate if NAD should be present */
                    /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
                    if ((pDataParams->bNadEnabled) && !(pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
                        bCheckNad = 1;
                    } else {
                        bCheckNad = 0;
                    }

                    /* Check if I-Block is valid */
                    status = phpalI14443p4_Sw_IsValidIBlock(
                                                           pDataParams->bCidEnabled,
                                                           pDataParams->bCid,
                                                           bCheckNad,
                                                           pDataParams->bNad,
                                                           pResp,
                                                           wRespLen);

                    /* Blocknumber is equal, I-Block Rx mode is active */
                    if (((status & PH_ERR_MASK) == PH_ERR_SUCCESS) && (PHPAL_I14443P4_SW_IS_BLOCKNR_EQUAL(pResp[PHPAL_I14443P4_SW_PCB_POS]))) {
                        /* Switch from Tx-Mode to Rx-Mode */
                        if ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) == PHPAL_I14443P4_SW_STATE_I_BLOCK_TX) {
                            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_RX;
                        }

                        /* Rule B, ISO/IEC 14443-4:2008(E), toggle Blocknumber */
                        pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;

                        /* Set chaining bit if card is chaining */
                        if (PHPAL_I14443P4_SW_IS_CHAINING(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
                            pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
                        }
                        /* Clear chaining bit otherwise */
                        else {
                            pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
                        }
                    }
                    /* We received an invalid block */
                    else {
                        /* Protocol violation */
                        bInvalidBlock = 1;
                    }
                }
            }
            /* R(ACK) handling */
            else if (PHPAL_I14443P4_SW_IS_R_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS]) && PHPAL_I14443P4_SW_IS_ACK(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
                /* Check if R-Block is valid */
                status = phpalI14443p4_Sw_IsValidRBlock(
                                                       pDataParams->bCidEnabled,
                                                       pDataParams->bCid,
                                                       pResp,
                                                       wRespLen);

                /* R-Block invalid or not in transmission mode */
                if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS) {
                    /* Protocol violation */
                    bInvalidBlock = 1;
                } else {
                    /* Blocknumber is equal */
                    if (PHPAL_I14443P4_SW_IS_BLOCKNR_EQUAL(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
                        /* Continue TX chaining */
                        if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
                            /* Rule B, ISO/IEC 14443-4:2008(E), toggle Blocknumber */
                            pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
                        }
                        /* Not in chaining mode */
                        else {
                            /* Protocol violation */
                            bInvalidBlock = 1;
                        }
                    }
                    /* Rule 6, ISO/IEC 14443-4:2008(E), unequal block number */
                    else {
                        /* Limit this behaviour with bMaxRetryCount, bRetry count is set by the caller in this case */
                        /* Note: pDataParams->bMaxRetryCount = 0 behaviour does NOT suppress this behaviour since it is mandatory. */
                        if (bRetryCount >= pDataParams->bMaxRetryCount) {
                            /* Protocol violation */
                            bInvalidBlock = 1;
                        }
                        /* Send last I-Block again */
                        else {
                            pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT;
                        }
                    }
                }
            }
            /* S(WTX) handling */
            else if (PHPAL_I14443P4_SW_IS_S_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS]) && PHPAL_I14443P4_SW_IS_WTX(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
                /* Check if S-Block is valid */
                status = phpalI14443p4_Sw_IsValidSBlock(
                                                       pDataParams->bCidEnabled,
                                                       pDataParams->bCid,
                                                       pResp,
                                                       wRespLen);

                /* Rule 3, ISO/IEC 14443-4:2008(E), S(WTX) handling */
                if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS) {
                    /* Retrieve WTXM */
                    bWtxm = pResp[wRespLen-1];

                    /* Ignore and clear the Power Level Indication */
                    bWtxm &= 0x3F;

                    /* Treat invalid WTXM value as protocol error, do not perform error correction. */
                    if (bWtxm == 0 || bWtxm > 59) {
                        status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
                        break;
                    }

                    /* Generate S(WTX) frame */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildSBlock(
                                                                                pDataParams->bCidEnabled,
                                                                                pDataParams->bCid,
                                                                                1,
                                                                                bWtxm,
                                                                                bIsoFrame,
                                                                                &wTxLength));
                }
                /* We received an invalid block */
                else {
                    /* Protocol violation */
                    bInvalidBlock = 1;
                }
            }
            /* We received an invalid block */
            else {
                /* Protocol violation */
                bInvalidBlock = 1;
            }

            /* Protocol violation */
            if (bInvalidBlock) {
                /* bMaxRetryCount = 0 suppresses the S(DESELECT) behaviour */
                if (pDataParams->bMaxRetryCount > 0) {
                    /* send S(DESELECT) (ignore return code) */
                    statusTmp = phpalI14443p4_Sw_Deselect(pDataParams);
                }

                /* bail out with protocol error */
                status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
                break;
            }
            /* Reset retry counter on no error */
            else {
                bRetryCount = 0;
            }
        }
        /* Invalid Block received */
        else {
            /* Increment retry count */
            ++bRetryCount;

            /* Only perform error correction if the max. retry count is not reached */
            if (bRetryCount <= pDataParams->bMaxRetryCount) {
                /* Rule 5, ISO/IEC 14443-4:2008(E) */
                if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_RX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
                    /* Generate R(ACK) frame */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                                                                                pDataParams->bCidEnabled,
                                                                                pDataParams->bCid,
                                                                                pDataParams->bPcbBlockNum,
                                                                                1,
                                                                                bIsoFrame,
                                                                                &wTxLength));
                }
                /* Rule 4, ISO/IEC 14443-4:2008(E) */
                else {
                    /* Generate R(NAK) frame */
                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
                                                                                pDataParams->bCidEnabled,
                                                                                pDataParams->bCid,
                                                                                pDataParams->bPcbBlockNum,
                                                                                0,
                                                                                bIsoFrame,
                                                                                &wTxLength));
                }
            }
            /* Bail out if the max. retry count is reached */
            else {
                /* Deselect card if behaviour is enabled */
                if (pDataParams->bMaxRetryCount > 0) {
                    /* backup retry count */
                    bRetryCount = pDataParams->bMaxRetryCount;

                    /* set retry count to zero to send only one S(DESELECT) */
                    pDataParams->bMaxRetryCount = 0;

                    /* send deselect (ignore return code) */
                    statusTmp = phpalI14443p4_Sw_Deselect(pDataParams);

                    /* restore retry count setting */
                    pDataParams->bMaxRetryCount = bRetryCount;

                    /* Return ERR_RECOVERY_FAILED if some response has been received before */
                    if (bResponseReceived) {
                        status = PH_ADD_COMPCODE(PHPAL_I14443P4_ERR_RECOVERY_FAILED, PH_COMP_PAL_ISO14443P4);
                    }
                }
                break;
            }
        }

        /* Map TxBuffer to ISO Frame buffer for next loop */
        pTxBuffer = bIsoFrame;
    }
    /* Loop as long as we generate transmissions */
    while (wTxLength);

    /* Reset RxBuffer Start Position */
    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, 0));

    return status;
}

/**************************************************************
*函数： 
*说明： 
*   检测I是否合法 
*   1、检测保留位 是否合法
*   2、判断接收的I块长度是否合法 
*   3、检测CID盒nad是否和发送的一样； 
***************************************************************/
phStatus_t phpalI14443p4_Sw_IsValidIBlock(
                                         uint8_t bCheckCid,
                                         uint8_t bCid,
                                         uint8_t bCheckNad,
                                         uint8_t bNad,
                                         uint8_t * pRxBuffer,
                                         uint16_t wRxLength
                                         )
{
    uint16_t PH_MEMLOC_REM wExpRxLength;

    /* Check RFU bits */
    if ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_I_BLOCK_RFU_MASK) != PHPAL_I14443P4_SW_I_BLOCK_RFU_BITS) {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* We always expect the PCB byte to be present */
    wExpRxLength = 1;

    /* Enable CID checking if neccessary */
    if (bCheckCid) {
        wExpRxLength++;
    }

    /* Enable NAD checking if neccessary */
    if (bCheckNad) {
        wExpRxLength++;
    }

    /* The frame should have the minimum frame length */
    if (wRxLength < wExpRxLength) {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* CID presence check */
    if ((bCheckCid) &&
        (pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING) &&
        ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS+1] & PHPAL_I14443P4_SW_CID_MASK) == bCid)) {
        /* CHECK SUCCEEDED */
    }
    /* CID absence check */
    else if (!(bCheckCid) && !(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING)) {
        /* CHECK SUCCEEDED */
    }
    /* CID protocol error */
    else {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* NAD presence check */
    if ((bCheckNad) &&
        (pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING) &&
        (pRxBuffer[wExpRxLength-1] == bNad)) {
        /* CHECK SUCCEEDED */
    }
    /* NAD absence check */
    else if (!(bCheckNad) && !(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING)) {
        /* CHECK SUCCEEDED */
    }
    /* NAD protocol error */
    else {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}


/**************************************************************
*函数： 
*说明： 
*   检测R是否合法 
*   1、检测保留位 是否合法
*   2、判断长度是否合法
*   3、检测CID是否合法
***************************************************************/


phStatus_t phpalI14443p4_Sw_IsValidRBlock(
                                         uint8_t bCheckCid,
                                         uint8_t bCid,
                                         uint8_t * pRxBuffer,
                                         uint16_t wRxLength
                                         )
{
    uint16_t PH_MEMLOC_REM wExpRxLength;

    /* Check RFU bits */
    if ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_R_BLOCK_RFU_MASK) != PHPAL_I14443P4_SW_R_BLOCK_RFU_BITS) {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* We always expect the PCB byte to be present */
    wExpRxLength = 1;

    /* If CID is enabled, we always expect it */
    if (bCheckCid) {
        wExpRxLength++;
    }

    /* The frame should have the exact frame length */
    if (wRxLength != wExpRxLength) {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* CID presence check */
    if ((bCheckCid) &&
        (pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING) &&
        ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS+1] & PHPAL_I14443P4_SW_CID_MASK) == bCid)) {
        /* CHECK SUCCEEDED */
    }
    /* CID absence check */
    else if (!(bCheckCid) && !(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING)) {
        /* CHECK SUCCEEDED */
    }
    /* CID protocol error */
    else {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}
/**************************************************************
*函数： 
*说明： 
*     检测R是否合法 
*   1、检测保留位 是否合法
*   2、判断长度是否合法
*   3、检测CID是否合法
***************************************************************/
phStatus_t phpalI14443p4_Sw_IsValidSBlock(
                                         uint8_t bCheckCid,
                                         uint8_t bCid,
                                         uint8_t * pRxBuffer,
                                         uint16_t wRxLength
                                         )
{
    uint16_t PH_MEMLOC_REM wExpRxLength;

    /* Check RFU bits */
    if ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_S_BLOCK_RFU_MASK) != PHPAL_I14443P4_SW_S_BLOCK_RFU_BITS) {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* We always expect the PCB byte to be present */
    wExpRxLength = 1;

    /* If CID is enabled, we always expect it */
    if (bCheckCid) {
        wExpRxLength++;
    }

    /* If this is a WTX request, we expect an additional INF byte */
    if (PHPAL_I14443P4_SW_IS_WTX(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS])) {
        wExpRxLength++;
    }

    /* The frame should have the exact frame length */
    if (wRxLength != wExpRxLength) {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    /* CID presence check */
    if ((bCheckCid) &&
        (pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING) &&
        ((pRxBuffer[PHPAL_I14443P4_SW_PCB_POS+1] & PHPAL_I14443P4_SW_CID_MASK) == bCid)) {
        /* CHECK SUCCEEDED */
    }
    /* CID absence check */
    else if (!(bCheckCid) && !(pRxBuffer[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING)) {
        /* CHECK SUCCEEDED */
    }
    /* CID protocol error */
    else {
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_BuildIBlock(
                                       uint8_t bCidEnabled,
                                       uint8_t bCid,
                                       uint8_t bNadEnabled,
                                       uint8_t bNad,
                                       uint8_t bPcbBlockNum,
                                       uint8_t bChaining,
                                       uint8_t * pTxBuffer,
                                       uint16_t * pTxLength
                                       )
{
    /* I-Block PCB */
    pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] = PHPAL_I14443P4_SW_I_BLOCK | PHPAL_I14443P4_SW_I_BLOCK_RFU_BITS | bPcbBlockNum;
    *pTxLength = 1;

    /* Check if chaining is neccessary */
    if (bChaining) {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_CHAINING;
    }

    /* Append CID if neccessary */
    if (bCidEnabled) {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_CID_FOLLOWING;
        pTxBuffer[(*pTxLength)++] = bCid;
    }

    /* Append NAD if neccessary */
    if (bNadEnabled) {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING;
        pTxBuffer[(*pTxLength)++] = bNad;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_BuildRBlock(
                                       uint8_t bCidEnabled,
                                       uint8_t bCid,
                                       uint8_t bPcbBlockNum,
                                       uint8_t bIsAck,
                                       uint8_t * pTxBuffer,
                                       uint16_t * pTxLength
                                       )
{
    /* R(ACK) PCB */
    pTxBuffer[PHPAL_I14443P4_SW_PCB_POS]  = PHPAL_I14443P4_SW_R_BLOCK | PHPAL_I14443P4_SW_R_BLOCK_RFU_BITS | bPcbBlockNum;
    *pTxLength = 1;

    /* Switch to R(NAK) if neccessary */
    if (!bIsAck) {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_NAK;
    }

    /* Append CID if supported */
    if (bCidEnabled != 0) {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_CID_FOLLOWING;
        pTxBuffer[(*pTxLength)++] = bCid;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_BuildSBlock(
                                       uint8_t bCidEnabled,
                                       uint8_t bCid,
                                       uint8_t bIsWtx,
                                       uint8_t bWtxm,
                                       uint8_t * pTxBuffer,
                                       uint16_t * pTxLength
                                       )
{
    /* S-Block PCB */
    pTxBuffer[PHPAL_I14443P4_SW_PCB_POS]  = PHPAL_I14443P4_SW_S_BLOCK | PHPAL_I14443P4_SW_S_BLOCK_RFU_BITS;
    *pTxLength = 1;

    /* Append CID if supported */
    if (bCidEnabled != 0) {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_CID_FOLLOWING;
        pTxBuffer[(*pTxLength)++] = bCid;
    }

    /* Set WTX bits and append WTXM if neccessary */
    if (bIsWtx) {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_WTX;
        pTxBuffer[(*pTxLength)++] = bWtxm;
    }
    /* Set DESELECT bits if neccessary */
    else {
        pTxBuffer[PHPAL_I14443P4_SW_PCB_POS] |= PHPAL_I14443P4_SW_PCB_DESELECT;
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_SetConfig(
                                     phpalI14443p4_Sw_DataParams_t * pDataParams,
                                     uint16_t wConfig,
                                     uint16_t wValue
                                     )
{
    switch (wConfig) {
    case PHPAL_I14443P4_CONFIG_BLOCKNO:
        {
            if (wValue == 0) {
                pDataParams->bPcbBlockNum = 0;
            } else {
                pDataParams->bPcbBlockNum = 1;
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_CID:
        {
            if (wValue & 0xFF00) {
                if ((wValue & 0x00FF) > PHPAL_I14443P4_CID_MAX) {
                    return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
                }
                pDataParams->bCidEnabled = 1;
                pDataParams->bCid = (uint8_t)(wValue);
            } else {
                pDataParams->bCidEnabled = 0;
                pDataParams->bCid = 0;
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_NAD:
        {
            if (wValue & 0xFF00) {
                pDataParams->bNadEnabled = 1;
                pDataParams->bNad = (uint8_t)(wValue);
            } else {
                pDataParams->bNadEnabled = 0;
                pDataParams->bNad = 0;
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_FWI:
        {
            if (wValue > PHPAL_I14443P4_FWI_MAX) {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
            }
            pDataParams->bFwi = (uint8_t)(wValue);
            break;
        }
    case PHPAL_I14443P4_CONFIG_FSI:
        {
            if (((wValue >> 8) > PHPAL_I14443P4_FRAMESIZE_MAX) ||
                ((wValue & 0xFF) > PHPAL_I14443P4_FRAMESIZE_MAX)) {
                return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
            }

            pDataParams->bFsdi = (uint8_t)(wValue >> 8);
            pDataParams->bFsci = (uint8_t)(wValue);
            break;
        }
    case PHPAL_I14443P4_CONFIG_MAXRETRYCOUNT:
        {
            pDataParams->bMaxRetryCount = (uint8_t)(wValue);
            break;
        }
    default:
        {
            return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_PAL_ISO14443P4);
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}

phStatus_t phpalI14443p4_Sw_GetConfig(
                                     phpalI14443p4_Sw_DataParams_t * pDataParams,
                                     uint16_t wConfig,
                                     uint16_t * pValue
                                     )
{
    switch (wConfig) {
    case PHPAL_I14443P4_CONFIG_BLOCKNO:
        {
            if (pDataParams->bPcbBlockNum == 0) {
                *pValue = 0;
            } else {
                *pValue = 1;
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_CID:
        {
            if (pDataParams->bCidEnabled) {
                *pValue = (uint16_t)(0x0100 | pDataParams->bCid);
            } else {
                *pValue = (uint16_t)(pDataParams->bCid);
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_NAD:
        {
            if (pDataParams->bNadEnabled) {
                *pValue = (uint16_t)(0x0100 | pDataParams->bNad);
            } else {
                *pValue = (uint16_t)(pDataParams->bNad);
            }

            break;
        }
    case PHPAL_I14443P4_CONFIG_FWI:
        {
            *pValue = (uint16_t)pDataParams->bFwi;
            break;
        }
    case PHPAL_I14443P4_CONFIG_FSI:
        {
            *pValue  = (uint16_t)((uint16_t)(pDataParams->bFsdi) << 8);
            *pValue |= (uint16_t)(pDataParams->bFsci);
            break;
        }
    case PHPAL_I14443P4_CONFIG_MAXRETRYCOUNT:
        {
            *pValue = (uint16_t)pDataParams->bMaxRetryCount;
            break;
        }
    default:
        {
            return PH_ADD_COMPCODE(PH_ERR_UNSUPPORTED_PARAMETER, PH_COMP_PAL_ISO14443P4);
        }
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
}



/**************************************************************
* 函数：一个I块数据交互
*                                                    
* 13.3.4.3 如果收到的R（ACK）块的块编号与PCD 当前的块编号不相等，并且这个R（ACK）块
*          是PCD 发送的通知PICC 超时的R（NAK）块的响应，则PCD 应重发最后的I 块
*          在任何其它情况下，当收到R（ACK）块的块编号与PCD当前的块编号不相等时，则PCD可直接
*          向终端报告一个协议错误，或者重发最后的I块
* 13.3.4.4 如果PCD 已经重发了一个I 块两次（即相同的I 块发送了3 次）,并且接收到R（ACK）
*          块的编号与PCD 的当前块编号不相等时,应认为发生协议错误
* 13.3.4.5 如果收到R（ACK）块的块编号与PCD 当前的块编号相等，并且PCD 最后所发的I 块指
*          示链接，则链接应继续；如果PCD 最后所发的I 块未指示链接，则PCD 应视收到R（ACK）块为协
*        议错误
* 13.3.4.6 如果PCD 接收到R（NAK）块，则应视为协议错误
* 
* 注意：如果bRetryCount次数到了，返回协议错误
* 
***************************************************************/
//phStatus_t phpalI14443p4_Sw_EMVHandling(
//                                       phpalI14443p4_Sw_DataParams_t * pDataParams,
//                                       uint16_t wOption,
//                                       uint8_t bIblockRetryCount,
//                                       uint8_t * pTxBuffer,
//                                       uint16_t wTxLength,
//                                       uint8_t ** ppRxBuffer,
//                                       uint16_t * pRxLength
//                                       )
//{
//    uint16_t  RxLength = 0;
//    phStatus_t  PH_MEMLOC_REM status = PH_ERR_SUCCESS;
//    phStatus_t  PH_MEMLOC_REM statusTmp;
//    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
//    uint8_t *   PH_MEMLOC_REM pResp;
//    uint16_t    PH_MEMLOC_REM wRespLen;
//    uint16_t    PH_MEMLOC_REM wRxBufferStartPos;
//    uint8_t     PH_MEMLOC_REM bInvalidBlock = 0;//1--错误可以重发；2--协议错误，不可以容错
//    uint8_t     PH_MEMLOC_REM bResponseReceived;
//    uint8_t     PH_MEMLOC_REM bWtxm = 0;
//    uint8_t     PH_MEMLOC_REM bCheckNad;
//    uint32_t    PH_MEMLOC_REM dwTimeout;
//    uint16_t    PH_MEMLOC_REM wTimeoutPrev = 0;
//    uint8_t     PH_MEMLOC_REM bTimeoutInMs = 0;
//    unsigned char bRetryCount = 0;
//
//    uint16_t wMaxPcdFrameSize;
//
//
//
//
//    uint8_t   bBlockType = 0;//0--I块1--ACK块；2--ACK块和NACK；3--WTX块//定义上次发送块的类型
//    wMaxPcdFrameSize = bI14443p4_FsTable[pDataParams->bFsdi];//PCB接收帧的最大长度
//    /* Buffered Exchange is not allowed here */
//    if (wOption & PH_EXCHANGE_BUFFERED_BIT) {
//        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
//    }
//
//    /* Ignore custom modes */
//    wOption &= (uint16_t)~(uint16_t)PH_EXCHANGE_MODE_MASK;
//
//    /* Reset response received flag */
//    bResponseReceived = 0;
//
//    bIblockRetryCount++;
//
//    if (bIblockRetryCount> 3) {
//        bInvalidBlock = 1;
//        printf("I块重复发送超过了3次\n\r");
//        status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
//        return status;
//    }
//
//    /* Loops in case of an error */
//    do {
//        /* WTX Timeout - set temporary FWT */
//        if (bWtxm > 0) {
//            /* Retrieve current timeout */
//            status = phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, &wTimeoutPrev);
//
//            /* Timeout is out of range, retrieve it in milliseconds */
//            if ((status & PH_ERR_MASK) == PH_ERR_PARAMETER_OVERFLOW) {
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, &wTimeoutPrev));
//                bTimeoutInMs = 1;
//            } else {
//                PH_CHECK_SUCCESS(status);
//                bTimeoutInMs = 0;
//            }
//            dwTimeout = (uint32_t)((330.4f * (1 << pDataParams->bFwi)) * bWtxm);
//            // printf("dwTimeout = %d;wTimeoutPrev = %d;\n\r",dwTimeout,wTimeoutPrev);
//
//            /* 设置 WTX 时间 */
//            if (dwTimeout > 0xFFFF) {
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, (uint16_t)(dwTimeout / 1000)+1));
//            } else {
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, (uint16_t)dwTimeout+200));
//            }
//        }
//
//        PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, &RxLength));
//
//        status = phhalHw_Exchange(pDataParams->pHalDataParams, wOption, pTxBuffer,
//                                  wTxLength, ppRxBuffer,pRxLength);
//        wOption = PH_EXCHANGE_DEFAULT;//清除所有的选项，这样没错发送ACK,NACK，WTX，都是从缓冲区 0地址开始的，
//                                      //因为在phhalHw_Exchange,如果缓冲区没有数据，就会把长度清除
//        wTxLength = 0;
//
//        /*   恢复帧的等待时间    */
//        if (bWtxm > 0) {
//            if (!bTimeoutInMs) {
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, wTimeoutPrev));
//            } else {
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_MS, wTimeoutPrev));
//            }
//        }
//
////      printf("P4_Handling status = %02x;总长度 = %d；之前保留长度：%d\n\r;",(status & PH_ERR_MASK),pRxLength[0],RxLength);
////      Print_LogHex("P4_Handling data:",*ppRxBuffer,pRxLength[0]);
//
//
//
//        bWtxm = 0;
//        /***************** 收到了一个数据块**************** */
//        if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS) {
//
//            if ((pRxLength[0]- RxLength)>(wMaxPcdFrameSize-2)) {
//                printf("接收的PICC字节超长 P4_Handling len = %d\n\r",(pRxLength[0]- RxLength));
//                status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
//                break;
//            }
//            //if (!bInvalidBlock) {
//            /* Signal that we've received something */
//            bResponseReceived = 1;
//
//            /* Map receive buffer pointer to current data */
//            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, &wRxBufferStartPos));
//            pResp = *ppRxBuffer + wRxBufferStartPos;
//            wRespLen = *pRxLength - wRxBufferStartPos;
//
//
//
//            /******************I-Block 处理**********************/
//            if (PHPAL_I14443P4_SW_IS_I_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
//                /* Rule 2/10, ISO/IEC 14443-4:2008(E) */
//                //如果是链接模式
//                if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
//                    printf("发送的是链接模式，而接收到了I块\n\r");
//                    /* Protocol violation */
//                    bInvalidBlock = 1;
//                } else {//非链接模式
//                    /* Evaluate if NAD should be present */
//                    /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
//                    if ((pDataParams->bNadEnabled) && !(pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
//                        bCheckNad = 1;
//                    } else {
//                        bCheckNad = 0;
//                    }
//                    /* Check if I-Block is valid */
//                    status = phpalI14443p4_Sw_IsValidIBlock(pDataParams->bCidEnabled,
//                                                            pDataParams->bCid,bCheckNad,
//                                                            pDataParams->bNad,pResp, wRespLen);
//                    /*非链接模式，如果块号相同，则结束*/
//                    if (((status & PH_ERR_MASK) == PH_ERR_SUCCESS) && (PHPAL_I14443P4_SW_IS_BLOCKNR_EQUAL(pResp[PHPAL_I14443P4_SW_PCB_POS]))) {
//                        /* Switch from Tx-Mode to Rx-Mode */
//                        if ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) == PHPAL_I14443P4_SW_STATE_I_BLOCK_TX) {
//                            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_RX;//切换到接收模式
//                        }
//                        pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
//
//                        /* Set chaining bit if card is chaining */
//                        if (PHPAL_I14443P4_SW_IS_CHAINING(pResp[PHPAL_I14443P4_SW_PCB_POS])) {//如果收的是是I块链接，设置链接
//                            pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
//                        }
//                        /* Clear chaining bit otherwise */
//                        else {
//                            pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
//                        }
//                    }
//                    /* We received an invalid block */
//                    else {
//                        printf("非链接模式，发送和接收的块号不同\n\r");
//                        /* Protocol violation */
//                        bInvalidBlock = 1;
//                    }
//                }
//            }
//            /*收到正确的R(ACK),如果收到了NACK，那就是协议出错*/
//            else if (PHPAL_I14443P4_SW_IS_R_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS]) && PHPAL_I14443P4_SW_IS_ACK(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
//                /**************************************************************
//                *ISO规则6、当接收到了R块，如果块号不等当前的块号，就重新发送
//                *ISO规则7、当接收到了R块，如果块号等于当前的块号，继续链接
//                ***************************************************************/
//                /* Check if R-Block is valid */
//                status = phpalI14443p4_Sw_IsValidRBlock(pDataParams->bCidEnabled,pDataParams->bCid,
//                                                        pResp,wRespLen);
//                /* R-Block invalid or not in transmission mode */
//                if ((status & PH_ERR_MASK) != PH_ERR_SUCCESS) {
//                    printf("接收到的R(ACK)数据无效\n\r");
//                    /*协议错误 */
//                    bInvalidBlock = 1;
//                } else {
//                    /**************块号相等**************/
//                    if (PHPAL_I14443P4_SW_IS_BLOCKNR_EQUAL(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
//                        //如果是链接模式，收到了相等的ACK块号
//                        if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT))
//                            pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
//                        /* Not in chaining mode */
//                        else {
//                            /* 协议错误 */
//                            printf("接收的ACK和发送的块好相同：非连接模式收到了ACK\n\r");
//                            bInvalidBlock = 1;
//                        }
//                    }
//                    /* Rule 6, ISO/IEC 14443-4:2008(E), unequal block number */
//                    //块号不相同
//                    else {
//                        //重试的次数大于最大的次数，那么就是协议错误
//                        if (bRetryCount >= pDataParams->bMaxRetryCount) {
//                            /* 协议错误 */
//                            bInvalidBlock = 1;
//                            printf("重试的次数大于最大的次数，那么就是协议错误 bRetryCount = %02x pDataParams->bMaxRetryCount\n\r",
//                                   bRetryCount,pDataParams->bMaxRetryCount);
//                        }
//                        /* Send last I-Block again */
//                        else {
//                            /*----------------------EMV -----------------------
//                            * 如果收到的R（ACK）块的块编号与PCD 当前的块编号不相等，并且这个R（ACK）块
//                            *是PCD 发送的通知PICC 超时的R（NAK）块的响应，则PCD 应重发最后的I 块  */
//                            if ( bBlockType==2) {
//                                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT;
//
//                            } else {
//                                printf("接收的ACK和发送的块好不同：而且发送的不是NACK-> 返回的\n\r");
//                                bInvalidBlock = 1;
//                            }
//                            /*--------------------ISO----------------------------*/
//                            //pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT;
//                        }
//                    }
//                }
//            }
//            /* S(WTX) handling */
//            else if (PHPAL_I14443P4_SW_IS_S_BLOCK(pResp[PHPAL_I14443P4_SW_PCB_POS]) && PHPAL_I14443P4_SW_IS_WTX(pResp[PHPAL_I14443P4_SW_PCB_POS])) {
//                /* Check if S-Block is valid */
//                status = phpalI14443p4_Sw_IsValidSBlock(pDataParams->bCidEnabled,
//                                                        pDataParams->bCid,pResp,wRespLen);
//                /* Rule 3, ISO/IEC 14443-4:2008(E), S(WTX) handling */
//                if ((status & PH_ERR_MASK) == PH_ERR_SUCCESS) {
//                    /* Retrieve WTXM */
//                    bWtxm = pResp[wRespLen-1];
//                    bWtxm &= 0x3F;
//                    //以下是EMV标准
//                    if (bWtxm>59) bWtxm = 59;
//                    if (!bWtxm) {
//                        /* 协议错误 */
//                        bInvalidBlock = 1;
//                        /* bail out with protocol error */
//                        printf("WTX 协议错误WTX = %02x\n\r",bWtxm);
//                        // PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
//                        //  break;
//                    }
//                    ////////////////////////////////////////////////
//                    /* Generate S(WTX) frame */
////                  PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildSBlock(pDataParams->bCidEnabled, pDataParams->bCid,
////                                                                               1,bWtxm,bIsoFrame, &wTxLength));
//                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildSBlock(pDataParams->bCidEnabled, pDataParams->bCid,
//                                                                                 1,pResp[wRespLen-1],bIsoFrame, &wTxLength));//TA204
//                    //PBOC 3.0 升级
//                    phhalHw_Wait(pDataParams->pHalDataParams,PHHAL_HW_TIME_MICROSECONDS,600);//保护时间
//
//                    bBlockType = 3;
//                }
//                /* We received an invalid block */
//                else {
//                    printf("WTX 协议错误WTX = %02x\n\r",bWtxm);
//
//                    /* Protocol violation */
//                    bInvalidBlock = 1;
//                }
//            }
//            /* We received an invalid block */
//            else {
//                if (DebugPinErr()) {
//                    printf(" S(WTX) handling  无效的 \n\r");
//                }
//                /* Protocol violation */
//                bInvalidBlock = 1;
//                PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
//            }
//
//            /* 协议错误，是不能重试的 */
//            if (bInvalidBlock) {
//                /* bail out with protocol error */
//                status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
//                break;
//            }
//            /* Reset retry counter on no error */
//            else {
//                //; bRetryCount = 0; 接收到了SWTX是不能清除NACK标识的TA421
//            }
//            /****************************数据超时或者CRC  PARITY错误************************/
//        } else if (((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT)||((status & PH_ERR_MASK) == PH_ERR_INTEGRITY_ERROR)) {
//
//            ++bRetryCount;
//            if (DebugPinRetry()) {
//                printf("超时或者其他的错误,发送NACK：%02x,status = %02x\n\r",bRetryCount,(status & PH_ERR_MASK));
//            }
//            if (bRetryCount <= pDataParams->bMaxRetryCount) {// 超时重试一试
//                /* 如果当前处于I块接收状态 */
//                if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_RX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
//                    /* Generate R(ACK) frame */
//                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(pDataParams->bCidEnabled,
//                                                                                 pDataParams->bCid, pDataParams->bPcbBlockNum,
//                                                                                 1,bIsoFrame, &wTxLength));
//                    bBlockType = 1;
//                }
//                /* Rule 4, ISO/IEC 14443-4:2008(E) */
//                else {
//                    /* Generate R(NAK) frame */
//                    PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock( pDataParams->bCidEnabled,pDataParams->bCid,
//                                                                                  pDataParams->bPcbBlockNum, 0,bIsoFrame,&wTxLength));
//                    bBlockType = 2;
//                }
//            } else {
//                printf("重试的次数超过最大数值\n\r");
//                bInvalidBlock = 1;
//                status = PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_PAL_ISO14443P4);
//                break;
//            }
//        }
//        /* Map TxBuffer to ISO Frame buffer for next loop */
//        pTxBuffer = bIsoFrame;
//    }
//    /* Loop as long as we generate transmissions */
//    while (wTxLength);
//
//    /* Reset RxBuffer Start Position */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_STARTPOS, 0));
//
//    return status;
//}
//PH_EXCHANGE_DEFAULT
//phStatus_t phpalI14443p4EMV_Sw_Exchange(
//                                       phpalI14443p4_Sw_DataParams_t * pDataParams,
//                                       uint16_t wOption,//PH_EXCHANGE_DEFAULT
//                                       uint8_t * pTxBuffer,
//                                       uint16_t wTxLength,
//                                       uint8_t ** ppRxBuffer,
//                                       uint16_t * pRxLength
//                                       ){
//
//    phStatus_t  PH_MEMLOC_REM status;
//    phStatus_t  PH_MEMLOC_REM statusTmp;
//    uint8_t     PH_MEMLOC_REM bBufferOverflow;
//
//    /* Used to build I/R/S block frames */
//    uint8_t     PH_MEMLOC_REM bIsoFrame[3];
//    uint16_t    PH_MEMLOC_REM wIsoFrameLen;//信息头域的长度，对到3个字节，I块标志、NAD 、CID
//    uint8_t     PH_MEMLOC_REM bRxOverlapped[3];
//    uint16_t    PH_MEMLOC_REM wRxOverlappedLen;
//    uint16_t    PH_MEMLOC_REM wRxStartPos;
//    uint8_t     PH_MEMLOC_REM bUseNad = 0;
//    uint8_t     PH_MEMLOC_REM bForceSend;
//
//    /* Used for Transmission */
//    uint16_t    PH_MEMLOC_REM wRxBufferSize;//接收缓冲区的大小
//    uint16_t    PH_MEMLOC_REM wTxBufferSize;//发送缓冲区的大小
//    uint16_t    PH_MEMLOC_REM wTxBufferLen; //发送缓冲区的有效字节数
//    uint16_t    PH_MEMLOC_REM wInfLength;   //信息域的数据长度
//    uint16_t    PH_MEMLOC_REM wMaxPcdFrameSize;
//    uint16_t    PH_MEMLOC_REM wMaxCardFrameSize;
//    uint16_t    PH_MEMLOC_REM wPcb = 0;
//    uint8_t     PH_MEMLOC_REM bRetryCountRetransmit;
//
//    /* Used for Reception */
//    uint16_t    PH_MEMLOC_REM RxLength;
//    uint8_t *   PH_MEMLOC_REM pRxBuffer;
//
//    /* Option parameter check */
//    if (wOption &  (uint16_t)~(uint16_t)
//        (
//        PH_EXCHANGE_BUFFERED_BIT | PH_EXCHANGE_LEAVE_BUFFER_BIT |
//        PH_EXCHANGE_TXCHAINING | PH_EXCHANGE_RXCHAINING | PH_EXCHANGE_RXCHAINING_BUFSIZE
//        )) {
//        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_PAL_ISO14443P4);
//    }
//
//    /* Check if caller has provided valid RxBuffer */
//    if (ppRxBuffer == NULL) {
//        printf("ppRxBuffer == NULL\n\r");
//        ppRxBuffer = &pRxBuffer;
//    }
//    if (pRxLength == NULL) {
//        pRxLength = &RxLength;
//    }
//    //禁止数据错误处理
//    ((phhalHw_Rc663_DataParams_t*)(pDataParams->pHalDataParams))->bDateFliter = PH_ON;
//
//    phhalHw_Wait(pDataParams->pHalDataParams,PHHAL_HW_TIME_MICROSECONDS,600);//保护时间
//
//    /* 获取RC663缓冲区的大小 */
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_RXBUFFER_BUFSIZE, &wRxBufferSize));
//    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER_BUFSIZE, &wTxBufferSize));
//
//    /* Retrieve maximum frame sizes */
//    wMaxPcdFrameSize = bI14443p4_FsTable[pDataParams->bFsdi];//PCB接收帧的最大长度
//    wMaxCardFrameSize = bI14443p4_FsTable[pDataParams->bFsci]-2;//PICC接收帧的最大长度,因为发送的时候，会自动添加CRC16
//
//    /* R(ACK) transmission in case of Rx-Chaining */
//    if (((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING) ||
//        ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE)) {
//        pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_RX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
//    }
//    /* I-Block transmission */
//    else {
//        /* Reset to default state if not in Tx-Mode */
//
//        if ((pDataParams->bStateNow & PH_EXCHANGE_MODE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_TX) {
//            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_I_BLOCK_TX;
//        }
//    }
//
//    /* Reset receive length */
//    *pRxLength = 0;
//
//    /* Reset RetryCount */
//    bRetryCountRetransmit = 0;//重试的次数
//
//    /* Reset BufferOverflow flag */
//    bBufferOverflow = 0;
//
//    /* ******************************** */
//    /*     I-BLOCK TRANSMISSION LOOP    */
//    /* ******************************** */
//    do {
//        /* Reset Preloaded bytes and ForceSend */
//        wTxBufferLen = 0;
//        bForceSend = 0;
//        switch (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) {
//        case PHPAL_I14443P4_SW_STATE_I_BLOCK_TX:
//
//            /* Retrieve Number of preloaded bytes */
//            /**********数据重发的时候执行**********/
//            if (wOption & PH_EXCHANGE_LEAVE_BUFFER_BIT) {//如果选项标记缓冲区有数据存在，那么就获取最该数据的长度
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(
//                                                                 pDataParams->pHalDataParams,
//                                                                 PHHAL_HW_CONFIG_TXBUFFER_LENGTH,
//                                                                 &wTxBufferLen));
//                printf("数据重新发送，缓冲区有效数据：%d;wOption = 0x%04x\n\r",wTxBufferLen,wOption);
//            }
//            /* Set initial INF length to (remaining) input data length */
//            wInfLength = wTxLength;
//
//
//            /** 缓冲区已经存在有效的数据，这个用于数据重发送*             */
//            if (wTxBufferLen > 0) {//标示缓冲区里面已经有了ISO 头
//                /* do not generate the iso frame */
//                wIsoFrameLen = 0;
//            }
//            /* 重新计算ISO，第一次的时候执行*/
//            else {
//                //如果缓冲区里面没有ISO 头
//                /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
//                if (!(pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
//                    bUseNad = pDataParams->bNadEnabled;
//                } else {
//                    bUseNad = 0;
//                }
//                /* Evaluate frame overhead */
//                wIsoFrameLen = 1;
//                if (bUseNad) {
//                    ++wIsoFrameLen;
//                }
//                if (pDataParams->bCidEnabled) {
//                    ++wIsoFrameLen;
//                }
//            }
//            /* Check if chaining is intended or not */
//            if ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING) {//一般不执行，外部调用触发
//                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;//
//            } else {//非链接
//                pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
//            }
//
//            /*    如果数据长度大于了最大帧的长度，设置链接位置     */
//            if ((wTxBufferLen + wIsoFrameLen + wInfLength) > wMaxCardFrameSize) {
//                // printf("设置链接标识wTxBufferLen = %d,wInfLength = %d;wMaxCardFrameSize = %d\n\r",
//                //       wTxBufferLen,wInfLength,wMaxCardFrameSize);
//                /* force frame exchange */
//                bForceSend = 1;
//                /* force chaining */
//                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
//                /* limit number of bytes to send */
//                wInfLength = wMaxCardFrameSize - wTxBufferLen - wIsoFrameLen;
//            }
//
//            /* Force frame exchange if
//            b) the TxBuffer is full;
//            */
//            if ((wOption & PH_EXCHANGE_BUFFERED_BIT) &&
//                ((wTxBufferLen + wIsoFrameLen + wInfLength) >= wTxBufferSize)) {//函数调用设置此项
//                /* force frame exchange */
//                bForceSend = 1;
//
//                /* force chaining */
//                pDataParams->bStateNow |= PHPAL_I14443P4_SW_STATE_CHAINING_BIT;
//            }
//
//            /*产生帧头，保存到缓冲区，但是不立即发送，第一次发送I块有效*/
//            if (wIsoFrameLen > 0) {
//
//                /*    产生 I-Block帧头   */
//                PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildIBlock(
//                                                                            pDataParams->bCidEnabled,
//                                                                            pDataParams->bCid,
//                                                                            bUseNad,
//                                                                            pDataParams->bNad,
//                                                                            pDataParams->bPcbBlockNum,
//                                                                            (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT) ? 1 : 0,
//                                                                            bIsoFrame,
//                                                                            &wIsoFrameLen));
//
//                /* Write Frame to HAL TxBuffer but do not preform Exchange */
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(
//                                                                pDataParams->pHalDataParams,
//                                                                PH_EXCHANGE_BUFFER_FIRST,
//                                                                bIsoFrame,
//                                                                wIsoFrameLen,
//                                                                NULL,
//                                                                NULL));
//
//                /* Retain the preloaded bytes from now on */
//                wOption |= PH_EXCHANGE_LEAVE_BUFFER_BIT;//保留缓冲区里面的字节
//            }
//
//            /* Tx-Buffering mode (and no forced exchange) */
//            if ((!bForceSend) && (wOption & PH_EXCHANGE_BUFFERED_BIT)) {
//                /* Preload the data into the TxBuffer */
//                printf("Preload the data into the TxBuffer\n\r");
//                return phhalHw_Exchange(
//                                       pDataParams->pHalDataParams,
//                                       (wOption & (uint16_t)~(uint16_t)PH_EXCHANGE_MODE_MASK),
//                                       pTxBuffer,
//                                       wInfLength,
//                                       NULL,
//                                       NULL);
//            }
//
//            /* Content has been buffered before */
//            if (wTxBufferLen > 0) {
//                /* retrieve PCB byte */
//                //获取发送缓冲区第0字节的头域，因为在发送NACK SWT的时候，修改了该字节
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_ADDITIONAL_INFO, 0x00));
//                PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_GetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, &wPcb));
//                printf("get wPCB1  = %04x\n\r",wPcb);
//                /* Preloaded Data or ForceSend -> Modify PCB byte if neccessary */
//                if ((bForceSend) || ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING)) {
//                    /* modify PCB byte */
//                    wPcb |= PHPAL_I14443P4_SW_PCB_CHAINING;
//                    printf("set wPCB2  = %04x\n\r",wPcb);
//
//                    PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, wPcb));
//                }
//            }
//            break;
//
//        case PHPAL_I14443P4_SW_STATE_I_BLOCK_RX:
//
//            /* Build R(ACK) frame */
//            PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
//                                                                        pDataParams->bCidEnabled,
//                                                                        pDataParams->bCid,
//                                                                        pDataParams->bPcbBlockNum,
//                                                                        1,
//                                                                        bIsoFrame,
//                                                                        &wIsoFrameLen));
//
//            /* Write Frame to HAL TxBuffer but do not preform Exchange */
//            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_Exchange(
//                                                            pDataParams->pHalDataParams,
//                                                            PH_EXCHANGE_BUFFERED_BIT,
//                                                            bIsoFrame,
//                                                            wIsoFrameLen,
//                                                            NULL,
//                                                            NULL));
//
//            /* Retain the preloaded bytes from now on */
//            wOption |= PH_EXCHANGE_LEAVE_BUFFER_BIT;
//
//            /* do not append any data */
//            wInfLength = 0;
//            printf("PHPAL_I14443P4_SW_STATE_I_BLOCK_RX\n\r");
//            break;
//
//            /* Should NEVER happen! */
//        default:
//            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
//        }
//        //printf("Exchange wInfLength = %d\n\r",wInfLength);
//
//        status = phpalI14443p4_Sw_EMVHandling( pDataParams,wOption & (uint16_t)~(uint16_t)PH_EXCHANGE_BUFFERED_BIT, //wOption  立即发送数据
//                                               bRetryCountRetransmit,
//                                               pTxBuffer, wInfLength,ppRxBuffer,pRxLength);
//
//        /* Complete chaining if buffer is full */
//        if (((status & PH_ERR_MASK) == PH_ERR_BUFFER_OVERFLOW) &&
//            ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE)) {
//            /* Indicate Buffer Overflow */
//            bBufferOverflow = 1;
//            /* Toggle Blocknumber */
//            pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
//        }
//        /* Else bail out on error */
//        else {
//            PH_CHECK_SUCCESS(status);//如果是协议出错，就返回
//        }
//
//        /* 重新发送I 块流程     */
//        if (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT) {
//            ++bRetryCountRetransmit;
//
//            //printf("重新发送I块 wTxBufferLen = %d\n\r",wTxBufferLen);
//
//            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig( pDataParams->pHalDataParams,
//                                                               PHHAL_HW_CONFIG_TXBUFFER_LENGTH,
//                                                               0));
//            wOption &= ~PH_EXCHANGE_LEAVE_BUFFER_BIT;//清除标识，gancj
//
//
////          if (wTxBufferLen > 0) {
////              //获取发送缓冲区第0字节的头域，因为在发送NACK SWT的时候，修改了该字节
////              /* restore PCB byte */
////              //设置额外寄存器参数为0
////              PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_ADDITIONAL_INFO, 0x00));
////              //获取发送缓冲区地址为0的字节  设置发送缓冲区地址字节 0 为WPCB
////              PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(pDataParams->pHalDataParams, PHHAL_HW_CONFIG_TXBUFFER, wPcb));
////              /* 设置发送缓冲区里面的有效数据长度，在这里是为  0  */
////              PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig( pDataParams->pHalDataParams,
////                                                                 PHHAL_HW_CONFIG_TXBUFFER_LENGTH,
////                                                                 wTxBufferLen));//设置缓冲区有效字节的长度
////
////
////
////              // printf("重新发送I卡 wTxBufferLen = %d\n\r",wTxBufferLen);
////              // memcpy(bIsoFrame,wIsoFrameLen)
////
////          }
//            /*  清除重新发送的标志位 */
//            pDataParams->bStateNow &= (uint8_t)~(uint8_t)PHPAL_I14443P4_SW_STATE_RETRANSMIT_BIT;
//        }
//        /* No retransmission in progress */
//        else {
//            /* Reset Retransmit RetryCount */
//            bRetryCountRetransmit = 0;
//
//            /* Chaining is active */
//            if (pDataParams->bStateNow == (PHPAL_I14443P4_SW_STATE_I_BLOCK_TX | PHPAL_I14443P4_SW_STATE_CHAINING_BIT)) {
//                /* Bytes to send cannot be less than sent bytes */
//                if (wTxLength < wInfLength) {
//                    return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
//                }
//
//                /* Remove sent bytes from TxBuffer */
//                pTxBuffer += wInfLength;
//                wTxLength = wTxLength - wInfLength;
//
//                wOption &= ~PH_EXCHANGE_LEAVE_BUFFER_BIT;//清除标识，gancj
//
//                printf("链接，字节wTxLength = %d发送\n\r",wTxLength);
//            }
//
//            /* Buffered / TxChaining mode -> finished after sending */
//            if ((wTxLength == 0) &&
//                (
//                (wOption & PH_EXCHANGE_BUFFERED_BIT) ||
//                ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_TXCHAINING)
//                )) {
//                return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
//            }
//        }
//    }
//    /* Loop as long as the state does not transit to RX mode */
//    while ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_RX);
//
//
//    /* Overlap PCB */
//    wRxOverlappedLen = 1;
//
//    /* Overlap CID */
//    if ((*ppRxBuffer)[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_CID_FOLLOWING) {
//        wRxOverlappedLen++;
//    }
//
//    /* Overlap NAD */
//    if ((*ppRxBuffer)[PHPAL_I14443P4_SW_PCB_POS] & PHPAL_I14443P4_SW_PCB_NAD_FOLLOWING) {
//        wRxOverlappedLen++;
//    }
//
//    /* Reset RxStartPos */
//    wRxStartPos = 0;
//
//    /* ******************************** */
//    /*      I-BLOCK RECEPTION LOOP      */
//    /* ******************************** */
//    do {
//        /* Only allow receive state at this point */
//        if ((pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_MASK) != PHPAL_I14443P4_SW_STATE_I_BLOCK_RX) {
//            return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_PAL_ISO14443P4);
//        }
//
//        /* Rule 2, ISO/IEC 14443-4:2008(E), PICC chaining */
//        /*****************************链接模式****************************/
//        if (pDataParams->bStateNow & PHPAL_I14443P4_SW_STATE_CHAINING_BIT) {
//            /* Skip overlapping / SUCCESS_CHAINING checks in case of BufferOverflow */
//            if (!bBufferOverflow) {
//                /* This is first chained response */
//                if (wRxStartPos == 0) {
//                    /* Special NAD chaining handling */
//                    /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
//                    if (pDataParams->bNadEnabled) {
//                        --wRxOverlappedLen;
//                    }
//                }
//
//                /* Backup overlapped bytes *///备份接收I块的头  3字节或者1字节
//                memcpy(bRxOverlapped, &(*ppRxBuffer)[((*pRxLength) - wRxOverlappedLen)], wRxOverlappedLen);  /* PRQA S 3200 */
//
//                /* Calculate RxBuffer Start Position */
//                wRxStartPos = (*pRxLength) - wRxOverlappedLen;//计算接收缓冲区的位置
//
//                /* Skip SUCCESS_CHAINING check for RXCHAINING_BUFSIZE mode */
//                if ((wOption & PH_EXCHANGE_MODE_MASK) != PH_EXCHANGE_RXCHAINING_BUFSIZE) {
//                    /* Return with chaining status if the next chain may not fit into our buffer */
//                    if ((*pRxLength + wMaxPcdFrameSize) > wRxBufferSize) {
//                        /* Adjust RxBuffer position */
//                        (*ppRxBuffer) += wRxOverlappedLen;
//                        *pRxLength = *pRxLength - wRxOverlappedLen;
//
//                        return PH_ADD_COMPCODE(PH_ERR_SUCCESS_CHAINING, PH_COMP_PAL_ISO14443P4);
//                    }
//                }
//            }
//
//            /*    设置接收缓冲区的开始位置    */
//            PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_SetConfig(
//                                                             pDataParams->pHalDataParams,
//                                                             PHHAL_HW_CONFIG_RXBUFFER_STARTPOS,
//                                                             wRxStartPos));
//
//            /* Prepare R(ACK) frame */
//            PH_CHECK_SUCCESS_FCT(statusTmp, phpalI14443p4_Sw_BuildRBlock(
//                                                                        pDataParams->bCidEnabled,
//                                                                        pDataParams->bCid,
//                                                                        pDataParams->bPcbBlockNum,
//                                                                        1,
//                                                                        bIsoFrame,
//                                                                        &wIsoFrameLen));
//            //PBOC 3.0 升级
//            phhalHw_Wait(pDataParams->pHalDataParams,PHHAL_HW_TIME_MICROSECONDS,600);//保护时间
//
//            /* Perform Exchange using complete ISO handling */
//            status = phpalI14443p4_Sw_EMVHandling(
//                                                 pDataParams,
//                                                 PH_EXCHANGE_DEFAULT,
//                                                 0,
//                                                 bIsoFrame,
//                                                 wIsoFrameLen,
//                                                 ppRxBuffer,
//                                                 pRxLength);
//
//            /* Complete chaining if buffer is full */
//            if (((status & PH_ERR_MASK) == PH_ERR_BUFFER_OVERFLOW) &&
//                ((wOption & PH_EXCHANGE_MODE_MASK) == PH_EXCHANGE_RXCHAINING_BUFSIZE)) {
//                /* Reset wRxStartPos */
//                wRxStartPos = 0;
//
//                /* Indicate Buffer Overflow */
//                bBufferOverflow = 1;
//
//                /* Toggle Blocknumber */
//                pDataParams->bPcbBlockNum ^= PHPAL_I14443P4_SW_PCB_BLOCKNR;
//            }
//            /* Default behaviour */
//            else {
//                /* Bail out on error */
//                PH_CHECK_SUCCESS(status);
//
//                /* Restore overlapped INF bytes *///恢复该字节
//                memcpy(&(*ppRxBuffer)[wRxStartPos], bRxOverlapped, wRxOverlappedLen);  /* PRQA S 3200 */ /* PRQA S 3354 */
//            }
//        }
//        /* 非链接模式 -> reception finished */
//        else {
//            /* Return data */
//            if (!bBufferOverflow) {
//                /* Special NAD chaining handling */
//                /* 7.1.1.3 c), ISO/IEC 14443-4:2008(E), "During chaining the NAD shall only be transmitted in the first block of chain." */
//                if ((wRxStartPos > 0) && (pDataParams->bNadEnabled)) {
//                    ++wRxOverlappedLen;
//                }
//
//                /* Do not return protocol bytes, advance to INF field */
//                (*ppRxBuffer) += wRxOverlappedLen;
//                *pRxLength = *pRxLength - wRxOverlappedLen;
//
//                /* Reception successful */
//                status = PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_PAL_ISO14443P4);
//            }
//            /* do not return any data in case of Buffer Overflow */
//            else {
//                *pRxLength = 0;
//                status = PH_ADD_COMPCODE(PH_ERR_BUFFER_OVERFLOW, PH_COMP_PAL_ISO14443P4);
//            }
//
//            /* Reception finished */
//            pDataParams->bStateNow = PHPAL_I14443P4_SW_STATE_FINISHED;
//        }
//    }
//    while (pDataParams->bStateNow != PHPAL_I14443P4_SW_STATE_FINISHED);
//
//    //Print_LogHex("P4_EMV data:",*ppRxBuffer,pRxLength[0]);
//
//    return status;
//}

#endif /* NXPBUILD__PHPAL_I14443P4_SW */
