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
* File Name    : battery.h
* Description  : Declaration of the voltage readout function for an energy storage device
**********************************************************************************************************************/

#ifndef BATTERY_H_
#define BATTERY_H_

//#define BATT_VOLTAGE_MEAS_THRESHOLD  (0x2E7CU) /* 2.415V (11900point) */   
//#define BATT_VOLTAGE_MEAS_THRESHOLD  (0x2C45U) /* 2.300V (11333point) */
//#define BATT_VOLTAGE_MEAS_THRESHOLD  (0x2C45U) /* 2.300V (11333point) */
#define BATT_VOLTAGE_MEAS_THRESHOLD  (0x22A5U) /* 1.800V (8869point) */

void battery_read(uint16_t * val);

#endif /* BATTERY_H_ */
