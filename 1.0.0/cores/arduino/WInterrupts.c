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

#include "Arduino.h"

#include <string.h>

/* Configure I/O interrupt sources */
static void __initialize()
{
  //TODO:
}

/*
 * \brief Specifies a named Interrupt Service Routine (ISR) to call when an interrupt occurs.
 *        Replaces any previous function that was attached to the interrupt.
 */
void attachInterrupt(uint32_t intnum, voidFuncPtr callback, uint32_t mode)
{
  //TODO:
}

/*
 * \brief Turns off the given interrupt.
 */
void detachInterrupt(uint32_t intnum)
{
  //TODO:
}


/*Entry Point for PLIC Interrupt Handler*/
void handle_m_ext_interrupt()
{
  //TODO:
}

/* TODO: Entry Point for Timer Interrupt Handler*/
void handle_m_time_interrupt() {

  // Does nothing for now.
  
}

uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc)
{
  //TODO:
  return epc;
}
