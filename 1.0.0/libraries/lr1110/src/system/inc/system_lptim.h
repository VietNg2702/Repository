#ifndef __SYSTEM_LPTIM_H__
#define __SYSTEM_LPTIM_H__

#include "stdint.h"
#include "RE01_256KB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "r_system_api.h"
#include "r_lpm_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void system_lptim_init() __attribute__ ((section(".ramfunc"))) ;

void system_lptim_set_and_run( uint32_t ticks )  __attribute__ ((section(".ramfunc")));

void system_lptim_stop(void)  __attribute__ ((section(".ramfunc")));

uint32_t system_lptim_get(void)  __attribute__ ((section(".ramfunc")));

void system_int_timer_init(void) __attribute__ ((section(".ramfunc")));

uint16_t system_int_timer_get(void)  __attribute__ ((section(".ramfunc")));

void system_int_timer_set_and_run( uint16_t ticks )  __attribute__ ((section(".ramfunc")));

void system_int_timer_stop(void)  __attribute__ ((section(".ramfunc")));


#ifdef __cplusplus
}
#endif

#endif
