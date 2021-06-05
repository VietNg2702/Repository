/*******************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only
 * intended for use with Renesas products. No other uses are authorized. This
 * software is owned by Renesas Electronics Corporation and is protected under
 * all applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 * LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 * TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 * ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 * ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software
 * and to discontinue the availability of this software. By using this software,
 * you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ******************************************************************************/
/******************************************************************************
 * File Name    : r_agt_api.h
 * Description  : Interface file for AGT Driver
 ******************************************************************************/
/******************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 28.12.2020 1.00     First Release
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

/* Used to get which MCU is currently being used. */
/* User specific options for Flash API */

#ifndef AGT_API_H
#define AGT_API_H

/******************************************************************************
 Macro definitions
 ******************************************************************************/
 
#define AGT1_INTERRUPT_PRIORITY (2)

#define MAIN_PRV_GROUP0_EVENT_AGT1_AGTI (0x6)

#define CHECK_STATUS_INTERVAL   (1)
#define CHECK_STATUS_COUNT      (6000)

/*******************************************************************************
 Typedef definitions
 ******************************************************************************/

/*******************************************************************************
 Exported global variables
 ******************************************************************************/

typedef enum e_get_err
{
    AGT_SUCCESS = 0,
    AGT_ERROR
} agt_err_t;

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
agt_err_t agt1_open(void);
agt_err_t agt1_start(uint32_t time);
agt_err_t agt1_stop(void);
agt_err_t agt1_close(void);

#endif  /* AGT_API_H */
/******************************************************************************
 End  Of File
 ******************************************************************************/

