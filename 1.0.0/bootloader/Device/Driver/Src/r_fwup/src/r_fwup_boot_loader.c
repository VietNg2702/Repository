/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_fwup_boot_loader.c
* Version      : 1.00
* Description  : Boot loader part of Firmware updater Driver.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 28.12.2020 1.00     Initial Release
**********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "r_fwup_cfg.h"  /* Firmware update config definitions */
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == 2) /* FWUP_IMPLEMENTATION_AFRTOS */
#include "aws_iot_ota_agent.h"
#endif

#ifdef BSP_MCU_SERIES_RX600
#include "r_smc_entry.h"
#include "r_flash_rx_if.h"
#include "r_sci_rx_if.h"
#include "r_sys_time_rx_if.h"
#else /* #ifdef BSP_MCU_SERIES_RX600 */
#include "RE01_256KB.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"
#include "r_flash_cfg.h"
#include "r_flash_re01_256kb.h"
#include "r_flash_api.h"
#include "r_agt_api.h"
#if (FWUP_CFG_COMMUNICATION_FUNCTION == 0) /* FWUP_COMMUNICATION_SCI */
#include "r_usart_cmsis_api.h"
#endif /* FWUP_COMMUNICATION_SCI */
#endif /* #ifdef BSP_MCU_SERIES_RX600 */

#ifdef BSP_MCU_SERIES_RX600
#include "r_fwup_if.h"
#include "r_fwup_private.h"
#else /* BSP_MCU_SERIES_RX600 */
#include "r_fwup_api.h"
#endif  /* BSP_MCU_SERIES_RX600 */

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
#ifdef BSP_MCU_SERIES_RX600
#include "r_sci_rx_pinset.h"
#endif

#define FLASH_DF_TOTAL_BLOCK_SIZE (FLASH_DF_BLOCK_INVALID - FLASH_DF_BLOCK_0)

#ifdef BSP_MCU_SERIES_RX600
#if !defined(MY_BSP_CFG_SERIAL_TERM_SCI)
#error "Error! Need to define MY_BSP_CFG_SERIAL_TERM_SCI in r_bsp_config.h"
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (0)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI0()
#define SCI_CH_serial_term          SCI_CH0
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (1)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI1()
#define SCI_CH_serial_term          SCI_CH1
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (2)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI2()
#define SCI_CH_serial_term          SCI_CH2
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (3)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI3()
#define SCI_CH_serial_term          SCI_CH3
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (4)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI4()
#define SCI_CH_serial_term          SCI_CH4
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (5)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI5()
#define SCI_CH_serial_term          SCI_CH5
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (6)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI6()
#define SCI_CH_serial_term          SCI_CH6
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (7)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI7()
#define SCI_CH_serial_term          SCI_CH7
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (8)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI8()
#define SCI_CH_serial_term          SCI_CH8
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (9)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI9()
#define SCI_CH_serial_term          SCI_CH9
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (10)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI10()
#define SCI_CH_serial_term          SCI_CH10
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (11)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI11()
#define SCI_CH_serial_term          SCI_CH11
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (12)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI12()
#define SCI_CH_serial_term          SCI_CH12
#else
#error "Error! Invalid setting for MY_BSP_CFG_SERIAL_TERM_SCI in r_bsp_config.h"
#endif /* !defined(MY_BSP_CFG_SERIAL_TERM_SCI) */
#endif /* #ifdef BSP_MCU_SERIES_RX600  */

#ifdef BSP_MCU_SERIES_RX600
void my_sci_callback(void *pArgs) FUNC_LOCATION_FWUP_RESOURCES;
#endif /* #ifdef BSP_MCU_SERIES_RX600 */

FWUP_STATIC int32_t firm_block_read(uint32_t *firmware, uint32_t offset) FUNC_LOCATION_FWUP_RESOURCES;
FWUP_STATIC int32_t const_data_block_read(uint32_t *const_data, uint32_t offset) FUNC_LOCATION_FWUP_RESOURCES;
static void bank_swap_with_software_reset(void) FUNC_LOCATION_FWUP_RESOURCES;
static void software_reset(void) FUNC_LOCATION_FWUP_RESOURCES;
FWUP_STATIC const uint8_t *get_status_string(uint8_t status) FUNC_LOCATION_FWUP_RESOURCES;
FWUP_STATIC void my_flash_callback(void *event) FUNC_LOCATION_FWUP_RESOURCES;
FWUP_STATIC fwup_err_t fwup_flash_err_print(flash_err_t error_code, flash_prt_err_t err_prt);

extern flash_err_t fwup_flash_open(void);
extern flash_err_t fwup_flash_close(void);
extern flash_err_t fwup_flash_set_callback(flash_interrupt_config_t *);
#ifdef BSP_MCU_SERIES_RX600
extern flash_err_t fwup_flash_get_bank_info(flash_bank_t *);
#endif /* #if(BSP_MCU_SERIES_RX600) */
extern void fwup_flash_set_bank_toggle(void);
extern flash_err_t fwup_flash_erase(flash_block_address_t block_start_address, uint32_t num_blocks);
extern flash_err_t fwup_flash_write(uint32_t src_address, uint32_t dest_address, uint32_t num_bytes);
extern comm_err_t fwup_communication_open(void);
extern comm_err_t fwup_communication_close(void);
extern void fwup_update_status(fwup_state_t state);
extern fwup_state_t fwup_get_status(void);
extern state_monitoring_err_t fwup_state_monitoring_open(void);
extern state_monitoring_err_t fwup_state_monitoring_start(void);
extern state_monitoring_err_t fwup_state_monitoring_close(void);
extern state_monitoring_flag_t fwup_state_monitoring_is_error(void);
extern void fwup_interrupts_disable(void);
extern void fwup_register_protect_disable(void);
extern bool fwup_get_boot_protect(void);
extern void fwup_set_boot_protect(void);
extern void my_sw_charget_function(void);
extern void my_sw_charput_function(uint8_t data);
extern void fwup_software_delay_ms(uint32_t delay);
extern int32_t fwup_firmware_verification(uint32_t start_address, uint32_t length, char* signature_type, uint8_t* signature, uint32_t signature_size);

FWUP_STATIC FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank0 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;
FWUP_STATIC FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank1 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;
FWUP_STATIC LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
FWUP_STATIC LOAD_CONST_DATA_CONTROL_BLOCK load_const_data_control_block;
FWUP_STATIC uint32_t flash_error_code;

/* Handle storage. */

static const uint8_t *pboot_loader_magic_code = (uint8_t *)BOOT_LOADER_MIRROR_LOW_ADDRESS;

extern int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize);
uint8_t bank0_state_flag = LIFECYCLE_STATE_BLANK;
uint8_t bank1_state_flag = LIFECYCLE_STATE_BLANK;
#endif /* FWUP_IMPLEMENTATION_BOOTLOADER */

#define BOOT_LOADER_SCI_CONTROL_BLOCK_TOTAL_NUM (2)
SCI_BUFFER_CONTROL sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_TOTAL_NUM];
#define BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY (0)
#define BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL  (1)
#define BOOT_LOADER_SCI_CONTROL_BLOCK_A (0)
#define BOOT_LOADER_SCI_CONTROL_BLOCK_B (1)

SCI_RECEIVE_CONTROL_BLOCK sci_receive_control_block;
#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)
extern ARM_DRIVER_USART FWUP_CFG_USART;
#endif

#ifndef BSP_MCU_SERIES_RX600
    void my_sci_callback(uint32_t event) FUNC_LOCATION_FWUP_RESOURCES;
