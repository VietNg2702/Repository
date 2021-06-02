#include "variant.h"
#include "wiring_digital.h"
#include "r_system_api.h"

__IOM uint32_t *ARDUINO_DIGITAL_PINS[10]={
    ARDUINO_IO0_RXD5_B,
    ARDUINO_IO1_TXD5_B,
    ARDUINO_IO2_IRQ7_B,
    ARDUINO_IO3_IRQ8_C,
    ARDUINO_IO4,
    ARDUINO_IO5_GTIOC5A_B,
    ARDUINO_IO6_GTIOC5B_B,
    ARDUINO_IO7,
    ARDUINO_IO8,
    ARDUINO_IO9_GTIOC2A_B
    };

void pinMode(uint32_t pin, uint32_t mode)
{
    /* Disable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->ASEL = 0U;  /* 0: Do not use as an analog pin, 1: Use as an analog pin. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PSEL = 0U;  /* 0: Do not assign Peripheral */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PDR  = mode&(0x00000001);  /* 0: Input port,  1: Output port */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PMR  = 0U;  /* 0: Use the pin as a general I/O port, 1: Use the pin as a peripheral module. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->ISEL = 1U;  /* 0: Do not use as an IRQn input pin,  1: Use as an IRQn input pin. */
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PUCR = mode&(0x00000010); /* 0: No pull, 1: Pull up */

    /* Enable protection for PFS function (Set to PWPR register) */
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}

void digitalWrite(uint32_t pin, uint32_t val)
{
    ((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PODR = val&(0x00000001);
}

int digitalRead(uint32_t pin)
{

    return (((PxxPFS_b_t *)ARDUINO_DIGITAL_PINS[pin])->PIDR&(0x00000001));
}

