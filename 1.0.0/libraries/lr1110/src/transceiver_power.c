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
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "system/inc/system.h"
#include "lr1110_user_config_io.h"
#include "lr1110/lr1110_hal.h"
#include "lr1110/lr1110_system_types.h"
#include "lr1110/lr1110_system.h"

/***********************************************************************************************************************
*
* LR1110 Power Control;
*
***********************************************************************************************************************/
extern radio_t lr1110;


void lr1110_power_on(void)
{
    lr1110_hal_wakeup( &lr1110);
}

void lr1110_sleep_enter(uint32_t timeout)
{
    lr1110_system_sleep_cfg_t sleep_cfg;

    sleep_cfg.is_warm_start = 1;
    sleep_cfg.is_rtc_timeout  = 1;

    lr1110_system_set_sleep( &lr1110, sleep_cfg, timeout );
}

void lr1110_power_off(void)
{
    lr1110_system_sleep_cfg_t sleep_cfg;

    sleep_cfg.is_warm_start = 0;
    sleep_cfg.is_rtc_timeout  = 0;

    lr1110_system_set_sleep( &lr1110, sleep_cfg, 0 );
}

void lr1110_lna_on(void)
{
    system_gpio_init_output( LR1110_LNA_PORT, LR1110_LNA_PIN, 1 );
}

void lr1110_lna_off(void)
{
    system_gpio_init_output( LR1110_LNA_PORT, LR1110_LNA_PIN, 0 );
}

void lr1110_dcdc_on(void)
{
    system_gpio_init_output( DCDC_EN_PORT, DCDC_EN_PIN, 1 );
//    R_LPM_IOPowerSupplyModeSet(0xF2); // set VOCR for IOVCC0,IOVCC1,AVCCO power on
}

void lr1110_dcdc_off(void)
{
//    R_LPM_IOPowerSupplyModeSet(0xFF); // set VOCR for  IOVCC0,IOVCC1,AVCCO shutdown
    system_gpio_init_output( DCDC_EN_PORT, DCDC_EN_PIN, 0 );
}

