
#ifndef __TRANSCEIVER_RADIO_H
#define __TRANSCEIVER_RADIO_H

#ifdef __cplusplus
extern "C" {
#endif

#define UART_BUFFER_SIZE 256
#define RADIO_DATA_BUFFER_SIZE 512

#define PAYLOAD_LENGTH 64

void DeviceModem_Init(void);

int8_t radio_init(void);
int8_t radio_send(int8_t* buffer);
int8_t radio_wait_for_tx_done(uint32_t timeout);

int8_t  radio_receive();
int8_t  radio_wait_for_rx_done(int8_t* buffer, uint32_t timeout);

int8_t semtech_radio_tx_len(char* radio_tx_buf ,int16_t len, uint32_t timeout);
int8_t semtech_radio_tx(char* radio_tx_buf ,uint32_t timeout);
int8_t semtech_radio_rx_len(char* radio_rx_buf , int16_t *p_receive_len, int16_t max_len, uint32_t timeout);
int8_t semtech_radio_rx(char* radio_rx_buf ,uint32_t timeout);


#ifdef __cplusplus
}
#endif


#endif
