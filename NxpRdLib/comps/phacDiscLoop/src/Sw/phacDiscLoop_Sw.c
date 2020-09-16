/*
*         Copyright (c), NXP Semiconductors Bangalore / India
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
* Software Discovery Loop Activities Component of Reader Library Framework.
* $Author: prabakaran.c $
* $Revision: 1.22 $
* $Date: Mon Dec  3 09:40:16 2012 $
*
* History:
*  PC: Generated 23. Aug 2012
*
*/

#include <ph_Status.h>
#include <ph_RefDefs.h>
#include <phacDiscLoop.h>
#include <phpalI14443p3a.h>
#include <phhalHw.h>
#include <phpalFelica.h>
#include <phpalI14443p3b.h>
#include <phpalI18092mPI.h>
#include <phOsal.h>

#include "RFCARD_App_RC531.h"

#ifdef NXPBUILD__PHAC_DISCLOOP_SW

    #include "phacDiscLoop_Sw.h"
    #include "phacDiscLoop_Sw_Int.h"
		
		
/******************************************
//˽����ʱ����---us
******************************************/		
static void DelayUs(unsigned int delayus)
{
	unsigned int i,j;
	for (i=0; i<delayus; i++)
	{
			for (j = 0;j<11;j++) ;
	}
	return ;
}

/*******************************************
//˽����ʱ����----ms
********************************************/
static void DelayMs(unsigned int delayms)
{
	DelayUs(delayms*1000);
	return ; 
}		

phStatus_t phacDiscLoop_Sw_Init(
                               phacDiscLoop_Sw_DataParams_t * pDataParams,
                               uint16_t                       wSizeOfDataParams,
                               void                         * pHalDataParams,
                               void                         * pOsal
                               )
{
    if (sizeof(phacDiscLoop_Sw_DataParams_t) != wSizeOfDataParams) {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_DATA_PARAMS, PH_COMP_AC_DISCLOOP);
    }

    PH_ASSERT_NULL(pDataParams);

    /* Initialize the private data */
    pDataParams->wId                    = PH_COMP_AC_DISCLOOP | PHAC_DISCLOOP_SW_ID;
    pDataParams->pHalDataParams         = pHalDataParams;
    pDataParams->pPal1443p3aDataParams  = NULL;
    pDataParams->pPal1443p3bDataParams  = NULL;
    pDataParams->pPalFelicaDataParams   = NULL;
    pDataParams->pPal1443p4aDataParams  = NULL;
    pDataParams->pPal18092mPIDataParams = NULL;
    pDataParams->pOsal                  = pOsal;
    pDataParams->wTagsFound             = PHAC_DISCLOOP_NO_TAGS_FOUND;
    pDataParams->bState                 = PHAC_DISCLOOP_STATE_IDLE;
    pDataParams->wGTa                   = PHAC_DISCLOOP_TYPEA_DEFAULT_GT;
    pDataParams->bGTaUnit               = PH_OSAL_TIMER_UNIT_US;
    pDataParams->wGTb                   = PHAC_DISCLOOP_TYPEB_DEFAULT_GT;
    pDataParams->bGTbUnit               = PH_OSAL_TIMER_UNIT_US;
    pDataParams->wGTf                   = PHAC_DISCLOOP_TYPEF_DEFAULT_GT;
    pDataParams->bGTfUnit               = PH_OSAL_TIMER_UNIT_US;
    pDataParams->wGTbf                  = PHAC_DISCLOOP_TYPEB_TOF_DEFAULT_GT;
    pDataParams->bGTbfUnit              = PH_OSAL_TIMER_UNIT_US;
    pDataParams->pErrHandler            = NULL;
    pDataParams->wPausePeriod           = PHAC_DISCLOOP_DEFAULT_PAUSE_TIME;
    pDataParams->bPausePeriodUnit       = PH_OSAL_TIMER_UNIT_MS;
    pDataParams->bStopFlag              = PH_OFF;
    pDataParams->bBailOut               = 0;
    pDataParams->bLoopMode              = PHAC_DISCLOOP_SET_POLL_MODE | PHAC_DISCLOOP_SET_LISTEN_MODE;
    pDataParams->bNumPollLoops          = PHAC_DISCLOOP_POLL_MIN_LOOPTIME;
    pDataParams->bDetectionConfiguration= PHAC_DISCLOOP_CON_POLL_A| PHAC_DISCLOOP_CON_POLL_B | PHAC_DISCLOOP_CON_POLL_F;
    pDataParams->bConColl               = 0;
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS
    pDataParams->sTypeATargetInfo.bTotalTagsFound_A     = 0;
    pDataParams->sTypeATargetInfo.bActivatedTagNumber_A = 0;
    pDataParams->sTypeATargetInfo.bDeviceLimit_A        = PHAC_DISCLOOP_TYPEA_DEFAULT_DEVICE_LIMIT;
    pDataParams->sTypeATargetInfo.bLoopLimit_A          = PHAC_DISCLOOP_TYPEA_DEFAULT_POLL_LIMIT; 

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS
    pDataParams->sTypeATargetInfo.sTypeA_I3P4.bFsdi     = 0;
    pDataParams->sTypeATargetInfo.sTypeA_I3P4.bCid      = 0;
    pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDri      = 0;
    pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDsi      = 0;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bDid          = 0;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bLri          = 0;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable    = 0;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bNad          = 0;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.pGi           = NULL;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bGiLength     = 0;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.pAtrRes       = NULL;
    pDataParams->sTypeATargetInfo.sTypeA_P2P.bAtrResLength = 0;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS */

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS */


