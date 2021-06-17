#ifndef _SYSTEM_GPIO_H
#define _SYSTEM_GPIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "configuration.h"
#include "RE01_256KB.h"
#include "system_it.h"
#include "system_gpio.h"
#include "r_system_api.h"

typedef struct configuration
{
	uint8_t port;
	uint8_t pin;
} gpio_t;

typedef enum
{
    SYSTEM_GPIO_NO_INTERRUPT = 0,
    SYSTEM_GPIO_RISING,
    SYSTEM_GPIO_FALLING,
    SYSTEM_GPIO_BOTH,
} system_gpio_interrupt_t;

typedef enum
{
    SYSTEM_GPIO_PIN_ANTENNA_SELECTOR_CTRL,
    SYSTEM_GPIO_PIN_ANTENNA_SELECTOR_N_CTRL,
    SYSTEM_GPIO_PIN_LED_SCAN,
} system_gpio_pin_t;

typedef enum
{
    SYSTEM_GPIO_PIN_STATE_LOW,
    SYSTEM_GPIO_PIN_STATE_HIGH,
} system_gpio_pin_state_t;

typedef enum
{
    SYSTEM_GPIO_PIN_DIRECTION_INPUT,
    SYSTEM_GPIO_PIN_DIRECTION_OUTPUT,
} system_gpio_pin_direction_t;

#ifdef __cplusplus
extern "C" {
#endif

void system_gpio_init( void );
void system_gpio_init_direction_state( const gpio_t gpio, const system_gpio_pin_direction_t direction, const system_gpio_pin_state_t state );
void system_gpio_init_input(uint16_t port, uint16_t pin, system_gpio_interrupt_t irq_mode );
void system_gpio_init_output( uint16_t port, uint16_t pin, uint8_t value );

void system_gpio_set_pin_state( gpio_t gpio, const system_gpio_pin_state_t state )  __attribute__ ((section(".ramfunc")));
system_gpio_pin_state_t system_gpio_get_pin_state( gpio_t gpio )  __attribute__ ((section(".ramfunc")));
void system_gpio_wait_for_state( gpio_t io, uint8_t state )  __attribute__ ((section(".ramfunc")));

#ifdef __cplusplus
}
#endif

#endif