#endif

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
fwup_err_t R_FWUP_SecureBoot(void)
{
    flash_err_t flash_api_error_code = FLASH_SUCCESS;
    fwup_err_t secure_boot_error_code = FWUP_IN_PROGRESS;
    state_monitoring_err_t monitoring_error_code = MONI_SUCCESS;  // 20201Q_No5
    uint32_t bank_info = 255;
    flash_interrupt_config_t cb_func_info;
    FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_tmp = (FIRMWARE_UPDATE_CONTROL_BLOCK*)load_firmware_control_block.flash_buffer;
    int32_t verification_result = -1;
#ifndef BSP_MCU_SERIES_RX600
    comm_err_t   my_comm_err;
    agt_err_t agt_error_code = AGT_SUCCESS;
#endif
    // 20201Q_No5 start
    /* If state transit error has occured, return FWUP_FAIL to main function */
    if (STATE_MONITORING_ERROR == fwup_state_monitoring_is_error())
    {
        DEBUG_LOG("Boot status has not changed for 1 mitute.\r\n");
        DEBUG_LOG("system error.\r\n");
        fwup_update_status(FWUP_STATE_FATAL_ERROR);
        secure_boot_error_code = FWUP_FAIL;
    }
    // 20201Q_No5 end

    switch(fwup_get_status())
    {
        case FWUP_STATE_INITIALIZING:
#ifdef BSP_MCU_SERIES_RX600
            R_SCI_PinSet_serial_term();
            comm_err_t   my_comm_err;
#endif

            /* OPEN ASYNC CHANNEL
            *  Provide address of the configure structure,
            *  the callback function to be assigned,
            *  and the location for the handle to be stored.*/
            my_comm_err = fwup_communication_open();

            /* If there were an error this would demonstrate error detection of API calls. */
            if (COMM_SUCCESS != my_comm_err)
            {
#ifdef BSP_MCU_SERIES_RX600
                nop(); // Your error handling code would go here.
#endif
                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                secure_boot_error_code = FWUP_FAIL;
                break;
            }

            load_firmware_control_block.progress = 0;
            load_firmware_control_block.offset = 0;

            flash_api_error_code = fwup_flash_open();
            if (FLASH_SUCCESS == flash_api_error_code)
            {
                /* nothing to do */
            }
            else
            {
                DEBUG_LOG2("R_FLASH_Open() returns error. %d.\r\n", flash_error_code);
                DEBUG_LOG("system error.\r\n");
                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                secure_boot_error_code = FWUP_FAIL;
                break;
            }

            // 20201Q_No5 start
            /* Set up the configuration of System-timer for check the status transition. */
            monitoring_error_code = fwup_state_monitoring_open();
            if (MONI_SUCCESS != monitoring_error_code)
            {
                DEBUG_LOG2("fwup_state_monitoring_open() returns error. %d.\r\n", monitoring_error_code);
                DEBUG_LOG("system error.\r\n");
                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                secure_boot_error_code = FWUP_FAIL;
                break;
            }
            // 20201Q_No5 end

            /* startup system */

            DEBUG_LOG("-------------------------------------------------\r\n");
#ifdef BSP_MCU_SERIES_RX600
            DEBUG_LOG("RX65N secure boot program\r\n");
#else /* BSP_MCU_SERIES_RX600 */
            DEBUG_LOG("RE 256KB secure boot program\r\n");
#endif /* BSP_MCU_SERIES_RX600 */
            DEBUG_LOG("-------------------------------------------------\r\n");

            DEBUG_LOG("Checking flash ROM status.\r\n");

            bank0_state_flag = firmware_update_control_block_bank0->image_flag;
            bank1_state_flag = firmware_update_control_block_bank1->image_flag;

            DEBUG_LOG2("bank 0 status = 0x%x [%s]\r\n", bank0_state_flag, get_status_string(bank0_state_flag));
            DEBUG_LOG2("bank 1 status = 0x%x [%s]\r\n", bank1_state_flag, get_status_string(bank1_state_flag));

#ifdef BSP_MCU_SERIES_RX600
#if (BSP_CFG_CODE_FLASH_BANK_MODE == 0)
            fwup_flash_get_bank_info((flash_bank_t *)&bank_info);
            DEBUG_LOG2("bank info = %d. (start bank = %d)\r\n", bank_info, (bank_info ^ 0x01));
#endif  /* (BSP_CFG_CODE_FLASH_BANK_MODE == 0) */
#endif /* BSP_MCU_SERIES_RX600 */

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
            cb_func_info.pcallback = my_flash_callback;
            cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
            fwup_flash_set_callback((void *)&cb_func_info);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */

            // 20201Q_No5 start
            /* Set System-timer for check status */
            monitoring_error_code = fwup_state_monitoring_start();
            if (MONI_SUCCESS != monitoring_error_code)
            {
                DEBUG_LOG2("R_SYS_TIME_RegisterPeriodicCallback() returns error. %d.\r\n", monitoring_error_code);
                DEBUG_LOG("system error.\r\n");
                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                secure_boot_error_code = FWUP_FAIL;
                break;
            }
            // 20201Q_No5 end

            fwup_update_status(FWUP_STATE_BANK1_CHECK);
            break;
#ifndef BSP_MCU_SERIES_RX600
        case FWUP_STATE_EOL_BANK0_ERASE_WAIT:
        case FWUP_STATE_EOL_BANK0_ERASE_COMPLETE:
        case FWUP_STATE_EOL_BANK0_LIFECYCLE_WRITE_WAIT:
        case FWUP_STATE_EOL_BANK0_LIFECYCLE_WRITE_COMPLETE:
        case FWUP_STATE_EOL_ERASE_COMPLETE:
        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT2:
        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE2:
        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_WAIT:
        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_COMPLETE:
        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_WRITE_WAIT:
        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_WRITE_COMPLETE:
        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT:
        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE:
        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT:
        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE:
        case FWUP_STATE_BANK1_CHECK:
            switch(bank1_state_flag)
            {
                case LIFECYCLE_STATE_TESTING:
                    switch(fwup_get_status())
                    {
                        case FWUP_STATE_BANK1_CHECK:
                            memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);

                            DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
                            DEBUG_LOG("bank1(temporary area) on code flash integrity check...");
                            verification_result = fwup_firmware_verification(
                                    (BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH),
                                    (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
                                    firmware_update_control_block_bank1->signature_type, firmware_update_control_block_bank1->signature,
                                    firmware_update_control_block_bank1->signature_size);

                            if(0 == verification_result)
                            {
                                DEBUG_LOG("OK\r\n");
                                firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_VALID;
                            }
                            else
                            {
                                DEBUG_LOG("NG\r\n");
                                firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_INVALID;
                            }
                            DEBUG_LOG2("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(bank1_state_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
                            DEBUG_LOG("bank1(temporary area) block0 erase (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code, ERR_FLASH_ERASE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
                                DEBUG_LOG("system error.\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;
                        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;
                        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code, ERR_FLASH_ERASE);
                                break;
                            }
                            DEBUG_LOG("bank1(temporary area) block0 write (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code, ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT);
#else
                            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
                                DEBUG_LOG("system error.\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE);

#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;
                        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT:
                            /* wait */
                            break;
                        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {

                                secure_boot_error_code = fwup_flash_err_print(flash_error_code, ERR_FLASH_ERASE_CB);
                                break;
                            }
                            bank1_state_flag = firmware_update_control_block_bank1->image_flag;
                            break;

                    }
                    break;
                case LIFECYCLE_STATE_VALID:
                {
                    switch(fwup_get_status())
                    {
                        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE:
                            memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);

                            DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
                            DEBUG_LOG("bank1(temporary area) on code flash integrity check...");

                            verification_result = fwup_firmware_verification(
                                    (BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH),
                                    (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
                                    firmware_update_control_block_bank1->signature_type, firmware_update_control_block_bank1->signature,
                                    firmware_update_control_block_bank1->signature_size);

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            if(0 == verification_result)
                            {
                                DEBUG_LOG("OK\r\n");
                                DEBUG_LOG("Erase bank0(execute area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_WAIT);
                            }
                            else
                            {
                                DEBUG_LOG("NG\r\n");
                                DEBUG_LOG("Erase bank1(temporary area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT2);
                            }
#else /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            if(0 == verification_result)
                            {
                                DEBUG_LOG("OK\r\n");
                                DEBUG_LOG("Erase bank0(execute area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_COMPLETE);
                            }
                            else
                            {
                                DEBUG_LOG("NG\r\n");
                                DEBUG_LOG("Erase bank1(temporary area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE2);
                            }
#endif /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;
                        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;
                        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                                break;
                            }

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            DEBUG_LOG("Copy bank1(temporary area) to block0 write (execute area)...");
                            flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,
                                                                          (uint32_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS,
                                                                          BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER * FWUP_WRITE_BLOCK_SIZE);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK0_UPDATE_CODE_FLASH_WRITE_WAIT);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            DEBUG_LOG("Copy bank1(temporary area) to block0 write (execute area)...");
                            flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,
                                                                          (uint32_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS,
                                                                          BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER * FWUP_WRITE_BLOCK_SIZE);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK0_UPDATE_CODE_FLASH_WRITE_COMPLETE);
#endif /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;
                        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_WRITE_WAIT:
                            /* this state will be update by flash callback */
                            break;
                        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_WRITE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }

                            DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank0->signature_type);
                            DEBUG_LOG("bank0(execute area) on code flash integrity check...");
                            verification_result = fwup_firmware_verification(
                                    (BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH),
                                    (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
                                    firmware_update_control_block_bank0->signature_type, firmware_update_control_block_bank0->signature,
                                    firmware_update_control_block_bank0->signature_size);
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            if(0 == verification_result)
                            {
                                bank0_state_flag = firmware_update_control_block_bank0->image_flag;
                                DEBUG_LOG("OK\r\n");
                                DEBUG_LOG("Erase bank1(temporary area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT2);
                            }
                            else
                            {
                                DEBUG_LOG("NG\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                            }
#else /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            if(0 == verification_result)
                            {
                                bank0_state_flag = firmware_update_control_block_bank0->image_flag;
                                DEBUG_LOG("OK\r\n");
                                DEBUG_LOG("Erase bank1(temporary area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE2);
                            }
                            else
                            {
                                DEBUG_LOG("NG\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                            }
#endif /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;
                        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT2:
                            /* wait */
                            break;
                        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE2:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                bank1_state_flag = firmware_update_control_block_bank1->image_flag;
                                printf("OK\r\n");
                                fwup_update_status(FWUP_STATE_FINALIZE);
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                            }
                            break;

                        default:
                            DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
                            DEBUG_LOG("bank1(temporary area) on code flash integrity check...");
                            verification_result = fwup_firmware_verification(
                                    (BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH),
                                    (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
                                    firmware_update_control_block_bank1->signature_type, firmware_update_control_block_bank1->signature,
                                    firmware_update_control_block_bank1->signature_size);
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            if(0 == verification_result)
                            {
                                DEBUG_LOG("OK\r\n");
                                DEBUG_LOG("Erase bank0(execute area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_WAIT);
                            }
                            else
                            {
                                DEBUG_LOG("NG\r\n");
                                DEBUG_LOG("Erase bank1(temporary area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT2);
                            }
#else /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            if(0 == verification_result)
                            {
                                DEBUG_LOG("OK\r\n");
                                DEBUG_LOG("Erase bank0(execute area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_COMPLETE);
                            }
                            else
                            {
                                DEBUG_LOG("NG\r\n");
                                DEBUG_LOG("Erase bank1(temporary area)...");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,
                                                       BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE2);
                            }
#endif /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;
                    }
                    break;

                }
                case LIFECYCLE_STATE_EOL:
                {
                    switch(fwup_get_status())
                    {
                        case FWUP_STATE_BANK1_CHECK:
                            DEBUG_LOG("End Of Life process start.\r\n");
                            /* The status of bank1 is NOT EOL */
                            DEBUG_LOG("erase install area (code flash): ");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                DEBUG_LOG("NG.\r\n");
                                  secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_EOL_BANK0_ERASE_WAIT);
#else
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    DEBUG_LOG("NG.\r\n");
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_EOL_BANK0_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;

                        case FWUP_STATE_EOL_BANK0_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_EOL_BANK0_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                DEBUG_LOG("NG.\r\n");
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                                break;
                            }
                            memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);
                            firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_EOL;
                            DEBUG_LOG2("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
                            DEBUG_LOG("bank1(temporary area) block0 write (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_EOL_BANK0_LIFECYCLE_WRITE_WAIT);
#else
                            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_EOL_BANK0_LIFECYCLE_WRITE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;

                        case FWUP_STATE_EOL_BANK0_LIFECYCLE_WRITE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_EOL_BANK0_LIFECYCLE_WRITE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                                DEBUG_LOG2("bank 0 status = 0x%x [%s]\r\n", firmware_update_control_block_bank0->image_flag, get_status_string(firmware_update_control_block_bank0->image_flag));
                                DEBUG_LOG2("bank 1 status = 0x%x [%s]\r\n", firmware_update_control_block_bank1->image_flag, get_status_string(firmware_update_control_block_bank1->image_flag));
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }
                            /* End of the EOL process, infinity loop on Bootloader */
                            DEBUG_LOG("End Of Life process finished.\r\n");
                            fwup_software_delay_ms(1000);
                            secure_boot_error_code = FWUP_END_OF_LIFE;
                            break;
                    }
                    break;
                }
                break;
                default:
                {
                    if (bank0_state_flag == LIFECYCLE_STATE_VALID)
                    {
                        fwup_update_status(FWUP_STATE_BANK0_UPDATE_CHECK);
                    }
                    else
                    {
                        fwup_update_status(FWUP_STATE_BANK0_CHECK);
                    }
                    break;
                }
                break;
            }
            break;
#else            /* #ifdef RE01_256KB_H */
        case FWUP_STATE_BANK1_CHECK:
            if(firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_TESTING)
            {
                memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);

                DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
                DEBUG_LOG("bank1(temporary area) on code flash integrity check...");
                verification_result = fwup_firmware_verification(
                        (BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH),
                        (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
                        firmware_update_control_block_bank1->signature_type, firmware_update_control_block_bank1->signature,
                        firmware_update_control_block_bank1->signature_size);

                if(0 == verification_result)
                {
                    DEBUG_LOG("OK\r\n");
                    firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_VALID;
                }
                else
                {
                    DEBUG_LOG("NG\r\n");
                    firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_INVALID;
                }
                DEBUG_LOG2("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
                DEBUG_LOG("bank1(temporary area) block0 erase (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
                if (FLASH_SUCCESS != flash_api_error_code)
                {
                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                    break;
                }
                fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT);
#else
                R_BSP_InterruptsDisable();
                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
                R_BSP_InterruptsEnable();
                if (FLASH_SUCCESS != flash_api_error_code)
                {
                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                    break;
                }
                fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
            }
            else
            {
                if (firmware_update_control_block_bank0->image_flag == LIFECYCLE_STATE_VALID)
                {
                    fwup_update_status(FWUP_STATE_BANK0_UPDATE_CHECK);
                }
                else
                {
                    fwup_update_status(FWUP_STATE_BANK0_CHECK);
                }
            }
            break;

        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT:
            /* this state will be update by flash callback */
            break;

        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE:
            if (FLASH_SUCCESS == flash_error_code)
            {
                DEBUG_LOG("OK\r\n");
            }
            else
            {
                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                break;
            }
            DEBUG_LOG("bank1(temporary area) block0 write (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
            if (FLASH_SUCCESS != flash_api_error_code)
            {
                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                break;
            }
            fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT);
#else
            R_BSP_InterruptsDisable();
            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
            R_BSP_InterruptsEnable();
            if (FLASH_SUCCESS != flash_api_error_code)
            {
                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                break;
            }
            fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
            break;

        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT:
            /* this state will be update by flash callback */
            break;

        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE:
            if (FLASH_SUCCESS == flash_error_code)
            {
                DEBUG_LOG("OK\r\n");
            }
            else
            {
                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                break;
            }
            if (LIFECYCLE_STATE_VALID == firmware_update_control_block_tmp->image_flag)
            {
                DEBUG_LOG("swap bank...\r\n");
                fwup_software_delay_ms(3000);
                bank_swap_with_software_reset();
                while(1);
            }
            else
            {
                DEBUG_LOG("software reset...\r\n");
                fwup_software_delay_ms(3000);
                software_reset();
                while(1);
            }
            break;
#endif
        case FWUP_STATE_BANK0_CHECK:
#if BSP_MCU_SERIES_RX600
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE:
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1:
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2:
        case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
        case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE:
#endif
        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE:
        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT:
        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_COMPLETE:
        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
#if BSP_MCU_SERIES_RX600
        case FWUP_STATE_INSTALL_DATA_FLASH_READ_WAIT:
        case FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE:
        case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
        case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE:
#endif
        case FWUP_STATE_BANK0_UPDATE_CHECK:
        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE:
#if BSP_MCU_SERIES_RX600
        case FWUP_STATE_EOL_BANK1_ERASE_WAIT:
        case FWUP_STATE_EOL_BANK1_ERASE_COMPLETE:
        case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT:
        case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE:
        case FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT:
        case FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE:
#endif /*BSP_MCU_SERIES_RX600*/
        case FWUP_STATE_FINALIZE:
            switch(bank0_state_flag)
            {
                case LIFECYCLE_STATE_BLANK:
                    switch(fwup_get_status())
                    {
                        case FWUP_STATE_BANK0_CHECK:
                            if(bank1_state_flag == LIFECYCLE_STATE_BLANK)
                            {
                                DEBUG_LOG("start installing user program.\r\n");
#ifdef BSP_MCU_SERIES_RX600
#if 0 // 20201Q_No1_RX72N_Optimization_of_processing
                                if (firmware_update_control_block_bank1->image_flag != LIFECYCLE_STATE_INSTALLING)
                                {
                                    DEBUG_LOG("erase bank1 secure boot mirror area...");
                                    flash_api_error_code = R_FLASH_Erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                    if(FLASH_SUCCESS != flash_api_error_code)
                                    {
                                        DEBUG_LOG("NG\r\n");
                                        secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                        break;
                                    }
                                    secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT;
                                }
                                else
                                {
                                    secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE;
                                }
#else
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
                                if (false == fwup_get_boot_protect())
                                {
                                    /* No Boot Protected */
                                    DEBUG_LOG("erase bank1 secure boot mirror area...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                                    flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                    if(FLASH_SUCCESS != flash_api_error_code)
                                    {
                                        DEBUG_LOG("NG\r\n");
                                        secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                        break;
                                    }
                                    fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT);
#else
                                    R_BSP_InterruptsDisable();
                                    flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                    R_BSP_InterruptsEnable();
                                    if(FLASH_SUCCESS != flash_api_error_code)
                                    {
                                        DEBUG_LOG("NG\r\n");
                                        secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                        break;
                                    }
                                    fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                                }
                                else
                                {
                                    /* Already Boot Protected */
                                    /* Skip boot copy process */
                                    DEBUG_LOG("boot protected: skip copy secure boot from bank0 to bank1...");
                                    flash_error_code = FLASH_SUCCESS;
                                    fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
                                }
#else
#ifdef BSP_MCU_SERIES_RX600
                                DEBUG_LOG("erase bank1 secure boot mirror area...");
#endif
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
#ifdef BSP_MCU_SERIES_RX600
                                flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                if(FLASH_SUCCESS != flash_api_error_code)
                                {
                                    DEBUG_LOG("NG\r\n");
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT);

#endif
#else /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
#ifdef BSP_MCU_SERIES_RX600
                                R_BSP_InterruptsDisable();
                                flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                R_BSP_InterruptsEnable();
                                if(FLASH_SUCCESS != flash_api_error_code)
                                {
                                    DEBUG_LOG("NG\r\n");
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE);
#else /* BSP_MCU_SERIES_RX600 */
                                DEBUG_LOG("erase install area (code flash): ");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t) BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS , BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if(FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE);
#endif /* BSP_MCU_SERIES_RX600 */
#endif /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
#endif  // FWUP_CFG_BOOT_PROTECT_ENABLE
#endif // 20201Q_No1_RX72N_Optimization_of_processing
#else /* BSP_MCU_SERIES_RX600  */

                                DEBUG_LOG("erase install area (code flash): ");
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t) BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS , BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                                if(FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT);
#else
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE);
#endif
                                break;
#endif /* BSP_MCU_SERIES_RX600  */

                            }
#ifdef BSP_MCU_SERIES_RX600
                            else if(bank1_state_flag == LIFECYCLE_STATE_VALID)
                            {
                                DEBUG_LOG("bank0(current) is blank, but bank1(previous) is still alive.\r\n");
                                DEBUG_LOG("swap bank...");
                                fwup_software_delay_ms(3000);
                                bank_swap_with_software_reset();
                                while(1);
                            }
#endif
                            else
                            {
                                DEBUG_LOG("user program installation failed.\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                            }
                            break;
#ifdef BSP_MCU_SERIES_RX600
                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE:
                            if (firmware_update_control_block_bank1->image_flag != LIFECYCLE_STATE_INSTALLING)
                            {
                                if (FLASH_SUCCESS == flash_error_code)
                                {
                                    DEBUG_LOG("OK\r\n");
                                }
                                else
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                                    break;
                                }
                            }
                            DEBUG_LOG("copy secure boot (part1) from bank0 to bank1...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE);
                            if(FLASH_SUCCESS != flash_api_error_code)
                            {
                                DEBUG_LOG("NG\r\n");
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1);
#else
                            R_BSP_InterruptsDisable();
                            flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE);
                            R_BSP_InterruptsEnable();
                            if(FLASH_SUCCESS != flash_api_error_code)
                            {
                                DEBUG_LOG("NG\r\n");
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }
                            DEBUG_LOG("copy secure boot (part2) from bank0 to bank1...");
                            if(BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM > 0)
                            {
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                                flash_api_error_code = fwup_flash_write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL * FLASH_CF_SMALL_BLOCK_SIZE);
                                if(FLASH_SUCCESS != flash_api_error_code)
                                {
                                    DEBUG_LOG("NG\r\n");
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2);
#else
                                R_BSP_InterruptsDisable();
                                flash_api_error_code = fwup_flash_write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL * FLASH_CF_SMALL_BLOCK_SIZE);
                                R_BSP_InterruptsEnable();
                                if(FLASH_SUCCESS != flash_api_error_code)
                                {
                                    DEBUG_LOG("NG\r\n");
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            }
                            else
                            {
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
                            }
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }
#if 0 // 20201Q_No1_RX72N_Optimization_of_processing
                            if (firmware_update_control_block_bank1->image_flag != LIFECYCLE_STATE_INSTALLING)
                            {
                                DEBUG_LOG("========== install user program phase ==========\r\n");
                                DEBUG_LOG("erase install area (data flash): ");
                                flash_api_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_WAIT;
                            }
                            else
                            {
                                memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
                                firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_VALID;
                                DEBUG_LOG2("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
                                DEBUG_LOG("bank1(temporary area) block0 erase (to update LIFECYCLE_STATE)...");
                                flash_api_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT;
                            }
#else
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
                            if (false == fwup_get_boot_protect())
                            {
                                fwup_set_boot_protect();
                                DEBUG_LOG("set boot protect.\r\n");
                            }
#endif /* (FWUP_CFG_BOOT_PROTECT_ENABLE == 1) */
                            DEBUG_LOG("========== install user program phase ==========\r\n");
                            DEBUG_LOG("erase install area (data flash): ");
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT);
#else /* (FLASH_CFG_DATA_FLASH_BGO == 1) */
                            R_BSP_InterruptsDisable();
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
                            R_BSP_InterruptsEnable();
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_DATA_FLASH_BGO == 1) */
#endif  // 20201Q_No1_RX72N_Optimization_of_processing
                            break;

                        case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }
                            DEBUG_LOG("erase install area (code flash): ");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT);
#else
                            R_BSP_InterruptsDisable();
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                            R_BSP_InterruptsEnable();
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;
#endif /* BSP_MCU_SERIES_RX600  */
                        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                                break;
                            }
                            sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
                            sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
                            sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A];
                            sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_A;
                            DEBUG_LOG2("send \"%s\" via UART.\r\n", INITIAL_FIRMWARE_FILE_NAME);
                            fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT);
                            break;

                        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT:
                            /* install code flash area */
                            if(!firm_block_read(load_firmware_control_block.flash_buffer, load_firmware_control_block.offset))
                            {
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                                flash_api_error_code = fwup_flash_write((uint32_t)load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT);
#else
#ifdef BSP_MCU_SERIES_RX600
                                /* Interrupt disabled by Receive callback function */
                                flash_api_error_code = fwup_flash_write((uint32_t)load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
                                R_BSP_InterruptsEnable();
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE);
#else
                                flash_api_error_code = fwup_flash_write((uint32_t)load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE);

#endif
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            }
                            break;

                        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
                            if (FLASH_SUCCESS != flash_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }

#ifdef BSP_MCU_SERIES_RX600
                            load_firmware_control_block.offset += FWUP_WRITE_BLOCK_SIZE;
                            load_firmware_control_block.progress = (uint32_t)(((float)(load_firmware_control_block.offset)/(float)((FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))*100));
                            DEBUG_LOG2("installing firmware...%d%(%d/%dKB).\r", load_firmware_control_block.progress, load_firmware_control_block.offset/1024, (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER)/1024);
                            if(load_firmware_control_block.offset < (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
#else /* BSP_MCU_SERIES_RX600 */
                            load_firmware_control_block.offset += FWUP_WRITE_BLOCK_SIZE;
                            load_firmware_control_block.progress = (uint32_t)(((float)(load_firmware_control_block.offset)/(float)((FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))*100));
                            DEBUG_LOG2("installing firmware...%d%(%d/%dKB).\r", load_firmware_control_block.progress, load_firmware_control_block.offset/1024, (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER)/1024);
                            if(load_firmware_control_block.offset < (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
#endif /* BSP_MCU_SERIES_RX600 */
                            {
                                /* one more loop */
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT);
                            }
#ifdef BSP_MCU_SERIES_RX600
                            else if(load_firmware_control_block.offset == (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
#else  /* BSP_MCU_SERIES_RX600 */
                              else if(load_firmware_control_block.offset == (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
#endif /* BSP_MCU_SERIES_RX600 */
                                {
                                DEBUG_LOG("\n");
                                DEBUG_LOG("completed installing firmware.\r\n");
                                DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
                                DEBUG_LOG("bank1(temporary area) on code flash integrity check...");

                                verification_result = fwup_firmware_verification(
                                        BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
#ifdef BSP_MCU_SERIES_RX600
                                        (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
#else
                                        (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
#endif
                                        firmware_update_control_block_bank1->signature_type, firmware_update_control_block_bank1->signature,
                                        firmware_update_control_block_bank1->signature_size);

                                if(0 == verification_result)
                                {
#ifdef BSP_MCU_SERIES_RX600
                                    DEBUG_LOG("OK\r\n");
                                    load_const_data_control_block.offset = 0;
                                    fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_READ_WAIT);
#else /* BSP_MCU_SERIES_RX600 */
                                    DEBUG_LOG("OK\r\n");
                                    bank1_state_flag = firmware_update_control_block_bank1->image_flag;
                                    load_firmware_control_block.offset = 0;
                                    DEBUG_LOG("completed installing const data.\r\n");
                                    DEBUG_LOG("software reset...\r\n");
                                    fwup_software_delay_ms(3000);
                                    software_reset();
#endif /* BSP_MCU_SERIES_RX600 */
                                }
                                else
                                {
                                    DEBUG_LOG("NG\r\n");
                                    fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                    secure_boot_error_code = FWUP_FAIL;
                                }
                            }
                            else
                            {
                                DEBUG_LOG("\n");
                                DEBUG_LOG("fatal error occurred.\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                            }
                            break;
#ifdef BSP_MCU_SERIES_RX600
                        case FWUP_STATE_INSTALL_DATA_FLASH_READ_WAIT:
                            /* install data flash area */
                            if(!const_data_block_read(load_const_data_control_block.flash_buffer, load_const_data_control_block.offset))
                            {
                                fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE);
                            }
                            break;

                        case FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE:
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_write((uint32_t)&load_const_data_control_block.flash_buffer[load_const_data_control_block.offset/4], (uint32_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS + load_const_data_control_block.offset, FLASH_DF_BLOCK_SIZE);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT);
#else
                            R_BSP_InterruptsDisable();
                            flash_api_error_code = fwup_flash_write((uint32_t)&load_const_data_control_block.flash_buffer[load_const_data_control_block.offset/4], (uint32_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS + load_const_data_control_block.offset, FLASH_DF_BLOCK_SIZE);
                            R_BSP_InterruptsEnable();
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE);
#endif  /* (FLASH_CFG_DATA_FLASH_BGO == 1) */
                            break;

                        case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE:
                            if (FLASH_SUCCESS != flash_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }

                            load_const_data_control_block.offset += FLASH_DF_BLOCK_SIZE;
                            load_const_data_control_block.progress = (uint32_t)(((float)(load_const_data_control_block.offset)/(float)((FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))*100));
                            static uint32_t previous_offset = 0;
                            if(previous_offset != (load_const_data_control_block.offset/1024))
                            {
                                DEBUG_LOG2("installing const data...%d%(%d/%dKB).\r", load_const_data_control_block.progress, load_const_data_control_block.offset/1024, (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER)/1024);
                                previous_offset = load_const_data_control_block.offset/1024;
                            }
                            if(load_const_data_control_block.offset < (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))
                            {
                                /* one more loop */
                                fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE);
                            }
                            else if(load_const_data_control_block.offset == (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))
                            {
                                DEBUG_LOG("\n");
                                DEBUG_LOG("completed installing const data.\r\n");
                                DEBUG_LOG("software reset...\r\n");
                                fwup_software_delay_ms(3000);
                                software_reset();
                            }
                            else
                            {
                                DEBUG_LOG("\n");
                                DEBUG_LOG("fatal error occurred.\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                            }
                            break;
#endif /* BSP_MCU_SERIES_RX600 */
                    }
                    break;
#ifdef BSP_MCU_SERIES_RX600
                case LIFECYCLE_STATE_TESTING:
                    DEBUG_LOG("illegal status\r\n");
                    DEBUG_LOG("swap bank...");
                    fwup_software_delay_ms(3000);
                    bank_swap_with_software_reset();
                    while(1);
                    break;
#endif

                case LIFECYCLE_STATE_INSTALLING:
                    switch(fwup_get_status())
                    {
                        case FWUP_STATE_BANK0_CHECK:
                            DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank0->signature_type);
                            DEBUG_LOG("bank0(execute area) on code flash integrity check...");

                            verification_result = fwup_firmware_verification(
                                    BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
#ifdef BSP_MCU_SERIES_RX600
                                    (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
#else
                                    (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
#endif
                                    firmware_update_control_block_bank0->signature_type, firmware_update_control_block_bank0->signature,
                                    firmware_update_control_block_bank0->signature_size);

                            if(0 == verification_result)
                            {
                                DEBUG_LOG("OK\r\n");

#ifdef BSP_MCU_SERIES_RX600
                                if (!strcmp((const char *)pboot_loader_magic_code, BOOT_LOADER_MAGIC_CODE))
                                {
                                    fwup_update_status(FWUP_STATE_FINALIZE);
                                }
                                else
                                {
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
                                    if (false == fwup_get_boot_protect())
                                    {
                                        /* No Boot Protected */
                                        DEBUG_LOG("erase bank1 secure boot mirror area...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                                        flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                        if(FLASH_SUCCESS != flash_api_error_code)
                                        {
                                            DEBUG_LOG("NG\r\n");
                                            secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                            break;
                                        }
                                        fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT);
#else
                                        R_BSP_InterruptsDisable();
                                        flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                        R_BSP_InterruptsEnable();
                                        if(FLASH_SUCCESS != flash_api_error_code)
                                        {
                                            DEBUG_LOG("NG\r\n");
                                            secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                            break;
                                        }
                                        fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                                    }
                                    else
                                    {
                                        /* Already Boot Protected */
                                        /* Skip boot copy process */
                                        DEBUG_LOG("boot protected: skip copy secure boot from bank0 to bank1...");
                                        flash_error_code = FLASH_SUCCESS;
                                        fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
                                    }
#else
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                                    DEBUG_LOG("erase bank1 secure boot mirror area...");
                                    flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                    if(FLASH_SUCCESS != flash_api_error_code)
                                    {
                                        DEBUG_LOG("NG\r\n");
                                        secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                        break;
                                    }
                                    fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT);
#else /* FLASH_CFG_CODE_FLASH_BGO == 1*/
                                    DEBUG_LOG("erase bank1 secure boot mirror area...");
                                    R_BSP_InterruptsDisable();
                                    flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                    R_BSP_InterruptsEnable();
                                    if(FLASH_SUCCESS != flash_api_error_code)
                                    {
                                        DEBUG_LOG("NG\r\n");
                                        secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                        break;
                                    }
                                    fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
#endif  /* FWUP_CFG_BOOT_PROTECT_ENABLE */
                                }
#else
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
                                if (false == fwup_get_boot_protect())
                                {
                                    fwup_set_boot_protect();
                                    DEBUG_LOG("set boot protect.\r\n");
                                }
#endif /* (FWUP_CFG_BOOT_PROTECT_ENABLE == 1) */
                                fwup_update_status(FWUP_STATE_FINALIZE);
#endif /* BSP_MCU_SERIES_RX600 */
                            }
                            else
                            {
                                DEBUG_LOG("NG.\r\n");
                                DEBUG_LOG("Code flash is completely broken.\r\n");
                                DEBUG_LOG("Please erase all code flash.\r\n");
                                DEBUG_LOG("And, write secure boot using debugger.\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                            }
                            break;
#ifdef BSP_MCU_SERIES_RX600
                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                                break;
                            }
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
#ifdef BSP_MCU_SERIES_RX600
                            DEBUG_LOG("copy secure boot (part1) from bank0 to bank1...");
                            flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FWUP_WRITE_BLOCK_SIZE);
                            if(FLASH_SUCCESS != flash_api_error_code)
                            {
                                DEBUG_LOG("NG\r\n");
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
#endif /* BSP_MCU_SERIES_RX600 */
                            fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1);
#else
#ifdef BSP_MCU_SERIES_RX600
                            DEBUG_LOG("copy secure boot (part1) from bank0 to bank1...");
                            R_BSP_InterruptsDisable();
                            flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE);
                            R_BSP_InterruptsEnable();

                            if(FLASH_SUCCESS != flash_api_error_code)
                            {
                                DEBUG_LOG("NG\r\n");
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
#endif /* BSP_MCU_SERIES_RX600 */
                            fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }
#ifdef BSP_MCU_SERIES_RX600
                            DEBUG_LOG("copy secure boot (part2) from bank0 to bank1...");
                            if(BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM > 0)
                            {
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
                                flash_api_error_code = fwup_flash_write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL * FLASH_CF_SMALL_BLOCK_SIZE);
                                if(FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2);
#else
                                R_BSP_InterruptsDisable();
                                flash_api_error_code = fwup_flash_write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL * FLASH_CF_SMALL_BLOCK_SIZE);
                                R_BSP_InterruptsEnable();
                                if(FLASH_SUCCESS != flash_api_error_code)
                                {
                                    DEBUG_LOG("NG\r\n");
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            }
                            else
                            {
                                fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
                            }
#endif /* BSP_MCU_SERIES_RX600 */
                            break;
#ifdef BSP_MCU_SERIES_RX600
                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }
#if 0  // 20201Q_No1_RX72N_Optimization_of_processing
                            if (LIFECYCLE_STATE_INSTALLING == firmware_update_control_block_bank0->image_flag)
                            {
                                secure_boot_state = BOOT_LOADER_STATE_FINALIZE;
                            }
                            else
                            {
                                DEBUG_LOG("========== install user program phase ==========\r\n");
                                DEBUG_LOG("erase install area (data flash): ");
                                flash_api_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                secure_boot_state = BOOT_LOADER_STATE_INSTALL_DATA_FLASH_ERASE_WAIT;
                            }
#else
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
                            if (false == fwup_get_boot_protect())
                            {
                                fwup_set_boot_protect();
                                DEBUG_LOG("set boot protect.\r\n");
                            }
#endif
                            fwup_update_status(FWUP_STATE_FINALIZE);
#endif  // 20201Q_No1_RX72N_Optimization_of_processing
                            break;
#endif
#endif /*BSP_MCU_SERIES_RX600*/
                        case FWUP_STATE_FINALIZE:
                            DEBUG_LOG("jump to user program\r\n");
                            fwup_software_delay_ms(1000);
                            secure_boot_error_code = FWUP_SUCCESS;
                            break;
                    }
                    break;

                case LIFECYCLE_STATE_VALID:
                    switch(fwup_get_status())
                    {
                        case FWUP_STATE_BANK0_UPDATE_CHECK:
                            DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank0->signature_type);
                            DEBUG_LOG("bank0(execute area) on code flash integrity check...");
                            verification_result = fwup_firmware_verification(
                                    BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
#ifdef BSP_MCU_SERIES_RX600
                                    (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
#else
                                    (FWUP_WRITE_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
#endif
                                    firmware_update_control_block_bank0->signature_type, firmware_update_control_block_bank0->signature,
                                    firmware_update_control_block_bank0->signature_size);

                            if(0 == verification_result)
                            {
                                DEBUG_LOG("OK\r\n");
                                if(bank1_state_flag != LIFECYCLE_STATE_BLANK)
                                {
                                    DEBUG_LOG("erase install area (code flash): ");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
#ifdef BSP_MCU_SERIES_RX600
                                    flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
#else  /* BSP_MCU_SERIES_RX600 */
                                    flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
#endif /* BSP_MCU_SERIES_RX600 */
                                    if (FLASH_SUCCESS != flash_api_error_code)
                                    {
                                        secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                        break;
                                    }
                                    fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT);
#else
#ifdef BSP_MCU_SERIES_RX600
                                    R_BSP_InterruptsDisable();
                                    flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                    R_BSP_InterruptsEnable();
#else
                                    flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
#endif
                                    if (FLASH_SUCCESS != flash_api_error_code)
                                    {
                                        secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                        break;
                                    }
                                    fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                                }
                                else
                                {
                                    fwup_update_status(FWUP_STATE_FINALIZE);
                                }
                            }
                            else
                            {
                                DEBUG_LOG("NG.\r\n");
                                DEBUG_LOG("Code flash is completely broken.\r\n");
                                DEBUG_LOG("Please erase all code flash.\r\n");
                                DEBUG_LOG("And, write secure boot using debugger.\r\n");
                                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                                secure_boot_error_code = FWUP_FAIL;
                            }
                            break;

                        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                bank1_state_flag = firmware_update_control_block_bank1->image_flag;
                                DEBUG_LOG("OK\r\n");
                                fwup_update_status(FWUP_STATE_FINALIZE);
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                            }

                            break;

                        case FWUP_STATE_FINALIZE:
                            DEBUG_LOG("jump to user program\r\n");
                            fwup_software_delay_ms(1000);
                            secure_boot_error_code = FWUP_SUCCESS;
                            break;
                    }
                    break;
#if BSP_MCU_SERIES_RX600
                case LIFECYCLE_STATE_EOL:
                    switch(fwup_get_status())
                    {
                        case FWUP_STATE_BANK0_CHECK:
                            if (LIFECYCLE_STATE_EOL == firmware_update_control_block_bank1->image_flag)
                            {
                                /* The status of bank1 is already EOL */
                                DEBUG_LOG("check bank1 status: ");
                                flash_error_code = FLASH_SUCCESS;
                                fwup_update_status(FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE);
                            }
                            else
                            {
                                /* The status of bank1 is NOT EOL */
                                DEBUG_LOG("erase install area (code flash): ");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
#ifdef BSP_MCU_SERIES_RX600
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
#else
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
#endif
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    DEBUG_LOG("NG.\r\n");
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_EOL_BANK1_ERASE_WAIT);
#else
#ifdef BSP_MCU_SERIES_RX600
                                R_BSP_InterruptsDisable();
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                                R_BSP_InterruptsEnable();
#else /* BSP_MCU_SERIES_RX600 */
                                flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
#endif /* BSP_MCU_SERIES_RX600 */
                                if (FLASH_SUCCESS != flash_api_error_code)
                                {
                                    DEBUG_LOG("NG.\r\n");
                                    secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                    break;
                                }
                                fwup_update_status(FWUP_STATE_EOL_BANK1_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            }
                            break;

                        case FWUP_STATE_EOL_BANK1_ERASE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_EOL_BANK1_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                DEBUG_LOG("NG.\r\n");
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                                break;
                            }
#ifdef BSP_MCU_SERIES_RX600
                            memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
#else
                            memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);
#endif
                            firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_EOL;
                            DEBUG_LOG2("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
                            DEBUG_LOG("bank1(temporary area) block0 write (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
#ifdef BSP_MCU_SERIES_RX600
                            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
#else /* BSP_MCU_SERIES_RX600 */
                            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);
#endif /* BSP_MCU_SERIES_RX600 */
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT);
#else
#ifdef BSP_MCU_SERIES_RX600
                            R_BSP_InterruptsDisable();
                            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
                            R_BSP_InterruptsEnable();
#else /* BSP_MCU_SERIES_RX600 */
                            flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FWUP_WRITE_BLOCK_SIZE);
#endif /* BSP_MCU_SERIES_RX600 */
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_WRITE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
                            break;

                        case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT:
                            /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                                DEBUG_LOG("OK\r\n");
                                DEBUG_LOG2("bank 0 status = 0x%x [%s]\r\n", firmware_update_control_block_bank0->image_flag, get_status_string(firmware_update_control_block_bank0->image_flag));
                                DEBUG_LOG2("bank 1 status = 0x%x [%s]\r\n", firmware_update_control_block_bank1->image_flag, get_status_string(firmware_update_control_block_bank1->image_flag));
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_WRITE_CB);
                                break;
                            }

                                  /* Erase Data Flash */
                            DEBUG_LOG("erase install area (data flash): ");
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT);
#else
#ifdef BSP_MCU_SERIES_RX600
                            R_BSP_InterruptsDisable();
                            flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
                            R_BSP_InterruptsEnable();
#endif /* BSP_MCU_SERIES_RX600 */
                            if (FLASH_SUCCESS != flash_api_error_code)
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_api_error_code,ERR_FLASH_ERASE);
                                break;
                            }
                            fwup_update_status(FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_DATA_FLASH_BGO == 1) */
                            break;

                        case FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT:
                               /* this state will be update by flash callback */
                            break;

                        case FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE:
                            if (FLASH_SUCCESS == flash_error_code)
                            {
                               DEBUG_LOG("OK\r\n");
                            }
                            else
                            {
                                secure_boot_error_code = fwup_flash_err_print(flash_error_code,ERR_FLASH_ERASE_CB);
                                break;
                            }

                            /* End of the EOL process, infinity loop on Bootloader */
                            DEBUG_LOG("End Of Life process finished.\r\n");
                            fwup_software_delay_ms(1000);
                            secure_boot_error_code = FWUP_END_OF_LIFE;
                            break;
                    }
                    break;
#endif /*BSP_MCU_SERIES_RX600*/
                default:
                    DEBUG_LOG2("illegal flash rom status code 0x%x.\r\n", firmware_update_control_block_bank0->image_flag);
                    DEBUG_LOG("Code flash is completely broken.\r\n");
                    DEBUG_LOG("Please erase all code flash.\r\n");
                    DEBUG_LOG("And, write secure boot using debugger.\r\n");
                    fwup_update_status(FWUP_STATE_FATAL_ERROR);
                    secure_boot_error_code = FWUP_FAIL;
                    break;
            }
    }
    // 20201Q_No5 start
    /* Close System-timer if boot process will end */
    if (secure_boot_error_code == FWUP_SUCCESS ||
            secure_boot_error_code == FWUP_FAIL ||
            secure_boot_error_code == FWUP_END_OF_LIFE)
    {
        monitoring_error_code = fwup_state_monitoring_close();
        if (MONI_SUCCESS != monitoring_error_code)
        {
            DEBUG_LOG("fwup_state_monitoring_close() error.\r\n");
            DEBUG_LOG("system error.\r\n");
            fwup_update_status(FWUP_STATE_FATAL_ERROR);
            secure_boot_error_code = FWUP_FAIL;
        }
    }
    // 20201Q_No5 end
    return secure_boot_error_code;
}