#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS
    pDataParams->sTypeBTargetInfo.bTotalTagsFound_B     = 0;
    pDataParams->sTypeBTargetInfo.bDeviceLimit_B        = PHAC_DISCLOOP_TYPEB_DEFAULT_DEVICE_LIMIT;
    pDataParams->sTypeBTargetInfo.bNCodingSlotNumber    = PHAC_DISCLOOP_TYPEB_DEFAULT_SLOT_NUM;
    pDataParams->sTypeBTargetInfo.bLoopLimit_B          = PHAC_DISCLOOP_TYPEB_DEFAULT_POLL_LIMIT;
    pDataParams->sTypeBTargetInfo.bCollPend             = 0;
    pDataParams->sTypeBTargetInfo.bAfiReq               = 0;
    pDataParams->sTypeBTargetInfo.bExtendedAtqBbit      = 0;
    pDataParams->sTypeBTargetInfo.bAdvFeaturesBit       = 0;
    pDataParams->sTypeBTargetInfo.bFsdi                 = 0;
    pDataParams->sTypeBTargetInfo.bCid                  = 0;
    pDataParams->sTypeBTargetInfo.bDri                  = 0;
    pDataParams->sTypeBTargetInfo.bDri                  = 0;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS  
    pDataParams->sTypeFTargetInfo.bTotalTagsFound_F     = 0;
    pDataParams->sTypeFTargetInfo.bDeviceLimit_F        = PHAC_DISCLOOP_TYPEF_DEFAULT_DEVICE_LIMIT;
    pDataParams->sTypeFTargetInfo.bActivatedTagNumber_F = 0;
    pDataParams->sTypeFTargetInfo.bLoopLimit_F          = PHAC_DISCLOOP_TYPEF_DEFAULT_POLL_LIMIT;
    pDataParams->sTypeFTargetInfo.aSystemCode[0]        = 0;
    pDataParams->sTypeFTargetInfo.aSystemCode[1]        = 0;
    pDataParams->sTypeFTargetInfo.bTimeSlot             = PHAC_DISCLOOP_TYPEF_DEFAULT_TIME_SLOT;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bDid          = 0;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bLri          = 0;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNadEnable    = 0;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNad          = 0;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.pGi           = NULL;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bGiLength     = 0;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.pAtrRes       = NULL;
    pDataParams->sTypeFTargetInfo.sTypeF_P2P.bAtrResLength = 0;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS */

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);

}


