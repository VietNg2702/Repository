#include "system_spi.h"


extern ARM_DRIVER_SPI Driver_SPI0;
extern ARM_DRIVER_SPI Driver_SPI1;

static void spi0_callback(uint32_t event)  __attribute__ ((section(".ramfunc")));
static void spi1_callback(uint32_t event)  __attribute__ ((section(".ramfunc")));

static uint8_t spi_eventWait;
static int err;



void system_spi_init( ARM_DRIVER_SPI* spi)
{
	GLOBAL_INT_DISABLE( );
	if (spi == &Driver_SPI0) 
	{
		err = spi->Initialize(spi0_callback); 
	} 
	else if (spi == &Driver_SPI1)
	{
		err = spi->Initialize(spi1_callback);
	} 
	else 
	{
		err=1;
	}

	APP_ERR_HANDLER(err);

	err = spi->PowerControl(ARM_POWER_FULL); 
	APP_ERR_HANDLER(err);

	err = spi->Control(ARM_SPI_MODE_MASTER  | 
					ARM_SPI_DATA_BITS(8) | 
					ARM_SPI_SS_MASTER_UNUSED | 
					ARM_SPI_CPOL0_CPHA0  | 
					ARM_SPI_MSB_LSB,1000000);//  MSB first 1Mbps

//   err = spi->Control(ARM_SPI_MODE_MASTER  |
//                                        ARM_SPI_DATA_BITS(8) |
//                                          ARM_SPI_SS_MASTER_UNUSED |
//                                            ARM_SPI_CPOL0_CPHA0  |
//                                            ARM_SPI_MSB_LSB,8000000);  // MSB first 8Mbps


	APP_ERR_HANDLER(err);

	GLOBAL_INT_RESTORE();
}

void system_spi_stop( ARM_DRIVER_SPI* spi)
{
	if (spi == &Driver_SPI0) 
	{
        err = spi->Uninitialize();
	    R_LPM_ModuleStop(LPM_MSTP_SPI0);
	} 
	else if (spi == &Driver_SPI1)
	{
        err = spi->Uninitialize();
	    R_LPM_ModuleStop(LPM_MSTP_SPI1);
	}
	else 
	{
		err=1;
	}

    APP_ERR_HANDLER(err);
}

void system_spi_write_read( ARM_DRIVER_SPI* spi, const uint8_t* cbuffer, uint8_t* rbuffer, uint16_t length )
{
	int err;
	uint32_t timeout = 0x020000;

	if (length ==0) 
	{
		return;
	}

	spi_eventWait = 0; 

	err = spi->Transfer(&cbuffer, &rbuffer, length);
	APP_ERR_HANDLER(err);

	while((spi_eventWait == 0) && (--timeout != 0));

	APP_ERR_HANDLER(spi_eventWait != ARM_SPI_EVENT_TRANSFER_COMPLETE);
}

void system_spi_write( ARM_DRIVER_SPI* spi, const uint8_t* buffer, uint16_t length )
{
	int err;
	uint32_t timeout = 0x02000000;

	if (length ==0) 
	{
		return;
	}

	spi_eventWait = 0; 

	err = spi->Send(buffer, length);
	APP_ERR_HANDLER(err);

	while((spi_eventWait == 0) && (--timeout != 0));

	APP_ERR_HANDLER(spi_eventWait != ARM_SPI_EVENT_TRANSFER_COMPLETE);
}

void system_spi_read( ARM_DRIVER_SPI* spi, uint8_t* buffer, uint16_t length )
{
	int err;
	uint32_t timeout = 0x02000000;
	
	if (length ==0) 
	{
		return;
	}

	spi_eventWait = 0; 
	
	err = spi->Receive(buffer, length);
	APP_ERR_HANDLER(err);
	
	while((spi_eventWait == 0) && (--timeout != 0));
  
  	APP_ERR_HANDLER(spi_eventWait != ARM_SPI_EVENT_TRANSFER_COMPLETE);
  
}

static void spi0_callback(uint32_t event)
{
	spi_eventWait  = event;
}

static void spi1_callback(uint32_t event)
{
	spi_eventWait  = event;
}



