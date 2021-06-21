#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "RE01_256KB.h"
#include "lr1110_user_config_mode.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"
#include "r_spi_cmsis_api.h"
#include "lib_additional.h"

#include "system/inc/system.h"
#include "lr1110/lr1110_system.h"
#include "lr1110/lr1110_gnss.h"
#include "lr1110/lr1110_gnss_types.h"
#include "lr1110/lr1110_hal.h"
#include "lr1110/lr1110_radio.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "transceiver_radio.h"
#include "transceiver_gnss.h"
#include "transceiver_wifi_scan.h"
#include "transceiver_almanac.h"
#include "transceiver_rtc.h"
#include "transceiver_power.h"

#include "peripherials/lis2de12.h"
#include "peripherials/isl9122.h"
#include "peripherials/battery.h"
#ifdef __cplusplus
}
#endif

#include "demo_configuration.h"
#include "demo_gnss_types.h"



 
#define EPOCH_BUFFER_LEN 10
#define ACK_WAIT_COUNT 1000  // msec
#define LR1110_RESPONSE_CODE_OK 0
#define MINIMUM_NB_SAT 4 // Minimum # of satellites to resolve position
#define ADC_MAX_VALUE 0x4000
#define WIFI_BINARY_DATA_SIZE (25)

radio_t lr1110;

UART_DRIVER_FIFO uart0_dev;
extern ARM_DRIVER_I2C Driver_I2C0;

extern demo_wifi_scan_all_results_t WifiScan_results;
extern environment_location_t location_assist;

uint8_t almanac_data[ALMANAC_DATA_LENGTH + PAYLOAD_LENGTH + 1];
char uart_send_buf[UART_BUFFER_SIZE+1];
char uart_receive_buf[UART_BUFFER_SIZE+1];
char radio_data_buf[RADIO_DATA_BUFFER_SIZE+1];

static lr1110_gnss_timings_t timings;
static lr1110_gnss_solver_assistance_position_t gnss_position;
static lr1110_system_irq_mask_t irq_mask_test = LR1110_SYSTEM_IRQ_NONE;

extern int8_t irq7_event_flag;
extern int8_t irq3_event_flag;
extern uint32_t rtc_global_count;
extern uint32_t agt_global_count;


static int8_t assist_data_valid_flag;
static int8_t almanac_data_valid_flag;
int8_t rtc_valid_flag;
uint16_t scan_adc;
float battery_voltage;

void setup(void)
{
    int32_t gs_ret;

    // Set Quickmode = 1
    SYSC->PRCR = 0xA50BU;
    EHC->EHCCR1 = 1U;
    SYSC->PRCR = 0xA500U;

    gs_ret = R_LPM_BackBiasModeEnable(LPM_VBB_CLOCK_SOSC);    /* Enabled BackBiasMode */
    APP_ERR_HANDLER(gs_ret);
    /* Check VBB setup status */
    do
    {
        gs_ret = R_LPM_BackBiasModeEnableStatusGet();
    }
    while ( LPM_VBB_ENABLED != gs_ret );

    
#if (TRACKER_TX == 1)

    R_LPM_PowerSupplyModeAllpwonSet();
    R_SYS_MediumSpeedClockStart();      /* Start MOCO */
    gs_ret = R_SYS_SystemClockMOCOSet();  /* set MOCO as System Clock */
    APP_ERR_HANDLER(gs_ret);
    gs_ret = R_SYS_HighSpeedClockStop();     /* Stop HOCO */
    APP_ERR_HANDLER(gs_ret);
    gs_ret = R_SYS_MainOscSpeedClockStop();     /* Stop MainOSC */
    APP_ERR_HANDLER(gs_ret);

#else //(TRACKER_TX == 0)

#ifdef USE_MAINOSC
    R_LPM_PowerSupplyModeAllpwonSet();
    R_SYS_MainOscSpeedClockStart();      /* Started MainOSC */
    gs_ret = R_SYS_SystemClockMOSCSet();  /* set MainOSC as System Clock */
    APP_ERR_HANDLER(gs_ret);
    gs_ret = R_SYS_HighSpeedClockStop();     /* Stop HOCO */
    APP_ERR_HANDLER(gs_ret);
    gs_ret = R_SYS_MediumSpeedClockStop();     /* Stop MOCO */
    APP_ERR_HANDLER(gs_ret);
#else //ifndef USE_MAINOSC
    R_LPM_PowerSupplyModeAllpwonSet();
    R_SYS_HighSpeedClockStart();      /* Started MainOSC */
    system_time_wait_ms( 100 );
    gs_ret = R_SYS_SystemClockHOCOSet();  /* set MainOSC as System Clock */
    APP_ERR_HANDLER(gs_ret);
    gs_ret = R_SYS_MainOscSpeedClockStop();     /* Stop HOCO */
    APP_ERR_HANDLER(gs_ret);
    gs_ret = R_SYS_MediumSpeedClockStop();     /* Stop MOCO */
    APP_ERR_HANDLER(gs_ret);
#endif //USE_MAINOSC

#endif //TRACKER_TX

#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    for (int ic = 0; ic < 3; ic++) 
    {
        PFS->P410PFS = 0x04 | 0x00;
        wait_long();
        PFS->P410PFS = 0x04 | 0x01;
        wait_long();
    }
#endif //DEBUG_LED
}


