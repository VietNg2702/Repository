#include "../inc/system_uart_fifo.h"
#include "../inc/system.h"

extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_USART2;
extern ARM_DRIVER_USART Driver_USART4;
extern ARM_DRIVER_USART Driver_USART5;


void fifo_local_init( FIFO_HEADER *fifo, uint8_t *buffer, uint16_t size );
void fifo_local_push( FIFO_HEADER *fifo, char c );
char fifo_local_pop( FIFO_HEADER *fifo  );
//  If empty , return true
int8_t fifo_local_empty(FIFO_HEADER *fifo);

//  If empty , return true
int8_t fifo_local_full(FIFO_HEADER *fifo);
void fifo_local_flush( FIFO_HEADER *fifo );
int16_t  fifo_receive_line(  FIFO_HEADER *fifo, char* line_buf, uint16_t max_size);
//char  fifo_receive_char(  FIFO_HEADER *fifo);


void fifo_local_init( FIFO_HEADER *fifo, uint8_t *buffer, uint16_t size )
{
    GLOBAL_INT_DISABLE();
    fifo->start = 0;
    fifo->next = 0;
    fifo->data = buffer;
    fifo->size = size;
    fifo->cdat = 0;
    fifo->send_end = true;
    GLOBAL_INT_RESTORE();
}

void fifo_local_push( FIFO_HEADER *fifo, char c )
{
//    volatile int16_t src;
    volatile int16_t dst;
    GLOBAL_INT_DISABLE();

    dst = fifo->next;
    fifo->data[dst] = c;
    dst++;
    if (dst >= fifo->size) {
            dst = 0;
    } 
    fifo->data[dst] = 0; // Terminate Buffer
    fifo->next = dst;
    GLOBAL_INT_RESTORE();
}

char fifo_local_pop( FIFO_HEADER *fifo  )
{
    volatile char c;
    volatile int16_t src;
//    volatile int16_t dst;

    GLOBAL_INT_DISABLE();

    src = fifo->start;
    c = fifo->data[src];
    src++;
    if (src >= fifo->size) {
            src = 0;
    } 
    fifo->start = src;
    GLOBAL_INT_RESTORE();

    return(c);
}

//  If empty , return true
int8_t fifo_local_empty(FIFO_HEADER *fifo)
{
	int8_t result;
    GLOBAL_INT_DISABLE();
    if (fifo->next == fifo->start) {
        result = true;
    }
    else {
        result = false;
    }
    GLOBAL_INT_RESTORE();
    return result;
}

//  If empty , return true
int8_t fifo_local_full(FIFO_HEADER *fifo)
{
//    volatile int16_t src;
    volatile int16_t dst;
    int8_t result;

    GLOBAL_INT_DISABLE();
    dst = fifo->next;
    dst++;
    if (dst >= fifo->size) {
            dst = 0;
    }
    if (dst == fifo->start) {result = true;}
    else {result =  false;}
    GLOBAL_INT_RESTORE();
    return result;
}

void fifo_local_flush( FIFO_HEADER *fifo )
{
    GLOBAL_INT_DISABLE();
    fifo->start = 0;
    fifo->next = 0;
    GLOBAL_INT_RESTORE();

}


//char  fifo_receive_char(  FIFO_HEADER *fifo)
//{
//	return fifo_local_pop(fifo);
//}


int16_t  fifo_receive_line_wait(  FIFO_HEADER *fifo, char* line_buf, uint16_t max_size, uint32_t timeout)
{
    volatile char c;
    volatile int16_t ic, jc;

    if (max_size < 2) {return 0;}
    if (max_size > UART_DRIVER_FIFO_BUFFER_SIZE) {max_size = UART_DRIVER_FIFO_BUFFER_SIZE-2;}

    ic=0;
    for (jc = 0; jc < timeout; jc++) {
		if (!(fifo_local_empty(fifo))) {
			c=  fifo_local_pop( fifo );
	        line_buf[ic] = c;
    	    ic++;
	        if (c == 0x0D || c == 0x0A || c == 0x00) {
	            line_buf[ic] = '\0';
        	   goto Skip;
	        }
	        continue;
		}
	    system_time_wait_ms( 1 );
	}
    line_buf[0] = '\0';
    ic=-1;
 Skip:
    return ic;
}


