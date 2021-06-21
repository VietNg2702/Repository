/**
 * @file      transceiver_gnss.c
 *
 * @brief     Implementation of the Packet Error Rate demonstration.
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
#include "system/inc/system.h"
#include "transceiver_gnss.h"
#include "transceiver_power.h"



uint8_t nav_message[GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH + 1];
uint16_t nav_message_size;
static uint8_t nb_sat;

environment_location_t location_assist;

extern char uart_send_buf[257];
extern char uart_receive_buf[257];
static uint32_t delay_since_capture;
lr1110_gnss_timings_t timings;

extern radio_t lr1110;

extern UART_DRIVER_FIFO uart0_dev;

static lr1110_system_irq_mask_t irq_mask_test = LR1110_SYSTEM_IRQ_NONE;

uint32_t last_rtc_adjust_second;


int8_t get_results_from_host(char* result_buf, uint32_t timeout)
{
    int16_t ic;
    int8_t result;
    uint32_t last_agt_count;

    *result_buf=0;
    result = 0;
    last_agt_count = system_lptim_get();
//    timeout_count = system_lptim_get() + timeout;
#if    (SEND_TO_SERVER == 1)
    strcpy(uart_send_buf, "!SEND\n");
    __NOP();
    uart_fifo_send_string(&uart0_dev, uart_send_buf);
    __NOP();
    system_time_wait_ms( 500 );
    strcpy(uart_send_buf, "\n!RESULT\n");
    uart_fifo_send_string(&uart0_dev, uart_send_buf);
    system_time_wait_ms( 100 );
    for (ic=0; ; ic++) {
        uart_fifo_receive_line(&uart0_dev, result_buf, 255, timeout );
        if ( (last_agt_count + timeout - system_lptim_get() ) > 0) {
            result = -1; break;
        }
//      if (agt0_global_count >= timeout_count ) { }
        if (*result_buf != 0) {result = 0; break;}
//      delCRLF(receive_buf);
        system_time_wait_ms( 100 );
    }
#endif
    return result;
}




int8_t GetResults( environment_location_t* location, float *accuracy,  char *geo_coding1, char *geo_coding2 )
{
    int16_t ic;

    strcpy(uart_send_buf, "!RESULT\n");
    uart_tx_fifo_wait_empty(&uart0_dev);
    uart_rx_fifo_flush(&uart0_dev);
    uart_fifo_send_string(&uart0_dev, uart_send_buf);
    for (ic=0; ; ic++) {
        if (ic == 100) {
            printf("UART Timeout");
            while (1);
        }
        uart_fifo_receive_line(&uart0_dev, uart_receive_buf, 255 ,10000);
        delCRLF(uart_receive_buf);
        if (*uart_receive_buf != 0) {break;}
        system_time_wait_ms( 100 );
    }
    delCommaSemicolon(uart_receive_buf);
    
    return true;
}


int8_t semtech_event_wait(uint16_t timeout)
{
    static uint32_t last_agt_count;
    static int8_t irq3_event_flag;

    irq3_event_flag=0;
    last_agt_count = system_lptim_get();
#if (1 == INHIBIT_SSTBY || TRACKER_TX == 0)
    for (;;) {
        irq3_event_flag = system_gpio_get_pin_state(lr1110.irq);
        if (irq3_event_flag == 1) {
            break;
        }
        if (timeout !=0 && ((int32_t)system_lptim_get() - (int32_t)last_agt_count > (int32_t)timeout)) {
            return -1; // Timeout
        }
    }
#else
    if (timeout !=0) {
        system_int_timer_set_and_run((uint16_t)timeout);
    }
    system_lpm_setup_agt();

    system_lpm_wait_agt(); // SSTBY wait
    system_int_timer_stop();
    system_time_wait_ms( 1 );
    irq3_event_flag = system_gpio_get_pin_state(lr1110.irq);
    if (irq3_event_flag == 0) {
        return -1; // Timeout
    }
#endif
    return 0;
}


int8_t semtech_GNSS_remote( char* gnss_result_buf, int8_t use_assist_mode , uint16_t *p_radio_tx_len)
{
    int8_t result;
    static lr1110_gnss_solver_assistance_position_t gnss_position;
    static lr1110_gnss_timings_t timings;
    uint32_t last_agt_count;
    uint8_t    inter_capture_delay_second;
    lr1110_status_t  modem_response_code;
    modem_response_code = LR1110_STATUS_OK;

    lr1110_system_set_dio_irq_params(&lr1110,
                                    LR1110_SYSTEM_IRQ_GNSS_SCAN_DONE | 
                                    LR1110_SYSTEM_IRQ_TX_DONE | 
                                    LR1110_SYSTEM_IRQ_RX_DONE | 
                                    LR1110_SYSTEM_IRQ_TIMEOUT, 
                                    0 );

    lr1110_gnss_set_constellations_to_use( &lr1110, LR1110_GNSS_GPS_MASK | LR1110_GNSS_BEIDOU_MASK );

    lr1110_gnss_set_scan_mode(&lr1110, DEMO_GNSS_SINGLE_SCAN_MODE, &inter_capture_delay_second );

    system_time_wait_ms( 10 );

    lr1110_lna_on();

    lr1110_system_clear_irq_status( &lr1110, LR1110_SYSTEM_IRQ_ALL_MASK );

    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);

#if (DEBUG_LED == 1)
R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
PFS->P410PFS = 0x04 | 0x00;
PFS->P210PFS = 0x04 | 0x00;
#endif

    if (use_assist_mode == true) 
    {

        gnss_position.latitude = location_assist.latitude;
        gnss_position.longitude = location_assist.longitude;

        lr1110_gnss_set_assistance_position( &lr1110, &gnss_position );
        last_agt_count = system_lptim_get();
        lr1110_gnss_scan_assisted( &lr1110,
                                    rtc_read_current_binary_time(),
                                    LR1110_GNSS_OPTION_DEFAULT,
                                    LR1110_GNSS_BIT_CHANGE_MASK | LR1110_GNSS_DOPPLER_MASK | LR1110_GNSS_IRQ_PSEUDO_RANGE_MASK,
                                    DEMO_GNSS_ASSISTED_N_SATELLLITE_DEFAULT );
    } 
    else 
    {

        last_agt_count = system_lptim_get();
        lr1110_gnss_scan_autonomous(&lr1110,
                                    rtc_read_current_binary_time() ,
                                    LR1110_GNSS_OPTION_BEST_EFFORT,
                                    LR1110_GNSS_BIT_CHANGE_MASK | LR1110_GNSS_DOPPLER_MASK | LR1110_GNSS_IRQ_PSEUDO_RANGE_MASK,
                                    DEMO_GNSS_AUTONOMOUS_N_SATELLLITE_DEFAULT );
    }
    last_agt_count = system_lptim_get();

    result = semtech_event_wait(GNSS_WAIT_INTERVAL);
    if (result != 0) {return result;}

#if (DEBUG_LED == 1)
R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
PFS->P410PFS = 0x04 | 0x01;
PFS->P210PFS = 0x04 | 0x01;
#endif

    last_agt_count = system_lptim_get();
    lr1110_lna_off();

    lr1110_system_get_irq_status( &lr1110, &irq_mask_test );
    lr1110_system_clear_irq_status( &lr1110, LR1110_SYSTEM_IRQ_ALL_MASK );
    lr1110_gnss_get_timings( &lr1110, &timings );
    lr1110_gnss_get_nb_detected_satellites(&lr1110,  &nb_sat );
    lr1110_gnss_get_result_size( &lr1110, &nav_message_size );

    if( nav_message_size <= GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH )
    {
        lr1110_gnss_read_results( &lr1110, &nav_message[0], nav_message_size );
    }

    delay_since_capture =  (system_lptim_get() - last_agt_count + 500)/1000;

    gnss_tx_header* p_header;
    p_header = (gnss_tx_header*)gnss_result_buf;

    strcpy(gnss_result_buf, "@G" );
    p_header->nav_message_size     = (uint16_t)(nav_message_size & 0xFFFF);
    p_header->radio_ms             = (uint32_t)(timings.radio_ms);
    p_header->computation_ms       = (uint32_t)(timings.computation_ms);
    p_header->delay_since_capture  = (uint16_t)(delay_since_capture & 0xFFFF);
    memcpy(gnss_result_buf+sizeof(gnss_tx_header), &nav_message[0], nav_message_size);

    *p_radio_tx_len = nav_message_size + sizeof(gnss_tx_header);

    return nb_sat;
}


void gnss_packet_to_string( gnss_tx_header* p_header, char* dst)
{
    uint16_t ic;
    uint16_t nav_message_size;
    uint8_t  *p_nav_message;
    char temp_buf[20];

    nav_message_size = p_header->nav_message_size;
    strcpy(dst, "@" );
    p_nav_message = (char*)p_header + sizeof(gnss_tx_header);
    for( ic = 0; ic < nav_message_size; ic++ )
    {
        sprintf(temp_buf, "%02x",  *p_nav_message);
        strcat(dst, temp_buf);
        p_nav_message++;
    }
    sprintf(temp_buf,  ", %u, %u, %u\n", 
            p_header->delay_since_capture,
            p_header->radio_ms      ,
            p_header->computation_ms
        );
    strcat(dst, temp_buf);
}


