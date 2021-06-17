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
* File Name    : isl9122.h
* Description  : Declaration of device accessor to the dcdc converter from RENESAS
**********************************************************************************************************************/

#include "config_mode.h"
#include "RE01_256KB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config_mode.h"
#include "configuration.h"
#include "system/inc/system.h"
#include "r_i2c_cmsis_api.h"
#include "r_lpm_api.h"

#include "isl9122.h"

static uint8_t read_buf[1] = {0}; // read buffer
static uint8_t snd_data[2]; 

extern ARM_DRIVER_I2C Driver_I2C0;


/**************************************************************************//**
* @brief  Output volatge setting request to ISL9122
*
* @retval       
******************************************************************************/
int isl9122_output_voltage_setting_request(float output_voltage)
{
  int ret = 0;
  uint8_t vset;
  
  if (output_voltage < 0)                      
  {  
    APP_ERR_RETURN(-1);
  }
   
  vset = (uint8_t) (output_voltage/0.025); 
  snd_data[0]= VSET_RESISTOR_ADDRESS;
  snd_data[1]= vset;

  if (vset > 215)                           /* 215=(max output voltage) 5.325/0.025 */
  {  
    APP_ERR_RETURN(-1);
  }
    
  ret = system_i2c_write(&Driver_I2C0, ISL9122_SLAVE_ADDRESS,  (uint8_t*)&snd_data[0], 2 , false );
  APP_ERR_RETURN(ret);
    
  return 0;
}

/**************************************************************************//**
* @brief  mask setting request to ISL9122
*
* @retval       
******************************************************************************/
int isl9122_intflg_mask_setting_request(uint8_t mask_value)
{
  int ret = 0;
   
  snd_data[0]= INTFLG_MASK_RESISTOR_ADDRESS;
  snd_data[1]= mask_value;
    
  ret = system_i2c_write(&Driver_I2C0, ISL9122_SLAVE_ADDRESS,  (uint8_t*)&snd_data[0], 2 , false );
  APP_ERR_RETURN(ret);
    
  return 0;
}

/**************************************************************************//**
* @brief  resistor data read from ISL9122
*
* @retval       
******************************************************************************/
int isl9122_resistor_read(isl9122_data_t * data, uint8_t read_resistor_address)
{
    int         ret = 0;
    
    snd_data[0] = read_resistor_address;  
    snd_data[1] = 0x00;  
    

    ret = system_i2c_write( &Driver_I2C0, ISL9122_SLAVE_ADDRESS, &(snd_data[0]), 1, true);
    APP_ERR_RETURN(ret);
//    system_time_wait_ms( 1 );
  
    ret = system_i2c_read( &Driver_I2C0, ISL9122_SLAVE_ADDRESS, &(read_buf[0]), 1 , false);
    APP_ERR_RETURN(ret);
    
      
    return 0;
}