phStatus_t phacDiscLoop_Sw_Start(
                                phacDiscLoop_Sw_DataParams_t * pDataParams
                                )
{

    phStatus_t PH_MEMLOC_REM status;
    uint8_t    PH_MEMLOC_REM bLoop;
    uint8_t    PH_MEMLOC_REM bLoopMode;
    uint16_t   PH_MEMLOC_REM wRfField = PH_OFF;
    uint8_t    PH_MEMLOC_REM bSkipPoll = PH_OFF;                                  /* Flag that indicates skipping of poll phase */
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS
    uint8_t    aNfcId3[10]; 
    uint8_t    bTypeFTagIdx = 0;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */


    bLoopMode = pDataParams->bLoopMode;

    pDataParams->wTagsFound = PHAC_DISCLOOP_NO_TAGS_FOUND;

    switch (bLoopMode) {
    /* The following modes are allowed */
    case PHAC_DISCLOOP_SET_POLL_MODE | PHAC_DISCLOOP_SET_LISTEN_MODE:
    case PHAC_DISCLOOP_SET_POLL_MODE | PHAC_DISCLOOP_SET_PAUSE_MODE:
    case PHAC_DISCLOOP_SET_LISTEN_MODE:
        break;
    default:
        /* Anything else, is not a valid combination for the discovery loop */
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
    }

    if (pDataParams->bDetectionConfiguration & 
        ~(PHAC_DISCLOOP_CON_POLL_A | 
          PHAC_DISCLOOP_CON_POLL_B | 
          PHAC_DISCLOOP_CON_POLL_F)
       ) {
        return PH_ADD_COMPCODE(PH_ERR_INVALID_PARAMETER, PH_COMP_AC_DISCLOOP);
    }
    /* Clear the information collected in previous cycle, if any */
//  phacDiscLoop_Sw_Int_ClearDataParams(pDataParams);//�������

    pDataParams->bState = PHAC_DISCLOOP_STATE_IDLE;

    if ((bLoopMode & PHAC_DISCLOOP_SET_LISTEN_MODE) ||
        (bLoopMode & PHAC_DISCLOOP_SET_PAUSE_MODE)) {
        status = phhalHw_GetConfig(pDataParams->pHalDataParams,
                                   PHHAL_HW_CONFIG_EXT_RF_ON, &wRfField);
        if ((status & PH_ERR_MASK) == PH_ERR_UNSUPPORTED_PARAMETER) {
            /* If this feature is not supported let us
               turn off listen mode */
            bLoopMode = bLoopMode & ~PHAC_DISCLOOP_SET_LISTEN_MODE;
            bSkipPoll = PH_OFF;
        } else if (status == PH_ERR_SUCCESS) {
            bSkipPoll = (uint8_t)wRfField; 
        } else {
            PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
        }
    }

    if ((bLoopMode & PHAC_DISCLOOP_SET_POLL_MODE) && bSkipPoll == PH_OFF) {
        bLoop = pDataParams->bNumPollLoops;
        if (bLoop <  PHAC_DISCLOOP_POLL_MIN_LOOPTIME  
            || bLoopMode > PHAC_DISCLOOP_POLL_MAX_LOOPTIME) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        /* We need to be in polling mode */
        while (bLoop > 0) {
            pDataParams->wTagsFound = PHAC_DISCLOOP_NO_TAGS_FOUND;
            /* Turn on the RF field */
            phhalHw_FieldOff(pDataParams->pHalDataParams);
            DelayMs(5);
            phhalHw_FieldOn(pDataParams->pHalDataParams);//�򿪵�ų�
            DelayMs(20);


            /***********************���A��***********************/
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS
            if (PHAC_DISCLOOP_CHECK_ANDMASK(
                                           pDataParams->bDetectionConfiguration, 
                                           PHAC_DISCLOOP_CON_POLL_A)
               ) {
                status = phhalHw_ApplyProtocolSettings(
                                                      pDataParams->pHalDataParams, 
                                                      PHHAL_HW_CARDTYPE_ISO14443A
                                                      );
                PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                /* Guard time */

                //DelayMs(1);

                pDataParams->bState = PHAC_DISCLOOP_STATE_DETECTING | PHAC_DISCLOOP_STATE_TAG_TYPEA;
                /* Let us see if we need to exit the discovery loop */
                PHAC_DISCLOOP_CHECK_STOP(pDataParams);
                status = phacDiscLoop_Sw_Int_DetectA(pDataParams);
                if ((status & PH_ERR_SUCCESS) != PH_ERR_SUCCESS &&
                    (status & PH_ERR_MASK) != PHAC_DISCLOOP_ERR_TYPEA_NO_TAG_FOUND &&
                    (status & PH_ERR_SUCCESS) != PH_ERR_COLLISION_ERROR) {
                    /* This is an error which can not be handled */
                    PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                }
                if (PHAC_DISCLOOP_CHECK_ANDMASK(
                                               pDataParams->wTagsFound, 
                                               PHAC_DISCLOOP_TYPEA_DETECTED)
                   ) {

                    pDataParams->bState = PHAC_DISCLOOP_STATE_DETECTED | PHAC_DISCLOOP_STATE_TAG_TYPEA;
                    if ((pDataParams->bConColl & PHAC_DISCLOOP_CON_ANTICOLL_A) == 0) {

                        status = phacDiscLoop_Sw_Int_CollisionResolutionA(pDataParams);
                        PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                        //printf("bTotalTagsFound_A = %02x \n\r",pDataParams->sTypeATargetInfo.bTotalTagsFound_A);
                        if (pDataParams->sTypeATargetInfo.bTotalTagsFound_A == 1 &&
                            pDataParams->bBailOut & PHAC_DISCLOOP_CON_BAIL_OUT_A) {
                            /* Let us see if we need to exit the discovery loop */
                            PHAC_DISCLOOP_CHECK_STOP(pDataParams);
                            /* Only one Type A tag in field, let's activate it */
                            // printf("Only one Type A tag in field, let's activate it\n\r");
                            status = phacDiscLoop_Sw_Int_ActivateA(pDataParams, 0);
                            PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                        }
                    }
                    if ((pDataParams->bBailOut &
                         PHAC_DISCLOOP_CON_BAIL_OUT_A) != 0) {
                        return PH_ADD_COMPCODE(
                                              PH_ERR_SUCCESS,
                                              PH_COMP_AC_DISCLOOP);
                    }

                } else {
                    pDataParams->bState = PHAC_DISCLOOP_STATE_IDLE;
                }
            }
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS */

            /***********************���B��***********************/
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS//  ���B��
            if (PHAC_DISCLOOP_CHECK_ANDMASK( pDataParams->bDetectionConfiguration,PHAC_DISCLOOP_CON_POLL_B)) {
                status = phhalHw_ApplyProtocolSettings(pDataParams->pHalDataParams, PHHAL_HW_CARDTYPE_ISO14443B);
                /* Guard time */
                DelayMs(5);
                pDataParams->bState = PHAC_DISCLOOP_STATE_DETECTING | PHAC_DISCLOOP_STATE_TAG_TYPEB;
                /* Let us see if we need to exit the discovery loop */
                PHAC_DISCLOOP_CHECK_STOP(pDataParams);
                status = phacDiscLoop_Sw_Int_DetectB(pDataParams);

                if ((status & PH_ERR_SUCCESS) != PH_ERR_SUCCESS &&
                    (status & PH_ERR_MASK) != PHAC_DISCLOOP_ERR_TYPEB_NO_TAG_FOUND &&
                    (status & PH_ERR_SUCCESS) != PH_ERR_INTEGRITY_ERROR) {
                    /* This is an error which can not be handled */
                    PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                }

                if (PHAC_DISCLOOP_CHECK_ANDMASK(pDataParams->wTagsFound, PHAC_DISCLOOP_TYPEB_DETECTED)) {
                    if (status == PH_ERR_SUCCESS) {
                        /* Only one Type B tag detected */
                        pDataParams->bState = PHAC_DISCLOOP_STATE_DETECTED | PHAC_DISCLOOP_STATE_TAG_TYPEB;
                        /* Let us see if we need to exit the discovery loop */
                        PHAC_DISCLOOP_CHECK_STOP(pDataParams);
                        /* ���ƽ */
                        if (pDataParams->bBailOut & PHAC_DISCLOOP_CON_BAIL_OUT_B) {
                            status = phacDiscLoop_Sw_Int_ActivateB(pDataParams, 0);
                        }
                        PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                        //������ڶ����Ƭ��һһPOLLING ,Hault
                    } else if (status == PH_ERR_INTEGRITY_ERROR &&
                               (pDataParams->bConColl & PHAC_DISCLOOP_CON_ANTICOLL_B) == 0) {
                        /* More than one Type B tag in field */
                        pDataParams->bState = PHAC_DISCLOOP_STATE_DETECTED | PHAC_DISCLOOP_STATE_TAG_TYPEB;
                        if ((pDataParams->bBailOut & 
                             PHAC_DISCLOOP_CON_BAIL_OUT_B) != 0) {
                            return PH_ADD_COMPCODE(
                                                  PH_ERR_SUCCESS, 
                                                  PH_COMP_AC_DISCLOOP
                                                  );
                        }
                        status = phacDiscLoop_Sw_Int_CollisionResolutionB(
                                                                         pDataParams
                                                                         );
                    }
                    PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                    if ((pDataParams->bBailOut &
                         PHAC_DISCLOOP_CON_BAIL_OUT_B) != 0) {
                        return PH_ADD_COMPCODE(
                                              PH_ERR_SUCCESS,
                                              PH_COMP_AC_DISCLOOP);
                    }
                } else {
                    pDataParams->bState = PHAC_DISCLOOP_STATE_IDLE;
                }
            }
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
            if (PHAC_DISCLOOP_CHECK_ANDMASK(pDataParams->bDetectionConfiguration, PHAC_DISCLOOP_CON_POLL_F)) {
                /* Guard time */
                /* NFC-F preceded by NFC-B detection */
//              if (PHAC_DISCLOOP_CHECK_ANDMASK( pDataParams->bDetectionConfiguration,PHAC_DISCLOOP_CON_POLL_B))
//                  DelayMs(5);
//              else
//                  DelayMs(5);

                status = phhalHw_ApplyProtocolSettings(
                                                      pDataParams->pHalDataParams, 
                                                      PHHAL_HW_CARDTYPE_FELICA
                                                      ); 
                PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                pDataParams->bState = PHAC_DISCLOOP_STATE_DETECTING | PHAC_DISCLOOP_STATE_TAG_TYPEF;
                /* Let us see if we need to exit the discovery loop */
                PHAC_DISCLOOP_CHECK_STOP(pDataParams);
                status = phacDiscLoop_Sw_Int_DetectF(pDataParams);
                if ((status & PH_ERR_SUCCESS) != PH_ERR_SUCCESS &&
                    (status & PH_ERR_MASK) != PHAC_DISCLOOP_ERR_TYPEF_NO_TAG_FOUND &&
                    (status & PH_ERR_SUCCESS) != PH_ERR_IO_TIMEOUT) {
                    /* This is an error which can not be handled */
                    PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                }
                /* The Type F tag is already activated */
                pDataParams->sTypeFTargetInfo.bActivatedTagNumber_F++;
                if (PHAC_DISCLOOP_CHECK_ANDMASK(pDataParams->wTagsFound, PHAC_DISCLOOP_TYPEF_DETECTED)) {
                    pDataParams->bState = PHAC_DISCLOOP_STATE_DETECTED | PHAC_DISCLOOP_STATE_TAG_TYPEF;
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS
                    if ((pDataParams->sTypeFTargetInfo.aTypeF[bTypeFTagIdx].aIDmPMm[0] == 0x01) 
                        && (pDataParams->sTypeFTargetInfo.aTypeF[bTypeFTagIdx].aIDmPMm[1] == 0xFE)) {
                        //printf("This is Type F tag with P2P capablities\n\r");
                        /* This is Type F tag with P2P capablities */
                        pDataParams->wTagsFound |= PHAC_DISCLOOP_TYPEF_DETECTED_TAG_P2P;
                        /* Reset the 18082 protocol */
                        status = phpalI18092mPI_ResetProtocol(pDataParams->pPal18092mPIDataParams);

                        PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                        //�����212 424 ��ô����
                        memcpy(aNfcId3, pDataParams->sTypeFTargetInfo.aTypeF[bTypeFTagIdx].aIDmPMm, 8);//
                        aNfcId3[8] = 0;
                        aNfcId3[9] = 0;
                        /* Activate the P2P capable Felica card */
                        status = phpalI18092mPI_Atr(
                                                   pDataParams->pPal18092mPIDataParams,
                                                   aNfcId3,
                                                   pDataParams->sTypeFTargetInfo.sTypeF_P2P.bDid,
                                                   pDataParams->sTypeFTargetInfo.sTypeF_P2P.bLri,
                                                   pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNadEnable,
                                                   pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNad,
                                                   pDataParams->sTypeFTargetInfo.sTypeF_P2P.pGi,
                                                   pDataParams->sTypeFTargetInfo.sTypeF_P2P.bGiLength,
                                                   pDataParams->sTypeFTargetInfo.sTypeF_P2P.pAtrRes,
                                                   &(pDataParams->sTypeFTargetInfo.sTypeF_P2P.bAtrResLength)
                                                   );   
                        if (status != PH_ERR_SUCCESS) {
                            /* The card could not be activated to P2P */
                            pDataParams->sTypeFTargetInfo.bActivatedTagNumber_F--;
                            // printf("The card could not be activated to P2P\n\r");
                        } else
                            ;
                        // printf("Activate the P2P capable Felica card OK\n\r");

                        PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status);
                        pDataParams->wTagsFound |= PHAC_DISCLOOP_TYPEF_DETECTED_TAG_P2P;
                    } else
                        pDataParams->sTypeFTargetInfo.bActivatedTagNumber_F--;

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */
                } else {
                    pDataParams->sTypeFTargetInfo.bActivatedTagNumber_F--;
                    pDataParams->bState = PHAC_DISCLOOP_STATE_IDLE;
                }
            }
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS */

            /* NOTE: Turning off the field will make the UIDs collected that are 
             *       random in nature invalid so should we really turn of the field ? 
             */
            /* Turn off the RF field */
            /* status = phhalHw_FieldOff(pDataParams->pHalDataParams); */
            /* PHAC_DISCLOOP_CHECK_STATUS(pDataParams, status); */

            if (pDataParams->wTagsFound != PHAC_DISCLOOP_NO_TAGS_FOUND) {
                /* This means we have detected on or more than one type of 
                   tag(s) so we can conclude the discovery loop */
                return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
            }
            bLoop--;
        }
    }

    if (bLoopMode & PHAC_DISCLOOP_SET_LISTEN_MODE) {
        /* Listen mode code here */
        return PH_ADD_COMPCODE(PH_ERR_PROTOCOL_ERROR, PH_COMP_AC_DISCLOOP);
    }

    if (bLoopMode & PHAC_DISCLOOP_SET_PAUSE_MODE) {
        /* Pause mode */
        DelayMs(10);
    }
    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_AC_DISCLOOP);
}



