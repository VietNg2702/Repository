/**
 * @file      demo_manager_interface.h
 *
 * @brief     Definition of the demonstration manager interface.
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

#ifndef __DEMO_MANAGER_INTERFACE_H__
#define __DEMO_MANAGER_INTERFACE_H__

#include <stddef.h>
#include "demo_transceiver_radio_ping_pong.h"
#include "demo_transceiver_radio_per.h"
#include "demo_configuration.h"
#include "demo_interface.h"
#include "antenna_selector_interface.h"
#include "signaling_interface.h"
#include "environment_interface.h"
#include "timer_interface.h"
#include "interruption_interface.h"
#include "connectivity_manager_interface.h"
#include "demo_modem_temperature.h"
#include "demo_modem_file_upload.h"

typedef enum
{
    DEMO_TYPE_NONE = 0,
    DEMO_TYPE_WIFI,
    DEMO_TYPE_WIFI_COUNTRY_CODE,
    DEMO_TYPE_GNSS_AUTONOMOUS,
    DEMO_TYPE_GNSS_ASSISTED,
    DEMO_TYPE_RADIO_PING_PONG,
    DEMO_TYPE_TX_CW,
    DEMO_TYPE_RADIO_PER_TX,
    DEMO_TYPE_RADIO_PER_RX,
    DEMO_TYPE_TEMPERATURE,
    DEMO_TYPE_FILE_UPLOAD,
} demo_type_t;

class DemoManagerInterface
{
   public:
    DemoManagerInterface( EnvironmentInterface* environment, AntennaSelectorInterface* antenna_selector,
                          SignalingInterface* signaling, TimerInterface* timer,
                          CommunicationInterface*       communication_interface,
                          ConnectivityManagerInterface* connectivity_interface );
    virtual ~DemoManagerInterface( );

    void Init( );

    void SetConfigToDefault( demo_type_t demo_type );
    void GetConfigDefault( demo_all_settings_t* settings );
    void GetConfig( demo_all_settings_t* settings );
    void GetConfigRadio( demo_radio_settings_t* settings );
    void GetConfigWifi( demo_wifi_settings_t* settings );
    void GetConfigAutonomousGnss( demo_gnss_settings_t* settings );
    void GetConfigAssistedGnss( demo_gnss_settings_t* settings );
    void UpdateConfigRadio( demo_radio_settings_t* radio_config );
    void UpdateConfigWifiScan( const demo_wifi_settings_t* wifi_config );
    void UpdateConfigWifiCountryCode( const demo_wifi_country_code_settings_t* wifi_config );
    void UpdateConfigAutonomousGnss( const demo_gnss_settings_t* gnss_autonomous_config );
    void UpdateConfigAssistedGnss( const demo_gnss_settings_t* gnss_assisted_config );

    virtual void Start( demo_type_t demo_type ) = 0;
    void         Stop( );
    void         Reset( );
    bool         HasIntermediateResults( ) const;
    void         InterruptHandler( const InterruptionInterface* interruption );

    demo_status_t Runtime( );

    demo_type_t   GetType( );
    virtual void* GetResults( ) = 0;

   protected:
    EnvironmentInterface*             environment;
    AntennaSelectorInterface*         antenna_selector;
    SignalingInterface*               signaling;
    TimerInterface*                   timer;
    DemoInterface*                    running_demo;
    demo_type_t                       demo_type_current;
    demo_wifi_settings_t              demo_wifi_settings;
    demo_wifi_settings_t              demo_wifi_settings_default;
    demo_wifi_country_code_settings_t demo_wifi_country_code_settings;
    demo_wifi_country_code_settings_t demo_wifi_country_code_settings_default;
    demo_gnss_settings_t              demo_gnss_autonomous_settings;
    demo_gnss_settings_t              demo_gnss_autonomous_settings_default;
    demo_gnss_settings_t              demo_gnss_assisted_settings;
    demo_gnss_settings_t              demo_gnss_assisted_settings_default;
    demo_radio_settings_t             demo_radio_settings;
    demo_radio_settings_t             demo_radio_settings_default;
    CommunicationInterface*           communication_interface;
    ConnectivityManagerInterface*     connectivity_interface;
};

#endif  // __DEMO_MANAGER_INTERFACE_H__
