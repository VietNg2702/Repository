/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UARTClass.h"

#include "r_system_api.h"
#include "r_usart_cmsis_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void UART_IRQHandler(uint32_t event)
{
    switch (event)
    {
        case ARM_USART_EVENT_RECEIVE_COMPLETE:
            static uint8_t data;
            USART_CH.Receive(&data, 1);
            Serial.irq_handler(data);
            
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
#ifdef __cplusplus
}
#endif


// Public Methods //////////////////////////////////////////////////////////////

UARTClass::UARTClass(uint8_t Tx_pin, uint8_t Rx_pin)
{

}

void UARTClass::begin(unsigned long bauds)
{
    //uint32_t freq_hz = 0;

    /* Initialize the USART driver */
    USART_CH.Initialize(UART_IRQHandler);

    /* Power up the USART peripheral */
    USART_CH.PowerControl(ARM_POWER_FULL);

    /*Configure the USART */
    //R_SYS_SystemClockFreqGet(&freq_hz);

    /* Set up the configuration data structure for asynchronous (UART) operation. */
    USART_CH.Control(ARM_USART_MODE_ASYNCHRONOUS |
                            ARM_USART_DATA_BITS_8 |
                            ARM_USART_PARITY_NONE |
                            ARM_USART_STOP_BITS_1 |
                            ARM_USART_FLOW_CONTROL_NONE, bauds);

    return;

}


void UARTClass::end(void)
{
    /* Power off the USART peripheral */
    USART_CH.PowerControl(ARM_POWER_OFF);

    /* Uninitialize the USART driver */
    USART_CH.Uninitialize();
    rxBuffer.clear();

    return;
}


int UARTClass::available(void)
{   
  return rxBuffer.available();
}


int UARTClass::peek(void)
{
  return rxBuffer.peek();
}


int UARTClass::read(void)
{
  return rxBuffer.read_char();
}


void UARTClass::flush(void)
{
  rxBuffer.clear();
}


size_t UARTClass::write(const uint8_t data)
{
  USART_CH.Send(&data, 1);
  return 1;
}

size_t UARTClass::write(const char * data)
{
	size_t writed = 0;
	while(*data != '\0')
	{
		writed += UARTClass::write(*data);
		++data;
	}
	
	return writed;
}

void UARTClass::irq_handler(uint8_t data)
{
  rxBuffer.store_char(data);
}