phStatus_t phacDiscLoop_Sw_SetConfig(
                                    phacDiscLoop_Sw_DataParams_t * pDataParams,
                                    uint16_t wConfig, 
                                    uint16_t wValue
                                    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;

    statusTmp = PH_ERR_SUCCESS;

    switch (wConfig) {
    
    case PHAC_DISCLOOP_CONFIG_GTA_VALUE_US:
        if (wValue < PHAC_DISCLOOP_TYPEA_DEFAULT_GT) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->wGTa = wValue;
        pDataParams->bGTaUnit = PH_OSAL_TIMER_UNIT_US;
        break;

    case PHAC_DISCLOOP_CONFIG_GTB_VALUE_US:
        if (wValue < PHAC_DISCLOOP_TYPEB_DEFAULT_GT) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->wGTb = wValue;
        pDataParams->bGTbUnit = PH_OSAL_TIMER_UNIT_US;
        break;

    case PHAC_DISCLOOP_CONFIG_GTF_VALUE_US:
        if (wValue < PHAC_DISCLOOP_TYPEF_DEFAULT_GT) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->wGTf = wValue;
        pDataParams->bGTfUnit = PH_OSAL_TIMER_UNIT_US;
        break;

    case PHAC_DISCLOOP_CONFIG_GTBF_VALUE_US:
        if (wValue < PHAC_DISCLOOP_TYPEB_TOF_DEFAULT_GT) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->wGTbf = wValue;
        pDataParams->bGTbfUnit = PH_OSAL_TIMER_UNIT_US;
        break;

    case PHAC_DISCLOOP_CONFIG_GTA_VALUE_MS:
        if (wValue < (PHAC_DISCLOOP_TYPEA_DEFAULT_GT/1000)) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->wGTa = wValue;
        pDataParams->bGTaUnit = PH_OSAL_TIMER_UNIT_MS;
        break;

    case PHAC_DISCLOOP_CONFIG_GTB_VALUE_MS:
        if (wValue < (PHAC_DISCLOOP_TYPEB_DEFAULT_GT/1000)) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->wGTb = wValue;
        pDataParams->bGTbUnit = PH_OSAL_TIMER_UNIT_MS;
        break;

    case PHAC_DISCLOOP_CONFIG_GTF_VALUE_MS:
        if (wValue < (PHAC_DISCLOOP_TYPEF_DEFAULT_GT/1000)) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->wGTf = wValue;
        pDataParams->bGTfUnit = PH_OSAL_TIMER_UNIT_MS;
        break;

    case PHAC_DISCLOOP_CONFIG_GTBF_VALUE_MS:
        if (wValue < (PHAC_DISCLOOP_TYPEB_TOF_DEFAULT_GT/1000)) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->wGTbf = wValue;
        pDataParams->bGTbfUnit = PH_OSAL_TIMER_UNIT_MS;
        break;

    case PHAC_DISCLOOP_CONFIG_MODE:
        /* Check if the mode selected is valid */
        switch (wValue) {
        /* The following modes are allowed */
        case PHAC_DISCLOOP_SET_POLL_MODE | PHAC_DISCLOOP_SET_LISTEN_MODE:
        case PHAC_DISCLOOP_SET_POLL_MODE | PHAC_DISCLOOP_SET_PAUSE_MODE:
        case PHAC_DISCLOOP_SET_LISTEN_MODE:
            break;
        default:
            /* Anything else, is not a valid combination 
               for the discovery loop */
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->bLoopMode = (uint8_t)(wValue);
        break;     

    case PHAC_DISCLOOP_CONFIG_NUM_POLL_LOOPS:
        if (wValue <  PHAC_DISCLOOP_POLL_MIN_LOOPTIME  || 
            wValue  > PHAC_DISCLOOP_POLL_MAX_LOOPTIME) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }
        pDataParams->bNumPollLoops = (uint8_t)(wValue);
        break;

    case PHAC_DISCLOOP_CONFIG_BAIL_OUT:
        if (wValue & ~(PHAC_DISCLOOP_CON_BAIL_OUT_A |  PHAC_DISCLOOP_CON_BAIL_OUT_B)) {
            /* Invalid bail out option set */
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }

        if ((wValue & PHAC_DISCLOOP_CON_BAIL_OUT_A) &&
            !(pDataParams->bDetectionConfiguration & PHAC_DISCLOOP_CON_POLL_A)) {
            /* Invalid to bail out of Type A if detection for Type A is
               not set */
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }

        if ((wValue & PHAC_DISCLOOP_CON_BAIL_OUT_B) &&
            !(pDataParams->bDetectionConfiguration & PHAC_DISCLOOP_CON_POLL_B)) {
            /* Invalid to bail out of Type F if detection for Type F is
               not set */
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }

        pDataParams->bBailOut = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_PAUSE_PERIOD_MS:
        if (wValue < PHAC_DISCLOOP_DEFAULT_PAUSE_TIME) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }
        pDataParams->bPausePeriodUnit = PH_OSAL_TIMER_UNIT_MS;
        pDataParams->wPausePeriod     = wValue;
        break;



    case PHAC_DISCLOOP_CONFIG_STOP:
        pDataParams->bStopFlag = PH_ON;
        break;

    case PHAC_DISCLOOP_CONFIG_DETECT_TAGS:
        if (pDataParams->bDetectionConfiguration & 
            ~(PHAC_DISCLOOP_CON_POLL_A | 
              PHAC_DISCLOOP_CON_POLL_B | 
              PHAC_DISCLOOP_CON_POLL_F)
           ) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }

        pDataParams->bDetectionConfiguration = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_ANTI_COLL:
        if (wValue & 
            ~(PHAC_DISCLOOP_CON_ANTICOLL_A | 
              PHAC_DISCLOOP_CON_ANTICOLL_B)
           ) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }
        pDataParams->bConColl = (uint8_t)wValue;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEA_POLL_LIMIT:
        if (wValue < 0 || wValue > PHAC_DISCLOOP_TYPEA_POLL_LIMIT) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeATargetInfo.bLoopLimit_A = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_DEVICE_LIMIT:
        if (wValue < 0 || wValue > PHAC_DISCLOOP_TYPEA_I3P3_NR) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.bDeviceLimit_A = (uint8_t)wValue;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_FSDI:
        if (wValue > 8) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.sTypeA_I3P4.bFsdi = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_CID:
        if (wValue > 14) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.sTypeA_I3P4.bCid = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_DRI:
        if (wValue > 3) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDri = (uint8_t)wValue;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_DSI:
        if (wValue > 3) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDsi = (uint8_t)wValue;
        break;                

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS   

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_DID:
        if (wValue > 14) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.sTypeA_P2P.bDid = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_LRI:
        if (wValue > 3) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.sTypeA_P2P.bLri = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_NAD_ENABLE:
        if (wValue == 0) {
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable = PH_OFF;
        } else {
            pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable = PH_ON;
        }
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_NAD:
        if (wValue > 255) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.sTypeA_P2P.bNad = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_GI_LEN:
        if (wValue > 255) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeATargetInfo.sTypeA_P2P.bGiLength = (uint8_t)wValue;
        break;

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS */

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEB_DEVICE_LIMIT:
        if (wValue < 0 || wValue > PHAC_DISCLOOP_TYPEB_I3P3B_NR) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeBTargetInfo.bDeviceLimit_B = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_NCODING_SLOT:
        if (wValue < 0 || wValue > PHAC_DISCLOOP_TYPEB_MAX_SLOT_NUM) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeBTargetInfo.bNCodingSlotNumber = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_POLL_LIMIT:
        if (wValue < 0 || wValue > PHAC_DISCLOOP_TYPEB_POLL_LIMIT) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeBTargetInfo.bLoopLimit_B = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_AFI_REQ:
        pDataParams->sTypeBTargetInfo.bAfiReq = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_EXTATQB:
        if (wValue == 0) {
            pDataParams->sTypeBTargetInfo.bExtendedAtqBbit = 0;
        } else {
            pDataParams->sTypeBTargetInfo.bExtendedAtqBbit = 1;
        }
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_FSDI:
        if (wValue > 0xff) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER,
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeBTargetInfo.bFsdi = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_CID:
        if (wValue > 0xff) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER,
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeBTargetInfo.bCid = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_DRI:
        if (wValue > 0xff) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER,
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeBTargetInfo.bDri = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_DSI:
        if (wValue > 0xff) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER,
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeBTargetInfo.bDsi = (uint8_t)wValue;
        break;

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEF_POLL_LIMIT:
        if (wValue < 0 || wValue > PHAC_DISCLOOP_TYPEF_POLL_LIMIT) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP);
        }
        pDataParams->sTypeFTargetInfo.bLoopLimit_F = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_DEVICE_LIMIT:
        if (wValue < 0 || wValue > PHAC_DISCLOOP_TYPEF_NR) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeFTargetInfo.bDeviceLimit_F = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_SYSTEM_CODE:
        pDataParams->sTypeFTargetInfo.aSystemCode[0] = (uint8_t)(wValue & 0xFF);
        pDataParams->sTypeFTargetInfo.aSystemCode[1] = (uint8_t)((wValue & 0xFF00) >> 8);
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_TIME_SLOT:
        if (wValue > 16) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }

        pDataParams->sTypeFTargetInfo.bTimeSlot = (uint8_t)wValue;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_DID:
        if (wValue > 14) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }

        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bDid = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_LRI:
        if (wValue > 3) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bLri = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD_ENABLE:
        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNadEnable = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD:
        if (wValue > 255) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNad = (uint8_t)wValue;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_GI_LEN:
        if (wValue > 255) {
            return PH_ADD_COMPCODE(
                                  PH_ERR_INVALID_PARAMETER, 
                                  PH_COMP_AC_DISCLOOP
                                  );
        }
        pDataParams->sTypeFTargetInfo.sTypeF_P2P.bGiLength = (uint8_t)wValue;
        break;

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS */


    default:
        statusTmp = PH_ADD_COMPCODE(
                                   PH_ERR_INVALID_PARAMETER, 
                                   PH_COMP_AC_DISCLOOP
                                   );

    }

    return statusTmp;
}

