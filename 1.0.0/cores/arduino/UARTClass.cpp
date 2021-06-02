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
#include "variant.h"

#include "r_system_api.h"
#include "r_usart_cmsis_api.h"
#include "usart.h"




// Public Methods //////////////////////////////////////////////////////////////

void UARTClass::begin(unsigned long bauds)
{
    usart_enable(bauds);
    return;
}


void UARTClass::end(void)
{
    usart_disable();
    return;
}


int UARTClass::available(void)
{
       
    if(USART_CH.GetRxCount()>0)
    {
        return 1;
    }

    return 0;
}


int UARTClass::availableForWrite(void)
{
    if(USART_CH.GetTxCount()>0)
    {
        return 1;
    }
    
    return 0;
}


int UARTClass::peek(void)
{
  //TODO:
  return(0);
}


int UARTClass::read(void)
{
    int data;

    USART_CH.Receive(&data, 1);
    
    return data;
}


void UARTClass::flush(void)
{
  //TODO:
}


size_t UARTClass::write(const uint8_t uc_data)
{
  //TODO:
  return (1);
}
