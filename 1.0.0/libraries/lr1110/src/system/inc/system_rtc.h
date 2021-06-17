#ifndef R_RTC_H_
#define R_RTC_H_

#include "RE01_256KB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config_mode.h"
#include "r_lpm_api.h"
#include "r_system_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_COUNTER_STOP        (0)
#define RTC_COUNTER_START       (1)
#define RTC_DUMMY_READ          (3)
#define RTC_INT_ENABLE          (0x07)

typedef void (*RTC_Event_t) (uint16_t event);

typedef enum e_rtc_periodic
{
    RTC_PERIODIC_OFF    = 0,
    RTC_PERIODIC_256_HZ = 6,
    RTC_PERIODIC_128_HZ = 7,
    RTC_PERIODIC_64_HZ  = 8,
    RTC_PERIODIC_32_HZ  = 9,
    RTC_PERIODIC_16_HZ  = 10,
    RTC_PERIODIC_8_HZ   = 11,
    RTC_PERIODIC_4_HZ   = 12,
    RTC_PERIODIC_2_HZ   = 13,
    RTC_PERIODIC_1_HZ   = 14,
    RTC_PERIODIC_2_SEC  = 15,
} rtc_periodic_t;

#if (!defined(_TIME_H) && !defined(__time_h__))
typedef struct
{
    int tm_sec;     /* Seconds (0-59) */
    int tm_min;     /* Minute (0-59) */
    int tm_hour;    /* Hour (0-23) */
    int tm_mday;    /* Day of the month (1-31) */
    int tm_mon;     /* Month (0-11, 0=January) */
    int tm_year;    /* Year since 1900 (100-199, 100=2000)*/
    int tm_wday;    /* Day of the week (0-6, 0=Sunday) */
    int tm_yday;    /* Day of the year (0-365) */
    int tm_isdst;   /* Daylight Savings enabled (>0), disabled (=0), or unknown (<0)*/
} tm_t;
#endif

typedef struct
{
    uint8_t     int_priority;       // INT priority; 0=disable, 1=low, 15=high

    /* set to true to cause alarm when all enabled alarm fields match current time */
    bool        sec;
    bool        min;
    bool        hour;
    bool        wday;
    bool        mday;
    bool        mon;
    bool        year;
} rtc_alarm_ctrl_t;

void rtc_init (void);
void rtc_set_periodic (rtc_periodic_t freq, uint8_t priority)  __attribute__ ((section(".ramfunc")));
void rtc_set_current_time (tm_t * p_current)  __attribute__ ((section(".ramfunc")));
void rtc_set_alarm_time (tm_t *p_alarm)  __attribute__ ((section(".ramfunc")));
void rtc_enable_alarms (rtc_alarm_ctrl_t *p_alm_ctrl)  __attribute__ ((section(".ramfunc")));
void rtc_disable_alarms(void)  __attribute__ ((section(".ramfunc")));
void rtc_read_alarm (tm_t *p_alarm)  __attribute__ ((section(".ramfunc")));
void rtc_read_current (tm_t *p_current)  __attribute__ ((section(".ramfunc")));
void rtc_counter_run (const uint8_t action)  __attribute__ ((section(".ramfunc")));
void rtc_reset (void)  __attribute__ ((section(".ramfunc")));
void rtc_enable_ints (void)  __attribute__ ((section(".ramfunc")));
void rtc_disable_ints (void)  __attribute__ ((section(".ramfunc")));

uint32_t rtc_read_current_binary_time (void)  __attribute__ ((section(".ramfunc")));
void rtc_set_current_binary_time (uint32_t current_time)  __attribute__ ((section(".ramfunc")));

void rtc_enable_alarms_binary (uint32_t alarm_mask)  __attribute__ ((section(".ramfunc")));
void rtc_set_alarm_binary_time (uint32_t alarm_time)  __attribute__ ((section(".ramfunc")));

#ifdef __cplusplus
}
#endif

#endif /* R_RTC_H_ */
