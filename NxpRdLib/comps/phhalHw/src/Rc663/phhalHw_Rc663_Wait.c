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
* Wait Routines for RC663 specific HAL-Component of Reader Library Framework.
* $Author: santosh.d.araballi $
* $Revision: 1.2 $
* $Date: Fri Apr 15 09:27:28 2011 $
*
* History:
*  CHu: Generated 19. May 2009
*
*/

#include <ph_Status.h>
#include <phbalReg.h>
#include <ph_RefDefs.h>

#ifdef NXPBUILD__PHHAL_HW_RC663

    #include "phhalHw_Rc663.h"
    #include "phhalHw_Rc663_Int.h"
    #include <phhalHw_Rc663_Reg.h>


    #include "ISO14443-4.h"

    #include "define.h"



extern ContextInfo ReaderISOPare;//读写器参数
extern unsigned char DebugPinErr();
/**************************************************************
* 函数：
* 参数：
* 说明：
* 
***************************************************************/
phStatus_t phhalHw_Rc663_WaitIrq(
                                phhalHw_Rc663_DataParams_t * pDataParams,
                                uint8_t bEnableIrqs,     //
                                uint8_t bWaitUntilPowerUp,
                                uint8_t bCheckErrCoLL,//是否时能冲突检测
                                uint8_t bCheckErrOther,//是否时能其他的错误检测
                                uint8_t bIrq0WaitFor,
                                uint8_t bIrq1WaitFor,
                                uint8_t * pIrq0Reg,  //返回的状态寄存器1
                                uint8_t * pIrq1Reg  //返回的状态寄存器2
                                )
{
    phStatus_t  PH_MEMLOC_REM status;
    phStatus_t  PH_MEMLOC_REM statusTmp;
    uint8_t     PH_MEMLOC_REM bIrq0EnReg;
    uint8_t     PH_MEMLOC_REM bIrq1EnReg;
    uint8_t     PH_MEMLOC_REM bRegister;
    uint8_t bError;

    uint16_t wTimeoutPrev;

    /* Parameter check */
    if (!bIrq0WaitFor && !bIrq1WaitFor) {
        return PH_ADD_COMPCODE(PH_ERR_INTERNAL_ERROR, PH_COMP_HAL);
    }

    /* Enable Irqs if requested */
    if (bEnableIrqs != PH_OFF) {
        /* Read IRQEn registers */
        //PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ0EN, &bIrq0EnReg));
        //PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ1EN, &bIrq1EnReg));
        bIrq0EnReg = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_IRQ0EN << 1) | 0x01U);
        bIrq1EnReg = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_IRQ1EN << 1) | 0x01U);

        /* Enable IRQ0 interrupt sources */
        bIrq0EnReg &= PHHAL_HW_RC663_BIT_IRQINV;
        bIrq0EnReg |= bIrq0WaitFor;

        // PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ0EN, bIrq0EnReg));
        SPI_RC663_Write((PHHAL_HW_RC663_REG_IRQ0EN << 1),bIrq0EnReg);

        /* Enable IRQ1 interrupt sources */
        bIrq1EnReg &= PHHAL_HW_RC663_BIT_IRQPUSHPULL;
        bIrq1EnReg |= PHHAL_HW_RC663_BIT_GLOBALIRQ | bIrq1WaitFor;
        //PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ1EN, bIrq1EnReg));
        SPI_RC663_Write((PHHAL_HW_RC663_REG_IRQ1EN << 1),bIrq1EnReg);


        //PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ1, &bRegister));
        bRegister = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_IRQ1 << 1) | 0x01U);

    }
    /* Else just init. the Irq values */
    else {
        bIrq0EnReg = bIrq0WaitFor;
        bIrq1EnReg = bIrq1WaitFor;
    }

    //启动软定时器，防止设备死机，解决电气测试死机的问题
