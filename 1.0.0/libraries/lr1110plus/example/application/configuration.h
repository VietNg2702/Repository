/**
 * @file      configuration.h
 *
 * @brief     LR1110 EVK configuration.
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

#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <stdint.h>
#define LR1110_NSS_PORT 		3
#define LR1110_NSS_PIN 			0
#define LR1110_RESET_PORT 		0
#define LR1110_RESET_PIN 		0
#define LR1110_IRQ_PORT 		6
#define LR1110_IRQ_PIN 			3
#define LR1110_IRQ_DUM_PORT 	6
#define LR1110_IRQ_DUM_PIN 		4
#define LR1110_BUSY_PORT 		2
#define LR1110_BUSY_PIN 		5

#define LR1110_LED_SCAN_PORT 	6
#define LR1110_LED_SCAN_PIN 	2
#define LR1110_LED_TX_PORT 		0
#define LR1110_LED_TX_PIN 		4
#define LR1110_LED_RX_PORT 		0
#define LR1110_LED_RX_PIN 		5

#define LR1110_LNA_PORT 		0
#define LR1110_LNA_PIN 			3

#define DISPLAY_NSS_PORT 		0
#define DISPLAY_NSS_PIN 		15
#define DISPLAY_DC_PORT 		3
#define DISPLAY_DC_PIN 			2

#define TOUCH_IRQ_PORT 			2
#define TOUCH_IRQ_PIN 			4

#define ACCELERATOR_IRQ_PORT 	4 // IRQ9
#define ACCELERATOR_IRQ_PIN 	9

#define ANTENNA_SWITCH_CTRL_PORT ( 3 )
#define ANTENNA_SWITCH_CTRL_PIN (14 )
#define ANTENNA_SWITCH_N_CTRL_PORT ( 3 )
#define ANTENNA_SWITCH_N_CTRL_PIN ( 15 )

#define BUTTON_BLUE_PORT ( 5 )
#define BUTTON_BLUE_PIN ( 9 )  // P509 .. SW1

#define DCDC_EN_PORT 			2
#define DCDC_EN_PIN 			9

//#define FLASH_NSS_PORT GPIOB
//#define FLASH_NSS_PIN LL_GPIO_PIN_10

typedef struct configuration
{
    GPIO_TypeDef* port;
    uint32_t      pin;
} gpio_t;

typedef struct
{
    SPI_TypeDef* spi;
    gpio_t       nss;
    gpio_t       reset;
    gpio_t       irq;
    gpio_t       busy;
} radio_t;

#endif