void uart_fifo_callback_sub(uint32_t event, ARM_DRIVER_USART* p_uart_dev, FIFO_HEADER* p_tx_fifo, FIFO_HEADER* p_rx_fifo)
{
    volatile uint8_t result;
    volatile char c0;
  /** Check event */
    switch( event )
    {
        case ARM_USART_EVENT_SEND_COMPLETE:
            {

                if (fifo_local_empty(p_tx_fifo)) {
                    p_tx_fifo->send_end = true;
                } else {
                    p_tx_fifo->cdat = fifo_local_pop(p_tx_fifo);
                    result = p_uart_dev->Send(&(p_tx_fifo->cdat), 1);
                    p_tx_fifo->send_end = false;
                }

               /* Describe the process when sending is completed */
                /* set flag to send complete status */
            }
        break;
        
        case ARM_USART_EVENT_RECEIVE_COMPLETE:

            /* Describe processing when receiving is completed */
            {
              if (fifo_local_full(p_rx_fifo) != true) {
            	c0 = p_rx_fifo->cdat;
                if (c0 != 0) {
                  fifo_local_push( p_rx_fifo, c0 );
                }
              }
            result = p_uart_dev->Receive(&(p_rx_fifo->cdat), 1);
            }
        break;
        
        default:
            {
            /* Resume reception when a reception error occurs */
            }
        break;
    }

}   /* End of function usart_callback() */



int8_t uart_fifo_init_sub(ARM_DRIVER_USART* p_uart_dev, void (*p_callback)(uint32_t), 
		uint32_t baud_rate, 
		uint32_t data_bits,
		uint32_t parity,
		uint32_t stop_bits,
		uint32_t flow_control
		)
{
    volatile int8_t  result;


    result = p_uart_dev->Initialize(p_callback);
    if (result != ARM_DRIVER_OK) {return(result);}
  
    /** Initialize USART PowerControl **/
    result = p_uart_dev->PowerControl(ARM_POWER_FULL);
    if (result != ARM_DRIVER_OK) {return(result);}

    /** Initialize USART transmition settings **/
    result = p_uart_dev->Control((ARM_USART_MODE_ASYNCHRONOUS |    /* Work1 Please select argument of UART (Asynchronous) */
                         data_bits    |    /* 8 Data bits */
                         parity       |    /* No Parity */
                         stop_bits    |    /* 1 Stop bit */
                         flow_control  )     /* No Flow Control */
                        ,baud_rate);
    if (result != ARM_DRIVER_OK) {return(result);}

    /** Enable send for UART2 Debug   **/
    result = p_uart_dev->Control(ARM_USART_CONTROL_TX_RX,1);

    return (result);
}


int8_t  uart_fifo_send_start_sub(ARM_DRIVER_USART* p_uart_dev, FIFO_HEADER* p_fifo, char* send_buf, uint16_t size)
{
    volatile int8_t  result;
    volatile int32_t i,j;
//    volatile char c;

    if (size == 0) { return ARM_DRIVER_ERROR;}

    result = ARM_DRIVER_OK;
    for( i = 0; i < size; i++ )
    {
    	for (j=0; ; j++) {
    		if (j > 100000) {
    			return ARM_DRIVER_ERROR; // Timeout 100sec
    		}
    		result = fifo_local_full(p_fifo);
    		if (result != true) {break;}
    		system_time_wait_ms( 1 );
    	}
        fifo_local_push(p_fifo, send_buf[i]);

        GLOBAL_INT_DISABLE();
        if (p_fifo->send_end == true) {
        	p_fifo->cdat = fifo_local_pop(p_fifo);
        	result = p_uart_dev->Send(&(p_fifo->cdat), 1);
        	p_fifo->send_end = false;
        }
        GLOBAL_INT_RESTORE();
    }
	return result;
}

int8_t  uart_fifo_send_finished_sub(FIFO_HEADER* p_fifo)
{
	return p_fifo->send_end;
}


int8_t  uart_fifo_receive_start_sub(ARM_DRIVER_USART* p_uart_dev, FIFO_HEADER* p_fifo)
{
    volatile int8_t  result;
    result = p_uart_dev->Receive(&(p_fifo->cdat), 1);
	return result;

}

int8_t  uart_fifo_receive_abort_sub(ARM_DRIVER_USART* p_uart_dev)
{
    volatile int8_t  result;
    result = p_uart_dev->Control( ARM_USART_ABORT_RECEIVE, 0);
	return result;

}