phStatus_t phacDiscLoop_Sw_GetConfig(
                                    phacDiscLoop_Sw_DataParams_t * pDataParams,
                                    uint16_t wConfig, 
                                    uint16_t * pValue
                                    )
{
    phStatus_t  PH_MEMLOC_REM statusTmp;

    statusTmp = PH_ERR_SUCCESS;

    switch (wConfig) {
    case PHAC_DISCLOOP_CONFIG_MODE:
        *pValue = pDataParams->bLoopMode; 
        break;     

    case PHAC_DISCLOOP_CONFIG_NUM_POLL_LOOPS:
        *pValue = pDataParams->bNumPollLoops;
        break;

    case PHAC_DISCLOOP_CONFIG_TAGS_DETECTED:
        *pValue = pDataParams->wTagsFound;
        break;

    case PHAC_DISCLOOP_CONFIG_GTA_VALUE:
        *pValue = pDataParams->wGTa;      
        break;

    case PHAC_DISCLOOP_CONFIG_GTB_VALUE:
        *pValue = pDataParams->wGTb;
        break;

    case PHAC_DISCLOOP_CONFIG_GTF_VALUE:
        *pValue = pDataParams->wGTf;
        break;

    case PHAC_DISCLOOP_CONFIG_GTBF_VALUE:
        *pValue = pDataParams->wGTbf;
        break;

    case PHAC_DISCLOOP_CONFIG_LOOP_STATE:
        *pValue = pDataParams->bLoopMode;
        break;

    case PHAC_DISCLOOP_CONFIG_BAIL_OUT:
        *pValue = pDataParams->bBailOut;
        break;

    case PHAC_DISCLOOP_CONFIG_DETECT_TAGS:
        *pValue = pDataParams->bDetectionConfiguration;
        break;

    case PHAC_DISCLOOP_CONFIG_STOP:
        *pValue = pDataParams->bStopFlag;
        break;

    case PHAC_DISCLOOP_CONFIG_ANTI_COLL:
        *pValue = pDataParams->bConColl;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEA_NR_TAGS_FOUND:
        *pValue = pDataParams->sTypeATargetInfo.bTotalTagsFound_A;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_NR_TAGS_ACTIVATED:
        *pValue = pDataParams->sTypeATargetInfo.bActivatedTagNumber_A;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_DEVICE_LIMIT:
        *pValue = pDataParams->sTypeATargetInfo.bDeviceLimit_A;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_POLL_LIMIT:
        *pValue = pDataParams->sTypeATargetInfo.bLoopLimit_A;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_FSDI:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_I3P4.bFsdi;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_CID:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_I3P4.bCid;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_DSI:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDsi;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_I3P4_DRI:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_I3P4.bDri;
        break;

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_I3P4_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_DID:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bDid;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_LRI:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bLri;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_NAD_ENABLE:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bNadEnable;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_NAD:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bNad;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_GI_LEN:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bGiLength;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEA_P2P_ATR_RES_LEN:
        *pValue = pDataParams->sTypeATargetInfo.sTypeA_P2P.bAtrResLength;
        break;

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_P2P_TAGS */

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEB_NR_TAGS_FOUND:
        *pValue = pDataParams->sTypeBTargetInfo.bTotalTagsFound_B;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_NR_TAGS_ACTIVATED:
        *pValue = pDataParams->sTypeBTargetInfo.bActivatedTagNumber_B;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_DEVICE_LIMIT:
        *pValue = pDataParams->sTypeBTargetInfo.bDeviceLimit_B;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEB_POLL_LIMIT:
        *pValue = pDataParams->sTypeBTargetInfo.bLoopLimit_B;
        break;


#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS
    case PHAC_DISCLOOP_CONFIG_TYPEF_NR_TAGS_FOUND:
        *pValue = pDataParams->sTypeFTargetInfo.bTotalTagsFound_F;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_NR_TAGS_ACTIVATED:
        *pValue = pDataParams->sTypeFTargetInfo.bActivatedTagNumber_F;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_DEVICE_LIMIT:
        *pValue = pDataParams->sTypeFTargetInfo.bDeviceLimit_F;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_POLL_LIMIT:
        *pValue = pDataParams->sTypeFTargetInfo.bLoopLimit_F;
        break;

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_DID:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bDid;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_LRI:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bLri;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD_ENABLE:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNadEnable;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_NAD:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bNad;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_GI_LEN:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bGiLength;
        break;

    case PHAC_DISCLOOP_CONFIG_TYPEF_P2P_ATR_RES_LEN:
        *pValue = pDataParams->sTypeFTargetInfo.sTypeF_P2P.bAtrResLength;
        break;

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_P2P_TAGS */

#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEF_TAGS */

    default:
        statusTmp = PH_ADD_COMPCODE(
                                   PH_ERR_INVALID_PARAMETER, 
                                   PH_COMP_AC_DISCLOOP
                                   );

    }

    return statusTmp;
}

