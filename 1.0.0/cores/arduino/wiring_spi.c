#include "wriring_spi.h"

/***********************************************************************************************************************
Macros
***********************************************************************************************************************/
#define SPI_TRANSFER_SPEED  (100000U)   /* SPI bus speed(bps) */

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/



static void spi_callback(uint32_t event)
{
    if (0 != (event & ARM_SPI_EVENT_TRANSFER_COMPLETE))
    {
        /* Set complete flag. */
    }
}   /* End of function spi1_callback() */

void spi_initialize(ARM_DRIVER_SPI *SPI);
{
    /** Initialization the SPI module */
    /** SPI0 setting. */
    if (ARM_DRIVER_OK != SPI->Initialize(&spi_callback))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (ARM_DRIVER_OK != SPI->PowerControl(ARM_POWER_FULL))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (ARM_DRIVER_OK != SPI->Control(ARM_SPI_MODE_MASTER |
                                        ARM_SPI_CPOL0_CPHA0 |
                                        ARM_SPI_MSB_LSB |
                                        ARM_SPI_SS_MASTER_HW_OUTPUT |
                                        ARM_SPI_DATA_BITS(8) , SPI_TRANSFER_SPEED))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}


void spi_receive(ARM_DRIVER_SPI *SPI, uint8_t *data, uint8_t size)
{
    /** Receive data */
    if (ARM_DRIVER_OK != SPI->Receive(data, size))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}

void spi_send(ARM_DRIVER_SPI *SPI, uint8_t *data, uint8_t size)
{
    /** Send data */
    if (SPI->Send(data, size) != ARM_DRIVER_OK)
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}

void spi_transfer(ARM_DRIVER_SPI *SPI, uint8_t *in_data, uint8_t *out_data, uint8_t size)
{
    /** Transfer data */
    if (SPI->Transfer(out_data, in_data, size) != ARM_DRIVER_OK)
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}
