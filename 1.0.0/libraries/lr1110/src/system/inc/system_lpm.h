#ifndef SYS_LPM_H_
#define SYS_LPM_H_

#include "config_mode.h"
#include "RE01_256KB.h"
#include "r_system_api.h"
#include "r_lpm_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void system_lpm_setup_rtc(void) __attribute__ ((section(".ramfunc")));
void system_lpm_setup_agt(void) __attribute__ ((section(".ramfunc")));
void system_lpm_prepare(void)  __attribute__ ((section(".ramfunc")));
void system_lpm_enter(void) __attribute__ ((section(".ramfunc"))) ;
void system_lpm_exit(void) __attribute__ ((section(".ramfunc"))) ;
void system_lpm_wait_rtc(void) __attribute__ ((section(".ramfunc")));
void system_lpm_wait_agt(void) __attribute__ ((section(".ramfunc")));

#ifdef __cplusplus
}
#endif

#endif /* SYS_LPM_H_ */
