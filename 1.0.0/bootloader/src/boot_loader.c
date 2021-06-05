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
/***********************************************************************************************************************
 * File Name    : boot_loader.c
 * Description  : sample program of Firmware Update main program.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 28.12.2020 1.00    MP
 **********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "RE01_256KB.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"
#include "usart.h"

#include "r_flash_cfg.h"
#include "r_flash_re01_256kb.h"
#include "r_flash_api.h"
#include "R_Driver_USART.h"
#include "r_agt_api.h"

#include "r_fwup_api.h"

void main(void);
#if (FLASH_CFG_CODE_FLASH_RUN_FROM_ROM == 1)
void main_secure_boot_loop (void);
#else
void main_secure_boot_loop (void) __attribute__ ((section(".ramfunc")));
#endif
extern void my_sci_callback(uint32_t event);
extern SCI_BUFFER_CONTROL sci_buffer_control[2];

/* Handle storage. */

void main(void)
{
    R_SYS_CodeCopy();
    R_SYS_Initialize();
    R_LPM_Initialize();
    R_LPM_IOPowerSupplyModeSet(LPM_IOPOWER_SUPPLY_NONE);

    if (ARM_DRIVER_OK != USART_CH.Initialize(my_sci_callback))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (ARM_DRIVER_OK != USART_CH.PowerControl(ARM_POWER_FULL))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (ARM_DRIVER_OK != USART_CH.Control((ARM_USART_MODE_ASYNCHRONOUS |    /* UART (Asynchronous) */
                                                ARM_USART_DATA_BITS_8       |    /* 8 Data bits */
                                                ARM_USART_PARITY_NONE       |    /* No Parity */
                                                ARM_USART_STOP_BITS_1       |    /* 1 Stop bit */
                                                ARM_USART_FLOW_CONTROL_NONE)     /* No Flow Control */
                                               ,UART_BUS_SPEED))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    USART_CH.Control(ARM_USART_CONTROL_TX, 1);
    /* Enable Receiver and Transmitter lines */
    USART_CH.Control(ARM_USART_CONTROL_RX, 1);
    /* Start reception. */
    USART_CH.Receive(&sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_A], FWUP_WRITE_BLOCK_SIZE);

    main_secure_boot_loop();
}

void main_secure_boot_loop (void)
{
    int32_t result_secure_boot;

    while(1)
    {
       result_secure_boot = R_FWUP_SecureBoot();
       if (FWUP_SUCCESS == result_secure_boot)
       {
          R_FWUP_ExecuteFirmware();
          while(1); /* infinite loop */
       }
       else if (FWUP_FAIL == result_secure_boot)
       {
          while(1)
          {
             /* infinity loop */
          }
       }
       else if (FWUP_END_OF_LIFE == result_secure_boot)
       {
          while(1)
          {
             /* infinity loop */
          }
       }
       else if (FWUP_IN_PROGRESS == result_secure_boot)
       {
          continue;
       }
       else
       {
          while(1)
          {
             /* infinite loop */
          }
       }
    }
}

