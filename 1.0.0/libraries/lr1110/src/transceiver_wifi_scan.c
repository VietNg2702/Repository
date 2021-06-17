/**
 * @file      demo_transceiver_wifi_scan.cpp
 *
 * @brief     Implementation of the Wi-Fi MAC address scan demonstration.
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

#include "config_mode.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"

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
#include "transceiver_wifi_scan.h"
#include "lr1110/lr1110_wifi.h"
#include "lr1110/lr1110_hal.h"



extern radio_t* lr1110;

#define WIFI_SCAN_ABORT_ON_TIMEOUT ( false )
#define DEMO_WIFI_MAX_RESULTS_PER_SCAN ( 20 )

demo_wifi_settings_t WifiScan_settings;

demo_wifi_scan_all_results_t WifiScan_results;



int8_t WifiScan_state;
int8_t  WifiScan_is_waiting_for_interrupt;


void WifiScan_SetWaitingForInterrupt( ) {
	 WifiScan_is_waiting_for_interrupt = true; 
}


void WifiScan_StartCapture( ) 
{ 
//	system_gpio_set_pin_state( Signaling::led_scan, SYSTEM_GPIO_PIN_STATE_HIGH ); 
}

bool WifiScan_has_received_interrupt;

bool WifiScan_InterruptHasRaised( )
{
    __disable_irq( );
    bool interrupt_flag          = WifiScan_has_received_interrupt;
    WifiScan_has_received_interrupt = false;
    __enable_irq( );
    return interrupt_flag;
}


//void WifiScan_SetWaitingForInterrupt( )
//{
//	 WifiScan_is_waiting_for_interrupt = true;
//}

void WifiScan_Terminate( ) 
{
//	 WifiScan_status = DEMO_STATUS_TERMINATED;
}


#define WIFI_DEMO_CONSUMPTION_DCDC_CORRELATION_MA ( 12 )
#define WIFI_DEMO_CONSUMPTION_DCDC_DEMODULATION_MA ( 4 )
#define WIFI_DEMO_CONSUMPTION_LDO_CORRELATION_MA ( 24 )
#define WIFI_DEMO_CONSUMPTION_LDO_DEMODULATION_MA ( 8 )


uint32_t WifiInterface_ComputeConsumption( const lr1110_system_reg_mode_t  regMode,
                                                           const lr1110_wifi_cumulative_timings_t *p_timing )
{
    uint32_t consumption_uas = 0;

    switch( regMode )
    {
    case LR1110_SYSTEM_REG_MODE_LDO:
        consumption_uas =
            ( ( p_timing->rx_correlation_us + p_timing->rx_capture_us ) * WIFI_DEMO_CONSUMPTION_LDO_CORRELATION_MA +
              p_timing->demodulation_us * WIFI_DEMO_CONSUMPTION_LDO_DEMODULATION_MA ) /
            1000;
        break;
    case LR1110_SYSTEM_REG_MODE_DCDC:
        consumption_uas =
            ( ( p_timing->rx_correlation_us + p_timing->rx_capture_us ) * WIFI_DEMO_CONSUMPTION_DCDC_CORRELATION_MA +
              p_timing->demodulation_us * WIFI_DEMO_CONSUMPTION_DCDC_DEMODULATION_MA ) /
            1000;
        break;
    }

    return consumption_uas;
}


void WifiScan_DeInit( ) 
{
}

lr1110_wifi_mode_t WifiInterface_transceiver_wifi_mode_from_demo(
    const demo_wifi_mode_t demo_wifi_mode )
{
    lr1110_wifi_mode_t transceiver_wifi_mode = LR1110_WIFI_SCAN_MODE_BEACON;
    switch( demo_wifi_mode )
    {
    case DEMO_WIFI_SCAN_MODE_BEACON:
    {
        transceiver_wifi_mode = LR1110_WIFI_SCAN_MODE_BEACON;
        break;
    }

    case DEMO_WIFI_SCAN_MODE_BEACON_AND_PACKET:
    {
        transceiver_wifi_mode = LR1110_WIFI_SCAN_MODE_BEACON_AND_PKT;
        break;
    }

    default:
        break;
    }
    return transceiver_wifi_mode;
}




lr1110_wifi_signal_type_scan_t WifiInterface_transceiver_wifi_scan_type_from_demo(
    const demo_wifi_signal_type_scan_t demo_wifi_scan_type )
{
    lr1110_wifi_signal_type_scan_t transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_B;
    switch( demo_wifi_scan_type )
    {
    case DEMO_WIFI_SETTING_TYPE_B:
    {
        transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_B;
        break;
    }
    case DEMO_WIFI_SETTING_TYPE_G:
    {
        transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_G;
        break;
    }
    case DEMO_WIFI_SETTING_TYPE_N:
    {
        transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_N;
        break;
    }
    case DEMO_WIFI_SETTING_TYPE_B_G_N:
    {
        transceiver_signal_type_scan = LR1110_WIFI_TYPE_SCAN_B_G_N;
        break;
    }
    }
    return transceiver_signal_type_scan;
}



demo_wifi_signal_type_t WifiInterface_demo_wifi_types_from_transceiver(
    lr1110_wifi_signal_type_result_t wifi_type_result )
{
    demo_wifi_signal_type_t demo_wifi_type = DEMO_WIFI_TYPE_B;
    switch( wifi_type_result )
    {
        case LR1110_WIFI_TYPE_RESULT_B:
        {                              
        	demo_wifi_type = DEMO_WIFI_TYPE_B;
            break;                     
        }
        case LR1110_WIFI_TYPE_RESULT_G:
        {                              
        	demo_wifi_type = DEMO_WIFI_TYPE_G;
            break;                     
        }
        case LR1110_WIFI_TYPE_RESULT_N:
        {                              
        	demo_wifi_type = DEMO_WIFI_TYPE_N;
            break;                     
        }
    }
    return demo_wifi_type;
}


demo_wifi_timings_t WifiInterface_demo_wifi_timing_from_transceiver(
    const lr1110_wifi_cumulative_timings_t *p_transceiver_timings )
{
    demo_wifi_timings_t demo_timing;
    demo_timing.demodulation_us   = p_transceiver_timings->demodulation_us;
    demo_timing.rx_capture_us     = p_transceiver_timings->rx_capture_us;
    demo_timing.rx_correlation_us = p_transceiver_timings->rx_correlation_us;
    demo_timing.rx_detection_us   = p_transceiver_timings->rx_detection_us;
    return demo_timing;
}

#define WIFI_RESULTS_NUM_MAX (15)
static lr1110_system_stat1_t wifi_stat1;
static lr1110_system_stat2_t wifi_stat2;
static lr1110_system_irq_mask_t irq_status1,irq_status2;
static uint8_t nb_results;

lr1110_wifi_basic_complete_result_t wifi_result_buffer[WIFI_RESULTS_NUM_MAX+1];

lr1110_hal_status_t lr1110_wifi_read_results_helper( const void* context, const uint8_t start_index,
                                                            const uint8_t n_elem, uint8_t* buffer,
                                                            const lr1110_wifi_result_format_t result_format );


int8_t wifi_scan_exec()
{
	int8_t result;

	result = 0;

//	lr1110_system_errors_t errors;

    lr1110_system_set_dio_irq_params( &lr1110,  LR1110_SYSTEM_IRQ_WIFI_SCAN_DONE, 0 );

    lr1110_wifi_reset_cumulative_timing( &lr1110 );

    lr1110_system_get_status( &lr1110, &wifi_stat1, &wifi_stat2, &irq_status2 );

    lr1110_system_get_and_clear_irq_status( &lr1110, &irq_status1 );

    lr1110_wifi_cfg_hardware_debarker( &lr1110,  0x1);


    lr1110_wifi_scan( &lr1110,
	   LR1110_WIFI_TYPE_SCAN_B,
	   0x421,
	   LR1110_WIFI_SCAN_MODE_BEACON_AND_PKT,
	   WIFI_RESULTS_NUM_MAX,
	   DEMO_WIFI_NBR_RETRIALS_DEFAULT,
   	   DEMO_WIFI_TIMEOUT_IN_MS_DEFAULT,
	   WIFI_SCAN_ABORT_ON_TIMEOUT
	 );


	result = semtech_event_wait(GNSS_WAIT_INTERVAL);
	if (result != 0) {return result;}

//   system_time_wait_ms( 3000 );

//Received '# #Mode=2, COMM=1,0, DATA=0x1,0,0,0,0,0,'  GetStatus 0x0100
 lr1110_system_get_status2( &lr1110, &wifi_stat1, &wifi_stat2, &irq_status2 );
 __NOP();

//Received '# #Mode=1, COMM=1,14, DATA=0x0,0x10,0,0,'  ClearIrq 0x0114
 lr1110_system_clear_irq_status( &lr1110,  0x0100000);

//Received '# #Mode=2, COMM=1,0, DATA=0x1,0,0,0,0,0,' GetStatus 0x0100
 lr1110_system_get_status( &lr1110, &wifi_stat1, &wifi_stat2, &irq_status2 );
 __NOP();

//Received '# #Mode=1, COMM=1,14, DATA=0,0,0,0,'  ClearIrq 0x0114
 lr1110_system_get_and_clear_irq_status( &lr1110, &irq_status1 );


//Received '# #Mode=0, COMM=3,5, DATA=0x0,' WifiGetNbResults 0x0305
 lr1110_wifi_get_nb_results( &lr1110, &nb_results );

__NOP();

	return result;
}




void WifiScan_ExecuteScan( radio_t* radio )
{
    lr1110_wifi_scan( radio, WifiInterface_transceiver_wifi_scan_type_from_demo( WifiScan_settings.types ),
                      WifiScan_settings.channels,
					  WifiInterface_transceiver_wifi_mode_from_demo( WifiScan_settings.scan_mode ),
					  WifiScan_settings.max_results, WifiScan_settings.nbr_retrials, 
					  WifiScan_settings.timeout,
                      WIFI_SCAN_ABORT_ON_TIMEOUT );
}

void WifiScan_FetchAndSaveResults( radio_t* radio )
{
    switch( WifiScan_settings.result_type )
    {
	    case DEMO_WIFI_RESULT_TYPE_BASIC_COMPLETE:
    	{
        	WifiScan_FetchAndSaveBasicCompleteResults( radio );
        	break;
	    }
    	case DEMO_WIFI_RESULT_TYPE_BASIC_MAC_TYPE_CHANNEL:
	    {
    	    WifiScan_FetchAndSaveBasicMacChannelTypeResults( radio );
        	break;
	    }	
    }
}

void WifiScan_AddScanToResults_complete( const lr1110_system_reg_mode_t             regMode,
                                                demo_wifi_scan_all_results_t              *p_results,
                                                lr1110_wifi_basic_complete_result_t* scan_result,
                                                uint8_t                              nbr_results )
{
    lr1110_wifi_basic_complete_result_t* local_basic_result;
	uint8_t index;
	
    for( index = 0; ( index < nbr_results ) && ( p_results->nbrResults < DEMO_WIFI_MAX_RESULT_TOTAL ); index++ )
    {
        local_basic_result = &scan_result[index];

        p_results->results[p_results->nbrResults].channel =
            lr1110_wifi_extract_channel_from_info_byte( local_basic_result->channel_info_byte );

        p_results->results[p_results->nbrResults].type = WifiInterface_demo_wifi_types_from_transceiver(
            lr1110_wifi_extract_signal_type_from_data_rate_info( local_basic_result->data_rate_info_byte ) );

        memcpy( p_results->results[p_results->nbrResults].mac_address, local_basic_result->mac_address,
                LR1110_WIFI_MAC_ADDRESS_LENGTH );

        p_results->results[p_results->nbrResults].rssi            = local_basic_result->rssi;
        p_results->results[p_results->nbrResults].country_code[0] = '?';
        p_results->results[p_results->nbrResults].country_code[1] = '?';
        p_results->nbrResults++;
    }
}


void WifiScan_AddScanToResults_mac( const lr1110_system_reg_mode_t                     regMode,
                                                demo_wifi_scan_all_results_t                      *p_results,
                                                lr1110_wifi_basic_mac_type_channel_result_t* scan_result,
                                                uint8_t                                      nbr_results )
{
    for( uint8_t index = 0; ( index < nbr_results ) && ( p_results->nbrResults < DEMO_WIFI_MAX_RESULT_TOTAL ); index++ )
    {
        lr1110_wifi_basic_mac_type_channel_result_t* local_basic_result = &scan_result[index];
        p_results->results[p_results->nbrResults].channel =
            lr1110_wifi_extract_channel_from_info_byte( local_basic_result->channel_info_byte );

        p_results->results[p_results->nbrResults].type = WifiInterface_demo_wifi_types_from_transceiver(
            lr1110_wifi_extract_signal_type_from_data_rate_info( local_basic_result->data_rate_info_byte ) );

        memcpy( p_results->results[p_results->nbrResults].mac_address, local_basic_result->mac_address,
                LR1110_WIFI_MAC_ADDRESS_LENGTH );

        p_results->results[p_results->nbrResults].rssi            = local_basic_result->rssi;
        p_results->results[p_results->nbrResults].country_code[0] = '?';
        p_results->results[p_results->nbrResults].country_code[1] = '?';
        p_results->nbrResults++;
    }
}


void WifiScan_FetchAndSaveBasicCompleteResults( radio_t* radio )
{
	uint8_t max_results_to_fetch;
    lr1110_wifi_basic_complete_result_t wifi_results_mac_addr[DEMO_WIFI_MAX_RESULTS_PER_SCAN] = { 0 };
    uint8_t nbr_results = 0;

    lr1110_wifi_get_nb_results( radio, &nbr_results );
    max_results_to_fetch =
        ( nbr_results > DEMO_WIFI_MAX_RESULTS_PER_SCAN ) ? DEMO_WIFI_MAX_RESULTS_PER_SCAN : nbr_results;

//    lr1110_wifi_read_basic_complete_results( WifiScan_device->GetRadio( ), 0, max_results_to_fetch,
    lr1110_wifi_read_basic_complete_results( radio, 0, max_results_to_fetch,
                                             wifi_results_mac_addr );

    WifiScan_AddScanToResults_complete( LR1110_SYSTEM_REG_MODE_DCDC, &WifiScan_results, wifi_results_mac_addr, max_results_to_fetch );
}

void WifiScan_FetchAndSaveBasicMacChannelTypeResults( radio_t* radio )
{

    uint8_t max_results_to_fetch;
    lr1110_wifi_basic_mac_type_channel_result_t wifi_results_mac_addr[DEMO_WIFI_MAX_RESULTS_PER_SCAN] = { 0 };
    uint8_t nbr_results = 0;

    lr1110_wifi_get_nb_results( radio, &nbr_results );
    max_results_to_fetch =
        ( nbr_results > DEMO_WIFI_MAX_RESULTS_PER_SCAN ) ? DEMO_WIFI_MAX_RESULTS_PER_SCAN : nbr_results;

    lr1110_wifi_read_basic_mac_type_channel_results( radio, 0, max_results_to_fetch,
                                                     wifi_results_mac_addr );

    WifiScan_AddScanToResults_mac( LR1110_SYSTEM_REG_MODE_DCDC, &WifiScan_results, wifi_results_mac_addr, max_results_to_fetch );
}

void WifiScan_Configure( demo_wifi_settings_t *p_config )
{
	WifiScan_settings = *p_config;
}


lr1110_system_irq_mask_t WifiScan_last_received_irq_mask;

lr1110_system_stat1_t    WifiScan_last_received_stat_1;



char* CommunicationInterface_WifiTypeToStr( demo_wifi_signal_type_t type )
{
    switch( type )
    {
    case DEMO_WIFI_TYPE_B:
    {
        return ( char* ) "TYPE_B";
    }
    case DEMO_WIFI_TYPE_G:
    {
        return ( char* ) "TYPE_G";
    }
    case DEMO_WIFI_TYPE_N:
    {
        return ( char* ) "TYPE_N";
    }
    default:
    {
        return ( char* ) "ERR TYPE";
    }
    }
}


int16_t CommunicationDemo_Store( const demo_wifi_scan_all_results_t* p_wifi_results , int8_t *p_wifi_result_str )
{
	uint8_t result_index;
//    demo_wifi_scan_single_result_t local_result;
	int16_t tx_len;
	wifi_tx_packet* p_packet;
	wifi_tx_header* p_header;

    p_header = (wifi_tx_header*)p_wifi_result_str;

	strcpy(p_wifi_result_str, "@W");
	p_header->nbrResults        =  p_wifi_results->nbrResults;
	p_header->demodulation_us   =  p_wifi_results->timings.demodulation_us;
	p_header->rx_capture_us     =  p_wifi_results->timings.rx_capture_us;
	p_header->rx_correlation_us =  p_wifi_results->timings.rx_correlation_us;
	p_header->rx_detection_us   =  p_wifi_results->timings.rx_detection_us;
	
	p_packet = (wifi_tx_packet*)(p_wifi_result_str + sizeof(wifi_tx_header));
    for(result_index = 0; result_index < p_wifi_results->nbrResults; result_index++ )
    {
		memcpy(&(p_packet->mac_address[0]), &(p_wifi_results->results[result_index].mac_address[0]),6);
		p_packet->channel=p_wifi_results->results[result_index].channel;
		p_packet->type   =p_wifi_results->results[result_index].type;
		p_packet->rssi   =p_wifi_results->results[result_index].rssi;
		p_packet++;
	}
	tx_len = sizeof(wifi_tx_header) + sizeof(wifi_tx_packet) * p_wifi_results->nbrResults;
	return tx_len;

}

void wifi_packet_to_string(wifi_tx_header* src_header, wifi_tx_packet* src_mac, char* dst)
{
    sprintf(dst,
        "@%.2x:%.2x:%.2x:%.2x:%.2x:%.2x, CHANNEL_%i, %s, %i, %i, %i, %i, %i\n",
		src_mac->mac_address[0],
		src_mac->mac_address[1],
		src_mac->mac_address[2],
		src_mac->mac_address[3],
		src_mac->mac_address[4],
		src_mac->mac_address[5],
		src_mac->channel,
        CommunicationInterface_WifiTypeToStr( src_mac->type ), 
        src_mac->rssi,
	    src_header->demodulation_us   ,
	    src_header->rx_capture_us     ,
	    src_header->rx_correlation_us ,
	    src_header->rx_detection_us   
		);
}

void WifiInterface_SpecificRuntime( )
{
	uint32_t                  consumption_uas;
    lr1110_wifi_cumulative_timings_t    wifi_results_timings;

	 wifi_scan_exec();

     WifiScan_FetchAndSaveResults( (radio_t*)&lr1110 );

     lr1110_wifi_read_cumulative_timing( &lr1110, &wifi_results_timings );


        consumption_uas =
            WifiInterface_ComputeConsumption( LR1110_SYSTEM_REG_MODE_DCDC, &wifi_results_timings );
        WifiScan_results.timings = WifiInterface_demo_wifi_timing_from_transceiver( (lr1110_wifi_cumulative_timings_t*)&wifi_results_timings );
        WifiScan_results.global_consumption_uas += consumption_uas;
        WifiScan_results.error = false;

	return;
	
}