void R_FWUP_ExecuteFirmware(void)
{
#if (BSP_MCU_SERIES_RX600)
    uint32_t addr;

    /* stop all interrupt completely */
    R_BSP_SET_PSW(0);
    addr = *(uint32_t*)USER_RESET_VECTOR_ADDRESS;
    ((void (*)())addr)();
#else
    R_SYS_EnterCriticalSection();
    volatile uint32_t addr = 0;
    SCB->VTOR = USER_MSP_VECTOR_ADDRESS;
    addr = *(uint32_t *)USER_MSP_VECTOR_ADDRESS;
    __set_MSP(addr);
    addr = *(uint32_t*)USER_RESET_VECTOR_ADDRESS;
    R_SYS_ExitCriticalSection();
    ((void (*)())addr)();
#endif
}

static void software_reset(void)
{

#ifdef BSP_MCU_SERIES_RX600
    /* stop all interrupt completely */
    R_BSP_SET_PSW(0);
    fwup_interrupts_disable();
    fwup_register_protect_disable();
    R_BSP_SoftwareReset();
    while(1);   /* software reset */
#else  /* BSP_MCU_SERIES_RX600 */
    R_NVIC_SystemReset();
#endif /* BSP_MCU_SERIES_RX600 */
}

#ifdef BSP_MCU_SERIES_RX600
static void bank_swap_with_software_reset(void)
{
    /* stop all interrupt completely */
    R_BSP_SET_PSW(0);
    fwup_interrupts_disable();
    fwup_flash_set_bank_toggle();
    fwup_register_protect_disable();
    R_BSP_SoftwareReset();
    while(1);   /* software reset */
}
#endif /* BSP_MCU_SERIES_RX600 */


