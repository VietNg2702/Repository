/**********************************************************************************************************************
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
* Copyright (C) 2018-2019 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : R_Driver_I2C.h
* Version      : 0.40
* Description  : CMSIS-Driver for I2C.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2019 0.40     Based on RE01 1500KB
**********************************************************************************************************************/

#ifndef R_DRIVER_I2C_H
#define R_DRIVER_I2C_H
/******************************************************************************************************************//**
 * @addtogroup grp_cmsis_drv_impl_iic
 * @{
 *********************************************************************************************************************/

/******************************************************************************
 Includes <System Includes> , "Project Includes"
 *****************************************************************************/
#ifdef  __cplusplus
extern "C"
{
#endif
#include "Driver_I2C.h"

/******************************************************************************
 Macro definitions
 *****************************************************************************/
/****** I2C Event for renesas original *****/
#define ARM_I2C_EVENT_UND_ERROR       (1UL << 31) ///< Undefined error occurred

/****** Do not output address *****/
#define RIIC_ADDR_NONE              (0xFFFFFFFF) ///< It does not output address

#ifdef  __cplusplus
}
#endif

/*******************************************************************************************************************//**
 * @} (end addtogroup grp_cmsis_drv_impl_iic)
 **********************************************************************************************************************/
#endif /** R_DRIVER_I2C_H */
/* End of file (R_Driver_I2C.h) */

