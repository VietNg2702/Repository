/**
 * @file      guiTestGnss.cpp
 *
 * @brief     Implementation of the gui test GNSS page.
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

#include "guiTestGnss.h"

#define TMP_BUFFERS_REFRESH_LENGTH ( 30 )
#define TMP_BUFFER_REVERSE_GEO_LOC_REFRESH_LENGTH ( 80 )

GuiTestGnss::GuiTestGnss( const GuiGnssResult_t* results, guiPageType_t pageType, bool at_least_one_scan_done )
    : GuiCommon( pageType ), _results( results )
{
    switch( pageType )
    {
    case GUI_PAGE_GNSS_AUTONOMOUS_TEST:
        this->createHeaderWithIcons( "GNSS AUTONOMOUS" );
        break;
    case GUI_PAGE_GNSS_ASSISTED_TEST:
        this->createHeaderWithIcons( "GNSS ASSISTED" );
        break;
    default:
        break;
    }

    this->createActionButton( &( this->btn_start_stop ), "START", GuiTestGnss::callback, GUI_BUTTON_POS_CENTER, -60,
                              true );

    this->createActionButton( &( this->btn_results ), "RESULTS", GuiTestGnss::callback, GUI_BUTTON_POS_LEFT, -60,
                              false );

    this->createActionButton( &( this->btn_send ), "SEND", GuiTestGnss::callback, GUI_BUTTON_POS_RIGHT, -60, false );

    this->createActionButton( &( this->btn_back ), "BACK", GuiTestGnss::callback, GUI_BUTTON_POS_CENTER, -5, true );

    this->createActionButton( &( this->btn_config ), "CONFIG", GuiTestGnss::callback, GUI_BUTTON_POS_RIGHT, -5, true );

    if( pageType == GUI_PAGE_GNSS_ASSISTED_TEST )
    {
        this->createActionButton( &( this->btn_assist ), "ASSIST", GuiTestGnss::callback, GUI_BUTTON_POS_LEFT, -5,
                                  true );
    }

    this->createInfoFrame( &( this->info_frame ), &( this->lbl_info_frame_1 ), "PRESS START TO BEGIN",
                           &( this->lbl_info_frame_2 ), "", &( this->lbl_info_frame_3 ), "" );

    if( at_least_one_scan_done == true )
    {
        this->refresh( );
    }

    lv_scr_load( this->screen );
}

GuiTestGnss::~GuiTestGnss( ) {}

void GuiTestGnss::start( )
{
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );

    lv_label_set_text( this->lbl_info_frame_1, "SCAN IN PROGRESS..." );
    lv_label_set_text( this->lbl_info_frame_2, "" );
    lv_label_set_text( this->lbl_info_frame_3, "" );

    lv_btn_set_state( this->btn_send, LV_BTN_STATE_INA );
    lv_btn_set_state( this->btn_results, LV_BTN_STATE_INA );
}

void GuiTestGnss::refresh( )
{
    char buffer_1[TMP_BUFFERS_REFRESH_LENGTH]                = { 0 };
    char buffer_2[TMP_BUFFERS_REFRESH_LENGTH]                = { 0 };
    char buffer_3[TMP_BUFFER_REVERSE_GEO_LOC_REFRESH_LENGTH] = { 0 };

    if( _results->status != GUI_DEMO_STATUS_OK )
    {
        lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_error ) );

        snprintf( buffer_1, TMP_BUFFERS_REFRESH_LENGTH, "Error" );

        switch( _results->status )
        {
        case GUI_DEMO_STATUS_KO_HOST_NOT_CONNECTED:
            snprintf( buffer_2, TMP_BUFFERS_REFRESH_LENGTH, "Start host application" );
            break;
        case GUI_DEMO_STATUS_KO_ALMANAC_TOO_OLD:
            snprintf( buffer_2, TMP_BUFFERS_REFRESH_LENGTH, "Almanac too old" );
            break;
        case GUI_DEMO_STATUS_KO_NAV_MESSAGE_TOO_LONG:
            snprintf( buffer_2, TMP_BUFFERS_REFRESH_LENGTH, "NAV message too long" );
            break;
        default:
            snprintf( buffer_2, TMP_BUFFERS_REFRESH_LENGTH, "Unknown error" );
            break;
        }
    }
    else
    {
        lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ok ) );

        if( _results->nb_satellites > 1 )
        {
            snprintf( buffer_1, TMP_BUFFERS_REFRESH_LENGTH, "%i satellites found", _results->nb_satellites );
        }
        else
        {
            snprintf( buffer_1, TMP_BUFFERS_REFRESH_LENGTH, "%i satellite found", _results->nb_satellites );
        }

        if( _results->almanac_too_old )
        {
            snprintf( buffer_2, TMP_BUFFERS_REFRESH_LENGTH, "ALMANACS NEED UPDATE" );
        }
        else
        {
            snprintf( buffer_2, TMP_BUFFERS_REFRESH_LENGTH, "%d ms / %.3f uAh", _results->timingTotal,
                      GuiCommon::convertConsoToUah( _results->powerConsTotal ) );
        }

        if( _results->nb_satellites != 0 )
        {
            lv_btn_set_state( this->btn_results, LV_BTN_STATE_REL );
            if( GuiCommon::_is_host_connected == true )
            {
                lv_btn_set_state( this->btn_send, LV_BTN_STATE_REL );
            }
        }

        if( strlen( _results->reverse_geo_loc.latitude ) != 0 )
        {
            if( strlen( _results->reverse_geo_loc.country ) != 0 )
            {
                snprintf( buffer_3, TMP_BUFFER_REVERSE_GEO_LOC_REFRESH_LENGTH, "%s %s\n%s - %s\n%s",
                          _results->reverse_geo_loc.latitude, _results->reverse_geo_loc.longitude,
                          _results->reverse_geo_loc.country, _results->reverse_geo_loc.city,
                          _results->reverse_geo_loc.street );
            }
            else
            {
                snprintf( buffer_3, TMP_BUFFER_REVERSE_GEO_LOC_REFRESH_LENGTH, "Lat.: %s\nLon.: %s",
                          _results->reverse_geo_loc.latitude, _results->reverse_geo_loc.longitude );
            }
        }
    }

    lv_label_set_text( this->lbl_info_frame_1, buffer_1 );
    lv_label_set_text( this->lbl_info_frame_2, buffer_2 );
    lv_label_set_text( this->lbl_info_frame_3, buffer_3 );

    this->updateButtons( );
}

void GuiTestGnss::propagateHostConnectivityStateChange( ) { this->updateButtons( ); }

void GuiTestGnss::propagateNetworkConnectivityStateChange( ) { this->updateButtons( ); }

void GuiTestGnss::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiTestGnss* self = ( GuiTestGnss* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_start_stop )
        {
            switch( self->_pageType )
            {
            case GUI_PAGE_GNSS_AUTONOMOUS_TEST:
                GuiCommon::_event = GUI_EVENT_START_GNSS_AUTONOMOUS;
                break;
            case GUI_PAGE_GNSS_ASSISTED_TEST:
                GuiCommon::_event = GUI_EVENT_START_GNSS_ASSISTED;
                break;
            default:
                break;
            }
        }
        else if( obj == self->btn_results )
        {
            GuiCommon::_event = GUI_EVENT_RESULTS;
        }
        else if( obj == self->btn_send )
        {
            GuiCommon::_event = GUI_EVENT_SEND;
        }
        else if( obj == self->btn_config )
        {
            GuiCommon::_event = GUI_EVENT_CONFIG;
        }
        else if( obj == self->btn_assist )
        {
            GuiCommon::_event = GUI_EVENT_ASSISTANCE_POSITION;
        }
    }
}

void GuiTestGnss::updateButtons( )
{
    if( ( ( GuiCommon::_network_connectivity_status.connectivity_state == GUI_CONNECTIVITY_STATUS_CONNECTED ) ||
          ( GuiCommon::_is_host_connected == true ) ) &&
        ( _results->nb_satellites > 0 ) )
    {
        lv_btn_set_state( this->btn_send, LV_BTN_STATE_REL );
    }
    else
    {
        lv_btn_set_state( this->btn_send, LV_BTN_STATE_INA );
    }
}