/***********************************************************************************************************************
* Function Name: firm_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
FWUP_STATIC int32_t firm_block_read(uint32_t *firmware, uint32_t offset)
{
    int32_t error_code = -1;
    if (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag)
    {
        memcpy(firmware, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer, FWUP_WRITE_BLOCK_SIZE);
        sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
        error_code = 0;
    }
    else if  (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag)
    {
        memcpy(firmware, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer, FWUP_WRITE_BLOCK_SIZE);
        sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
        error_code = 0;
    }
    else
    {
        error_code = -1;
    }
    return error_code;
}

/***********************************************************************************************************************
* Function Name: const_data_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
FWUP_STATIC int32_t const_data_block_read(uint32_t *const_data, uint32_t offset)
{
    int32_t error_code = -1;
    if (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag)
    {
        memcpy(const_data, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer, FWUP_WRITE_BLOCK_SIZE);
        sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
        error_code = 0;
    }
    else if  (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag)
    {
        memcpy(const_data, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer, FWUP_WRITE_BLOCK_SIZE);
        sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
        error_code = 0;
    }
    else
    {
        error_code = -1;
    }
    return error_code;
}

/*****************************************************************************
* Function Name: my_sci_callback
* Description  : This is a template for an SCI Async Mode callback function.
* Arguments    : pArgs -
*                pointer to sci_cb_p_args_t structure cast to a void. Structure
*                contains event and associated data.
* Return Value : none
******************************************************************************/
uint32_t error_count1 = 0;
uint32_t error_count2 = 0;
uint32_t rcv_count1 = 0;
uint32_t rcv_count2 = 0;

