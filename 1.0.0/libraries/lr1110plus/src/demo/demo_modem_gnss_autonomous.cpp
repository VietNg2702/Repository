/**
 * @file      demo_modem_gnss_autonomous.cpp
 *
 * @brief     Implementation of the GNSS autonomous demonstration for modem.
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

#include "demo_modem_gnss_autonomous.h"
#include "lr1110_modem_gnss.h"

DemoModemGnssAutonomous::DemoModemGnssAutonomous( DeviceModem* device, SignalingInterface* signaling,
                                                  EnvironmentInterface*     environment,
                                                  AntennaSelectorInterface* antenna_selector, TimerInterface* timer,
                                                  CommunicationInterface* communication_interface )
    : DemoModemGnssInterface( device, signaling, environment, antenna_selector, timer, communication_interface )
{
}

DemoModemGnssAutonomous::~DemoModemGnssAutonomous( ) {}

lr1110_modem_response_code_t DemoModemGnssAutonomous::CallScan( )
{
    const lr1110_modem_response_code_t call_response_code = lr1110_modem_gnss_scan_autonomous(
        this->device->GetRadio( ), DemoModemGnssInterface::ModemSearchModeFromDemo( this->GetSettings( ).option ),
        LR1110_MODEM_GNSS_BIT_CHANGE_MASK | LR1110_MODEM_GNSS_DOPPLER_MASK | LR1110_MODEM_GNSS_PSEUDO_RANGE_MASK,
        this->GetSettings( ).nb_satellites );

    return call_response_code;
}