/**************************************************************

 Wake up by Accelerometer

**************************************************************/
int32_t last_irq7_event_count;


void wait_for_wakeup_interrupt()
{

#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    PFS->P210PFS = 0x04 | 0x00;
    system_time_wait_ms( 1000 );
    PFS->P210PFS = 0x04 | 0x01;
    PFS->P410PFS = 0x04 | 0x00;
    system_time_wait_ms( 500 );
    PFS->P410PFS = 0x04 | 0x01;
#endif //DEBUG_LED
    __NOP();
    while (1) 
    {
        irq7_event_flag = false;
        rtc_global_count =0;
        system_lpm_wait_rtc(); // SSTBY wait
        __NOP();
        if ((irq7_event_flag == true)
           && (((int32_t)rtc_read_current_binary_time() - last_irq7_event_count) < (int32_t)IRQ_INT_MINUMIM_INTERVAL)) {
            continue;
        } else {
            break;
        }
    }
    __NOP();
    last_irq7_event_count = (int32_t)rtc_read_current_binary_time();
  
#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    PFS->P210PFS = 0x04 | 0x00;
    PFS->P410PFS = 0x04 | 0x00;
    system_time_wait_ms( 1000 );
    PFS->P210PFS = 0x04 | 0x01;
    PFS->P410PFS = 0x04 | 0x01;
#endif //DEBUG_LED
}




static uint32_t reg_P300;
static uint32_t reg_P010;
static uint32_t reg_P815;
static uint32_t reg_P011;
static uint32_t reg_P000;
static uint32_t reg_P001;
static uint32_t reg_P003;


void system_uninit_PORT()
{
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    
    reg_P300 = PFS->P300PFS;
    reg_P010 = PFS->P010PFS;
    reg_P815 = PFS->P815PFS;
    reg_P011 = PFS->P011PFS;
    reg_P000 = PFS->P000PFS;
    reg_P001 = PFS->P001PFS;
    reg_P003 = PFS->P003PFS;
    
    PFS->P300PFS = 0x04;
    PFS->P010PFS = 0x04;
    PFS->P815PFS = 0x04;
    PFS->P011PFS = 0x04;
    PFS->P000PFS = 0x04;
    PFS->P001PFS = 0x04;
    PFS->P003PFS = 0x04;
    
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}

void system_restore_PORT()
{
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    
    PFS->P300PFS = reg_P300;
    PFS->P010PFS = reg_P010;
    PFS->P815PFS = reg_P815;
    PFS->P011PFS = reg_P011;
    PFS->P000PFS = reg_P000;
    PFS->P001PFS = reg_P001;
    PFS->P003PFS = reg_P003;
    
    R_SYS_RegisterProtectEnable(SYSTEM_REG_PROTECT_MPC);
}


