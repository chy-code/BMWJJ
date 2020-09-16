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
* $Revision: 1.11 $
* $Date: Mon Dec  3 09:40:16 2012 $
*
* History:
*  PC: Generated 23. Aug 2012
*
*/
#ifndef PHACDICLOOP_SW_H
#ifdef NXPBUILD__PHAC_DISCLOOP_SW

/**
 * \brief    starts the discovery loop
 * 
 * \return  Error code
 * 
 * \retval  #PH_ERR_SUCCESS
 * \retval  Other error code returned by the underlaying
 *          component
 */
phStatus_t phacDiscLoop_Sw_Start(
    phacDiscLoop_Sw_DataParams_t * pDataParams     
    );



/**
 * \brief   Get the configuration value 
 *
 * \return  Error code
 *
 * \retval #PH_ERR_SUCCESS         operation successful
 * \retval #PH_ERR_PROTOCOL_ERROR  Invalid option/response received 
 * \retval other                   Other errors depending on underlaying component
 *
 */

phStatus_t phacDiscLoop_Sw_GetConfig( 
    phacDiscLoop_Sw_DataParams_t *  pDataParams,                               /**< [In] Pointer to this layer's parameter structure */
    uint16_t wConfig,                                                          /**< [In] Configuration identifier */
    uint16_t *pValue                                                           /**< [Out] Configuration value */
    );

/**
 * \brief   Set the configuration value 
 *
 * \return  Error code
 *
 * \retval #PH_ERR_SUCCESS            operation successful
 * \retval #PH_ERR_INVALID_PARAMETER  Invalid option/response received 
 * \retval other                      Other errors depending on underlaying component
 *
 */

phStatus_t phacDiscLoop_Sw_SetConfig(
    phacDiscLoop_Sw_DataParams_t *  pDataParams,                               /**< [In] Pointer to this layer's parameter structure */
    uint16_t wConfig,                                                          /**< [In] Configuration identifier */
    uint16_t wValue                                                            /**< [In] Configuration value */
    );

/**
 * \brief    Activates the given tag type with given index 
 *
 * \return   Error code
 * \retval   #PH_ERR_SUCCESS      On success
 * \retval   #PH_ERR_INVALID_ARG  If inavlid index or card type was given
 *
 */

phStatus_t phacDiscLoop_Sw_ActivateCard(
    phacDiscLoop_Sw_DataParams_t *  pDataParams,                               /**< [In] Pointer to this layer's parameter structure */
    uint8_t bTagType,                                                          /**< [In] Tag type could be Type A,B or F */
    uint8_t bTagIndex                                                          /**< [In] The tag which has to be activated */
    );

#ifdef __DEBUG
/**
 * \brief   Assigns the component data paramter to one of the filed in 
 *          discover loop parameter based on the option
 *
 * \return  Error code
 * \retval  #PH_ERR_INVALID_PARAMTER  If invalid option was passed
 * \retval  #PH_ERR_SUCCESS           on success
 *
 */

phStatus_t phacDiscLoop_Sw_SetPtr(
    phacDiscLoop_Sw_DataParams_t * pDataParams,
    void * pI14443p3a,                                                 
    void * pI14443p4a,
    void * pI14443p3b,
    void * pFeilca,
    void * pI18092mPI);

#endif /* #if __DEBUG */


phStatus_t phacDiscLoop_Sw_GetAts(phacDiscLoop_Sw_DataParams_t * pDataParams,uint8_t *pAts, uint16_t *pwAtsLen,
                                  uint16_t *pwCardType);

#endif /* NXPBUILD__PHAC_DISCLOOP_SW */
#endif /* PHACDICLOOP_H */

