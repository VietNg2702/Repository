/**
 * @file      command_interface.h
 *
 * @brief     Definitions of the HCI command interface.
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

#ifndef __COMMAND_INTERFACE_H__
#define __COMMAND_INTERFACE_H__

#include <stdint.h>

typedef enum
{
    COMMAND_BUILD_STATUS_OK,
    COMMAND_BUILD_STATUS_ERROR,
} CommandBuildStatus_t;

typedef enum
{
    COMMAND_NO_EVENT,
    COMMAND_START_WIFI_SCAN_DEMO_EVENT,
    COMMAND_START_WIFI_COUNTRY_CODE_DEMO_EVENT,
    COMMAND_START_GNSS_AUTONOMOUS_DEMO_EVENT,
    COMMAND_START_GNSS_ASSISTED_DEMO_EVENT,
    COMMAND_STOP_DEMO_EVENT,
    COMMAND_RESET_DEMO_EVENT,
} CommandEvent_t;

class CommandInterface
{
   public:
    virtual uint16_t       GetComCode( )                                                             = 0;
    virtual bool           ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size ) = 0;
    virtual CommandEvent_t Execute( )                                                                = 0;
};

#endif  // __COMMAND_INTERFACE_H__