phStatus_t phacDiscLoop_Sw_ActivateCard(
                                       phacDiscLoop_Sw_DataParams_t * pDataParams,
                                       uint8_t bTagType,
                                       uint8_t bTagIndex
                                       )
{
    phStatus_t status;

    switch (bTagType) {
#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS
    case PHAC_DISCLOOP_TYPEA_ACTIVATE:
        status = phacDiscLoop_Sw_Int_ActivateA(pDataParams, bTagIndex);
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEA_TAGS */

#ifdef NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS
    case PHAC_DISCLOOP_TYPEB_ACTIVATE:
        status = phacDiscLoop_Sw_Int_ActivateB(pDataParams, bTagIndex);
        break;
#endif /* NXPBUILD__PHAC_DISCLOOP_TYPEB_TAGS */

    default:
        status = PH_ADD_COMPCODE(
                                PH_ERR_INVALID_PARAMETER, 
                                PH_COMP_AC_DISCLOOP
                                );
    }

    return status;
}

    #ifdef __DEBUG
/* This API would be available only for debugging purpose */
phStatus_t phacDiscLoop_Sw_SetPtr(
                                 phacDiscLoop_Sw_DataParams_t * pDataParams,
                                 void * pI14443p3a,
                                 void * pI14443p4a,
                                 void * pI14443p3b,
                                 void * pFeilca,
                                 void * pI18092mPI
                                 )
{
    phStatus_t PH_MEMLOC_REM status;

    status = PH_ERR_SUCCESS;

    pDataParams->pPal1443p3aDataParams = pI14443p3a;
    pDataParams->pPal1443p4aDataParams = pI14443p4a;
    pDataParams->pPal1443p3bDataParams = pI14443p3b;
    pDataParams->pPalFelicaDataParams  = pFeilca;
    pDataParams->pPal18092mPIDataParams = pI18092mPI;

    return status;

}
    #endif /* __DEBUG */