//   SoftTimeStop(TIME_RF_OV);
//  if (bIrq0WaitFor==(PHHAL_HW_RC663_BIT_RXIRQ | PHHAL_HW_RC663_BIT_HIALERTIRQ | PHHAL_HW_RC663_BIT_IDLEIRQ | PHHAL_HW_RC663_BIT_EMDIRQ)) {
//      phhalHw_Rc663_GetConfig(pDataParams, PHHAL_HW_CONFIG_TIMEOUT_VALUE_US, &wTimeoutPrev);
//      SoftTimeStart(TIME_RF_OV,(wTimeoutPrev/10000)+2);
//  }
    /* wait until an IRQ occurs */
    do {
        /* INSERT CODE HERE */
        /* IMPORTANT: Do not clear GlobalIRQ bit in IRQ1 register since it is needed below! */

        /* Wait until power up mode */
        if (bWaitUntilPowerUp != PH_OFF) {
            /* Read command register */
            //status = phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_COMMAND, &bRegister);
            bRegister = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_COMMAND << 1) | 0x01U);

            /* Ignore timeout error */
            if ((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT) {
                /* Status check */
                PH_CHECK_SUCCESS(status);

                /* Only read IRQ register if not in standby mode */
                if (!(bRegister & PHHAL_HW_RC663_BIT_STANDBY)) {
                    //status = phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ1, &bRegister);

                    bRegister = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_IRQ1 << 1) | 0x01U);

                    /* Ignore timeout error */
                    if ((status & PH_ERR_MASK) != PH_ERR_IO_TIMEOUT) {
                        PH_CHECK_SUCCESS(status);
                    }
                } else {
                    bRegister = 0x00;
                }
            } else {
                bRegister = 0x00;
            }
        }
        /* Always read IRQ register */
        else {
            // PH_CHECK_SUCCESS_FCT(statusTmp, phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ1, &bRegister));
            bRegister = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_IRQ1 << 1) | 0x01U);
            if (bCheckErrCoLL==PH_ON) {
                if (ReaderISOPare.TypeAFlag==1) { //读错误寄存器
                    //读错误寄存器
                    //phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_ERROR, &bError);//
                    bError = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_ERROR << 1) | 0x01U);
                    if ((bError & PHHAL_HW_RC663_BIT_COLLDET)) {
                        return PH_ERR_COLLISION_ERROR;
                    }
                }
            }
            
            if (bCheckErrOther==PH_ON) {
                // phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_ERROR, &bError);//
                bError = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_ERROR << 1) | 0x01U);
                if ((bError & 0x1b)) {
                    return PH_ERR_FRAMING_ERROR;
                }
            }



        }
    }
    while (!(bRegister & PHHAL_HW_RC663_BIT_GLOBALIRQ));

    /* Leave standby mode by terminating the command */
    if (bWaitUntilPowerUp != PH_OFF) {
        do {
            status = phhalHw_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE);
        }
        while ((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT);
    }

    /* Clear IRQ0 interrupt sources */
    bIrq0EnReg &= PHHAL_HW_RC663_BIT_IRQINV;
    //phhalHw_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ0EN, bIrq0EnReg);
    SPI_RC663_Write((PHHAL_HW_RC663_REG_IRQ0EN << 1),bIrq0EnReg);

    /* Clear IRQ1 interrupt sources */
    bIrq1EnReg &= PHHAL_HW_RC663_BIT_IRQPUSHPULL;
    //phhalHw_WriteRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ1EN, bIrq1EnReg);
    SPI_RC663_Write((PHHAL_HW_RC663_REG_IRQ1EN << 1),bIrq1EnReg);

    /* return IRQ1 status */
    if (pIrq1Reg) {
        *pIrq1Reg = bRegister;
    }

    /* return Irq0 status */
    if (pIrq0Reg) {
        //phhalHw_ReadRegister(pDataParams, PHHAL_HW_RC663_REG_IRQ0, pIrq0Reg);
        *pIrq0Reg = SPI_RC663_Read( (uint8_t)(PHHAL_HW_RC663_REG_IRQ0 << 1) | 0x01U);
    }

    return PH_ADD_COMPCODE(PH_ERR_SUCCESS, PH_COMP_HAL);
}

#endif /* NXPBUILD__PHHAL_HW_RC663 */
