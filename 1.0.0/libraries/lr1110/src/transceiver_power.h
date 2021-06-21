
#ifndef __TRANSCEIVER_POWER_H
#define __TRANSCEIVER_POWER_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void lr1110_power_on(void);

void lr1110_power_off(void);

void lr1110_sleep_enter(uint32_t timeout);

void lr1110_lna_on(void);

void lr1110_lna_off(void);

void lr1110_dcdc_on(void);

void lr1110_dcdc_off(void);


#ifdef __cplusplus
}
#endif


#endif
