/**
 * @file      transceiver_radio.c
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

//#include "demo_transceiver_radio_per.h"
#include "RE01_256KB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "lr1110/lr1110_gnss.h"
#include "lr1110/lr1110_gnss_types.h"
#include "r_spi_cmsis_api.h"
#include "transceiver_radio.h"
#include "transceiver_gnss.h"
#include "transceiver_power.h"

typedef enum
{
    DEMO_RADIO_PER_STATE_INIT,
    DEMO_RADIO_PER_STATE_SEND,
    DEMO_RADIO_PER_STATE_WAIT_FOR_TX_DONE,
    DEMO_RADIO_PER_STATE_SET_RX,
    DEMO_RADIO_PER_STATE_WAIT_FOR_RX_DONE,
    DEMO_RADIO_PER_STATE_STOP,
} demo_radio_per_state_t;

typedef enum
{
    DEMO_RADIO_PER_MODE_TX,
    DEMO_RADIO_PER_MODE_RX,
} demo_radio_per_mode_t;


extern int8_t irq3_event_flag;


lr1110_radio_mod_params_lora_t modulation_lora;
lr1110_radio_pkt_params_lora_t packet_lora;
lr1110_radio_pa_cfg_t pa_configuration;

volatile int modem_result;


char lora_tx_buf[PAYLOAD_LENGTH+1];
char lora_rx_buf[PAYLOAD_LENGTH+1];


extern radio_t lr1110;

//static radio_t* radio;


void DeviceModem_Init( )
{
    lr1110_system_set_reg_mode( &lr1110, LR1110_SYSTEM_REG_MODE_DCDC );

    lr1110_system_rfswitch_cfg_t rf_switch_setup = { 0 };
    rf_switch_setup.enable                              = DEMO_COMMON_RF_SWITCH_ENABLE;
    rf_switch_setup.standby                             = DEMO_COMMON_RF_SWITCH_STANDBY;
    rf_switch_setup.tx                                  = DEMO_COMMON_RF_SWITCH_TX;
    rf_switch_setup.tx_hp                               = DEMO_COMMON_RF_SWITCH_TX_HP;
    rf_switch_setup.rx                                  = DEMO_COMMON_RF_SWITCH_RX;
    rf_switch_setup.wifi                                = DEMO_COMMON_RF_SWITCH_WIFI;
    rf_switch_setup.gnss                                = DEMO_COMMON_RF_SWITCH_GNSS;
    lr1110_system_set_dio_as_rf_switch( &lr1110, &rf_switch_setup );

    lr1110_system_set_tcxo_mode( &lr1110, LR1110_SYSTEM_TCXO_CTRL_3_0V, 500 );
    lr1110_system_cfg_lfclk( &lr1110, LR1110_SYSTEM_LFCLK_XTAL, true );
//  DeviceModem_SetAssistancePositionFromEnvironment();
}




int8_t  radio_init_sub()
{
    uint16_t errors;

	modem_result=    lr1110_system_set_reg_mode( &lr1110, LR1110_SYSTEM_REG_MODE_DCDC );

	lr1110_system_rfswitch_cfg_t rf_switch_setup = { 0 };
    rf_switch_setup.enable                       = DEMO_COMMON_RF_SWITCH_ENABLE;
    rf_switch_setup.standby                      = DEMO_COMMON_RF_SWITCH_STANDBY;
    rf_switch_setup.tx                           = DEMO_COMMON_RF_SWITCH_TX;
    rf_switch_setup.tx_hp                        = DEMO_COMMON_RF_SWITCH_TX_HP;
    rf_switch_setup.rx                           = DEMO_COMMON_RF_SWITCH_RX;
    rf_switch_setup.wifi                         = DEMO_COMMON_RF_SWITCH_WIFI;
    rf_switch_setup.gnss                         = DEMO_COMMON_RF_SWITCH_GNSS;
    lr1110_system_set_dio_as_rf_switch( &lr1110, &rf_switch_setup );

	modem_result= lr1110_system_set_tcxo_mode( &lr1110, LR1110_SYSTEM_TCXO_CTRL_3_0V, 500 );
	modem_result= lr1110_system_cfg_lfclk( &lr1110, LR1110_SYSTEM_LFCLK_XTAL, true );
	modem_result= lr1110_system_clear_errors( &lr1110 );
	modem_result= lr1110_system_calibrate( &lr1110, 0x3F );

	modem_result= lr1110_system_get_errors( &lr1110, &errors );
	modem_result= lr1110_system_clear_errors( &lr1110 );
	modem_result= lr1110_system_clear_irq_status( &lr1110, LR1110_SYSTEM_IRQ_ALL_MASK );

    return 0;
}

#define RF_FREQUENCY            923000000 // Hz


int8_t radio_init()
{
//	uint16_t i;
    modem_result=    radio_init_sub();

    modem_result = lr1110_radio_set_pkt_type( &lr1110, LR1110_RADIO_PKT_TYPE_LORA );

	modulation_lora.sf   = LR1110_RADIO_LORA_SF7;
	modulation_lora.bw   = LR1110_RADIO_LORA_BW_125;
	modulation_lora.cr   = LR1110_RADIO_LORA_CR_4_5;
	modulation_lora.ldro = 0;
	modem_result= lr1110_radio_set_lora_mod_params( &lr1110, &modulation_lora );

	packet_lora.preamble_len_in_symb  = 8;
	packet_lora.header_type           = LR1110_RADIO_LORA_PKT_EXPLICIT;  // = DEMO_RADIO_LORA_HDR_DEFAULT;
	packet_lora.crc                   = LR1110_RADIO_LORA_CRC_OFF;        // = DEMO_RADIO_LORA_CRC_DEFAULT;
	packet_lora.iq                    = LR1110_RADIO_LORA_IQ_STANDARD;   // = DEMO_RADIO_LORA_IQ_DEFAULT;
	packet_lora.pld_len_in_bytes = PAYLOAD_LENGTH;
	modem_result = lr1110_radio_set_lora_pkt_params( &lr1110, &packet_lora );

	modem_result=    lr1110_radio_set_rf_freq( &lr1110, RF_FREQUENCY );

	pa_configuration.pa_sel           = LR1110_RADIO_PA_SEL_LP;  // =DEMO_RADIO_PA_SEL_DEFAULT; Low-power Power Amplifier
	pa_configuration.pa_reg_supply    = LR1110_RADIO_PA_REG_SUPPLY_VREG; // = DEMO_RADIO_PA_REG_SUPPLY_DEFAULT;  Power amplifier supplied by the main regulator
	pa_configuration.pa_duty_cycle    = DEMO_RADIO_PA_DUTY_CYCLE_DEFAULT; // = 4
	pa_configuration.pa_hp_sel        = DEMO_RADIO_PA_HP_SEL_DEFAULT; // = 0
	modem_result = lr1110_radio_set_pa_cfg( &lr1110, &pa_configuration );

	modem_result=    lr1110_radio_set_tx_params( &lr1110, DEMO_RADIO_TX_POWER_DEFAULT, LR1110_RADIO_RAMP_240_US );
    modem_result =     lr1110_system_set_dio_irq_params(&lr1110,
     LR1110_SYSTEM_IRQ_GNSS_SCAN_DONE | LR1110_SYSTEM_IRQ_TX_DONE | LR1110_SYSTEM_IRQ_RX_DONE | LR1110_SYSTEM_IRQ_TIMEOUT, 0 );
    modem_result =    lr1110_system_clear_irq_status( &lr1110, LR1110_SYSTEM_IRQ_ALL_MASK );

    return 0;
}


int8_t  radio_send(int8_t* buffer)
{
    system_time_wait_ms( 1 );

	modem_result=         lr1110_regmem_write_buffer8( &lr1110, buffer,  PAYLOAD_LENGTH);

    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
    irq3_event_flag = false;
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);

	modem_result=         lr1110_radio_set_tx( &lr1110, 0x00005000 );
//    nb_of_packets_remaining--;
//    state = DEMO_RADIO_PER_STATE_WAIT_FOR_TX_DONE;
    return 0;
}


int8_t  radio_wait_for_tx_done(uint32_t timeout)
{
//	int16_t ic;
    int32_t last_received_irq_mask;
	int8_t result;

    
	result = semtech_event_wait(ACK_RECEIVE_INTERVAL);
	if (result !=0) {return result;}

	modem_result=     lr1110_system_get_irq_status( &lr1110, &last_received_irq_mask );

	modem_result= 	lr1110_system_clear_irq_status( &lr1110, LR1110_SYSTEM_IRQ_ALL_MASK );
    return 0;
}


int8_t  radio_receive()
{
    R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
    irq3_event_flag = false;
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);

    lr1110_radio_set_rx( &lr1110, 0x00000000 );
//    state = DEMO_RADIO_PER_STATE_WAIT_FOR_RX_DONE;
    return 0;
}

int8_t  radio_wait_for_rx_done(int8_t* buffer, uint32_t timeout)
{
//	int16_t ic;
    int32_t last_received_irq_mask;
	int8_t result;

	result = semtech_event_wait(timeout);
	if (result !=0) {return result;}

	modem_result=     lr1110_system_get_irq_status( &lr1110, &last_received_irq_mask );

    if( ( last_received_irq_mask & ( LR1110_SYSTEM_IRQ_CRC_ERROR | LR1110_SYSTEM_IRQ_HEADER_ERROR ) ) != 0 )
    {
//        count_rx_wrong_packet++;
//        has_intermediate_results = true;
//        state                    = DEMO_RADIO_PER_STATE_SET_RX;
    }
    else if( last_received_irq_mask & LR1110_SYSTEM_IRQ_RX_DONE )
    {
	    modem_result= lr1110_regmem_read_buffer8( &lr1110, buffer, 0,  PAYLOAD_LENGTH );
//        count_rx_correct_packet++;
//        has_intermediate_results = true;
//        state                    = DEMO_RADIO_PER_STATE_SET_RX;
    }
	modem_result= 	lr1110_system_clear_irq_status( &lr1110, LR1110_SYSTEM_IRQ_ALL_MASK );
    return 0;
}

/**********************************************************************

   Semtech Radio

**********************************************************************/

