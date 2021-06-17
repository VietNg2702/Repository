#ifndef _UART_DRIVER_FIFO
#define _UART_DRIVER_FIFO


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RE01_256KB.h"
#include "r_usart_cmsis_api.h"
#include "r_system_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define       UART_DRIVER_FIFO_BUFFER_SIZE  (256)

typedef struct Fifo_ring
{
    uint16_t start;
    uint16_t next;
    char *data;
    uint16_t size;
    char cdat;
    int8_t send_end;
} FIFO_HEADER;


typedef struct arm_driver_UART_DRIVER_FIFO
{
  ARM_DRIVER_USART* sci_dev;
  FIFO_HEADER* tx_fifo;
  FIFO_HEADER* rx_fifo;
  char* tx_buf;
  char* rx_buf;
  void (*p_callback)(uint32_t);
} UART_DRIVER_FIFO;


void uart0_set_param(UART_DRIVER_FIFO* p_uart_dev);
void uart2_set_param(UART_DRIVER_FIFO* p_uart_dev);
void uart4_set_param(UART_DRIVER_FIFO* p_uart_dev);
void uart5_set_param(UART_DRIVER_FIFO* p_uart_dev);

int8_t uart_fifo_init(UART_DRIVER_FIFO* p_uart_dev,  uint32_t baud_rate);
int8_t uart_fifo_init_custom(UART_DRIVER_FIFO* p_uart_dev,
		uint32_t baud_rate, 
		uint32_t data_bits,
		uint32_t parity,
		uint32_t stop_bits,
		uint32_t flow_control
		);

int8_t uart_fifo_send_start(UART_DRIVER_FIFO* p_uart_dev, char* buf, uint16_t size);
int8_t  uart_fifo_send_finished(UART_DRIVER_FIFO* p_uart_dev);
int8_t uart_fifo_receive_start(UART_DRIVER_FIFO* p_uart_dev);
int8_t  uart_fifo_receive_abort(UART_DRIVER_FIFO* p_uart_dev);
int16_t uart_fifo_receive_line_no_wait(UART_DRIVER_FIFO* p_uart_dev, char* buf, uint16_t max_size);
int16_t uart_fifo_receive_line(UART_DRIVER_FIFO* p_uart_dev, char* buf, uint16_t max_size, uint32_t timeout);
char uart_fifo_receive_char(UART_DRIVER_FIFO* p_uart_dev);
void uart_tx_fifo_flush(UART_DRIVER_FIFO* p_uart_dev);
void uart_rx_fifo_flush(UART_DRIVER_FIFO* p_uart_dev);

void uart_tx_fifo_wait_empty(UART_DRIVER_FIFO* p_uart_dev);
void uart_rx_fifo_wait_empty(UART_DRIVER_FIFO* p_uart_dev);

int8_t uart_tx_fifo_empty(UART_DRIVER_FIFO* p_uart_dev);
int8_t uart_rx_fifo_empty(UART_DRIVER_FIFO* p_uart_dev);

int8_t uart_fifo_send_string(UART_DRIVER_FIFO *p_uart_dev, char* send_buf);

void system_uart_init(UART_DRIVER_FIFO *uart_dev);
void system_uart_stop(UART_DRIVER_FIFO *uart_dev);

char uart_rx_fifo_pop(UART_DRIVER_FIFO* p_uart_dev);

#ifdef __cplusplus
}
#endif

#endif

