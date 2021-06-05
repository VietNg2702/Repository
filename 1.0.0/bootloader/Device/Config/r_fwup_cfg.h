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
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name     : r_fwup_cfg.h
* Description   : Configures the Firmware update module.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           28.12.2020 1.00    Initial Release
***********************************************************************************************************************/
#ifndef FWUP_CFG_H
#define FWUP_CFG_H

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Select the implementation environment.
    0 = Bootloader. (default)
    1 = Firmware update w/o OS.
    2 = Firmware update with Amazon FreeRTOS(OTA).
    3 = Firmware update with other FreeRTOS.
*/
#define FWUP_CFG_IMPLEMENTATION_ENVIRONMENT     (0)

/* Select the communication function.
    0 = SCI connection. (default)
    1 = Ethernet connection.
    2 = USB connection.
    3 = SDHI connection.
    4 = QSPI connection.
*/
#define FWUP_CFG_COMMUNICATION_FUNCTION        (0)

/* Select the algorithm of signature verification.
    0 = ECDSA.
    1 = SHA256 (default)
*/
#define FWUP_CFG_SIGNATURE_VERIFICATION        (1)

/* Enable Boot Protect Setting.
    0 = Disable.(Prohibit) （default）
    1 = Enable.(Allow)     [Note]

    [Note]
    When enabled (1), FAW.FSPR bit = 0 is set. After this setting,
    the area other than the area specified in FAW can never be rewritten.
    Be careful when setting this variable.
 */
#define FWUP_CFG_BOOT_PROTECT_ENABLE     (0)

/*  MAP FWUP DRIVER TO ROM
    Setting to 1 maps the firmware updater driver to the ROM. (instead of RAM)
    This mean, user can locate all of firmware updater driver into Flash memory.
    By using BGO, user can write and erase the block which does not has firmware updater driver.
 */

#define FWUP_CFG_FUNC_SECTION_ROM (0)

/* USE USART Channel
 */
#define FWUP_CFG_USART Driver_USART0

#if (FWUP_CFG_FUNC_SECTION_ROM == 0)
/*  R_FWUP_API_LOCATION_CONFIG
    Definition of R_FWUP API location configuration@n
    Please select "SYSTEM_SECTION_CODE" or "SYSTEM_SECTION_RAM_FUNC".@n
    It takes effect when FWUP_CFG_CODE_FLASH_RUN_FROM_ROM is 0.@n
    When FWUP_CFG_CODE_FLASH_RUN_FROM_ROM is 1, all R_FWUP API locate in Flash memory
*/
/* @{ */
#define FWUP_CFG_SECTION_R_FLASH_OPEN                        (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FLASH_CLOSE                       (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_OPERATION                    (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_SET_END_OF_LIFE              (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_SOFTWARE_RESET               (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_CREATE_FILE_FOR_RX           (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_ABOUT                        (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_WRITE_BLOCK                  (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_CLOSE_FILE                   (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_CHECK_FILE_SIGNATURE         (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_READ_AND_ASSUME              (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_RESET_DEVICE                 (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_ACTIVATE_NEWIMAGE            (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_SET_PLATFORM                 (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_GET_PLATFORM                 (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_GET_VERSION                  (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_SECURE_BOOT                  (SYSTEM_SECTION_RAM_FUNC)
#define FWUP_CFG_SECTION_R_FWUP_EXECUTE_FIRMWARE             (SYSTEM_SECTION_RAM_FUNC)
/* @} */
#else
#define FWUP_CFG_SECTION_R_FLASH_OPEN                        (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FLASH_CLOSE                       (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_OPERATION                    (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_SET_END_OF_LIFE              (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_SOFTWARE_RESET               (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_CREATE_FILE_FOR_RX           (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_ABOUT                        (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_WRITE_BLOCK                  (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_CLOSE_FILE                   (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_CHECK_FILE_SIGNATURE         (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_READ_AND_ASSUME              (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_RESET_DEVICE                 (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_ACTIVATE_NEWIMAGE            (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_SET_PLATFORM                 (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_GET_PLATFORM                 (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_GET_VERSION                  (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_SECURE_BOOT                  (SYSTEM_SECTION_CODE)
#define FWUP_CFG_SECTION_R_FWUP_EXECUTE_FIRMWARE             (SYSTEM_SECTION_CODE)
#endif /* (FWUP_CFG_CODE_FLASH_RUN_FROM_ROM == 0) */

#if(0 == FWUP_CFG_FUNC_SECTION_ROM)
#define DATA_LOCATION_FWUP_RESOURCES       __attribute__ ((section(".ramdata"))) /* @suppress("Macro expansion") */
#else
#define DATA_LOCATION_FWUP_RESOURCES
#endif

#if(0 == FWUP_CFG_FUNC_SECTION_ROM)
#define FUNC_LOCATION_FWUP_RESOURCES       __attribute__ ((section(".ramfunc"))) /* @suppress("Macro expansion") */
#else
#define FUNC_LOCATION_FWUP_RESOURCES
#endif


#endif /* FWUP_CFG_H */