//  Return  >=1 .. Read bytes
//            0 .. No CR or LF
//           -1 .. Buffer is empty
int16_t    uart_fifo_receive_line_no_wait_sub(FIFO_HEADER* p_fifo, char* receive_buf, uint16_t max_size)
{
    volatile int16_t receive_len;
    volatile int8_t result;
    receive_buf[0] = '\0';
    result = fifo_local_empty(p_fifo);
    if (result) {return -1;}
    receive_len = fifo_receive_line_wait(p_fifo, receive_buf, max_size,  10000);
    return (receive_len);
}


int16_t    uart_fifo_receive_line_wait_sub(FIFO_HEADER* p_fifo, char* receive_buf, uint16_t max_size, uint32_t timeout)
{
    volatile uint32_t ic;
    volatile int16_t receive_len;

    char c0;

	for (ic=0; ic < timeout; ic++) {
	    receive_buf[0] = '\0';
	    receive_len = fifo_receive_line_wait(p_fifo, receive_buf, max_size,  timeout);

    	if (receive_len >0) {
		              c0=receive_buf[0];
		              if (c0 != 0 && c0 != 0x0D && c0 != 0x0A) {
		                break;
		              }
		} else if (receive_len < 0) {
			receive_len = -1;
            break;
		}
	    system_time_wait_ms( 1 );
	}
    return (receive_len);
}


int8_t uart_fifo_uninit(ARM_DRIVER_USART* p_uart_dev)
{
    volatile int8_t result;
    result = p_uart_dev->Uninitialize();
	return result;
}

char uart_fifo_receive_char_sub(FIFO_HEADER* p_fifo)
{
	volatile char c2;
	volatile int8_t result1;
    result1 = fifo_local_empty(p_fifo);
__NOP();
    if (result1 != true) {
		c2=fifo_local_pop(p_fifo);
		__NOP();
	} else {
		c2='\0';
	}
    return c2;
}


/*******************************************************

   The following functions are needed for each different SCIn

*******************************************************/


//static ARM_DRIVER_USART *gsp_sci2_dev;
//static ARM_DRIVER_USART *gsp_sci4_dev;
//static ARM_DRIVER_USART *gsp_sci5_dev;

FIFO_HEADER uart0_tx_fifo;
FIFO_HEADER uart0_rx_fifo;
char uart0_tx_buf[UART_DRIVER_FIFO_BUFFER_SIZE];
char uart0_rx_buf[UART_DRIVER_FIFO_BUFFER_SIZE];

void uart0_fifo_callback(uint32_t event)
{
  GLOBAL_INT_DISABLE();
  uart_fifo_callback_sub(event, &Driver_USART0, &uart0_tx_fifo, &uart0_rx_fifo);
  GLOBAL_INT_RESTORE();
}

void uart0_set_param(UART_DRIVER_FIFO* p_uart_dev)
{
	p_uart_dev->sci_dev = &Driver_USART0;
	p_uart_dev->tx_fifo = &uart0_tx_fifo;
	p_uart_dev->rx_fifo = &uart0_rx_fifo;
	p_uart_dev->tx_buf = &uart0_tx_buf[0];
	p_uart_dev->rx_buf = &uart0_rx_buf[0];
	p_uart_dev->p_callback = uart0_fifo_callback;
}


#if 0
FIFO_HEADER uart2_tx_fifo;
FIFO_HEADER uart2_rx_fifo;
FIFO_HEADER uart4_tx_fifo;
FIFO_HEADER uart4_rx_fifo;
FIFO_HEADER uart5_tx_fifo;
FIFO_HEADER uart5_rx_fifo;
char uart2_tx_buf[UART_DRIVER_FIFO_BUFFER_SIZE];
char uart2_rx_buf[UART_DRIVER_FIFO_BUFFER_SIZE];
char uart4_tx_buf[UART_DRIVER_FIFO_BUFFER_SIZE];
char uart4_rx_buf[UART_DRIVER_FIFO_BUFFER_SIZE];
char uart5_tx_buf[UART_DRIVER_FIFO_BUFFER_SIZE];
char uart5_rx_buf[UART_DRIVER_FIFO_BUFFER_SIZE];

void uart2_fifo_callback(uint32_t event)
{
  GLOBAL_INT_DISABLE();
  uart_fifo_callback_sub(event, &Driver_USART2, &uart2_tx_fifo, &uart2_rx_fifo);
  GLOBAL_INT_RESTORE();
}