void tracker_tx()
{

    int8_t nb_sat_assist;
    int8_t nb_sat_autonomus;

    last_irq7_event_count = (int32_t)rtc_read_current_binary_time() - (int32_t)IRQ_INT_MINUMIM_INTERVAL;


    //  last_assist_date_count = 0;
    assist_data_valid_flag = false;
    radio_init();
    DeviceModem_Init();
    while (1) 
    {
        lr1110_lna_off();
        lr1110_power_off();
        system_lpm_setup_rtc();
#if (INHIBIT_SSTBY != 1)
        system_uninit_ISO2();
        system_uninit_PORT();
        lr1110_dcdc_off();
        __NOP();
        wait_for_wakeup_interrupt(); // Wait for IRQ from acceleromater at SSTBY mode
#if (TX_BOARD_VERSION == 1)
        battery_voltage = 3.3;
#else //TX_BOARD_VERSION != 1
        battery_read(&scan_adc);
        if (scan_adc > 0 && scan_adc < ADC_MAX_VALUE) {
            battery_voltage = 1.25/scan_adc * ADC_MAX_VALUE;
        } else {
            return;
        }
#endif //TX_BOARD_VERSION

        if (battery_voltage < OPERATION_MINIMUM_VOLTAGE) {
            continue;
        }

        system_restore_PORT();
        system_init_ISO2();
        system_time_wait_ms( 100 );
        lr1110_dcdc_on();

#endif //INHIBIT_SSTBY
        if (battery_voltage > 2.3 && battery_voltage <= 3.3) 
        {
            isl9122_output_voltage_setting_request(battery_voltage);
        }
        system_time_wait_ms( 1 );
        lr1110_power_on();
        system_time_wait_ms( 100 );
        lr1110_hal_reset(&lr1110);
        radio_init();
        DeviceModem_Init();
        rtc_adjust_TX(&location_assist, &assist_data_valid_flag);
#if (USE_ALMANAC_UPDATE == 1)
    if (assist_data_valid_flag) {
       AlmanacUpdateTX();
    }
#endif

    system_time_wait_ms( 100 );
    uint16_t radio_tx_len;
    nb_sat_assist = 0;
    nb_sat_autonomus = 0;
    WifiInterface_SpecificRuntime();
    if (WifiScan_results.nbrResults > 2) {
        radio_init();
        radio_tx_len  = CommunicationDemo_Store( &WifiScan_results , (int8_t *)radio_data_buf );
        semtech_radio_tx_len( radio_data_buf, radio_tx_len, ACK_RECEIVE_INTERVAL);
        system_time_wait_ms( 100 );
    } else {
        if (assist_data_valid_flag) {
            nb_sat_assist = semtech_GNSS_remote(radio_data_buf, true, &radio_tx_len);
        }
        if (nb_sat_assist < MINIMUM_NB_SAT ||  assist_data_valid_flag == false) {
            nb_sat_autonomus = semtech_GNSS_remote(radio_data_buf, false, &radio_tx_len);
        }
        if (nb_sat_assist < MINIMUM_NB_SAT && nb_sat_autonomus >= MINIMUM_NB_SAT) {
            assist_data_valid_flag = false;
        }
        system_time_wait_ms( 100 );
        radio_init();
        semtech_radio_tx_len( radio_data_buf ,radio_tx_len, ACK_RECEIVE_INTERVAL);
    }

#if (DEBUG_LED == 1)
        sprintf(radio_data_buf, "#Date_Valid=%d, WiFi=%d, Assist=%d, Auto=%d\n",(int)assist_data_valid_flag,(int)WifiScan_results.nbrResults,  (int)nb_sat_assist, (int)nb_sat_autonomus);
        semtech_radio_tx( radio_data_buf, ACK_RECEIVE_INTERVAL);
#endif //DEBUG_LED

#ifdef WAIT_GNSS_ACK
        last_agt_count = system_lptim_get();
        while (system_lptim_get() < last_agt_count + (ACK_WAIT_INTERVAL-10)) {
        }
        radio_data_buf[0] = 0;
        semtech_radio_rx(radio_data_buf, ACK_RECEIVE_INTERVAL);
        if (strncmp(radio_data_buf, "%ACK", 4) == 0) {
            read_result(radio_data_buf+4);
        }
#endif //WAIT_GNSS_ACK

#if (DEBUG_LED == 1)
        R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
        PFS->P210PFS = 0x04 | 0x01;
        PFS->P410PFS = 0x04 | 0x01;
#endif //DEBUG_LED
        system_time_wait_ms( 100 );
    }
}

