/**
 * @file      system_gpio.c
 *
 * @brief     MCU GPIO-related functions
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

#include "system_gpio.h"
#include "RE01_256KB.h"

static void system_gpio_init_input(uint16_t port, uint16_t pin, system_gpio_interrupt_t irq_mode )
{
	uint32_t *pAddr;
	uint32_t tempReg;
    tempReg = 0;

	pAddr = (uint32_t *) (PFS_BASE + (0x40 * port) + (0x04 * (0x0f & pin) ));

    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    *pAddr = tempReg;

    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

    if (irq_mode == SYSTEM_GPIO_RISING || irq_mode == SYSTEM_GPIO_FALLING || irq_mode == SYSTEM_GPIO_BOTH) 
    {
        GpioMcuSetInterrupt(port,  pin, irq_mode, 1);
    }

    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    tempReg = *pAddr;
    if (irq_mode == SYSTEM_GPIO_RISING) {
        tempReg |= 0x05 << 12;
    } else if (irq_mode == SYSTEM_GPIO_FALLING) {
        tempReg |= 0x06 << 12;
    } else if (irq_mode == SYSTEM_GPIO_BOTH) {
        tempReg |= 0x07 << 12;
    }
    *pAddr = tempReg;
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}

static void system_gpio_init_output( uint16_t port, uint16_t pin, uint8_t value )
{
    uint32_t *pAddr = (uint32_t *) (PFS_BASE + (0x40 * port) + (0x04 * (0x0f & pin) ));

    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    *pAddr = 0x04 | (value & 0x01);
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}

void system_gpio_init( void )
{
    system_gpio_init_output( LR1110_LED_SCAN_PORT, LR1110_LED_SCAN_PIN, 0 );
    system_gpio_init_output( LR1110_LED_TX_PORT, LR1110_LED_TX_PIN, 0 );
    system_gpio_init_output( LR1110_LED_RX_PORT, LR1110_LED_RX_PIN, 0 );
    system_gpio_init_output( LR1110_RESET_PORT, LR1110_RESET_PIN, 1 );
    system_gpio_init_output( LR1110_NSS_PORT, LR1110_NSS_PIN, 1 );
    system_gpio_init_input( LR1110_IRQ_PORT, LR1110_IRQ_PIN, SYSTEM_GPIO_NO_INTERRUPT );
    system_gpio_init_input( LR1110_IRQ_DUM_PORT, LR1110_IRQ_DUM_PIN, SYSTEM_GPIO_RISING );
    system_gpio_init_input( LR1110_BUSY_PORT, LR1110_BUSY_PIN, SYSTEM_GPIO_NO_INTERRUPT );
    system_gpio_init_input( TOUCH_IRQ_PORT, TOUCH_IRQ_PIN, SYSTEM_GPIO_RISING );
    system_gpio_init_output( DISPLAY_NSS_PORT, DISPLAY_NSS_PIN, 1 );
    system_gpio_init_output( DISPLAY_DC_PORT, DISPLAY_DC_PIN, 0 );
    system_gpio_init_input( TOUCH_IRQ_PORT, TOUCH_IRQ_PIN, SYSTEM_GPIO_RISING ); // PORT204 IRQ7 
    system_gpio_init_input( ACCELERATOR_IRQ_PORT, ACCELERATOR_IRQ_PIN, SYSTEM_GPIO_RISING );// PORT409 IRQ9 
    system_gpio_init_output( LR1110_LNA_PORT, LR1110_LNA_PIN, 0 );
    system_gpio_init_output( DCDC_EN_PORT, DCDC_EN_PIN, 1 );
    system_gpio_init_input( BUTTON_BLUE_PORT, BUTTON_BLUE_PIN, SYSTEM_GPIO_NO_INTERRUPT );
}

void system_gpio_init_direction_state( const gpio_t gpio, const system_gpio_pin_direction_t direction,
                                       const system_gpio_pin_state_t state )
{
    switch( direction )
    {
    case SYSTEM_GPIO_PIN_DIRECTION_INPUT:
        system_gpio_init_input( gpio.port, gpio.pin, SYSTEM_GPIO_NO_INTERRUPT );
        break;
    case SYSTEM_GPIO_PIN_DIRECTION_OUTPUT:
        system_gpio_init_output( gpio.port, gpio.pin, state );
        break;
    default:
        break;
    }
}

void system_gpio_init_irq( const gpio_t gpio, const system_gpio_interrupt_t interrupt )
{
    system_gpio_init_input( gpio.port, gpio.pin, interrupt );
}
void system_gpio_set_pin_state( gpio_t gpio, const system_gpio_pin_state_t state )
{
    uint8_t pin = gpio.pin;
    uint8_t port = gpio.port;
    uint32_t *pAddr = (uint32_t *) (PORT0_BASE + (0x20 * port) ); 

    if (state)
    {
        *pAddr |= 0x1 << (pin + 16);
    }
    else
    {
        *pAddr &= ~( 0x1 << (pin +16) );
    }
}

system_gpio_pin_state_t system_gpio_get_pin_state( gpio_t gpio )
{
    uint8_t pin = gpio.pin;
    uint8_t port = gpio.port;

    uint32_t *pAddr = (uint32_t *) (PORT0_BASE + (0x20 * port) + 4); 

    uint32_t value = *pAddr & (0x1 << pin); 

    return (value > 0? 1 : 0);
}

void system_gpio_wait_for_state( gpio_t gpio, system_gpio_pin_state_t state )
{
	system_gpio_pin_state_t current_state;
	for (;;) 
    {
    	current_state = system_gpio_get_pin_state(gpio );
		if (current_state == state) {break;}
	}
}