void uart4_fifo_callback(uint32_t event)
{
  GLOBAL_INT_DISABLE();
  uart_fifo_callback_sub(event, &Driver_USART4, &uart4_tx_fifo, &uart4_rx_fifo);
  GLOBAL_INT_RESTORE();
}

void uart5_fifo_callback(uint32_t event)
{
  GLOBAL_INT_DISABLE();
  uart_fifo_callback_sub(event, &Driver_USART5, &uart5_tx_fifo, &uart5_rx_fifo);
  GLOBAL_INT_RESTORE();
}


void uart2_set_param(UART_DRIVER_FIFO* p_uart_dev)
{
	p_uart_dev->sci_dev = &Driver_USART2;
	p_uart_dev->tx_fifo = &uart2_tx_fifo;
	p_uart_dev->rx_fifo = &uart2_rx_fifo;
	p_uart_dev->tx_buf = &uart2_tx_buf[0];
	p_uart_dev->rx_buf = &uart2_rx_buf[0];
	p_uart_dev->p_callback = uart2_fifo_callback;
}


void uart4_set_param(UART_DRIVER_FIFO* p_uart_dev)
{
	p_uart_dev->sci_dev = &Driver_USART4;
	p_uart_dev->tx_fifo = &uart4_tx_fifo;
	p_uart_dev->rx_fifo = &uart4_rx_fifo;
	p_uart_dev->tx_buf = &uart4_tx_buf[0];
	p_uart_dev->rx_buf = &uart4_rx_buf[0];
	p_uart_dev->p_callback = uart4_fifo_callback;
}



void uart5_set_param(UART_DRIVER_FIFO* p_uart_dev)
{
	p_uart_dev->sci_dev = &Driver_USART5;
	p_uart_dev->tx_fifo = &uart5_tx_fifo;
	p_uart_dev->rx_fifo = &uart5_rx_fifo;
	p_uart_dev->tx_buf = &uart5_tx_buf[0];
	p_uart_dev->rx_buf = &uart5_rx_buf[0];
	p_uart_dev->p_callback = uart5_fifo_callback;
}
#endif


/****************************

  Global functions

****************************/


int8_t uart_fifo_init(UART_DRIVER_FIFO* p_uart_dev,  uint32_t baud_rate)
{
    volatile int8_t  result;
    fifo_local_init( p_uart_dev->rx_fifo, p_uart_dev->rx_buf, UART_DRIVER_FIFO_BUFFER_SIZE );
    fifo_local_init( p_uart_dev->tx_fifo, p_uart_dev->tx_buf, UART_DRIVER_FIFO_BUFFER_SIZE );

	result =  uart_fifo_init_sub( p_uart_dev->sci_dev, p_uart_dev->p_callback, 
				baud_rate, 
				ARM_USART_DATA_BITS_8,
				ARM_USART_PARITY_NONE,
				ARM_USART_STOP_BITS_1,
				ARM_USART_FLOW_CONTROL_NONE
				);
	return result;
}


int8_t uart_fifo_init_custom(UART_DRIVER_FIFO* p_uart_dev,
		uint32_t baud_rate, 
		uint32_t data_bits,
		uint32_t parity,
		uint32_t stop_bits,
		uint32_t flow_control
		)
{
    volatile int8_t  result;
    fifo_local_init( p_uart_dev->rx_fifo, p_uart_dev->rx_buf, UART_DRIVER_FIFO_BUFFER_SIZE );
    fifo_local_init( p_uart_dev->tx_fifo, p_uart_dev->tx_buf, UART_DRIVER_FIFO_BUFFER_SIZE );

	result =  uart_fifo_init_sub( p_uart_dev->sci_dev, p_uart_dev->p_callback, 
				baud_rate, 
				data_bits,
				parity,
				stop_bits,
				flow_control
				);
	return result;
}





int8_t uart_fifo_init_default_sub(ARM_DRIVER_USART* p_uart_dev, void (*p_callback)(uint32_t), uint32_t baud_rate, uint32_t stop_bits)
{
    volatile int8_t  result;
	result =  uart_fifo_init_sub( p_uart_dev, p_callback, 
				baud_rate, 
				ARM_USART_DATA_BITS_8,
				ARM_USART_PARITY_NONE,
				ARM_USART_STOP_BITS_1,
				ARM_USART_FLOW_CONTROL_NONE
				);
	return result;
}



