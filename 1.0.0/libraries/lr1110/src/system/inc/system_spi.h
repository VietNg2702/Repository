#ifndef _SYSTEM_SPI_H
#define _SYSTEM_SPI_H


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lr1110_user_config_mode.h"
#include "RE01_256KB.h"
#include "r_lpm_api.h"
#include "r_spi_cmsis_api.h"
#include "r_system_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void system_spi_init( ARM_DRIVER_SPI* spi) __attribute__ ((section(".ramfunc")));

void system_spi_stop( ARM_DRIVER_SPI* spi) __attribute__ ((section(".ramfunc")));

void system_spi_write( ARM_DRIVER_SPI* spi, const uint8_t* buffer, uint16_t length )  __attribute__ ((section(".ramfunc")));

void system_spi_read( ARM_DRIVER_SPI* spi, uint8_t* buffer, uint16_t length )  __attribute__ ((section(".ramfunc")));

void system_spi_write_read( ARM_DRIVER_SPI* spi, const uint8_t* cbuffer, uint8_t* rbuffer, uint16_t length )  __attribute__ ((section(".ramfunc")));

#ifdef __cplusplus
}
#endif

#endif