void tracker_rx()
{
    uint32_t last_agt_count;
    wifi_tx_packet* packet_pos;
    int16_t jc;
    uint8_t nbrResults;
	lr1110_radio_pkt_status_lora_t pkt_status;

    assist_data_valid_flag = false;

    radio_init();
    uart_tx_fifo_flush(&uart0_dev);
    uart_rx_fifo_flush(&uart0_dev);
    strcpy(uart_send_buf, "###### ===== Radio RX ==== ######\r\n"); 
    uart_fifo_send_string(&uart0_dev, uart_send_buf);
    system_time_wait_ms( 10 );
    assist_data_valid_flag = GetDateAndApproximateLocationFromPC( &location_assist , 0);
    almanac_data_valid_flag = GetAlmanacDataFromPC(almanac_data, 3000);

    while (1) 
    {
        semtech_radio_rx(radio_data_buf, 0);
        last_agt_count = system_lptim_get(); 

#if (GET_RSSI_SNR == 1)
		lr1110_radio_get_lora_pkt_status( &lr1110, &pkt_status );
#endif //GET_RSSI_SNR
        if (radio_data_buf[0] != '@') {
            sprintf(uart_send_buf, "#Receive from TX ZZ%sZZ\n\n",radio_data_buf);
        } else {
            strcpy(uart_send_buf, "#Receive from TX ZZ");
            strncat(uart_send_buf, radio_data_buf,2);
            strcat(uart_send_buf, "\n");
        }
        uart_fifo_send_string(&uart0_dev, uart_send_buf);
        system_time_wait_ms( 1 );
        if (strncmp(radio_data_buf, "!DATE", 5) == 0) {
            system_time_wait_ms( 1 );
            while (system_lptim_get() < last_agt_count + ACK_WAIT_COUNT ) {
            }
            if (assist_data_valid_flag) {
                sprintf(radio_data_buf, " DATE%u,%f,%f,%f\n",rtc_read_current_binary_time(), location_assist.altitude, location_assist.latitude, location_assist.longitude);
            } else {
                strcpy(radio_data_buf, " DATE0,0.0,0.0,0.0\n");
            }
            radio_data_buf[0]='%';
            semtech_radio_tx(radio_data_buf, ACK_RECEIVE_INTERVAL);
            sprintf(uart_send_buf, "#Send to TX ZZ%sZZ\n\n",radio_data_buf);
            uart_fifo_send_string(&uart0_dev, uart_send_buf);
            if (assist_data_valid_flag != true) {
                system_time_wait_ms( 1000 );
                assist_data_valid_flag = GetDateAndApproximateLocationFromPC( &location_assist ,  ACK_WAIT_COUNT);
            }
        } else if (strncmp(radio_data_buf, "!ALMA", 5) == 0) {
            system_time_wait_ms( 1 );
            while (system_lptim_get() < last_agt_count + ACK_WAIT_COUNT ) {
            }
            if (almanac_data_valid_flag) {
                strcpy(radio_data_buf, " ALMA1\n");
            } else {
                strcpy(radio_data_buf, " ALMA0\n");
            }
            radio_data_buf[0]='%';
            semtech_radio_tx(radio_data_buf, 3000);
            sprintf(uart_send_buf, "#Send to TX ZZ%sZZ\n\n",radio_data_buf);
            uart_fifo_send_string(&uart0_dev, uart_send_buf);
            if (almanac_data_valid_flag) {
                system_time_wait_ms( 5000 );
                strcpy(uart_send_buf, "#Send to TX ALMANAC Start\n\n");
                uart_fifo_send_string(&uart0_dev, uart_send_buf);
                semtech_radio_tx_len((char *)almanac_data, ALMANAC_DATA_LENGTH, 60000);
                strcpy(uart_send_buf, "#Send to TX ALMANAC Finish\n\n");
                uart_fifo_send_string(&uart0_dev, uart_send_buf);
            }
            if (almanac_data_valid_flag != true) {
                almanac_data_valid_flag = GetAlmanacDataFromPC(almanac_data, 3000);
            }
        } else if (strncmp(radio_data_buf, "!SEND", 5) == 0) {
#if (SEND_TO_SERVER == 1)
            strcpy(uart_send_buf, "!SEND\n");
            uart_fifo_send_string(&uart0_dev, uart_send_buf);
            while (uart_fifo_send_finished(&uart0_dev) == false);
            strcpy(uart_send_buf, "!RESULT\n");
            uart_fifo_send_string(&uart0_dev, uart_send_buf);
            while (uart_fifo_send_finished(&uart0_dev) == false);
#endif //SEND_TO_SERVER
        } else if (radio_data_buf[0] == '@') {
            strcpy(uart_send_buf, "!FLUSH\n");
            uart_fifo_send_string(&uart0_dev, uart_send_buf);
            while (uart_fifo_send_finished(&uart0_dev) == false);
            if (radio_data_buf[1] == 'G') {
                gnss_packet_to_string((gnss_tx_header*)radio_data_buf, uart_send_buf);
                uart_fifo_send_string(&uart0_dev, uart_send_buf);
                while (uart_fifo_send_finished(&uart0_dev) == false);
            } else if (radio_data_buf[1] == 'W') {
                nbrResults = ((wifi_tx_header*)radio_data_buf)->nbrResults;
                packet_pos = (wifi_tx_packet*)(radio_data_buf+sizeof(wifi_tx_header));
                for (jc=0; jc<nbrResults; jc++) {
                    WifiScan_PacketToString((wifi_tx_header*)radio_data_buf, packet_pos, uart_send_buf);
                    packet_pos ++;
                    uart_fifo_send_string(&uart0_dev, uart_send_buf);
                    while (uart_fifo_send_finished(&uart0_dev) == false);
                }
                sprintf(uart_send_buf, "#Received # of mac address %d\n\n",(int)nbrResults);
                uart_fifo_send_string(&uart0_dev, uart_send_buf);
            }

#if SEND_TO_SERVER
            strcpy(uart_send_buf, "!SEND\n");
            uart_fifo_send_string(&uart0_dev, uart_send_buf);
            while (uart_fifo_send_finished(&uart0_dev) == false);
            strcpy(uart_send_buf, "!RESULT\n");
            uart_fifo_send_string(&uart0_dev, uart_send_buf);
            while (uart_fifo_send_finished(&uart0_dev) == false);
#endif //SEND_TO_SERVER
        } 

#if (GET_RSSI_SNR == 1)
        sprintf(uart_send_buf, "#RSSI=  %d, SNR= %d, Signal_RSSI = %d\n", (int)(pkt_status.rssi_pkt_in_dbm), (int)(pkt_status.snr_pkt_in_db), (int)(pkt_status.signal_rssi_pkt_in_dbm) );
        uart_fifo_send_string(&uart0_dev, uart_send_buf);
#endif //GET_RSSI_SNR
        sprintf(uart_send_buf, "#current_time %d\n", system_lptim_get());
        uart_fifo_send_string(&uart0_dev, uart_send_buf);
    }
}


