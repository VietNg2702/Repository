#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "RE01_256KB.h"
#include "r_spi_cmsis_api.h"
#include "r_system_api.h"

#include "system_clock.h"
#include "system_gpio.h"
#include "system_spi.h"
#include "system_uart_fifo.h"
#include "system_i2c.h"
#include "system_time.h"
#include "system_lptim.h"
#include "system_lpm.h"
#include "system_it.h"
#include "system_rtc.h"


#define APP_ERR_HANDLER(x)    if (x) {while(1);}
#define APP_ERR_RETURN(x)      {if(x){return x;}}
#define UNUSED(x)  (void)(x)


#define GLOBAL_INT_DISABLE()                        \
    do {                                            \
        uint32_t  __int_state = __get_PRIMASK();    \
        __set_PRIMASK(1U);                          \

  /* One or more line breaks are required */

#define GLOBAL_INT_RESTORE()                        \
        __set_PRIMASK(__int_state);                 \
    } while(0)


#define HAL_DBG_TRACE_PRINTF( ... )  printf (  __VA_ARGS__ )

#define HAL_DBG_TRACE_INFO( ... )                                          \
do                                                                         \
{                                                                          \
    HAL_DBG_TRACE_PRINTF( "INFO : " );                                     \
    HAL_DBG_TRACE_PRINTF( __VA_ARGS__ );                                   \
} while ( 0 );

#define HAL_DBG_TRACE_MSG( msg )                                           \
do                                                                         \
{                                                                          \
    HAL_DBG_TRACE_PRINTF( msg );                                           \
} while ( 0 );

typedef struct
{
    float latitude;
    float longitude;
    float altitude;
} environment_location_t;


typedef struct
{
    ARM_DRIVER_SPI* spi;
    gpio_t       nss;
    gpio_t       reset;
    gpio_t       busy;
    gpio_t       irq;
} radio_t;

#ifdef __cplusplus
extern "C" {
#endif
void system_init( radio_t* radio )  __attribute__ ((section(".ramfunc")));
void system_init_ISO2( )  __attribute__ ((section(".ramfunc")));
void system_uninit_ISO2(  )  __attribute__ ((section(".ramfunc")));

char* delComma(char* date_buf)  __attribute__ ((section(".ramfunc")));
char* delCRLF(char* date_buf)  __attribute__ ((section(".ramfunc")));
void rtc_alarm_init(void) __attribute__ ((section(".ramfunc")));

#ifdef __cplusplus
}
#endif

#endif