#ifdef BSP_MCU_SERIES_RX600
void my_sci_callback(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        if(sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size < sizeof(sci_receive_control_block.p_sci_buffer_control->buffer))
        {
            R_SCI_Receive(p_args->hdl, &sci_receive_control_block.p_sci_buffer_control->buffer[sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size++], 1);
            if (sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size == sizeof(sci_receive_control_block.p_sci_buffer_control->buffer))
            {
#if (FLASH_CFG_CODE_FLASH_BGO == 0)
                R_BSP_InterruptsDisable();  /* In case of Blocking, interrupts are disabled until the flash write is completed. */
#endif /* FLASH_CFG_CODE_FLASH_BGO == 0 */
                sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size = 0;
                sci_receive_control_block.p_sci_buffer_control->buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL;
                sci_receive_control_block.total_byte_size += FWUP_WRITE_BLOCK_SIZE;
                if (BOOT_LOADER_SCI_CONTROL_BLOCK_A == sci_receive_control_block.current_state)
                {
                    sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_B;
                    sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B];
                }
                else
                {
                    sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_A;
                    sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A];
                }
            }
            rcv_count1++;
        }
        rcv_count2++;
    }
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
        nop();
        error_count1++;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
        error_count2++;
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */
#else
#endif

/***********************************************************************************************************************
* Function Name: my_flash_callback
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
FWUP_STATIC void my_flash_callback(void *event)
{
    uint8_t event_code = FLASH_ERR_FAILURE;
    event_code = *((uint8_t*)event);

    flash_error_code = FLASH_ERR_FAILURE;

    if((event_code == FLASH_INT_EVENT_WRITE_COMPLETE) || (event_code == FLASH_INT_EVENT_ERASE_COMPLETE))
    {
        flash_error_code = FLASH_SUCCESS;
    }

    switch(fwup_get_status())
    {
        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE);
            break;
        case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT:
            fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE);
            break;
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE);
            break;
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
            fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1);
            break;
        case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
            fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
            break;
        case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE);
            break;
        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE);
            break;
        case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
            fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE);
            break;
        case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
            fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE);
            break;
        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE);
            break;
#ifdef BSP_MCU_SERIES_RX600
        case FWUP_STATE_EOL_BANK1_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_EOL_BANK1_ERASE_COMPLETE);
            break;
        case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT:
            fwup_update_status(FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE);
            break;
        case FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE);
            break;
#else
        case FWUP_STATE_EOL_BANK0_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_EOL_BANK0_ERASE_COMPLETE);
            break;
        case FWUP_STATE_EOL_BANK0_LIFECYCLE_WRITE_WAIT:
            fwup_update_status(FWUP_STATE_EOL_BANK0_LIFECYCLE_WRITE_COMPLETE);
            break;
#endif
#ifdef RE01_256KB_H
        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT2:
            fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE2);
            break;
        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_WAIT:
            fwup_update_status(FWUP_STATE_BANK0_UPDATE_CODE_FLASH_ERASE_COMPLETE);
            break;
        case FWUP_STATE_BANK0_UPDATE_CODE_FLASH_WRITE_WAIT:
            fwup_update_status(FWUP_STATE_BANK0_UPDATE_CODE_FLASH_WRITE_COMPLETE);
            break;
#endif
        default:
            break;
    }
}

FWUP_STATIC const uint8_t *get_status_string(uint8_t status)
{
    static const uint8_t status_string[][64] = {{"LIFECYCLE_STATE_BLANK"},
                                                {"LIFECYCLE_STATE_TESTING"},
                                                {"LIFECYCLE_STATE_INSTALLING"},
                                                {"LIFECYCLE_STATE_VALID"},
                                                {"LIFECYCLE_STATE_INVALID"},
                                                {"LIFECYCLE_STATE_EOL"},
                                                {"LIFECYCLE_STATE_UNKNOWN"}};
    const uint8_t *tmp;

    if(status == LIFECYCLE_STATE_BLANK)
    {
        tmp = status_string[0];
    }
    else if(status == LIFECYCLE_STATE_TESTING)
    {
        tmp = status_string[1];
    }
    else if(status == LIFECYCLE_STATE_INSTALLING)
    {
        tmp = status_string[2];
    }
    else if(status == LIFECYCLE_STATE_VALID)
    {
        tmp = status_string[3];
    }
    else if(status == LIFECYCLE_STATE_INVALID)
    {
        tmp = status_string[4];
    }
    else if(status == LIFECYCLE_STATE_EOL)
    {
        tmp = status_string[5];
    }
    else
    {
        tmp = status_string[6];
    }
    return tmp;
}
fwup_err_t fwup_flash_err_print(flash_err_t error_code, flash_prt_err_t err_prt)
{
    switch(err_prt)
    {
        case ERR_FLASH_WRITE:
            DEBUG_LOG2("fwup_flash_write() returns error. %d.\r\n", error_code);
            break;
        case ERR_FLASH_WRITE_CB:
            DEBUG_LOG2("flash_write callback returns error. %d.\r\n", error_code);
            break;
        case ERR_FLASH_ERASE:
            DEBUG_LOG2("fwup_flash_erase() returns error. %d.\r\n", error_code);
            break;
        case ERR_FLASH_ERASE_CB:
            DEBUG_LOG2("flash_erase callback returns error. %d.\r\n", error_code);
            break;
        default:
            break;
    }
    DEBUG_LOG("system error.\r\n");
    fwup_update_status(FWUP_STATE_FATAL_ERROR);
    return FWUP_FAIL;
}
#endif  /* FWUP_IMPLEMENTATION_BOOTLOADER */

#ifndef BSP_MCU_SERIES_RX600
void my_sci_callback(uint32_t event)
{
    if (0 != (event & ARM_USART_EVENT_RECEIVE_COMPLETE))
    {
        /* 正常に受信完了した場合の処理を記述 */
        sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size = 0;
        sci_receive_control_block.p_sci_buffer_control->buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL;
        sci_receive_control_block.total_byte_size += FWUP_WRITE_BLOCK_SIZE;
        if (BOOT_LOADER_SCI_CONTROL_BLOCK_A == sci_receive_control_block.current_state)
        {
            sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_B;
            sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B];
            FWUP_CFG_USART.Receive(&sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B], FWUP_WRITE_BLOCK_SIZE);
        }
        else
        {
            sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_A;
            sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A];
            FWUP_CFG_USART.Receive(&sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A], FWUP_WRITE_BLOCK_SIZE);
        }
    }
    else if (event >= ARM_USART_EVENT_TX_UNDERFLOW)
    {
        __ASM volatile ("nop");
    }
    else
    {
        __ASM volatile ("nop");
    }
}
#endif /* BSP_MCU_SERIES_RX600 */