int8_t uart_fifo_send_start(UART_DRIVER_FIFO* p_uart_dev, char* buf, uint16_t size)
{
    return(uart_fifo_send_start_sub(p_uart_dev->sci_dev, p_uart_dev->tx_fifo, buf, size));
}

int8_t  uart_fifo_send_finished(UART_DRIVER_FIFO* p_uart_dev)
{
	return p_uart_dev->tx_fifo->send_end;
}


int8_t uart_fifo_receive_start(UART_DRIVER_FIFO* p_uart_dev)
{
    return(uart_fifo_receive_start_sub(p_uart_dev->sci_dev, p_uart_dev->rx_fifo));
}

int8_t  uart_fifo_receive_abort(UART_DRIVER_FIFO* p_uart_dev)
{
	return (uart_fifo_receive_abort_sub(p_uart_dev->sci_dev));
}

int16_t uart_fifo_receive_line(UART_DRIVER_FIFO* p_uart_dev, char* buf, uint16_t max_size, uint32_t timeout)
{
    return(uart_fifo_receive_line_wait_sub(p_uart_dev->rx_fifo, buf,  max_size, timeout));
}

int16_t uart_fifo_receive_line_no_wait(UART_DRIVER_FIFO* p_uart_dev, char* buf, uint16_t max_size)
{
    return(uart_fifo_receive_line_no_wait_sub(p_uart_dev->rx_fifo, buf,  max_size));
}

volatile int16_t start0;
volatile int16_t next0;

char uart_fifo_receive_char(UART_DRIVER_FIFO* p_uart_dev)
{
start0=    p_uart_dev->rx_fifo->start;
next0=    p_uart_dev->rx_fifo->next;

	return uart_fifo_receive_char_sub(p_uart_dev->rx_fifo);
}

void uart_rx_fifo_flush(UART_DRIVER_FIFO* p_uart_dev)
{
    return fifo_local_flush(p_uart_dev->rx_fifo);
}

void uart_tx_fifo_flush(UART_DRIVER_FIFO* p_uart_dev)
{
    return fifo_local_flush(p_uart_dev->tx_fifo);
}
 
void uart_rx_fifo_wait_empty(UART_DRIVER_FIFO* p_uart_dev)
{
	while(fifo_local_empty(p_uart_dev->rx_fifo) != true) {
	}
    return;
}

void uart_tx_fifo_wait_empty(UART_DRIVER_FIFO* p_uart_dev)
{
	while(fifo_local_empty(p_uart_dev->tx_fifo) != true) {
	}
    return;
}

int8_t uart_rx_fifo_empty(UART_DRIVER_FIFO* p_uart_dev)
{
	return fifo_local_empty(p_uart_dev->rx_fifo);
}

int8_t uart_tx_fifo_empty(UART_DRIVER_FIFO* p_uart_dev)
{
	return fifo_local_empty(p_uart_dev->tx_fifo);
}

char uart_rx_fifo_pop(UART_DRIVER_FIFO* p_uart_dev)
{
	return fifo_local_pop(p_uart_dev->rx_fifo);
}

int8_t uart_fifo_send_string(UART_DRIVER_FIFO *p_uart_dev, char* send_buf)
{
	int8_t result;
	result = uart_fifo_send_start(p_uart_dev, send_buf, strlen(send_buf));
	if (result !=ARM_DRIVER_OK) return result;
//	while  (uart_fifo_send_finished(p_uart_dev) != true);
	return ARM_DRIVER_OK;
}


/******************************************************
 *
 *
******************************************************/

void system_uart_init(UART_DRIVER_FIFO *p_uart_dev)
{
	int8_t result;
	uart0_set_param(p_uart_dev);
	result = uart_fifo_init(p_uart_dev, 115200);
	APP_ERR_HANDLER(result);

    system_gpio_init_output( 2, 2, 1 );  // CTS
    system_gpio_init_input( 7, 4, SYSTEM_GPIO_NO_INTERRUPT ); // RTS
}

void system_uart_stop(UART_DRIVER_FIFO *p_uart_dev)
{
	int8_t result;
	result = uart_fifo_uninit(p_uart_dev->sci_dev);
	APP_ERR_HANDLER(result);
}



