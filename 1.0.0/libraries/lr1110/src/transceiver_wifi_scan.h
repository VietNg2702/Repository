/**
 * @file      demo_transceiver_wifi_scan.h
 *
 * @brief     Definition of Wi-Fi MAC address scan demo.
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

#ifndef __DEMO_TRANSCEIVER_WIFI_SCAN_H__
#define __DEMO_TRANSCEIVER_WIFI_SCAN_H__

#include "demo_configuration.h"
//#include "demo_transceiver_wifi_interface.h"

typedef struct {
	uint8_t mac_address[6];
	int8_t channel;
	int8_t type;
	int8_t rssi;
} __attribute__ ((packed)) wifi_tx_packet;

typedef struct {
	uint8_t src_name[3];
	uint8_t  nbrResults;
	uint32_t demodulation_us;
	uint32_t rx_capture_us;
	uint32_t rx_correlation_us;
	uint32_t rx_detection_us;
} __attribute__ ((packed)) wifi_tx_header;



typedef enum
{
    DEMO_WIFI_INIT,
    DEMO_WIFI_SCAN,
    DEMO_WIFI_WAIT_FOR_SCAN,
    DEMO_WIFI_GET_RESULTS,
    DEMO_WIFI_TERMINATED,
} demo_wifi_state_t;


void WifiScan_DeInit( );
void WifiScan_Configure( demo_wifi_settings_t *p_config );
void WifiScan_ExecuteScan( radio_t* radio );
void WifiScan_FetchAndSaveResults( radio_t* radio );
void WifiScan_FetchAndSaveBasicCompleteResults( radio_t* radio );
void WifiScan_FetchAndSaveBasicMacChannelTypeResults( radio_t* radio );
void WifiScan_AddScanToResults( const lr1110_system_reg_mode_t regMode, 
								demo_wifi_scan_all_results_t* p_results,
                               const lr1110_wifi_basic_complete_result_t* scan_result, 
                               const uint8_t nbr_results );



void WifiInterface_SpecificRuntime(void);

int16_t CommunicationDemo_Store( const demo_wifi_scan_all_results_t* p_wifi_results , int8_t *p_wifi_result_str );


void wifi_packet_to_string(wifi_tx_header* src_header, wifi_tx_packet* src_mac, char* dst);





#endif  //__DEMO_TRANSCEIVER_WIFI_SCAN_H__
