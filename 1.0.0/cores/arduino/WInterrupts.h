/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

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

#ifndef _WIRING_INTERRUPTS_
#define _WIRING_INTERRUPTS_

#include <stdint.h>

typedef void (*interrupt_handle_t)( void ) ;

typedef enum{
  LOW_EDGE = 0x00,
  FALLING_EDGE,
  RISING_EDGE,
  CHANGE_EDGE
}interrupt_mode_t;


void attachInterrupt(uint32_t pin, interrupt_handle_t callback, interrupt_mode_t mode);

/*
 * \brief Turns off the given interrupt.
 */
void detachInterrupt(uint32_t pin);

  

#endif
