
#ifndef __TRANSCEIVER_POWER_H
#define __TRANSCEIVER_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

void lr1110_power_on(void);
void lr1110_power_off(void);
void lr1110_sleep_enter(uint32_t timeout);
void lna_on(void);
void lna_off(void);
void DCDC_on(void);
void DCDC_off(void);


#ifdef __cplusplus
}
#endif


#endif
