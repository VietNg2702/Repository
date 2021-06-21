
#ifndef __TRANSCEIVER_RTC_H
#define __TRANSCEIVER_RTC_H

#ifdef __cplusplus
extern "C" {
#endif


int8_t read_validate_location(uint32_t last_adjust_count, char* date_buf, environment_location_t* p_location);

void rtc_adjust_TX(environment_location_t* p_location_assist, int8_t* p_valid_flag);

int8_t GetDateAndApproximateLocationFromPC( environment_location_t* p_location , uint32_t timeout);



#ifdef __cplusplus
}
#endif


#endif