/**************************************************************
* 
* º¯Êý£º
* ËµÃ÷£º
* ·µ»Ø£º±êÇ©¸öÊý+±êÇ©1+±êÇ©2+±êÇ©3
*       AÀà'A'+ATQ+SAK+UIDLen+uid+ATSLEN+ATS
*       BÀà±ð 'B'+ ATQBLEN + atqb + mbli
*       CÀà£º'C'+ATQfLEN + atqf ·µ»Ø
*       pwCardType:Ö»ÓÐÒ»ÕÅ¿¨µÄÊ±ºò£¬²ÅÓÐÐ§
***************************************************************/
phStatus_t phacDiscLoop_Sw_GetAts(phacDiscLoop_Sw_DataParams_t * pDataParams,uint8_t *pAts, uint16_t *pwAtsLen,
                                  uint16_t *pwCardType){
    unsigned char i;
    unsigned char bOffset = 0;
    //ÅÐ¶ÏA¿¨ÊÇ·ñ´æÔÚ¶à¸ö

    bOffset = 0;

    pAts[bOffset] = pDataParams->sTypeATargetInfo.bTotalTagsFound_A+//¿¨Æ¬µÄÊýÁ¿
                    pDataParams->sTypeBTargetInfo.bTotalTagsFound_B+
                    pDataParams->sTypeFTargetInfo.bTotalTagsFound_F;
    bOffset++;
    /******************AÀà±êÇ©ÅÐ¶Ï*****************/
    if (pDataParams->sTypeATargetInfo.bTotalTagsFound_A) {
        for (i = 0; i<pDataParams->sTypeATargetInfo.bTotalTagsFound_A; i++) {
            pAts[bOffset++] = 'A';
            memcpy(pAts+bOffset,pDataParams->sTypeATargetInfo.aTypeA_I3P3[i].aAtqa,2);//ATQ
            bOffset+=2;
            pAts[bOffset++] = pDataParams->sTypeATargetInfo.aTypeA_I3P3[i].aSak[0];//SAK
            pAts[bOffset++] = pDataParams->sTypeATargetInfo.aTypeA_I3P3[i].bUidSize;//uidlen
            memcpy(pAts+bOffset,pDataParams->sTypeATargetInfo.aTypeA_I3P3[i].aUid,//uidsize
                   pDataParams->sTypeATargetInfo.aTypeA_I3P3[i].bUidSize);
            bOffset+=pDataParams->sTypeATargetInfo.aTypeA_I3P3[i].bUidSize;
            pAts[bOffset++] = 0;//atsµÄ³¤¶ÈÎª0
        }
        if (pAts[0]==0x01) {
            if ((0==memcmp(pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa,"\x04\x00",2))&&
                (pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak[0]==0x08)) { //S50¿¨Æ¬

                pwCardType[0] = PICC_TagA_S50;
            } else if ((0==memcmp(pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa,"\x02\x00",2))&&
                       (pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak[0]==0x18)) {
                pwCardType[0] = PICC_TagA_S70;
            } else if (0==memcmp(pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aAtqa,"\x44\x00",2)) {
                pwCardType[0] = PICC_TagA_UL;
                //((bSak&0x24)==0x20)
            } else if (((pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak[0]&0x24)==0x20)&&
                       pDataParams->sTypeATargetInfo.bActivatedTagNumber_A) {
                pwCardType[0] = PICC_TagA_CPU ;
                bOffset--;
                pAts[bOffset++] = pDataParams->sTypeATargetInfo.sTypeA_I3P4.aAts[0];
                memcpy(&pAts[bOffset],pDataParams->sTypeATargetInfo.sTypeA_I3P4.aAts,pDataParams->sTypeATargetInfo.sTypeA_I3P4.aAts[0]);
                bOffset+=pDataParams->sTypeATargetInfo.sTypeA_I3P4.aAts[0];

            } else if (((pDataParams->sTypeATargetInfo.aTypeA_I3P3[0].aSak[0]&0X40)==0x40) &&//PICC_TagA_P2P¶øÇÒ±»¼¤»î
                       (pDataParams->sTypeATargetInfo.bActivatedTagNumber_A)) {
                pAts[bOffset++] = 0;//ATrµÄ³¤¶ÈÎª0
                pwCardType[0] = PICC_TagA_P2P;
            } else {
                pAts[bOffset++] = 0;//ATrµÄ³¤¶ÈÎª0
                pwCardType[0] = PICC_TagA_Unkown;
            }
        }
    }

    /******************BÀà±êÇ©ÅÐ¶Ï******************/
    if (pDataParams->sTypeBTargetInfo.bTotalTagsFound_B) {
        for (i = 0; i<pDataParams->sTypeBTargetInfo.bTotalTagsFound_B; i++) {
            pAts[bOffset++] = 'C';
            pAts[bOffset++] = pDataParams->sTypeBTargetInfo.aI3P3B[i].bAtqBLength+1;
            memcpy(pAts+bOffset,pDataParams->sTypeBTargetInfo.aI3P3B[i].aAtqB,
                   pDataParams->sTypeBTargetInfo.aI3P3B[i].bAtqBLength);//ATQ
            bOffset+=pDataParams->sTypeBTargetInfo.aI3P3B[i].bAtqBLength;
            pAts[bOffset++] = pDataParams->sTypeBTargetInfo.sI3P4B.bMbli<<4;
        }
        if (pAts[0]==0x01) {
            //Èç¹û¿¨Æ¬¼¤»îÁË£¬ÄÇÃ´¾ÍÊÇCPU¿¨
            if (pDataParams->sTypeBTargetInfo.bActivatedTagNumber_B)//Èç¹û¿¨Æ¬¼¤»î£¬ÄÇÃ´¾ÍÊÇ·ûºÏISO14443-4µÄ¿¨Æ¬
                pwCardType[0] = PICC_TagB_CPU;
            else
                pwCardType[0] = PICC_TagB_Unkown;
        }
    }
    /*********************FÀà±êÇ©ÅÐ¶Ï*******************/
    if (pDataParams->sTypeFTargetInfo.bTotalTagsFound_F) {
        for (i = 0; i<pDataParams->sTypeFTargetInfo.bTotalTagsFound_F; i++) {
            pAts[bOffset++] = 'F';
            pAts[bOffset++] = 16;
            memcpy(pAts+bOffset,pDataParams->sTypeFTargetInfo.aTypeF[i].aIDmPMm,16);//ATQ
            bOffset+=16;
        }

        if (pAts[0]==0x01) {
            //Èç¹û¿¨Æ¬¼¤»îÁË£¬ÄÇÃ´¾ÍÊÇCPU¿¨
            if (pDataParams->sTypeFTargetInfo.bActivatedTagNumber_F)//Èç¹û¿¨Æ¬¼¤»î£¬ÄÇÃ´¾ÍÊÇ·ûºÏISO14443-4µÄ¿¨Æ¬
                pwCardType[0] = PICC_TagF_P2P;
            else
                pwCardType[0] = PICC_TagF_Unkown;
        }
    }

    if (pAts[0]==0x00) {
        pwCardType[0] = PICC_Tag_NoCard;
    } else if (pAts[0]>0x01) {
        pwCardType[0] = PICC_Tag_Multi;
    }
    pwAtsLen[0] = bOffset;

    return 0;
}

#endif  /* NXPBUILD__PHAC_DISCLOOP_SW */


