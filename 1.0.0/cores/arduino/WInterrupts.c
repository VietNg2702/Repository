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
#include "wiring_digital.h"

/*
 * \brief Specifies a named Interrupt Service Routine (ISR) to call when an interrupt occurs.
 *        Replaces any previous function that was attached to the interrupt.
 */
void attachInterrupt(uint32_t pin, interrupt_handle_t callback, uint32_t mode)
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PSEL = 0U;  /* 0: Do not assign Peripheral */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PDR  = 0U;  /* 0: Input port,  1: Output port */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

    uint32_t IELSRn_IELS = 0x01;
    R_SYS_IrqEventLinkSet(pin, IELSRn_IELS, callback);
}

/*
 * \brief Turns off the given interrupt.
 */
void detachInterrupt(uint32_t pin)
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    /*!< Clear bit : PSEL, PMR, ASEL, ISEL */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PSEL = 0U;  /* 0: Do not assign Peripheral */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}

