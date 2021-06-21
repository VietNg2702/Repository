/**********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MECHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2018-2019 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : battery.c
* Description  : Voltage readout for an energy storage device on the RTK70E017DC01000BJ board. 14-bit ADC on RE01 MCU
*                is used for the measurement.
**********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lr1110_user_config_mode.h"
#include "RE01_256KB.h"
#include "r_system_api.h"
#include "r_adc_api.h" 

extern DRIVER_S14AD Driver_S14AD;
DRIVER_S14AD *adcDev = &Driver_S14AD;              

/**************************************************************************//**
* @brief   Battery voltage data read
*
* @retval       
******************************************************************************/
void battery_read(uint16_t * val)
{
    st_adc_pins_t scanset_pin;
    st_adc_add_mode_t arg;              /* for AD_CMD_SET_ADD_MODE,     2nd parameter */
    uint8_t     autoc;                  /* for AD_CMD_SET_AUTO_CLEAR,   2nd parameter */
    st_adc_status_info_t result_status; /* for AD_CMD_GET_AD_STATE,     2nd parameter */
  
    /* Enable AVCC0 power domain to check the voltage of Secondary battery */
    SYSC->VOCR_b.AV0CTL = 0U;

    // ADC Modlue Start
    MSTP->MSTPCRD_b.MSTPD16 = 0U;
    
    /* Initialize VREF */
    MSTP->MSTPCRD_b.MSTPD17 = 0U;
    VREF->AVCR = 0x00040011;
    R_SYS_SoftwareDelay(50, SYSTEM_DELAY_UNITS_MILLISECONDS);

    /* Initialize S14AD */
    adcDev->Open(ADC_SINGLE_SCAN | ADC_14BIT | ADC_RIGHT, 0xFF, NULL);
    
    /* execute offset calibration */ 
    adcDev->Control(AD_CMD_CALIBRATION, NULL) ;
    
    /* channel select */
    scanset_pin.an_chans = ADC_MSEL_AN07 ;                      /* AN007 select */
    scanset_pin.sensor   = ADC_SENSOR_NOTUSE;                   /* not use of temp sensor */
    adcDev->ScanSet(ADC_GROUP_A, scanset_pin, ADC_TRIGER_SOFT); /* software trigger */
    
    
   /* settings of av/add mode */
    arg.add_mode       = ADC_ADD_AVG_4_SAMPLES;
    arg.chans.an_chans = ADC_MSEL_AN07;
    arg.chans.sensor   = ADC_SENSOR_NOTUSE;
    adcDev->Control(AD_CMD_SET_ADD_MODE, &arg);
    
    /* A/D data resistor, auto clear setting  */
    autoc = ADC_ENABLE;                                         
    adcDev->Control(AD_CMD_SET_AUTO_CLEAR, &autoc);  

    /* Start AD conversion */
    adcDev->Start(); 
    
    do
    {
       adcDev->Control(AD_CMD_GET_AD_STATE, &result_status);        /* get A/D status  */
    }
    while (ADC_STATE_RUN == result_status.ad_info);                /* wait A/D conversion end */

    adcDev->Read(ADC_SSEL_AN07, val);

    /* Disable S14AD */
    adcDev->Close();

    /* Disable VREF */
    VREF->AVCR = 0x00040000;   

    // VREF Modlue Stop
    MSTP->MSTPCRD_b.MSTPD17 = 1U;

    // ADC Modlue Stop
    MSTP->MSTPCRD_b.MSTPD16 = 1U;
    /* Disnable AVCC0 power domain  */
    SYSC->VOCR_b.AV0CTL = 1U;

    return;
}
