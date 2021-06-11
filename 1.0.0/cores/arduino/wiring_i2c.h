#ifndef _WIRING_I2C_H__
#define _WIRING_I2C_H__

#include <stdint.h>
#include <stdbool.h>
#include "r_i2c_cmsis_api.h"
#include "R_Driver_I2C.h"

void i2c_initialize(void);

void i2c_transmit(uint8_t *data, uint8_t size);

void i2c_receive(uint8_t *data, uint8_t size);

#endif