int8_t semtech_radio_tx_len(char* radio_tx_buf ,int16_t len, uint32_t timeout)
{
    uint8_t jc;
    int8_t result;
    uint32_t timeout_count;
//    lr1110_system_sleep_cfg_t sleep_cfg;
	uint8_t payload_count;
	uint8_t crc8;

//    sleep_cfg.is_rtc_timeout = true;
//    sleep_cfg.is_warm_start = true;

//    len = strlen(radio_tx_buf);
    timeout_count = system_lptim_get() + timeout;

	payload_count = (uint8_t)(((len-1) / (PAYLOAD_LENGTH-2)) & 0xFF);
    for (jc=payload_count; ; jc--) {
        lora_tx_buf[0] = jc;
        memcpy(lora_tx_buf+2, radio_tx_buf, PAYLOAD_LENGTH-2 );
		crc8 = CRC8_calc(lora_tx_buf+2, PAYLOAD_LENGTH-2);
		lora_tx_buf[1] = crc8;
#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    PFS->P210PFS = 0x04 | (jc & 1);
    PFS->P410PFS = 0x04 | (~jc & 1);
#endif
        radio_send(lora_tx_buf);
        result = radio_wait_for_tx_done(timeout);
        if (timeout !=0 && system_lptim_get() >= timeout_count) {
            result = -1; break;
        }
        if (jc == 0) {
            result = 1; break;
        }
        radio_tx_buf += (PAYLOAD_LENGTH-2);
//        lr1110_system_set_sleep(&lr1110, sleep_cfg, 2000);
        lr1110_sleep_enter( 2000);
        system_time_wait_ms( 1200 );
    }
#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    PFS->P210PFS = 0x04 | 1;
    PFS->P410PFS = 0x04 | 1;
#endif
    return result;
}



