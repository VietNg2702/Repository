/**
 * @file      supervisor.cpp
 *
 * @brief     Implementation of the supervisor class.
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

#include "supervisor.h"
#include "connectivity_conversions.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void SupervisorInterruptHandlerGui( bool is_down ) { Supervisor::InterruptHandlerGui( is_down ); }

extern void SupervisorInterruptHandlerDemo( void ) { Supervisor::InterruptHandlerDemo( ); }

#ifdef __cplusplus
}
#endif

volatile bool Supervisor::is_demo_interrupt_raised = false;
volatile bool Supervisor::is_gui_interrupt_raised  = false;
bool          Supervisor::is_down_gui_interrupt    = false;

Supervisor::Supervisor( Gui* gui, DeviceInterface* device, DemoManagerInterface* demo_manager,
                        EnvironmentInterface* environment, CommunicationManager* communication_manager,
                        ConnectivityManagerInterface* connectivity_manager )
    : run_demo( false ),
      demo_manager( demo_manager ),
      gui( gui ),
      environment( environment ),
      device( device ),
      connectivity_manager( connectivity_manager ),
      communication_manager( communication_manager ),
      has_connectivity( connectivity_manager->IsConnectable( ) )
{
    version_handler.almanac_crc  = 0;
    version_handler.almanac_date = 0;
    version_handler.modem        = { 0 };
    strcpy( version_handler.version_driver, "TBD" );
    strcpy( version_handler.version_sw, "TBD" );
}

Supervisor::~Supervisor( ) {}

void Supervisor::Init( )
{
    demo_all_settings_t             demo_all_settings;
    demo_all_settings_t             demo_all_settings_default;
    GuiDemoSettings_t               gui_demo_settings;
    GuiDemoSettings_t               gui_demo_settings_default;
    GuiGnssDemoAssistancePosition_t gui_gnss_demo_assistance_position;
    GuiGnssDemoAssistancePosition_t gui_gnss_demo_assistance_position_default;

    this->demo_manager->Init( );

    this->demo_manager->GetConfigDefault( &demo_all_settings_default );
    this->demo_manager->GetConfig( &demo_all_settings );

    this->ConvertSettingsFromDemoToGui( &demo_all_settings_default, &gui_demo_settings_default );
    this->ConvertSettingsFromDemoToGui( &demo_all_settings, &gui_demo_settings );

    this->GetAndPropagateVersion( );

    const environment_location_t location                    = this->environment->GetLocation( );
    gui_gnss_demo_assistance_position.latitude               = location.latitude;
    gui_gnss_demo_assistance_position.longitude              = location.longitude;
    gui_gnss_demo_assistance_position.set_by_network         = false;
    gui_gnss_demo_assistance_position_default.latitude       = location.latitude;
    gui_gnss_demo_assistance_position_default.longitude      = location.longitude;
    gui_gnss_demo_assistance_position_default.set_by_network = false;

    this->gui->Init( &gui_demo_settings, &gui_demo_settings_default, &gui_gnss_demo_assistance_position,
                     &gui_gnss_demo_assistance_position_default, &version_handler );

    if( this->has_connectivity == true )
    {
        this->gui->EnableConnectivity( );
    }
    else
    {
        this->gui->DisableConnectivity( );
    }
}

void Supervisor::ConvertSettingsFromDemoToGui( const demo_all_settings_t* demo_settings,
                                               GuiDemoSettings_t*         gui_demo_settings )
{
    // Wi-Fi
    gui_demo_settings->wifi_settings.channel_mask = demo_settings->wifi_settings.channels;

    gui_demo_settings->wifi_settings.is_type_b =
        ( demo_settings->wifi_settings.types == DEMO_WIFI_SETTING_TYPE_B ) ? true : false;

    gui_demo_settings->wifi_settings.is_type_g =
        ( demo_settings->wifi_settings.types == DEMO_WIFI_SETTING_TYPE_G ) ? true : false;

    gui_demo_settings->wifi_settings.is_type_n =
        ( demo_settings->wifi_settings.types == DEMO_WIFI_SETTING_TYPE_N ) ? true : false;

    gui_demo_settings->wifi_settings.is_type_all =
        ( demo_settings->wifi_settings.types == DEMO_WIFI_SETTING_TYPE_B_G_N ) ? true : false;

    gui_demo_settings->wifi_settings.abort_on_timeout = demo_settings->wifi_settings.does_abort_on_timeout;

    // GNSS autonomous
    gui_demo_settings->gnss_autonomous_settings.is_beidou_enabled =
        ( ( demo_settings->gnss_autonomous_settings.constellation_mask & DEMO_GNSS_BEIDOU_MASK ) != 0 ) ? true : false;

    gui_demo_settings->gnss_autonomous_settings.is_gps_enabled =
        ( ( demo_settings->gnss_autonomous_settings.constellation_mask & DEMO_GNSS_GPS_MASK ) != 0 ) ? true : false;

    gui_demo_settings->gnss_autonomous_settings.is_dual_scan_activated =
        ( demo_settings->gnss_autonomous_settings.capture_mode == DEMO_GNSS_DOUBLE_SCAN_MODE ) ? true : false;

    gui_demo_settings->gnss_autonomous_settings.is_best_effort_activated =
        ( demo_settings->gnss_autonomous_settings.option == DEMO_GNSS_OPTION_BEST_EFFORT ) ? true : false;

    // GNSS assisted
    gui_demo_settings->gnss_assisted_settings.is_beidou_enabled =
        ( ( demo_settings->gnss_assisted_settings.constellation_mask & DEMO_GNSS_BEIDOU_MASK ) != 0 ) ? true : false;

    gui_demo_settings->gnss_assisted_settings.is_gps_enabled =
        ( ( demo_settings->gnss_assisted_settings.constellation_mask & DEMO_GNSS_GPS_MASK ) != 0 ) ? true : false;

    gui_demo_settings->gnss_assisted_settings.is_dual_scan_activated =
        ( demo_settings->gnss_assisted_settings.capture_mode == DEMO_GNSS_DOUBLE_SCAN_MODE ) ? true : false;

    gui_demo_settings->gnss_assisted_settings.is_best_effort_activated =
        ( demo_settings->gnss_assisted_settings.option == DEMO_GNSS_OPTION_BEST_EFFORT ) ? true : false;

    // Radio
    gui_demo_settings->radio_settings.rf_freq_in_hz = demo_settings->radio_settings.rf_frequency;

    gui_demo_settings->radio_settings.pwr_in_dbm = demo_settings->radio_settings.tx_power;

    gui_demo_settings->radio_settings.nb_of_packets = demo_settings->radio_settings.nb_of_packets;

    gui_demo_settings->radio_settings.payload_length = demo_settings->radio_settings.nb_of_packets;

    gui_demo_settings->radio_settings.is_lora =
        ( demo_settings->radio_settings.pkt_type == LR1110_RADIO_PKT_TYPE_LORA ) ? true : false;

    gui_demo_settings->radio_settings.is_hp_pa_enabled =
        ( demo_settings->radio_settings.pa_configuration.pa_sel == LR1110_RADIO_PA_SEL_HP ) ? true : false;

    gui_demo_settings->radio_settings.lora.sf = demo_settings->radio_settings.modulation_lora.sf - 5;

    switch( demo_settings->radio_settings.modulation_lora.bw )
    {
    case LR1110_RADIO_LORA_BW_125:
        gui_demo_settings->radio_settings.lora.bw = 0;
        break;
    case LR1110_RADIO_LORA_BW_250:
        gui_demo_settings->radio_settings.lora.bw = 1;
        break;
    case LR1110_RADIO_LORA_BW_500:
        gui_demo_settings->radio_settings.lora.bw = 2;
        break;
    default:
        gui_demo_settings->radio_settings.lora.bw = 0;
    }

    switch( demo_settings->radio_settings.modulation_lora.cr )
    {
    case LR1110_RADIO_LORA_CR_4_5:
        gui_demo_settings->radio_settings.lora.cr = 0;
        break;
    case LR1110_RADIO_LORA_CR_4_6:
        gui_demo_settings->radio_settings.lora.cr = 1;
        break;
    case LR1110_RADIO_LORA_CR_4_7:
        gui_demo_settings->radio_settings.lora.cr = 2;
        break;
    case LR1110_RADIO_LORA_CR_4_8:
        gui_demo_settings->radio_settings.lora.cr = 3;
        break;
    default:
        gui_demo_settings->radio_settings.lora.cr = 0;
    }

    switch( demo_settings->radio_settings.packet_lora.iq )
    {
    case LR1110_RADIO_LORA_IQ_STANDARD:
        gui_demo_settings->radio_settings.lora.is_iq_inverted = false;
        break;
    case LR1110_RADIO_LORA_IQ_INVERTED:
        gui_demo_settings->radio_settings.lora.is_iq_inverted = true;
        break;
    }

    switch( demo_settings->radio_settings.packet_lora.crc )
    {
    case LR1110_RADIO_LORA_CRC_OFF:
        gui_demo_settings->radio_settings.lora.is_crc_activated = false;
        break;
    case LR1110_RADIO_LORA_CRC_ON:
        gui_demo_settings->radio_settings.lora.is_crc_activated = true;
        break;
    }

    switch( demo_settings->radio_settings.packet_lora.header_type )
    {
    case LR1110_RADIO_LORA_PKT_EXPLICIT:
        gui_demo_settings->radio_settings.lora.is_hdr_implicit = false;
        break;
    case LR1110_RADIO_LORA_PKT_IMPLICIT:
        gui_demo_settings->radio_settings.lora.is_crc_activated = true;
        break;
    }

    gui_demo_settings->radio_settings.gfsk.br_in_bps = demo_settings->radio_settings.modulation_gfsk.br_in_bps;

    gui_demo_settings->radio_settings.gfsk.fdev_in_hz = demo_settings->radio_settings.modulation_gfsk.fdev_in_hz;

    switch( demo_settings->radio_settings.packet_gfsk.crc_type )
    {
    case LR1110_RADIO_GFSK_CRC_OFF:
        gui_demo_settings->radio_settings.gfsk.crc = 0;
        break;
    case LR1110_RADIO_GFSK_CRC_1_BYTE:
        gui_demo_settings->radio_settings.gfsk.crc = 1;
        break;
    case LR1110_RADIO_GFSK_CRC_2_BYTES:
        gui_demo_settings->radio_settings.gfsk.crc = 2;
        break;
    case LR1110_RADIO_GFSK_CRC_1_BYTE_INV:
        gui_demo_settings->radio_settings.gfsk.crc = 3;
        break;
    case LR1110_RADIO_GFSK_CRC_2_BYTES_INV:
        gui_demo_settings->radio_settings.gfsk.crc = 4;
        break;
    }

    switch( demo_settings->radio_settings.packet_gfsk.dc_free )
    {
    case LR1110_RADIO_GFSK_DC_FREE_OFF:
        gui_demo_settings->radio_settings.gfsk.is_dcfree_enabled = false;
        break;
    case LR1110_RADIO_GFSK_DC_FREE_WHITENING:
        gui_demo_settings->radio_settings.gfsk.is_dcfree_enabled = true;
        break;
    }

    switch( demo_settings->radio_settings.packet_gfsk.header_type )
    {
    case LR1110_RADIO_GFSK_PKT_FIX_LEN:
        gui_demo_settings->radio_settings.gfsk.is_hdr_implicit = true;
        break;
    case LR1110_RADIO_GFSK_PKT_VAR_LEN:
        gui_demo_settings->radio_settings.gfsk.is_hdr_implicit = false;
        break;
    }
}

void Supervisor::ConvertSettingsFromGuiToDemo( const GuiRadioSetting_t* gui_settings,
                                               demo_radio_settings_t*   demo_settings )
{
    demo_settings->rf_frequency   = gui_settings->rf_freq_in_hz;
    demo_settings->tx_power       = gui_settings->pwr_in_dbm;
    demo_settings->nb_of_packets  = gui_settings->nb_of_packets;
    demo_settings->payload_length = gui_settings->payload_length;
    demo_settings->pkt_type =
        ( gui_settings->is_lora == true ) ? LR1110_RADIO_PKT_TYPE_LORA : LR1110_RADIO_PKT_TYPE_GFSK;

    demo_settings->pa_configuration.pa_sel =
        ( gui_settings->is_hp_pa_enabled == true ) ? LR1110_RADIO_PA_SEL_HP : LR1110_RADIO_PA_SEL_LP;

    switch( gui_settings->pwr_in_dbm )
    {
    case 10:
        demo_settings->tx_power                       = 14;
        demo_settings->pa_configuration.pa_duty_cycle = 0;
        demo_settings->pa_configuration.pa_hp_sel     = 0;
        demo_settings->pa_configuration.pa_reg_supply = LR1110_RADIO_PA_REG_SUPPLY_VREG;
        break;
    case 14:
        if( demo_settings->pa_configuration.pa_sel == LR1110_RADIO_PA_SEL_LP )
        {
            demo_settings->tx_power                       = 14;
            demo_settings->pa_configuration.pa_duty_cycle = 4;
            demo_settings->pa_configuration.pa_hp_sel     = 0;
            demo_settings->pa_configuration.pa_reg_supply = LR1110_RADIO_PA_REG_SUPPLY_VREG;
        }
        else
        {
            demo_settings->tx_power                       = 22;
            demo_settings->pa_configuration.pa_duty_cycle = 2;
            demo_settings->pa_configuration.pa_hp_sel     = 2;
            demo_settings->pa_configuration.pa_reg_supply = LR1110_RADIO_PA_REG_SUPPLY_VBAT;
        }
        break;
    case 15:
        demo_settings->tx_power                       = 14;
        demo_settings->pa_configuration.pa_duty_cycle = 7;
        demo_settings->pa_configuration.pa_hp_sel     = 0;
        demo_settings->pa_configuration.pa_reg_supply = LR1110_RADIO_PA_REG_SUPPLY_VREG;
        break;
    case 17:
        demo_settings->tx_power                       = 22;
        demo_settings->pa_configuration.pa_duty_cycle = 1;
        demo_settings->pa_configuration.pa_hp_sel     = 5;
        demo_settings->pa_configuration.pa_reg_supply = LR1110_RADIO_PA_REG_SUPPLY_VBAT;
        break;
    case 20:
        demo_settings->tx_power                       = 22;
        demo_settings->pa_configuration.pa_duty_cycle = 2;
        demo_settings->pa_configuration.pa_hp_sel     = 7;
        demo_settings->pa_configuration.pa_reg_supply = LR1110_RADIO_PA_REG_SUPPLY_VBAT;
        break;
    case 22:
        demo_settings->tx_power                       = 22;
        demo_settings->pa_configuration.pa_duty_cycle = 4;
        demo_settings->pa_configuration.pa_hp_sel     = 7;
        demo_settings->pa_configuration.pa_reg_supply = LR1110_RADIO_PA_REG_SUPPLY_VBAT;
    }

    demo_settings->modulation_lora.sf = ( lr1110_radio_lora_sf_t )( gui_settings->lora.sf + 5 );

    switch( gui_settings->lora.bw )
    {
    case 0:
        demo_settings->modulation_lora.bw = LR1110_RADIO_LORA_BW_125;
        break;
    case 1:
        demo_settings->modulation_lora.bw = LR1110_RADIO_LORA_BW_250;
        break;
    case 2:
        demo_settings->modulation_lora.bw = LR1110_RADIO_LORA_BW_500;
        break;
    default:
        demo_settings->modulation_lora.bw = LR1110_RADIO_LORA_BW_125;
    }

    switch( gui_settings->lora.cr )
    {
    case 0:
        demo_settings->modulation_lora.cr = LR1110_RADIO_LORA_CR_4_5;
        break;
    case 1:
        demo_settings->modulation_lora.cr = LR1110_RADIO_LORA_CR_4_6;
        break;
    case 2:
        demo_settings->modulation_lora.cr = LR1110_RADIO_LORA_CR_4_7;
        break;
    case 3:
        demo_settings->modulation_lora.cr = LR1110_RADIO_LORA_CR_4_8;
        break;
    }

    demo_settings->packet_lora.iq =
        ( gui_settings->lora.is_iq_inverted == false ) ? LR1110_RADIO_LORA_IQ_STANDARD : LR1110_RADIO_LORA_IQ_INVERTED;

    demo_settings->packet_lora.crc =
        ( gui_settings->lora.is_crc_activated == false ) ? LR1110_RADIO_LORA_CRC_OFF : LR1110_RADIO_LORA_CRC_ON;

    demo_settings->packet_lora.header_type = ( gui_settings->lora.is_hdr_implicit == false )
                                                 ? LR1110_RADIO_LORA_PKT_EXPLICIT
                                                 : LR1110_RADIO_LORA_PKT_IMPLICIT;

    demo_settings->modulation_gfsk.br_in_bps = gui_settings->gfsk.br_in_bps;

    demo_settings->modulation_gfsk.fdev_in_hz = gui_settings->gfsk.fdev_in_hz;

    switch( gui_settings->gfsk.crc )
    {
    case 0:
        demo_settings->packet_gfsk.crc_type = LR1110_RADIO_GFSK_CRC_OFF;
        break;
    case 1:
        demo_settings->packet_gfsk.crc_type = LR1110_RADIO_GFSK_CRC_1_BYTE;
        break;
    case 2:
        demo_settings->packet_gfsk.crc_type = LR1110_RADIO_GFSK_CRC_2_BYTES;
        break;
    case 3:
        demo_settings->packet_gfsk.crc_type = LR1110_RADIO_GFSK_CRC_1_BYTE_INV;
        break;
    case 4:
        demo_settings->packet_gfsk.crc_type = LR1110_RADIO_GFSK_CRC_2_BYTES_INV;
        break;
    }

    demo_settings->packet_gfsk.dc_free = ( gui_settings->gfsk.is_dcfree_enabled == false )
                                             ? LR1110_RADIO_GFSK_DC_FREE_OFF
                                             : LR1110_RADIO_GFSK_DC_FREE_WHITENING;

    demo_settings->packet_gfsk.header_type =
        ( gui_settings->gfsk.is_hdr_implicit == false ) ? LR1110_RADIO_GFSK_PKT_VAR_LEN : LR1110_RADIO_GFSK_PKT_FIX_LEN;
}

void Supervisor::ConvertSettingsFromGuiToDemo( const GuiWifiDemoSetting_t* gui_settings,
                                               demo_wifi_settings_t*       demo_settings )
{
    demo_settings->channels = gui_settings->channel_mask;

    if( gui_settings->is_type_b == true )
    {
        demo_settings->types = DEMO_WIFI_SETTING_TYPE_B;
    }
    else if( gui_settings->is_type_g == true )
    {
        demo_settings->types = DEMO_WIFI_SETTING_TYPE_G;
    }
    else if( gui_settings->is_type_n == true )
    {
        demo_settings->types = DEMO_WIFI_SETTING_TYPE_N;
    }
    else if( gui_settings->is_type_all == true )
    {
        demo_settings->types = DEMO_WIFI_SETTING_TYPE_B_G_N;
    }
    demo_settings->does_abort_on_timeout = gui_settings->abort_on_timeout;
    demo_settings->result_type           = DEMO_WIFI_RESULT_TYPE_BASIC_COMPLETE;
}

void Supervisor::ConvertSettingsFromGuiToDemo( const GuiGnssDemoSetting_t* gui_settings,
                                               demo_gnss_settings_t*       demo_settings )
{
    demo_settings->constellation_mask = 0;

    if( gui_settings->is_beidou_enabled == true )
    {
        demo_settings->constellation_mask |= DEMO_GNSS_BEIDOU_MASK;
    }

    if( gui_settings->is_gps_enabled == true )
    {
        demo_settings->constellation_mask |= DEMO_GNSS_GPS_MASK;
    }

    if( gui_settings->is_dual_scan_activated == true )
    {
        demo_settings->capture_mode = DEMO_GNSS_DOUBLE_SCAN_MODE;
    }
    else
    {
        demo_settings->capture_mode = DEMO_GNSS_SINGLE_SCAN_MODE;
    }

    if( gui_settings->is_best_effort_activated == true )
    {
        demo_settings->option = DEMO_GNSS_OPTION_BEST_EFFORT;
    }
    else
    {
        demo_settings->option = DEMO_GNSS_OPTION_DEFAULT;
    }
}

void Supervisor::GetAndPropagateVersion( ) { this->device->FetchVersion( this->version_handler ); }

void Supervisor::Runtime( )
{
    this->InterruptionRuntime( );
    this->NetworkConnectivityRuntimeAndProcess( );
    this->GuiRuntimeAndProcess( );
    this->CommunicationManagerRuntime( );
    this->DeviceRuntime( );

    if( this->run_demo )
    {
        this->DemoRuntimeAndProcess( );
    }
}

void Supervisor::GuiRuntimeAndProcess( )
{
    GuiLastEvent_t last_gui_event = GUI_LAST_EVENT_NONE;

    this->gui->Runtime( );

    last_gui_event = this->gui->GetLastEvent( );

    switch( last_gui_event )
    {
    case GUI_LAST_EVENT_NONE:
    {
        break;
    }
    case GUI_LAST_EVENT_JOIN:
    {
        GuiNetworkConnectivitySettings_t gui_network_connectivity_settings;
        network_connectivity_settings_t  network_connectivity_settings;

        this->gui->GetNetworkConnectivitySettings( &gui_network_connectivity_settings );

        if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_EU868 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_EU868;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_AS923_GRP1 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_AS923_GRP1;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_US915 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_US915;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_AU915 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_AU915;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_CN470 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_CN470;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_AS923_GRP2 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_AS923_GRP2;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_AS923_GRP3 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_AS923_GRP3;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_IN865 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_IN865;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_KR920 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_KR920;
        }
        else if( gui_network_connectivity_settings.region == GUI_NETWORK_CONNECTIVITY_REGION_RU864 )
        {
            network_connectivity_settings.region = NETWORK_CONNECTIVITY_REGION_RU864;
        }

        if( gui_network_connectivity_settings.adr_profile == GUI_NETWORK_CONNECTIVITY_ADR_NETWORK_SERVER_CONTROLLED )
        {
            network_connectivity_settings.adr_profile = NETWORK_CONNECTIVITY_ADR_NETWORK_SERVER_CONTROLLED;
        }
        else if( gui_network_connectivity_settings.adr_profile == GUI_NETWORK_CONNECTIVITY_ADR_MOBILE_LONG_RANGE )
        {
            network_connectivity_settings.adr_profile = NETWORK_CONNECTIVITY_ADR_MOBILE_LONG_RANGE;
        }
        else if( gui_network_connectivity_settings.adr_profile == GUI_NETWORK_CONNECTIVITY_ADR_MOBILE_LOW_POWER )
        {
            network_connectivity_settings.adr_profile = NETWORK_CONNECTIVITY_ADR_MOBILE_LOW_POWER;
        }

        if( gui_network_connectivity_settings.lorawan_class == GUI_NETWORK_CONNECTIVITY_LORAWAN_CLASS_A )
        {
            network_connectivity_settings.lorawan_class = NETWORK_CONNECTIVITY_LORAWAN_CLASS_A;
        }
        else if( gui_network_connectivity_settings.lorawan_class == GUI_NETWORK_CONNECTIVITY_LORAWAN_CLASS_C )
        {
            network_connectivity_settings.lorawan_class = NETWORK_CONNECTIVITY_LORAWAN_CLASS_C;
        }

        this->connectivity_manager->Join( &network_connectivity_settings );
        break;
    }
    case GUI_LAST_EVENT_LEAVE:
    {
        this->connectivity_manager->Leave( );
        break;
    }
    case GUI_LAST_EVENT_RESET_SEMTECH_DEFAULT_COMMISSIONING:
    {
        this->connectivity_manager->ResetCommissioningToSemtechJoinServer( );
        this->GetAndPropagateVersion( );
        this->gui->CommissioningChange( );
        break;
    }
    case GUI_LAST_EVENT_PRINT_EUI:
    {
        const version_handler_t* version_handler = GetVersionHandler( );
        this->communication_manager->Log( "CHIP EUI - %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n",
                                          version_handler->chip_uid[0], version_handler->chip_uid[1],
                                          version_handler->chip_uid[2], version_handler->chip_uid[3],
                                          version_handler->chip_uid[4], version_handler->chip_uid[5],
                                          version_handler->chip_uid[6], version_handler->chip_uid[7] );

        this->communication_manager->Log( "DEV EUI - %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n",
                                          version_handler->dev_eui[0], version_handler->dev_eui[1],
                                          version_handler->dev_eui[2], version_handler->dev_eui[3],
                                          version_handler->dev_eui[4], version_handler->dev_eui[5],
                                          version_handler->dev_eui[6], version_handler->dev_eui[7] );

        this->communication_manager->Log( "JOIN EUI - %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n",
                                          version_handler->join_eui[0], version_handler->join_eui[1],
                                          version_handler->join_eui[2], version_handler->join_eui[3],
                                          version_handler->join_eui[4], version_handler->join_eui[5],
                                          version_handler->join_eui[6], version_handler->join_eui[7] );

        this->communication_manager->Log( "PIN - %02X-%02X-%02X-%02X\n", version_handler->pin[0],
                                          version_handler->pin[1], version_handler->pin[2], version_handler->pin[3] );

        break;
    }
    case GUI_LAST_EVENT_START_DEMO_WIFI:
    {
        demo_manager->Start( DEMO_TYPE_WIFI );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_START_DEMO_GNSS_AUTONOMOUS:
    {
        demo_manager->Start( DEMO_TYPE_GNSS_AUTONOMOUS );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_START_DEMO_GNSS_ASSISTED:
    {
        demo_manager->Start( DEMO_TYPE_GNSS_ASSISTED );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_START_DEMO_TEMPERATURE:
    {
        demo_manager->Start( DEMO_TYPE_TEMPERATURE );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_START_DEMO_FILE_UPLOAD:
    {
        demo_manager->Start( DEMO_TYPE_FILE_UPLOAD );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_START_DEMO_TX_CW:
    {
        demo_manager->Start( DEMO_TYPE_TX_CW );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_START_DEMO_PER_TX:
    {
        demo_manager->Start( DEMO_TYPE_RADIO_PER_TX );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_START_DEMO_PER_RX:
    {
        demo_manager->Start( DEMO_TYPE_RADIO_PER_RX );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_START_DEMO_PING_PONG:
    {
        demo_manager->Start( DEMO_TYPE_RADIO_PING_PONG );
        this->run_demo = true;
        break;
    }
    case GUI_LAST_EVENT_STOP_DEMO:
    {
        demo_manager->Stop( );
        this->run_demo = false;
        break;
    }
    case GUI_LAST_EVENT_SEND:
    {
        this->communication_manager->EraseDataStored( );
        this->communication_manager->Store( this->version_handler );

        demo_type_t demo_type = demo_manager->GetType( );

        switch( demo_type )
        {
        case DEMO_TYPE_WIFI:
        case DEMO_TYPE_WIFI_COUNTRY_CODE:
        {
            const demo_wifi_scan_all_results_t* results = ( demo_wifi_scan_all_results_t* ) demo_manager->GetResults( );
            this->TransferResultToSerial( results );
            this->TransferResultToConnectivity( results );
            break;
        }

        case DEMO_TYPE_GNSS_AUTONOMOUS:
        case DEMO_TYPE_GNSS_ASSISTED:
        {
            const demo_gnss_all_results_t* results = ( demo_gnss_all_results_t* ) demo_manager->GetResults( );
            this->TransferResultToSerial( results );
            this->TransferResultToConnectivity( results );
            break;
        }
        default:
            break;
        }

        this->communication_manager->SendDataStoredToServer( );

        float         latitude              = 0;
        float         longitude             = 0;
        float         altitude              = 0;
        float         accuracy              = 0;
        char          geo_coding[64]        = { 0 };
        const uint8_t geo_coding_max_length = 64;

        const bool success = this->communication_manager->GetResults( latitude, longitude, altitude, accuracy,
                                                                      geo_coding, geo_coding_max_length );
        if( success == true )
        {
            GuiResultGeoLoc_t new_reverse_geo_loc;
            sscanf( geo_coding, "%[^,],%[^,],%s", new_reverse_geo_loc.street, new_reverse_geo_loc.city,
                    new_reverse_geo_loc.country );
            snprintf( new_reverse_geo_loc.latitude, GUI_RESULT_GEO_LOC_LATITUDE_LENGTH, "%.5f", latitude );
            snprintf( new_reverse_geo_loc.longitude, GUI_RESULT_GEO_LOC_LATITUDE_LENGTH, "%.5f", longitude );
            this->gui->UpdateReverseGeoCoding( new_reverse_geo_loc );
        }
        else
        {
            if( this->connectivity_manager->IsConnectable( ) == true )
            {
                // It is ok to fail here if there is a LoRaWan connectivity, as the message has been sent through
                // LoRaWan and not through the communication manager
                GuiResultGeoLoc_t new_reverse_geo_loc;
                // strncpy( new_reverse_geo_loc.country, "", GUI_RESULT_GEO_LOC_COUNTRY_LENGTH );
                snprintf( new_reverse_geo_loc.country, GUI_RESULT_GEO_LOC_COUNTRY_LENGTH, " " );
                snprintf( new_reverse_geo_loc.city, GUI_RESULT_GEO_LOC_CITY_LENGTH, "" );
                snprintf( new_reverse_geo_loc.street, GUI_RESULT_GEO_LOC_STREET_LENGTH, "See application server" );
                snprintf( new_reverse_geo_loc.latitude, GUI_RESULT_GEO_LOC_LATITUDE_LENGTH, "Added to stream" );
                snprintf( new_reverse_geo_loc.longitude, GUI_RESULT_GEO_LOC_LATITUDE_LENGTH, " " );
                this->gui->UpdateReverseGeoCoding( new_reverse_geo_loc );
            }
            else
            {
                // It is not ok to fail on the get result if on top of that there is no lorawan connectivity
                GuiResultGeoLoc_t new_reverse_geo_loc;
                strncpy( new_reverse_geo_loc.country, "Failure", GUI_RESULT_GEO_LOC_COUNTRY_LENGTH );
                snprintf( new_reverse_geo_loc.latitude, GUI_RESULT_GEO_LOC_LATITUDE_LENGTH, "XXX" );
                snprintf( new_reverse_geo_loc.longitude, GUI_RESULT_GEO_LOC_LATITUDE_LENGTH, "XXX" );
                this->gui->UpdateReverseGeoCoding( new_reverse_geo_loc );
            }
        }
        break;
    }
    case GUI_LAST_EVENT_UPDATE_DEMO_RADIO:
    {
        demo_radio_settings_t demo_settings;
        GuiRadioSetting_t     gui_settings;
        this->demo_manager->GetConfigRadio( &demo_settings );
        this->gui->GetRadioSettings( &gui_settings );
        this->ConvertSettingsFromGuiToDemo( &gui_settings, &demo_settings );
        this->demo_manager->UpdateConfigRadio( &demo_settings );
        break;
    }
    case GUI_LAST_EVENT_UPDATE_DEMO_WIFI:
    {
        demo_wifi_settings_t demo_settings;
        GuiWifiDemoSetting_t gui_settings;
        this->demo_manager->GetConfigWifi( &demo_settings );
        this->gui->GetWifiSettings( &gui_settings );
        this->ConvertSettingsFromGuiToDemo( &gui_settings, &demo_settings );
        this->demo_manager->UpdateConfigWifiScan( &demo_settings );
        break;
    }
    case GUI_LAST_EVENT_UPDATE_DEMO_GNSS_AUTONOMOUS:
    {
        demo_gnss_settings_t demo_settings;
        GuiGnssDemoSetting_t gui_settings;
        this->demo_manager->GetConfigAutonomousGnss( &demo_settings );
        this->gui->GetGnssAutonomousSettings( &gui_settings );
        this->ConvertSettingsFromGuiToDemo( &gui_settings, &demo_settings );
        this->demo_manager->UpdateConfigAutonomousGnss( &demo_settings );
        break;
    }
    case GUI_LAST_EVENT_UPDATE_DEMO_GNSS_ASSISTED:
    {
        demo_gnss_settings_t demo_settings;
        GuiGnssDemoSetting_t gui_settings;
        this->demo_manager->GetConfigAssistedGnss( &demo_settings );
        this->gui->GetGnssAssistedSettings( &gui_settings );
        this->ConvertSettingsFromGuiToDemo( &gui_settings, &demo_settings );
        this->demo_manager->UpdateConfigAssistedGnss( &demo_settings );
        break;
    }
    case GUI_LAST_EVENT_UPDATE_DEMO_GNSS_ASSISTANCE_POSITION:
    {
        environment_location_t          environment_location;
        GuiGnssDemoAssistancePosition_t assistance_position;

        this->gui->GetGnssAssistancePosition( &assistance_position );

        environment_location.latitude  = assistance_position.latitude;
        environment_location.longitude = assistance_position.longitude;
        environment_location.altitude  = 0;

        environment->SetLocation( environment_location );
        this->device->NotifyEnvironmentChange( );
        break;
    }
    default:
        break;
    }
}

void Supervisor::DemoRuntimeAndProcess( )
{
    switch( demo_manager->Runtime( ) )
    {
    case DEMO_STATUS_RUNNING:
    {
        if( this->demo_manager->HasIntermediateResults( ) )
        {
            this->TransfertDemoResultsToGui( );
        }
        break;
    }
    case DEMO_STATUS_TERMINATED:
    {
        demo_manager->Stop( );
        this->run_demo = false;
        this->TransfertDemoResultsToGui( );
        this->communication_manager->EventNotify( );
        break;
    }
    default:
    {
        break;
    }
    }
}

void Supervisor::CommunicationManagerRuntime( )
{
    this->communication_manager->Runtime( );
    CommunicationManagerHostType_t new_host_type = COMMUNICATION_MANAGER_NO_HOST;
    if( this->communication_manager->HasHostJustChanged( &new_host_type ) )
    {
        switch( new_host_type )
        {
        case COMMUNICATION_MANAGER_FIELD_TEST_HOST:
        {
            this->gui->HostConnectivityChange( true );
            break;
        }
        case COMMUNICATION_MANAGER_DEMO_HOST:
        {
            this->gui->HostConnectivityChange( true );
            break;
        }
        case COMMUNICATION_MANAGER_NO_HOST:
        case COMMUNICATION_MANAGER_UNKNOWN_HOST:
        {
            this->gui->HostConnectivityChange( false );
            break;
        }
        default:
        {
            break;
        }
        }
    }
    if( this->communication_manager->HasNewCommand( ) )
    {
        CommandEvent_t hci_event = this->communication_manager->FetchCommand( )->Execute( );
        switch( hci_event )
        {
        case COMMAND_NO_EVENT:
        {
            break;
        }
        case COMMAND_START_WIFI_SCAN_DEMO_EVENT:
        {
            demo_manager->Start( DEMO_TYPE_WIFI );
            this->run_demo = true;
            break;
        }
        case COMMAND_START_WIFI_COUNTRY_CODE_DEMO_EVENT:
        {
            demo_manager->Start( DEMO_TYPE_WIFI_COUNTRY_CODE );
            this->run_demo = true;
            break;
        }
        case COMMAND_START_GNSS_AUTONOMOUS_DEMO_EVENT:
        {
            demo_manager->Start( DEMO_TYPE_GNSS_AUTONOMOUS );
            this->run_demo = true;
            break;
        }
        case COMMAND_START_GNSS_ASSISTED_DEMO_EVENT:
        {
            demo_manager->Start( DEMO_TYPE_GNSS_ASSISTED );
            this->run_demo = true;
            break;
        }
        case COMMAND_STOP_DEMO_EVENT:
        {
            this->demo_manager->Stop( );
            this->run_demo = false;
            break;
        }
        case COMMAND_RESET_DEMO_EVENT:
        {
            this->demo_manager->Stop( );
            this->run_demo = false;
            this->device->Init( );
            this->demo_manager->Reset( );
        }
        }
    }
}

void Supervisor::NetworkConnectivityRuntimeAndProcess( )
{
    switch( this->connectivity_manager->Runtime( ) )
    {
    case NETWORK_CONNECTIVITY_STATUS_JOIN:
    {
        this->communication_manager->Log( "Network joined!\r\n" );
        GuiNetworkConnectivityStatus_t network_connectivity_status;
        network_connectivity_status.connectivity_state = GUI_CONNECTIVITY_STATUS_CONNECTED;
        network_connectivity_status.is_time_sync       = this->connectivity_manager->getTimeSyncState( );
        this->gui->NetworkConnectivityChange( &network_connectivity_status );
        break;
    }

    case NETWORK_CONNECTIVITY_STATUS_GOT_ALC_SYNC:
    {
        this->communication_manager->Log( "Got ALC sync!\r\n" );
        GuiNetworkConnectivityStatus_t network_connectivity_status;
        network_connectivity_status.connectivity_state = GUI_CONNECTIVITY_STATUS_CONNECTED;
        network_connectivity_status.is_time_sync       = this->connectivity_manager->getTimeSyncState( );
        this->gui->NetworkConnectivityChange( &network_connectivity_status );
        break;
    }
    case NETWORK_CONNECTIVITY_STATUS_LOST_ALC_SYNC:
    {
        this->communication_manager->Log( "Lost ALC sync!\r\n" );
        GuiNetworkConnectivityStatus_t network_connectivity_status;
        network_connectivity_status.connectivity_state = GUI_CONNECTIVITY_STATUS_CONNECTED;
        network_connectivity_status.is_time_sync       = this->connectivity_manager->getTimeSyncState( );
        this->gui->NetworkConnectivityChange( &network_connectivity_status );
        break;
    }
    case NETWORK_CONNECTIVITY_STATUS_HAS_DOWNLINK:
    {
        this->communication_manager->Log( "Received dnlink?\r\n" );
        network_connectivity_downlink_t downlink = { };
        if( this->connectivity_manager->FetchNewDownlink( &downlink ) == true )
        {
            this->communication_manager->Log( "Received downlink:\r\n" );
            this->communication_manager->Log( "  -> RSSI: %i dBm\r\n", downlink.rssi );
            this->communication_manager->Log( "  -> SNR: %i dB\r\n", downlink.snr );
            this->communication_manager->Log( "  -> Port: %u\r\n", downlink.port );
            this->communication_manager->Log( "  -> Payload (%u bytes):\r\n    ", downlink.buffer_size );
            for( uint8_t index_buffer = 0; index_buffer < downlink.buffer_size; index_buffer++ )
            {
                this->communication_manager->Log( "%02x", downlink.buffer[index_buffer] );
            }
            this->communication_manager->Log( "\n\r" );
        }
        else
        {
            this->communication_manager->Log( "Something went wrong\r\n" );
        }

        // 1. Check if this downlink is to be handled directly by the device based on the port
        if( this->device->IsLorawanPortForDeviceManagement( downlink.port ) == true )
        {
            // 2. If so: ask the device to handle it
            this->device->HandleLorawanDeviceManagement( downlink.port, downlink.buffer, downlink.buffer_size );
        }

        break;
    }
    default:
        break;
    }
}

void Supervisor::DeviceRuntime( )
{
    switch( this->device->Runtime( ) )
    {
    case DEVICE_EVENT_ASSISTANCE_LOCATION_UPDATED:
    {
        DeviceAssistedLocation_t        assisted_location       = { 0 };
        GuiGnssDemoAssistancePosition_t gui_assistance_position = { 0 };
        this->device->FetchAssistanceLocation( &assisted_location );

        gui_assistance_position.latitude       = assisted_location.latitude;
        gui_assistance_position.longitude      = assisted_location.longitude;
        gui_assistance_position.set_by_network = true;
        this->gui->SetGnssAssistancePosition( &gui_assistance_position );
        break;
    }
    case DEVICE_EVENT_APPLICATION_SERVER:
    {
        ApplicationServerEvent_t app_serv_event = APPLICATION_SERVER_NO_EVENT;
        this->device->FetchLastApplicationServerEvent( &app_serv_event );
        switch( app_serv_event )
        {
        case APPLICATION_SERVER_LED_ON:
        {
            this->communication_manager->Log( "Turn on LED\n" );
            this->gui->FakeLedStateChange( true );
            break;
        }
        case APPLICATION_SERVER_LED_OFF:
        {
            this->communication_manager->Log( "Turn off LED\n" );
            this->gui->FakeLedStateChange( false );
            break;
        }
        case APPLICATION_SERVER_LED_TOGGLE:
        {
            this->communication_manager->Log( "Toggle LED\n" );
            this->gui->FakeLedStateToggle( );
            break;
        }
        default:
        {
            break;
        }
        }
        break;
    }
    case DEVICE_EVENT_NONE:
    default:
    {
        break;
    }
    }
}

void Supervisor::InterruptHandlerGui( bool is_down )
{
    Supervisor::is_gui_interrupt_raised = true;
    Supervisor::is_down_gui_interrupt   = is_down;
}

void Supervisor::InterruptHandlerDemo( ) { Supervisor::is_demo_interrupt_raised = true; }

void Supervisor::InterruptionRuntime( )
{
    if( Supervisor::is_demo_interrupt_raised == true )
    {
        Supervisor::is_demo_interrupt_raised = false;
        InterruptionInterface* interruption  = 0;
        while( this->device->FetchInterrupt( &interruption ) )
        {
            const bool interrupt_to_propagate_to_connectivity =
                interruption->is_radio_interruption( ) && this->has_connectivity;
            if( interrupt_to_propagate_to_connectivity == true )
            {
                this->connectivity_manager->InterruptHandler( interruption );
            }
            if( this->run_demo == true )
            {
                this->demo_manager->InterruptHandler( interruption );
            }
        }
    }

    if( Supervisor::is_gui_interrupt_raised == true )
    {
        Supervisor::is_gui_interrupt_raised = false;
        Gui::InterruptHandler( Supervisor::is_down_gui_interrupt );
    }
}

bool Supervisor::CanEnterLowPower( ) const
{
    bool can_enter_low_power = true;
    // DemoInterface*                   running_demo        = NULL;
    // DemoContainerSelectedType_t type                = DEMO_TYPE_NONE;
    // type = this->demo_container.GetSelectedDemoAndType( running_demo );

    // can_enter_low_power &= !Supervisor::is_demo_interrupt_raised;
    // can_enter_low_power &= !this->gui->HasRefreshPending( );

    // if( type != DEMO_TYPE_NONE )
    // {
    //     can_enter_low_power &= ( ( running_demo->IsStarted( ) &&
    //                                running_demo->IsWaitingForInterrupt( )
    //                                )
    //                                ||
    //                              ( !running_demo->IsStarted( ) ) );
    // }
    return can_enter_low_power;
}

void Supervisor::EnterWaitForInterrupt( ) const
{
    while( !Supervisor::is_demo_interrupt_raised )
    {
    }
}

void Supervisor::TransfertDemoResultsToGui( )
{
    demo_type_t demo_type = demo_manager->GetType( );

    switch( demo_type )
    {
    case DEMO_TYPE_WIFI:
    case DEMO_TYPE_WIFI_COUNTRY_CODE:
        this->TransferResultToGui( ( ( demo_wifi_scan_all_results_t* ) demo_manager->GetResults( ) ) );
        break;

    case DEMO_TYPE_GNSS_AUTONOMOUS:
        this->TransferResultToGui( ( ( demo_gnss_all_results_t* ) demo_manager->GetResults( ) ) );
        break;

    case DEMO_TYPE_GNSS_ASSISTED:
        this->TransferResultToGui( ( ( demo_gnss_all_results_t* ) demo_manager->GetResults( ) ) );
        break;

    case DEMO_TYPE_RADIO_PING_PONG:
        this->TransferResultToGui( ( demo_ping_pong_results_t* ) demo_manager->GetResults( ) );
        break;

    case DEMO_TYPE_RADIO_PER_TX:
    case DEMO_TYPE_RADIO_PER_RX:
        this->TransferResultToGui( ( demo_radio_per_results_t* ) demo_manager->GetResults( ) );
        break;

    case DEMO_TYPE_TEMPERATURE:
        this->TransferResultToGui( ( demo_modem_temperature_results_t* ) demo_manager->GetResults( ) );
        break;

    case DEMO_TYPE_FILE_UPLOAD:
        this->TransferResultToGui( ( demo_modem_file_upload_results_t* ) demo_manager->GetResults( ) );
        break;

    default:
        this->communication_manager->Log( "Error: unknown demo type in result handling: 0x%x\n", demo_type );
    }
}

void Supervisor::TransferResultToGui( const demo_wifi_scan_all_results_t* result )
{
    GuiWifiResult_t guiWifiResults;

    guiWifiResults.status         = GUI_DEMO_STATUS_OK;
    guiWifiResults.powerConsTotal = result->global_consumption_uas;
    guiWifiResults.nbMacAddrTotal = result->nbrResults;
    guiWifiResults.timingTotal    = ( result->timings.demodulation_us + result->timings.rx_capture_us +
                                   result->timings.rx_correlation_us + result->timings.rx_detection_us ) /
                                 1000;
    guiWifiResults.error = result->error;

    for( uint8_t id = 0; id < GUI_WIFI_CHANNELS; id++ )
    {
        guiWifiResults.typeB.channel[id].nbMacAddr = 0;
        guiWifiResults.typeG.channel[id].nbMacAddr = 0;
    }

    for( uint8_t index = 0; index < result->nbrResults; index++ )
    {
        const demo_wifi_mac_address_t* mac = &result->results[index].mac_address;
        GuiWifiMacAddress_t            strMac;
        GuiWifiResultChannel_t*        chan;

        const demo_wifi_channel_t     wifi_chan = result->results[index].channel;
        const demo_wifi_signal_type_t wifi_type = result->results[index].type;

        snprintf( strMac, GUI_WIFI_STRING_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x", ( *mac )[0], ( *mac )[1],
                  ( *mac )[2], ( *mac )[3], ( *mac )[4], ( *mac )[5] );

        if( wifi_type == DEMO_WIFI_TYPE_B )
        {
            chan = &guiWifiResults.typeB.channel[wifi_chan - 1];
        }
        else
        {
            chan = &guiWifiResults.typeG.channel[wifi_chan - 1];
        }

        if( chan->nbMacAddr < GUI_MAX_WIFI_MAC_ADDRESSES )
        {
            memcpy( chan->data[chan->nbMacAddr].macAddr, strMac, GUI_WIFI_STRING_LENGTH );
            snprintf( chan->data[chan->nbMacAddr].countryCode, GUI_WIFI_STRING_COUNTRY_CODE_LENGTH, "%c%c",
                      result->results[index].country_code[0], result->results[index].country_code[1] );
            chan->data[chan->nbMacAddr].rssi = result->results[index].rssi;
            chan->nbMacAddr++;
        }
    }

    this->gui->UpdateWifiDemoResult( guiWifiResults );
}

void Supervisor::TransferResultToGui( const demo_gnss_all_results_t* result )
{
    GuiGnssResult_t guiGnssResults;

    guiGnssResults.status          = Supervisor::DemoGnssErrorCodeToGuiStatus( result->error );
    guiGnssResults.powerConsTotal  = result->consumption_uas;
    guiGnssResults.timingTotal     = result->timings.computation_ms + result->timings.radio_ms;
    guiGnssResults.nb_satellites   = result->nb_result;
    guiGnssResults.almanac_age     = result->almanac_age_days;
    guiGnssResults.almanac_too_old = result->almanac_too_old;

    for( uint8_t index = 0; ( index < result->nb_result ) && ( index < GUI_MAX_GNSS_SATELLITES ); index++ )
    {
        switch( result->result[index].constellation )
        {
        case DEMO_GNSS_CONSTELLATION_GPS:
            guiGnssResults.satellite_ids[index].constellation = GUI_GNSS_CONSTELLATION_GPS;
            break;
        case DEMO_GNSS_CONSTELLATION_BEIDOU:
            guiGnssResults.satellite_ids[index].constellation = GUI_GNSS_CONSTELLATION_BEIDOU;
            break;
        default:
            break;
        }
        guiGnssResults.satellite_ids[index].satellite_id = result->result[index].satellite_id;
        guiGnssResults.satellite_ids[index].snr          = result->result[index].snr;
    }

    this->gui->UpdateGnssDemoResult( guiGnssResults );
}

void Supervisor::TransferResultToGui( const demo_ping_pong_results_t* result )
{
    GuiRadioPingPongResult_t guiResult;

    guiResult.count_tx                = result->count_tx;
    guiResult.count_rx_correct_packet = result->count_rx_correct_packet;
    guiResult.count_rx_timeout        = result->count_rx_timeout;
    guiResult.count_rx_wrong_packet   = result->count_rx_wrong_packet;

    this->gui->UpdateRadioPingPongResult( guiResult );
}

void Supervisor::TransferResultToGui( const demo_radio_per_results_t* result )
{
    GuiRadioPerResult_t guiResult;

    guiResult.count_tx                = result->count_tx;
    guiResult.count_rx_correct_packet = result->count_rx_correct_packet;
    guiResult.count_rx_timeout        = result->count_rx_timeout;
    guiResult.count_rx_wrong_packet   = result->count_rx_wrong_packet;

    this->gui->UpdateRadioPerResult( guiResult );
}

void Supervisor::TransferResultToGui( const demo_modem_temperature_results_t* result )
{
    GuiTemperatureResult_t guiResult = { };

    guiResult.temperature = result->temperature;
    guiResult.sent        = result->sent;

    this->gui->UpdateTemperatureResult( guiResult );
}

void Supervisor::TransferResultToGui( const demo_modem_file_upload_results_t* result )
{
    GuiFileUploadResult_t guiResult = { .success =
                                            ( result->termination_status == DEMO_MODEM_FILE_UPLOAD_TERMINATED_SUCCESS ),
                                        .terminated =
                                            ( result->termination_status != DEMO_MODEM_FILE_UPLOAD_NOT_TERMINATED ) };

    this->gui->UpdateFileUploadResult( guiResult );
}

void Supervisor::TransferResultToConnectivity( const demo_wifi_scan_all_results_t* result )
{
    uint8_t  buffer[255] = { 0 };
    uint16_t buffer_size = 0;
    ConnectivityConversions::copy_demo_result_to_tlv_payload_buffer( *result, buffer, &buffer_size, 255 );

    this->communication_manager->Log( "Sending buffer (%u bytes): ", buffer_size );
    const network_connectivity_cmd_status_t send_status = this->connectivity_manager->Send( buffer, buffer_size );
    for( uint16_t index_buffer = 0; index_buffer < buffer_size; index_buffer++ )
    {
        this->communication_manager->Log( "0x%02x ", buffer[index_buffer] );
    }
    this->communication_manager->Log( "\r\n" );
    this->communication_manager->Log( "Send status code: 0x%x\r\n", send_status );
}

void Supervisor::TransferResultToConnectivity( const demo_gnss_all_results_t* result )
{
    uint8_t  buffer[255] = { 0 };
    uint16_t buffer_size = 0;
    ConnectivityConversions::copy_demo_result_to_tlv_payload_buffer( *result, buffer, &buffer_size, 255 );

    this->communication_manager->Log( "Sending buffer (%u bytes): ", buffer_size );
    const network_connectivity_cmd_status_t send_status = this->connectivity_manager->Send( buffer, buffer_size );
    for( uint16_t index_buffer = 0; index_buffer < buffer_size; index_buffer++ )
    {
        this->communication_manager->Log( "0x%02x ", buffer[index_buffer] );
    }
    this->communication_manager->Log( "\r\n" );
    this->communication_manager->Log( "Send status code: 0x%x\r\n", send_status );
}

GuiDemoStatus_t Supervisor::DemoGnssErrorCodeToGuiStatus( const demo_gnss_error_t error_code )
{
    GuiDemoStatus_t gui_status = GUI_DEMO_STATUS_KO_UNKNOWN;
    switch( error_code )
    {
    case DEMO_GNSS_BASE_NO_ERROR:
    {
        gui_status = GUI_DEMO_STATUS_OK;
        break;
    }
    case DEMO_GNSS_BASE_ERROR_NO_DATE:
    case DEMO_GNSS_BASE_ERROR_NO_LOCATION:
    {
        gui_status = GUI_DEMO_STATUS_KO_HOST_NOT_CONNECTED;
        break;
    }
    case DEMO_GNSS_BASE_ERROR_ALMANAC_TOO_OLD:
    {
        gui_status = GUI_DEMO_STATUS_KO_ALMANAC_TOO_OLD;
        break;
    }
    case DEMO_GNSS_BASE_NAV_MESSAGE_TOO_LONG:
    {
        gui_status = GUI_DEMO_STATUS_KO_NAV_MESSAGE_TOO_LONG;
        break;
    }
    case DEMO_GNSS_BASE_ERROR_NO_SATELLITE:
    {
        // It is ok if no satellites have been detected
        gui_status = GUI_DEMO_STATUS_OK;
        break;
    }
    case DEMO_GNSS_BASE_ERROR_UNKNOWN:
    {
        gui_status = GUI_DEMO_STATUS_KO_UNKNOWN;
        break;
    }
    }
    return gui_status;
}

void Supervisor::TransferResultToSerial( const demo_wifi_scan_all_results_t* result )
{
    this->communication_manager->Store( *result );
}

void Supervisor::TransferResultToSerial( const demo_gnss_all_results_t* result )
{
    const uint32_t delay_capture_s = this->environment->GetLocalTimeSeconds( ) - result->local_instant_measurement;

    this->communication_manager->Store( *result, delay_capture_s );
}

bool Supervisor::HasPendingInterrupt( ) const { return Supervisor::is_demo_interrupt_raised; }

const version_handler_t* Supervisor::GetVersionHandler( ) const { return &this->version_handler; }
