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


#include "RE01_256KB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lr1110_user_config_mode.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"
#include "lib_additional.h"

#include "system/inc/system.h"
#include "lr1110/lr1110_types.h"
#include "lr1110/lr1110_system.h"
#include "lr1110/lr1110_system_types.h"
#include "lr1110/lr1110_radio.h"
#include "demo_configuration.h"
#include "demo_gnss_types.h"
#include "lr1110/lr1110_hal.h"
#include "lr1110/lr1110_gnss.h"
#include "lr1110/lr1110_gnss_types.h"
#include "r_spi_cmsis_api.h"
#include "transceiver_gnss.h"
#include "transceiver_radio.h"
#include "transceiver_almanac.h"
#include "transceiver_rtc.h"



/**********************************************************************
  RTC
**********************************************************************/

extern radio_t lr1110;

extern uint32_t last_rtc_adjust_second;
extern int8_t rtc_valid_flag;
extern char radio_data_buf[RADIO_DATA_BUFFER_SIZE+1];
extern UART_DRIVER_FIFO uart0_dev;
extern char uart_send_buf[];
extern char uart_receive_buf[];


int8_t read_validate_location(uint32_t last_adjust_count, char* date_buf, environment_location_t* p_location)
{
    int8_t valid_flag;
    uint32_t gps_second;
    delComma(date_buf);
    delCRLF(date_buf);
    sscanf(date_buf, "%u%f%f%f", &gps_second, &(p_location->altitude), &(p_location->latitude), &(p_location->longitude) );

    if ((gps_second < 1200000000) || 
        (p_location->latitude < -90 || p_location->latitude > 90)||  
        (p_location->longitude <-180 || p_location->longitude >180)||  
        (p_location->altitude <-100 )) 
    {
        valid_flag = false;
    } 
    else 
    {
        last_rtc_adjust_second = gps_second + (system_lptim_get() - last_adjust_count + 500) / 1000;
        rtc_set_current_binary_time (last_rtc_adjust_second);
        valid_flag = true;
        rtc_valid_flag = true;
    }
    return valid_flag;
}

void rtc_adjust_TX(environment_location_t* p_location_assist, int8_t* p_valid_flag)
{
    int8_t valid_flag;
    uint32_t last_adjust_count;
    int rtc_progress_second;

    valid_flag = *p_valid_flag;
    rtc_progress_second = (int32_t)rtc_read_current_binary_time() - (int32_t)last_rtc_adjust_second;

    if ( rtc_progress_second < RTC_ADJUST_INTERVAL && rtc_valid_flag == true) {
        *p_valid_flag = true;
        return ;
    }
    valid_flag = false;
    radio_init();
    semtech_radio_tx("!DATE\n", ACK_RECEIVE_INTERVAL);

    last_adjust_count = system_lptim_get();
    radio_data_buf[0] = 0;
    semtech_radio_rx(radio_data_buf, ACK_RECEIVE_INTERVAL);
    if (strncmp(radio_data_buf, "%DATE", 5) == 0) {
        valid_flag = read_validate_location(last_adjust_count, radio_data_buf+5, p_location_assist);
    } else if (strncmp(radio_data_buf, "DATE", 4) == 0) {
            valid_flag = read_validate_location(last_adjust_count, radio_data_buf+4, p_location_assist);
    } else {
        valid_flag = false;
    }
#if (DEBUG_LED == 1)
    system_time_wait_ms( 2000 );
    radio_data_buf[0] = '#';
    semtech_radio_tx( radio_data_buf, ACK_RECEIVE_INTERVAL);
    system_time_wait_ms( 2000 );
#endif
    *p_valid_flag = valid_flag;
    return ;
}


int8_t GetDateAndApproximateLocationFromPC( environment_location_t* p_location , uint32_t timeout)
{
    int8_t valid_flag;
    uint32_t timeout_count;
    uint32_t last_adjust_count;
    int16_t receive_len;
    char c;

    timeout_count = system_lptim_get() + timeout;
    uart_tx_fifo_wait_empty(&uart0_dev);
    uart_rx_fifo_flush(&uart0_dev);
    strcpy(uart_send_buf, "!DATE\n");
    uart_fifo_send_string(&uart0_dev, uart_send_buf);
    while(1) 
    {
        system_time_wait_ms( 10 );
        if (timeout !=0 && system_lptim_get() > timeout_count) 
        {
            valid_flag=false;
            return valid_flag;
        }
        last_adjust_count = system_lptim_get();
        receive_len = uart_fifo_receive_line(&uart0_dev, uart_receive_buf, 255 ,5000 );
        if (receive_len < 0) 
        {
            valid_flag=false;
            return valid_flag;
        }
        c=uart_receive_buf[0];
        if (c >= '0' && c <= '9') {break;}
    }
    sprintf(uart_send_buf, "#Date ZZ%sZZ\n",uart_receive_buf);
    uart_fifo_send_string(&uart0_dev, uart_send_buf);
    system_time_wait_ms( 100 );
    __NOP();
    valid_flag = read_validate_location(last_adjust_count, uart_receive_buf, p_location);

    return valid_flag;
}

