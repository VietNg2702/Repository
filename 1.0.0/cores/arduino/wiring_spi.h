#ifndef _WIRING_SPI_H__
#define _WIRING_SPI_H__

#include <stdint.h>
#include <stdbool.h>
#include "r_spi_cmsis_api.h"

extern ARM_DRIVER_SPI *ARDUINO_SPI0;
extern ARM_DRIVER_SPI *ARDUINO_SPI1;

void spi_initialize(ARM_DRIVER_SPI *SPI);

void spi_receive(ARM_DRIVER_SPI *SPI, uint8_t *data, uint8_t size);

void spi_send(ARM_DRIVER_SPI *SPI, uint8_t *data, uint8_t size);

void spi_transfer(ARM_DRIVER_SPI *SPI, uint8_t *in_data, uint8_t *out_data, uint8_t size);
#endif