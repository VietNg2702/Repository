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
/***********************************************************************************************************************
 * File Name    : usart.h
 * Description  : sample program of usart program.
 ***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 28.12.2020 1.00    MP
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "r_usart_cmsis_api.h"

#ifndef USART_H_
#define USART_H_

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
#define USART_CH Driver_USART0
#define UART_BUS_SPEED (115200)

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
#define BUFFER_SIZE    (256)

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/
extern ARM_DRIVER_USART USART_CH;
extern volatile uint32_t g_detect_command_flag;
extern uint8_t g_receive_buffer[BUFFER_SIZE];
extern uint32_t g_current_received_data;

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
extern void usart_disable(void);
extern void usart_enable(ARM_USART_SignalEvent_t callback);
extern void usart_start_transmission(void);
extern void usart_start_reception(uint8_t * p_data);
extern void usart_bussy(void);

#endif /* USART_H_ */
