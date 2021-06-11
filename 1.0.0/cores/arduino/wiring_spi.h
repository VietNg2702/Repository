#ifndef _WIRING_SPI_H__
#define _WIRING_SPI_H__

#include <stdint.h>
#include <stdbool.h>
#include "r_spi_cmsis_api.h"

void spi0_initialize(void);

void spi0_receive(uint8_t *data, uint8_t size);

void spi0_send(uint8_t *data, uint8_t size);

void spi1_initialize(void);

void spi1_receive(uint8_t *data, uint8_t size);

void spi1_send(uint8_t *data, uint8_t size);

#endif