int8_t semtech_radio_tx(char* radio_tx_buf ,uint32_t timeout)
{
    int16_t len;
    int8_t result;

    len = strlen(radio_tx_buf);
    result = semtech_radio_tx_len(radio_tx_buf, len, timeout);
    return result;
}

int8_t semtech_radio_rx_len(char* radio_rx_buf , int16_t *p_receive_len, int16_t max_len, uint32_t timeout)
{
    uint32_t timeout_count;
//  int16_t ic, len;
//    int16_t ic;
    int8_t result;
    int8_t first;
    int16_t len;
    int16_t receive_count;
	uint8_t payload_count;
	uint8_t crc8;

    *radio_rx_buf=0;
    result = 0;
    first = true;
    for (receive_count=0; ;receive_count++ ) {
        timeout_count = system_lptim_get() + timeout;
        *(int16_t*)lora_rx_buf =0;
#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    PFS->P210PFS = 0x04 | (receive_count & 1);
    PFS->P410PFS = 0x04 | (~receive_count & 1);
#endif
        radio_receive();
        result = radio_wait_for_rx_done(lora_rx_buf, timeout);
        if (timeout !=0 && system_lptim_get() >= timeout_count) {
            result = -1; 
            break; // Timeout Error
        }
        payload_count = lora_rx_buf[0];
		crc8 = CRC8_calc(lora_rx_buf+2, PAYLOAD_LENGTH-2);
		if ( (uint8_t)crc8 != (uint8_t)lora_rx_buf[1]) {
            result = -1;
            break; // CRC error
		}
        if (first) {
            *p_receive_len = (payload_count + 1) * (PAYLOAD_LENGTH-2);
            first = false;
        }
        memcpy(radio_rx_buf, lora_rx_buf+2, (PAYLOAD_LENGTH-2));
        radio_rx_buf[PAYLOAD_LENGTH-2] = 0;
        if (payload_count == 0) {
            result = 0;
            break;
        }
        radio_rx_buf += (PAYLOAD_LENGTH-2);
        len -= (PAYLOAD_LENGTH-2);
    }
#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    PFS->P210PFS = 0x04 | 1;
    PFS->P410PFS = 0x04 | 1;
#endif
    return result;
}


int8_t semtech_radio_rx(char* radio_rx_buf ,uint32_t timeout)
{
    int16_t receive_len;
    int8_t result;

    result = semtech_radio_rx_len(radio_rx_buf , &receive_len, 256, timeout);
    return result;
}

