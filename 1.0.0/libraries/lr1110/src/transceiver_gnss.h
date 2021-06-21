
#ifndef __TRANSCEIVER_GNSS_H
#define __TRANSCEIVER_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	uint8_t src_name[2];
	uint16_t nav_message_size;
	uint32_t radio_ms;
	uint32_t computation_ms;
	uint16_t delay_since_capture;
} __attribute__ ((packed)) gnss_tx_header;


int8_t semtech_GNSS_remote( char* gnss_result_buf, int8_t use_assist_mode ,	uint16_t *p_radio_tx_len);
void set_count_gnss_date();
int8_t get_results_from_host(char* result_buf, uint32_t timeout);
int8_t get_date_from_host(char* receive_buf, uint32_t timeout);
int8_t read_gnss_date(char* receive_buf);
int8_t semtech_event_wait(uint16_t timeout);
void gnss_packet_to_string( gnss_tx_header* src, char* dst);



#ifdef __cplusplus
}
#endif


#endif
