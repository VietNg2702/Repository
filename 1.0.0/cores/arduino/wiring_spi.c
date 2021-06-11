#include "wriring_spi.h"

/***********************************************************************************************************************
Macros
***********************************************************************************************************************/
#define SPI_TRANSFER_SPEED  (100000U)   /* SPI bus speed(bps) */
#define SPI_DATA_SIZE       (5)         /* Transfer data size(byte) */

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
extern ARM_DRIVER_SPI Driver_SPI0;
static ARM_DRIVER_SPI *gsp_spi0_dev = &Driver_SPI0;
extern ARM_DRIVER_SPI Driver_SPI1;
static ARM_DRIVER_SPI *gsp_spi1_dev = &Driver_SPI1;

static void spi0_callback(uint32_t event);
static void spi1_callback(uint32_t event);

static uint8_t  gs_spi_snd_data[SPI_DATA_SIZE] = {0xA1,0xA2,0x63,0x94,0xAB};
static uint8_t gs_spi_rcv_data[SPI_DATA_SIZE] = {0x00,0x00,0x00,0x00,0x00};

volatile static uint8_t gs_spi_snd_flag;
volatile static uint8_t gs_spi_rcv_flag;

static void spi0_callback(uint32_t event)
{
    if (0 != (event & ARM_SPI_EVENT_TRANSFER_COMPLETE))
    {
        gs_spi_snd_flag = true; /* Set send complete flag. */
    }
}   /* End of function spi0_callback() */

static void spi1_callback(uint32_t event)
{
    if (0 != (event & ARM_SPI_EVENT_TRANSFER_COMPLETE))
    {
        gs_spi_rcv_flag = true; /* Set receive complete flag. */
    }
}   /* End of function spi1_callback() */

void spi0_initialize(void)
{
    /** Initialization the SPI module */
    /** SPI0 setting. */
    if (ARM_DRIVER_OK != gsp_spi0_dev->Initialize(&spi0_callback))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (ARM_DRIVER_OK != gsp_spi0_dev->PowerControl(ARM_POWER_FULL))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (ARM_DRIVER_OK != gsp_spi0_dev->Control(ARM_SPI_MODE_MASTER |
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

void spi1_initialize(void)
{
    /** SPI1 setting. */
    if (ARM_DRIVER_OK != gsp_spi1_dev->Initialize(&spi1_callback))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (ARM_DRIVER_OK != gsp_spi1_dev->PowerControl(ARM_POWER_FULL))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
    if (ARM_DRIVER_OK != gsp_spi1_dev->Control(ARM_SPI_MODE_SLAVE |
                                                  ARM_SPI_CPOL0_CPHA0 |
                                                  ARM_SPI_MSB_LSB |
                                                  ARM_SPI_SS_SLAVE_HW |
                                                  ARM_SPI_DATA_BITS(8) , SPI_TRANSFER_SPEED))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}

void spi0_receive(uint8_t *data, uint8_t size)
{
    /** Receive data */
    if (ARM_DRIVER_OK != gsp_spi0_dev->Receive(gs_spi_rcv_data, SPI_DATA_SIZE))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}

void spi0_send(uint8_t *data, uint8_t size)
{
    /** Send data */
        if (gsp_spi0_dev->Send(gs_spi_snd_data, SPI_DATA_SIZE) != ARM_DRIVER_OK)
        {
           while(1)
           {
               ;   /* Intentionally empty braces. */
           }
        }
}

void spi1_receive(uint8_t *data, uint8_t size)
{
    /** Receive data */
    if (ARM_DRIVER_OK != gsp_spi1_dev->Receive(gs_spi_rcv_data, SPI_DATA_SIZE))
    {
        while(1)
        {
            ;   /* Intentionally empty braces. */
        }
    }
}

void spi1_send(uint8_t *data, uint8_t size)
{
    /** Send data */
        if (gsp_spi1_dev->Send(gs_spi_snd_data, SPI_DATA_SIZE) != ARM_DRIVER_OK)
        {
           while(1)
           {
               ;   /* Intentionally empty braces. */
           }
        }
}