#include "config_mode.h"
#include "configuration.h"
#include "../inc/system.h"
#include "../../lr1110/lr1110_hal.h"

extern ARM_DRIVER_I2C Driver_I2C0;
//extern ARM_DRIVER_I2C Driver_I2C1;
extern ARM_DRIVER_SPI Driver_SPI0;
extern ARM_DRIVER_USART Driver_USART0;

extern uint32_t rtc_global_count;
extern UART_DRIVER_FIFO uart0_dev;


void system_init(radio_t* radio)
{

	radio->nss.port   = LR1110_NSS_PORT;
	radio->nss.pin    = LR1110_NSS_PIN;
	radio->reset.port = LR1110_RESET_PORT;
	radio->reset.pin  = LR1110_RESET_PIN;
	radio->busy.port  = LR1110_BUSY_PORT;
	radio->busy.pin  = LR1110_BUSY_PIN;
	radio->irq.port  = LR1110_IRQ_PORT;
	radio->irq.pin  = LR1110_IRQ_PIN;
	radio->spi = &Driver_SPI0;

//    R_LPM_ModuleStart(LPM_MSTP_SCI0);
//    R_LPM_ModuleStart(LPM_MSTP_SPI0);
//    R_LPM_ModuleStart(LPM_MSTP_RTC);
//    R_LPM_ModuleStart(LPM_MSTP_AGT0);


    system_clock_init( );
    system_gpio_init( );
    system_time_init( );
    system_lptim_init( );
    rtc_init( );
//    rtc_init( (void*)0);
	rtc_set_current_binary_time(10);
	rtc_counter_run(RTC_COUNTER_START);
	rtc_alarm_init();

	system_lptim_set_and_run(0xFFFFFFFF); // agtw0 32bit timer
	lr1110_hal_reset( radio );
    system_time_wait_ms( 1 );
}




void system_init_ISO2()
{
    system_spi_init(&Driver_SPI0);
    system_i2c_init(&Driver_I2C0);

#if (TRACKER_TX != 1)
	system_uart_init(&uart0_dev);
	uart_fifo_receive_start(&uart0_dev);
	uart_rx_fifo_flush(&uart0_dev);
#endif
}

void system_uninit_ISO2()
{
    system_spi_stop(&Driver_SPI0);
    system_i2c_stop(&Driver_I2C0);
#if (TRACKER_TX != 1)
	system_uart_stop(&uart0_dev);

#endif
}

void rtc_alarm_init()
{
    uint32_t current_time;
	rtc_global_count=0;
    current_time = rtc_read_current_binary_time();
    rtc_set_alarm_binary_time (current_time);
    rtc_enable_alarms_binary (0x00000FFF); // RTC interrupt interval 4096sec
}
