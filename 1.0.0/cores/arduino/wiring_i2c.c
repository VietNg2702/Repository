#include "wiring_i2c.h"

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#define I2C_DEVICE_ADDRESS 0x01

extern ARM_DRIVER_I2C Driver_I2C0;
ARM_DRIVER_I2C * ARDUINO_I2C0 = &Driver_I2C0;
extern ARM_DRIVER_I2C Driver_I2C1;
ARM_DRIVER_I2C * ARDUINO_I2C1 = &Driver_I2C1;

static void i2c_callback(uint32_t event)
{
    if (0 != (event & ARM_I2C_EVENT_TRANSFER_DONE))
    {
        /* Transfer complete event. */
        // if (RIIC_TRANSMITTER == gsp_i2c1_dev->GetStatus().direction)
        // {
        //     /* in send mode. */
        // }
        // else
        // {
        //     /* in receive mode. */
        // }
    }
}   /* End of function iic_callback() */

void i2c_initialize(ARM_DRIVER_I2C *I2C)
{
    /** Initialization the I2C module */
    if (I2C->Initialize(i2c_callback) != ARM_DRIVER_OK)
    {
        while (1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (I2C->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK)
    {
        while (1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (I2C->Control(ARM_I2C_OWN_ADDRESS, I2C_DEVICE_ADDRESS) != ARM_DRIVER_OK)
    {
        while (1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (I2C->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_STANDARD) != ARM_DRIVER_OK)
    {
        while (1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}

void i2c_transmit(ARM_DRIVER_I2C *I2C, uint32_t addr, uint8_t *data, uint8_t size)
{
     /** Writing process to EEPROM */
    if (I2C->MasterTransmit(addr, data, size, false) != ARM_DRIVER_OK)
    {
        
    }
}

void i2c_receive(ARM_DRIVER_I2C *I2C, uint32_t addr, uint8_t *data, uint8_t size)
{
    /** Start restart condition receive. */
    if (I2C->MasterReceive(addr, data, size, false) != ARM_DRIVER_OK)
    {
       
    }
}