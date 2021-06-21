
#ifndef __TRANSCEIVER_ALMANAC_H
#define __TRANSCEIVER_ALMANAC_H

#ifdef __cplusplus
extern "C" {
#endif

#define ALMANAC_DATA_LENGTH 2580
#define ALMANAC_DATA_UNIT_LENGTH 20
#define ALMANAC_UART_TRANS_LENGTH 60
#define ALMANAC_DAYS_LIMIT 31


uint16_t GetAlmanacAgeFromGpsEpochInDays( uint16_t almanac_age );

uint16_t GetAlmanacAgeDaysSinceToday(void);

int8_t   AlmanacUpdateTX(void);

int8_t   GetAlmanacDataFromPC(uint8_t data_buffer[], uint32_t timeout);



#ifdef __cplusplus
}
#endif


#endif
