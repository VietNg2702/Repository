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

#ifndef ISL9122_H_
#define ISL9122_H_

#define ISL9122_SLAVE_ADDRESS           0x18
#define VSET_RESISTOR_ADDRESS           0x11
#define INTFLG_RESISTOR_ADDRESS         0x03
#define INTFLG_MASK_RESISTOR_ADDRESS    0x13

typedef struct isl9122_data
{
  //float temperature;
  //float humidity;
  uint8_t intflg;
  uint8_t intflg_mask;
}isl9122_data_t;
  
int isl9122_output_voltage_setting_request(float output_voltage);
int isl9122_intflg_mask_setting_request(uint8_t mask_value);
int isl9122_resistor_read(isl9122_data_t * data, uint8_t resistor_address);

#endif /* ISL9122_H_ */
