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
#include "WInterrupts.h"
#define DetectionSenseSelect(i)                     ICU->IRQCR##i##_b.IRQMD
#define SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ(i)         SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ##i

#define ENABLE_INTERRUPT(irq, callback, mode)\
    {\
        DetectionSenseSelect(irq) = mode;\
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ(irq),0x13,callback);\
        R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ(irq),3);\
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ(irq));\
        R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ(irq));\
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ(irq));\
    }
    
void ICUPinSet(uint8_t pin)
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

}

void ICUPinClr(uint8_t pin)
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PSEL = 0U;  /* 0: Do not assign Peripheral */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PDR  = 0U;  /* 0: Input port,  1: Output port */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->ISEL = 0U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}

/*
 * \brief Specifies a named Interrupt Service Routine (ISR) to call when an interrupt occurs.
 *        Replaces any previous function that was attached to the interrupt.
 */
void attachInterrupt(uint32_t pin, interrupt_handle_t callback, interrupt_mode_t mode)
{
    uint32_t irq;
    switch(pin)
    {
        case 2:
            ICUPinSet(pin);
            ENABLE_INTERRUPT(7, callback, mode);
            break;
        case 3:
            ICUPinSet(pin);
            ENABLE_INTERRUPT(8, callback, mode);
            break;
        case 6:
            ICUPinSet(pin);
            ENABLE_INTERRUPT(3, callback, mode);
            break;
        case 8:
            ICUPinSet(pin);
            ENABLE_INTERRUPT(9, callback, mode);
            break;
        default:
            //error: "only support 2,3,6,8 pin"
            break;
    }
}

/*
 * \brief Turns off the given interrupt.
 */
void detachInterrupt(uint32_t pin)
{
    ICUPinClr(pin);
}