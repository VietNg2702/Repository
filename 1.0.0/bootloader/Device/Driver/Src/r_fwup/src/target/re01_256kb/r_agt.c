/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_agt.c
* Version      : 1.00
* Description  : AGT Driver.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.12.2020 1.00     Initial Release
**********************************************************************************************************************/


#include "RE01_256KB.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"
#include "r_agt_api.h"

extern void fwup_state_monitoring_callback(void);

/********************************************************************************************************
* Function Name: agt1_open
* Description  : This function sets AGT1 to module stop release state and resource looking state.
* Arguments    : none
* Return Value : AGT_SUCCESS
*              : AGT_ERROR
*********************************************************************************************************/
agt_err_t agt1_open(void)
{
    if (0 != R_SYS_ResourceLock(SYSTEM_LOCK_AGT1))
    {
        return AGT_ERROR;
    }
    if (0 != R_LPM_ModuleStart(LPM_MSTP_AGT1))
    {
        return AGT_ERROR;
    }

    return AGT_SUCCESS;
} /* End of function agt1_open */

/********************************************************************************************************
* Function Name: agt1_start
* Description  : This function starts counting the timer. When count specified in argument ends, an interrupt
                 occurs.
* Arguments    : uint32_t time - Set the count time until an interrupt is generated (unit 10ms).
* Return Value : AGT_SUCCESS
*              : AGT_ERROR
*********************************************************************************************************/
agt_err_t agt1_start(uint32_t time)
{
    uint32_t pclkb_freq_hz;
    uint32_t period_counts;
    IRQn_Type irq = SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTI;

    /* Stop AGT1 count */
    /*----------------------------------------------------------------------
     * AGTCR register - AGT Control Register
     * b1 TCSTF -[0] Counting is stopped.
     * b0 TSTART -[0] Stop counting.
     *--------------------------------------------------------------------*/
    AGT1->AGTCR = 0x00U;
    while(AGT1->AGTCR_b.TSTART)
    {
        /* wait */
    }
    while(AGT1->AGTCR_b.TCSTF)
    {
        /* wait */
    }

    /* The CKS bit must be set to 000 before changing the TCK bit. */
    /*----------------------------------------------------------------------
     * AGTMR2 register -AGT Mode Register 2
     * b7 LPM -[0] Normal mode.
     * b2:b0 CKS[2:0] -[000] 1/1: AGTSCLK/AGTLCLK Count Source Clock Frequency Division Ratio.
     *--------------------------------------------------------------------*/
     AGT1 ->AGTMR2 = 0x00U;

    /*----------------------------------------------------------------------
     * AGTMR1 register -AGT Mode Register 1
     * b7 LPM -[0] Normal mode.
     * b2:b0 CKS[2:0] -[000] 1/1: AGTSCLK/AGTLCLK Count Source Clock Frequency Division Ratio.
     *--------------------------------------------------------------------*/
     AGT1 ->AGTMR1_b.TMOD = 0U;

    /* Get the pclkb frequency in Hz to set count source and value. */
    R_SYS_PeripheralClockFreqGet(&pclkb_freq_hz);
    if (pclkb_freq_hz < 4000000)
    {
        /*----------------------------------------------------------------------
         * AGTMR1 register -AGT Mode Register 1
         * b6:b4 TCK[2:0] -[001] PCLKB/8
         *--------------------------------------------------------------------*/
        AGT1 ->AGTMR1_b.TCK = 0x01U; /* Set PCLKB/8 as count source */

        /* Calculate the perod counts */
        period_counts = (time * 10) / 1000 * (pclkb_freq_hz / 8);
    }
    else
    {
        /*----------------------------------------------------------------------
         * AGTMR1 register -AGT Mode Register 1
         * b6:b4 TCK[2:0] -[000] PCLKB/1
         *--------------------------------------------------------------------*/
        AGT1 ->AGTMR1_b.TCK = 0x00U; /* Set PCLKB/1 as count source */

        /* Calculate the perod counts */
        period_counts = (time * 10) / 1000 * (pclkb_freq_hz / 1);
    }

    /* ---------------------------------------------------------------------
     * AGT register -AGT Counter Register
     * b15:b0 -16 -bit counter and reload register.
     *--------------------------------------------------------------------*/
    AGT1 ->AGT = (period_counts -1); /* Set count number. */

    /* AGT1 interrupt setting */
    if (0 != R_SYS_IrqStatusClear(irq)) /* Clear NVIC flag */
    {
        return AGT_ERROR;
    }

    R_NVIC_ClearPendingIRQ(irq);                      /* Clear NVIC pending status */
    R_NVIC_SetPriority(irq, AGT1_INTERRUPT_PRIORITY); /* Set interrupt priority */
    R_NVIC_EnableIRQ(irq);                            /* Enable NVIC */

    /* Connect NVIC with ICU, and Set callback function */
    if (0 != R_SYS_IrqEventLinkSet(irq, MAIN_PRV_GROUP0_EVENT_AGT1_AGTI, &fwup_state_monitoring_callback))
    {
        return AGT_ERROR;
    }

    /* Start AGT1 count */
    /* ----------------------------------------------------------------------
     * AGTCR register -AGT Control Register
     * b0 TSTART -[1] Start counting.
     *---------------------------------------------------------------------*/
    AGT1 ->AGTCR = 0x01U;

    return AGT_SUCCESS;
} /* End of function agt1_start */


/********************************************************************************************************
* Function Name: agt1_stop
* Description : This function stops counting the timer.
* Arguments    : none
* Return Value : AGT_SUCCESS
*              : AGT_ERROR
*********************************************************************************************************/
agt_err_t agt1_stop(void)
{
    IRQn_Type irq = SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTI;

    /* Stop AGT1 count */
    /*------------------------------------------------------------------------
     * AGTCR register -AGT Control Register
     * b0 TSTART -[0] Stop counting.
     *----------------------------------------------------------------------*/
    AGT1 ->AGTCR = 0x00U;

    R_NVIC_DisableIRQ(irq); /* Disable NVIC */
    if (0 != R_SYS_IrqStatusClear(irq)) /* Clear NVIC flag */
    {
        return AGT_ERROR;
    }
    R_NVIC_ClearPendingIRQ(irq); /* Clear NVIC pending status */

    return AGT_SUCCESS;
} /* End of function agt1_stop */


/********************************************************************************************************
* Function Name: agt1_close
* Description : This function sets AGT1 to module stop state and resource unlocking state.
* Arguments    : none
* Return Value : AGT_SUCCESS
*              : AGT_ERROR
*********************************************************************************************************/
agt_err_t agt1_close(void)
{
    if (0 != R_LPM_ModuleStop(LPM_MSTP_AGT1))
    {
        return AGT_ERROR;
    }
    R_SYS_ResourceUnlock(SYSTEM_LOCK_AGT1);

    return AGT_SUCCESS;
} /* End of function agt1_close */

