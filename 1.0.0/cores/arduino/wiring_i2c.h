#ifndef __WIRING_I2C_H__
#define __WIRING_I2C_H__

#include <stdint.h>
#include <stdbool.h>
#include "r_i2c_cmsis_api.h"
#include "R_Driver_I2C.h"

extern ARM_DRIVER_I2C * ARDUINO_I2C0;
extern ARM_DRIVER_I2C * ARDUINO_I2C1;

void i2c_initialize(ARM_DRIVER_I2C *I2C);

void i2c_transmit(ARM_DRIVER_I2C *I2C, uint32_t addr, uint8_t *data, uint8_t size);

void i2c_receive(ARM_DRIVER_I2C *I2C, uint32_t addr, uint8_t *data, uint8_t size);

#endif