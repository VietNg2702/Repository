#include "system_uart.h"

extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_USART2;
extern ARM_DRIVER_USART Driver_USART4;
extern ARM_DRIVER_USART Driver_USART5;

static ARM_DRIVER_USART* p_uart_dev = &Driver_USART0;

static Callback_t RxDoneCallback;
static Callback_t TxDoneCallback;

static void UART_IRQHandler(uint32_t event)
{
    switch (event)
    {
        case ARM_USART_EVENT_SEND_COMPLETE:
            if( TxDoneCallback.object != NULL && TxDoneCallback.callback != NULL )
            {
                TxDoneCallback.callback( TxDoneCallback.object );
            }
            break;
        case ARM_USART_EVENT_RECEIVE_COMPLETE:
            if( RxDoneCallback.object != NULL && RxDoneCallback.callback != NULL )
            {
                RxDoneCallback.callback( RxDoneCallback.object );
            }
            break;
        case ARM_USART_EVENT_RX_OVERFLOW:
            __ASM volatile ("nop");
            break;
        case ARM_USART_EVENT_RX_TIMEOUT:
            __ASM volatile ("nop");
            break;
        case ARM_USART_EVENT_RX_BREAK:
            __ASM volatile ("nop");
            break;
        case ARM_USART_EVENT_RX_FRAMING_ERROR:
            __ASM volatile ("nop");
            break;
        case ARM_USART_EVENT_RX_PARITY_ERROR:
            __ASM volatile ("nop");
            break;
        default:
            __ASM volatile ("nop");
            break;
    }
}

int32_t system_uart_config(ARM_DRIVER_USART* p_uart, 
                        void (*p_callback)(uint32_t), 
                        uint32_t baud_rate, 
                        uint32_t data_bits,
                        uint32_t parity,
                        uint32_t stop_bits,
                        uint32_t flow_control
                        )
{
    volatile int32_t  result;
    p_uart_dev = p_uart;

    result = p_dev_uart->Initialize(UART_IRQHandler);
    if (result != ARM_DRIVER_OK) {return(result);}
  
    /** Initialize USART PowerControl **/
    result = p_dev_uart->PowerControl(ARM_POWER_FULL);
    if (result != ARM_DRIVER_OK) {return(result);}

    /** Initialize USART transmition settings **/
    result = p_dev_uart->Control((ARM_USART_MODE_ASYNCHRONOUS |   /* Work1 Please select argument of UART (Asynchronous) */
                                                data_bits    |    /* 8 Data bits */
                                                parity       |    /* No Parity */
                                                stop_bits    |    /* 1 Stop bit */
                                                flow_control  )   /* No Flow Control */
                                                ,baud_rate);
    if (result != ARM_DRIVER_OK) {return(result);}

    /** Enable send for UART2 Debug   **/
    result = p_dev_uart->Control(ARM_USART_CONTROL_TX_RX,1);

    return (result);
}                            

int32_t system_uart_init(void)
{
    return system_uart_config(p_uart_dev, UART_IRQHandler, 115200, ARM_USART_DATA_BITS_8, ARM_USART_PARITY_NONE, ARM_USART_STOP_BITS_1, ARM_USART_FLOW_CONTROL_NONE);
}

int32_t system_uart_uninit(void)
{
    volatile int32_t result;

    /* Power off the USART peripheral */
    p_uart_dev->PowerControl(ARM_POWER_OFF);

    result = p_uart_dev->Uninitialize();
    
	return result;
}


int32_t system_uart_receive( uint8_t *buffer, uint32_t len)
{
    volatile int32_t  result;

    result = p_uart_dev->Receive(buffer, len);

	return result;
}

int32_t system_uart_receive_abort(void)
{
    volatile int32_t  result;

    result = p_uart_dev->Control( ARM_USART_ABORT_RECEIVE, 0);

	return result;
}

int32_t system_uart_send( uint8_t * buffer, uin32_t len)
{
    volatile int32_t result;

    result = p_uart_dev->Send(buffer, len);

    return result;
}


int32_t system_uart_send_abort(void)
{
    volatile int32_t  result;

    result = p_uart_dev->Control( ARM_USART_ABORT_SEND, 0);

	return result;
}

int32_t system_uart_flush(void)
{
	return 0;
}

bool system_uart_is_tx_terminated( void)
{
    volatile ARM_USART_STATUS  result;

    result = p_uart_dev->GetStatus();

	return result.tx_busy;
}


void system_uart_register_tx_done_callback( void* object, void ( *callback )( void* ) )
{
    TxDoneCallback.object   = object;
    TxDoneCallback.callback = callback;
}

void system_uart_register_rx_done_callback( void* object, void ( *callback )( void* ) )
{
    RxDoneCallback.object   = object;
    RxDoneCallback.callback = callback;
}

void system_uart_unregister_rx_done_callback( void )
{
    RxDoneCallback.object   = 0;
    RxDoneCallback.callback = 0;
}

void system_uart_unregister_tx_done_callback( void )
{
    TxDoneCallback.object   = 0;
    TxDoneCallback.callback = 0;
}