#include "wiring_i2c.h"


#define I2C_DEVICE_ADDRESS      (0x50)          /* Device Address of eeprom */
#define I2C_TRANSFER_DATA_SIZE  (7)             /* Send data size(byte) */
#define I2C_RECEIVE_DATA_SIZE   (5)             /* Receive data size(byte) */
#define I2C_DATA_ADDRESS_SIZE   (2)             /* Data address size(byte) */

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
extern ARM_DRIVER_I2C Driver_I2C1;
static ARM_DRIVER_I2C * gsp_i2c1_dev = &Driver_I2C1;

static uint8_t gs_iic_snd_data[I2C_TRANSFER_DATA_SIZE] = {0x00, 0x00, 0x11, 0x12, 0x13, 0x14, 0x15};
static uint8_t gs_iic_rcv_data[I2C_RECEIVE_DATA_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00};

static void i2c_callback(uint32_t event)
{
    if (0 != (event & ARM_I2C_EVENT_TRANSFER_DONE))
    {
        /* Transfer complete event. */
        if (RIIC_TRANSMITTER == gsp_i2c1_dev->GetStatus().direction)
        {
            /* in send mode. */
        }
        else
        {
            /* in receive mode. */
        }
    }
}   /* End of function iic_callback() */

void i2c_initialize(void)
{
    /** Initialization the I2C module */
    if (gsp_i2c1_dev->Initialize(i2c_callback) != ARM_DRIVER_OK)
    {
        while (1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (gsp_i2c1_dev->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK)
    {
        while (1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (gsp_i2c1_dev->Control(ARM_I2C_OWN_ADDRESS, I2C_DEVICE_ADDRESS) != ARM_DRIVER_OK)
    {
        while (1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (gsp_i2c1_dev->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_STANDARD) != ARM_DRIVER_OK)
    {
        while (1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}

void i2c_transmit(uint8_t *data, uint8_t size)
{
     /** Writing process to EEPROM */
    if (gsp_i2c1_dev->MasterTransmit(I2C_DEVICE_ADDRESS, gs_iic_snd_data, I2C_TRANSFER_DATA_SIZE, false) != ARM_DRIVER_OK)
    {
        
    }
}

void i2c_receive(uint8_t *data, uint8_t size)
{
    /** Start restart condition receive. */
    if (gsp_i2c1_dev->MasterReceive(I2C_DEVICE_ADDRESS, gs_iic_rcv_data, I2C_RECEIVE_DATA_SIZE, false) != ARM_DRIVER_OK)
    {
       
    }
}