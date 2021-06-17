/**
 * @file      transceiver_almanac.c
 *
 *
 * Revised BSD License
 * Copyright Semtech Corporation 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "RE01_256KB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "config_mode.h"
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



/**********************************************************************

   Semtech Almanac 

**********************************************************************/



extern uint8_t almanac_data[];
extern char radio_data_buf[];
extern UART_DRIVER_FIFO uart0_dev;
extern char uart_send_buf[];
extern char uart_receive_buf[];
extern radio_t lr1110;


uint16_t GetAlmanacAgeFromGpsEpochInDays( uint16_t almanac_age )
{
    // The almanac_age is the number of days since last GPS week number rollover
    // As we are in year 2020, our last rollover was on April 7th 2019. The next
    // one will be around 2038. So if you are reading this code because you
    // suspect mistake in the almanac age handling, and the current year is >
    // 2038, consider revisiting this piece of code...
    // (and now I'm a 50 year old guy... Greetings from the past ;) )

    // Let's convert in the number of days elapsed since GPS epoch (January
    // 6th 1980)
    uint16_t days_between_2019_rollover_to_epoch = 14336;
    uint16_t almanac_age_days_since_gps_epoch;

    almanac_age_days_since_gps_epoch    = almanac_age + days_between_2019_rollover_to_epoch;
__NOP();
    return almanac_age_days_since_gps_epoch;
}


uint16_t GetAlmanacAgeDaysSinceToday( )
{
    // First, we get the age of the almanac of the satellite id 0
    uint16_t almanac_age_since_epoch;
    uint32_t now_seconds;
    uint16_t now_days;
    uint16_t almanac_age_since_update;
    uint16_t almanac_age;
    
    almanac_age = 0;

    lr1110_gnss_get_almanac_age_for_satellite( &lr1110, 0, &almanac_age );

    // Then we convert the age from almanac into number of days elapsed since
    // GPS epoch
    almanac_age_since_epoch = GetAlmanacAgeFromGpsEpochInDays( almanac_age );

    // We get the actual time in days
    
    now_seconds = rtc_read_current_binary_time();
    
    now_days    = now_seconds / 3600 / 24;

    almanac_age_since_update = now_days - almanac_age_since_epoch;
__NOP();
    return almanac_age_since_update;
}



int8_t almanac_update_TX()
{
    int8_t result;
    uint16_t almanac_age_days;
    uint16_t receive_length;
    int16_t ic;

    almanac_age_days = GetAlmanacAgeDaysSinceToday();

    result = 0;
#if (FORCE_ALMANAC_UPDATE != 1)
    if (almanac_age_days < ALMANAC_DAYS_LIMIT) {
        goto Skip2;
    }
#endif
    system_time_wait_ms( 2000 );

    radio_init();
    semtech_radio_tx("!ALMA\n", ACK_RECEIVE_INTERVAL);
    radio_data_buf[0] = 0;
    semtech_radio_rx(radio_data_buf, 3000);
    if (strncmp(radio_data_buf, "%ALMA1", 6) != 0) {
       result = -1; goto Skip;  // Can't receive Almanac Data
    }

    result = semtech_radio_rx_len(almanac_data, &receive_length, ALMANAC_DATA_LENGTH + PAYLOAD_LENGTH, 60000);
    if (result != 0) {
   __NOP();
        goto Skip;
    }
    __NOP();

#if (USE_ALMANAC_UPDATE == 1)
    for (ic=0; ic < ALMANAC_DATA_LENGTH / ALMANAC_DATA_UNIT_LENGTH ; ic++ ) {
#if (DEBUG_LED == 1)
        R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
        PFS->P210PFS = 0x04 | (ic & 1);
        PFS->P410PFS = 0x04 | (~ic & 1);
#endif
        lr1110_gnss_one_satellite_almanac_update(&lr1110, &almanac_data[ic * ALMANAC_DATA_UNIT_LENGTH]);  
        system_time_wait_ms( 100 );
    }
#endif

Skip:
    __NOP();

#if (DEBUG_LED == 1)
    system_time_wait_ms( 2000 );
    if (result == 0) {
        sprintf(radio_data_buf, "#ALMANAC Receive %huZZ\n\n", receive_length);
    } else {
        strcpy(radio_data_buf, "#ALMANAC Receive Failed\n\n");
    }
    semtech_radio_tx(radio_data_buf, ACK_RECEIVE_INTERVAL);
    system_time_wait_ms( 2000 );
#endif

Skip2:
    if (result == 0) {return true;}
    else {return false;}
}


int8_t get_AlmanacData_from_PC(uint8_t data_buffer[], uint32_t timeout)
{
    int8_t result;
    int16_t ic, jc;
    int16_t receive_len;
    uint32_t last_agt_count;
    uint8_t almanac_responce[] = "ACK_OK";
    uint8_t *dpos, *spos;
    uint8_t c;

    last_agt_count = system_lptim_get();
    uart_tx_fifo_wait_empty(&uart0_dev);
    uart_rx_fifo_flush(&uart0_dev);
    strcpy(uart_send_buf, "!ALMA\n");
    result = uart_fifo_send_string(&uart0_dev, uart_send_buf);
    if (result !=0) {goto Skip;}
    system_time_wait_ms( 10 );

    dpos = &data_buffer[0];
    result = 0;
    for (ic=0; ic< ALMANAC_DATA_LENGTH / ALMANAC_UART_TRANS_LENGTH  ; ic++) {
        system_time_wait_ms( 10 );
        last_agt_count = system_lptim_get();
        for (; ;) {
            if (system_lptim_get() - last_agt_count > timeout) {
__NOP();
                result = -1;
                goto Skip; // Timeout
            }
            uart_receive_buf[0]=0;
            receive_len = uart_fifo_receive_line(&uart0_dev, uart_receive_buf, 255 ,timeout );
            if (receive_len == -1) {
                result = -1;
                goto Skip; // Timeout
            }
            c=uart_receive_buf[0];
            if (c != 0 && c != 'r' && c!= '\n') {
                break;
            }
        }
        spos=&uart_receive_buf[8];
        for (jc=0; jc < ALMANAC_UART_TRANS_LENGTH ;jc++) {
            if (isalnum(*spos) ==0 || isalnum(*(spos+1) ==0)) {
                __NOP();
                result = -1;
                goto Skip; // Read Error
            }
            *dpos=readHEX8(spos);
            dpos++; spos+=2;
        }
        result = uart_fifo_send_start(&uart0_dev, almanac_responce, strlen(almanac_responce));
        if (result !=0) {goto Skip;}

        __NOP();
    }
    system_time_wait_ms( 500 );
Skip:
    if (result ==0) {return true;}
    else {return false;}
}