static int semtech_main( void )
{
    lr1110_system_version_t version;

    rtc_valid_flag = false;

#if (TX_BOARD_VERSION == 1)
    battery_voltage = 3.3;
#else //TX_BOARD_VERSION =! 1
    scan_adc=0;
    battery_read(&scan_adc);
    if (scan_adc > 0 && scan_adc < ADC_MAX_VALUE) {
        battery_voltage = 1.25/scan_adc * ADC_MAX_VALUE;
    } else {
        APP_ERR_RETURN(-1);
    }
#endif //TX_BOARD_VERSION

    system_init(&lr1110);
    system_init_ISO2();
    system_time_wait_ms( 10 );
    lr1110_dcdc_on(); // DCDC must be ON before Initialize
    if (battery_voltage > 2.3 && battery_voltage <= 3.3) {
        isl9122_output_voltage_setting_request(battery_voltage);
    }
    system_time_wait_ms( 100 );

#if (TRACKER_TX == 1)
    accelerometer_init();
    system_i2c_stop(&Driver_I2C0);
#endif //TRACKER_TX

    DeviceModem_Init();

#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    PFS->P410PFS = 0x04 | 0x01;
    PFS->P210PFS = 0x04 | 0x00;
#endif //DEBUG_LED

    lr1110_system_set_dio_irq_params( &lr1110, LR1110_SYSTEM_IRQ_ALL_MASK, 0x00 );

    for (int ic = 0; ic < 1; ic++) 
    {
        lr1110_system_get_version( &lr1110, &version );
        system_time_wait_ms( 1000 );
    }

    lr1110_system_get_irq_status( &lr1110, &irq_mask_test );
    lr1110_system_clear_irq_status( &lr1110, LR1110_SYSTEM_IRQ_ALL_MASK );

#if (DEBUG_LED == 1)
    R_SYS_RegisterProtectDisable(SYSTEM_REG_PROTECT_MPC);
    PFS->P410PFS = 0x04 | 0x01;
    PFS->P210PFS = 0x04 | 0x01;
#endif //DEBUG_LED

#if (TRACKER_TX == 1)
    tracker_tx();
#else //TRACKER_TX != 1
    tracker_rx();
#endif //TRACKER_TX
    return 0;
}

void loop(void)
{
    semtech_main();
}
