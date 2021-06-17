#include "../inc/system_gpio.h"
#include "../inc/system.h"

void GpioMcuSetInterrupt(uint16_t port, uint16_t pin, int16_t irqMode, int8_t irqPriority )
{
    if (0x06 == port && 0x4 == pin)  // F_CS .. IRQ3
    {
        ICU->IRQCR3_b.IRQMD = 0x1; // rising edge interrupt (0x0 for falling edge)
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
        R_ICU_Pinset_CH3();
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3, 0x1, (system_int_cb_t)IRQ3_IRQHandler);
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
        R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3, 1);
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
        
        
    }
    else if (0x02 == port && 0x4 == pin)  // .. IRQ7
    {
        ICU->IRQCR7_b.IRQMD = 0x1; // rising edge interrupt (0x0 for falling edge)
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ7);
        R_ICU_Pinset_CH7();
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ7, 0x13, (system_int_cb_t)IRQ7_IRQHandler);
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ7);
        R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ7, 1);
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ7);
        
    }
    else if (0x04 == port && 0x9 == pin)  // .. IRQ9
    {
        ICU->IRQCR9_b.IRQMD = 0x1; // rising edge interrupt (0x0 for falling edge)
        R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ9);
        R_ICU_Pinset_CH9();
        R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ9, 0x1F, (system_int_cb_t)IRQ9_IRQHandler);
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ9);
        R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ9, 1);
        R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ9);
           
    }
    else
    {
        // no other GPIO interrupts configured
    }     
}


void system_gpio_init_input(uint16_t port, uint16_t pin, system_gpio_interrupt_t irq_mode )
{
	uint32_t *pAddr;
	uint32_t tempReg;
    tempReg = 0;

	pAddr = (uint32_t *) (PFS_BASE + (0x40 * port) + (0x04 * (0x0f & pin) ));

    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    *pAddr = tempReg;

    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

    if (irq_mode == SYSTEM_GPIO_RISING || irq_mode == SYSTEM_GPIO_FALLING || irq_mode == SYSTEM_GPIO_BOTH) 
    {
        GpioMcuSetInterrupt(port,  pin, irq_mode, 1);
    }

    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    tempReg = *pAddr;
    if (irq_mode == SYSTEM_GPIO_RISING) {
        tempReg |= 0x05 << 12;
    } else if (irq_mode == SYSTEM_GPIO_FALLING) {
        tempReg |= 0x06 << 12;
    } else if (irq_mode == SYSTEM_GPIO_BOTH) {
        tempReg |= 0x07 << 12;
    }
    *pAddr = tempReg;
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);

}

void system_gpio_init_output( uint16_t port, uint16_t pin, uint8_t value )
{
    uint32_t *pAddr = (uint32_t *) (PFS_BASE + (0x40 * port) + (0x04 * (0x0f & pin) ));

    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);

    *pAddr = 0x04 | (value & 0x01);
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}


void system_gpio_init( void )
{
    system_gpio_init_output( LR1110_LED_SCAN_PORT, LR1110_LED_SCAN_PIN, 0 );
    system_gpio_init_output( LR1110_LED_TX_PORT, LR1110_LED_TX_PIN, 0 );
    system_gpio_init_output( LR1110_LED_RX_PORT, LR1110_LED_RX_PIN, 0 );
    system_gpio_init_output( LR1110_RESET_PORT, LR1110_RESET_PIN, 1 );
    system_gpio_init_output( LR1110_NSS_PORT, LR1110_NSS_PIN, 1 );
    system_gpio_init_input( LR1110_IRQ_PORT, LR1110_IRQ_PIN, SYSTEM_GPIO_NO_INTERRUPT );
    system_gpio_init_input( LR1110_IRQ_DUM_PORT, LR1110_IRQ_DUM_PIN, SYSTEM_GPIO_RISING );
    system_gpio_init_input( LR1110_BUSY_PORT, LR1110_BUSY_PIN, SYSTEM_GPIO_NO_INTERRUPT );
    system_gpio_init_input( TOUCH_IRQ_PORT, TOUCH_IRQ_PIN, SYSTEM_GPIO_RISING );
    system_gpio_init_output( DISPLAY_NSS_PORT, DISPLAY_NSS_PIN, 1 );
    system_gpio_init_output( DISPLAY_DC_PORT, DISPLAY_DC_PIN, 0 );
    system_gpio_init_input( TOUCH_IRQ_PORT, TOUCH_IRQ_PIN, SYSTEM_GPIO_RISING ); // PORT204 IRQ7 
    system_gpio_init_input( ACCELERATOR_IRQ_PORT, ACCELERATOR_IRQ_PIN, SYSTEM_GPIO_RISING );// PORT409 IRQ9 
    system_gpio_init_output( LR1110_LNA_PORT, LR1110_LNA_PIN, 0 );
    system_gpio_init_output( DCDC_EN_PORT, DCDC_EN_PIN, 1 );
    system_gpio_init_input( BUTTON_BLUE_PORT, BUTTON_BLUE_PIN, SYSTEM_GPIO_NO_INTERRUPT );
}

void system_gpio_init_direction_state( const gpio_t gpio, const system_gpio_pin_direction_t direction,
                                       const system_gpio_pin_state_t state )
{
    switch( direction )
    {
    case SYSTEM_GPIO_PIN_DIRECTION_INPUT:
        system_gpio_init_input( gpio.port, gpio.pin, SYSTEM_GPIO_NO_INTERRUPT );
        break;
    case SYSTEM_GPIO_PIN_DIRECTION_OUTPUT:
        system_gpio_init_output( gpio.port, gpio.pin, state );
        break;
    default:
        break;
    }
}

void system_gpio_set_pin_state( gpio_t gpio, const system_gpio_pin_state_t state )
{
    uint8_t pin = gpio.pin;
    uint8_t port = gpio.port;
    uint32_t *pAddr = (uint32_t *) (PORT0_BASE + (0x20 * port) ); 

    if (state)
    {
        *pAddr |= 0x1 << (pin + 16);
    }
    else
    {
        *pAddr &= ~( 0x1 << (pin +16) );
    }
}

system_gpio_pin_state_t system_gpio_get_pin_state( gpio_t gpio )
{
    uint8_t pin = gpio.pin;
    uint8_t port = gpio.port;

    uint32_t *pAddr = (uint32_t *) (PORT0_BASE + (0x20 * port) + 4); 

    uint32_t value = *pAddr & (0x1 << pin); 

    return (value > 0? 1 : 0);
}

void system_gpio_wait_for_state( gpio_t gpio, system_gpio_pin_state_t state )
{
	system_gpio_pin_state_t current_state;
	for (;;) {
    	current_state = system_gpio_get_pin_state(gpio );
		if (current_state == state) {break;}
	}
}
