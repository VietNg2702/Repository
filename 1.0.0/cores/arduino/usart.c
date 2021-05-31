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
 * File Name    : usart.c
 * Description  : sample program of Firmware update main program.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 28.12.2020 1.00    MP
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "r_system_api.h"
#include "r_usart_cmsis_api.h"
#include "usart.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
uint32_t g_current_received_data = 0;
uint32_t g_prog_mode = 0;
volatile uint32_t g_detect_command_flag = 0;
uint8_t g_receive_buffer[BUFFER_SIZE] = {0};

void myusart_callback(uint32_t event);
extern void my_sci_callback(uint32_t event);

/***********************************************************************************************************************
 * Function Name: usart_disable
 * Description  : usart_disable
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void usart_disable(void)
{
    /* Power off the USART peripheral */
    USART_CH.PowerControl(ARM_POWER_OFF);

    /* Uninitialize the USART driver */
    USART_CH.Uninitialize();

    return;
}
/***********************************************************************************************************************
End of function usart_disable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: usart_enable
 * Description  : usart_enable
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void usart_enable(void)
{
    uint32_t freq_hz = 0;

    /* Initialize the USART driver */
    USART_CH.Initialize(my_sci_callback);

    /* Power up the USART peripheral */
    USART_CH.PowerControl(ARM_POWER_FULL);

    /*Configure the USART */
    R_SYS_SystemClockFreqGet(&freq_hz);

    /* Set up the configuration data structure for asynchronous (UART) operation. */
    USART_CH.Control(ARM_USART_MODE_ASYNCHRONOUS |
                          ARM_USART_DATA_BITS_8 |
                          ARM_USART_PARITY_NONE |
                          ARM_USART_STOP_BITS_1 |
                          ARM_USART_FLOW_CONTROL_NONE, 115200);

    return;
}
/***********************************************************************************************************************
End of function usart_enable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: usart_start_transmission
 * Description  : usart_start_transmission
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void usart_start_transmission(void)
{
    /* Enable transmitter lines */
    USART_CH.Control(ARM_USART_CONTROL_TX, 1);
    return;
}
/***********************************************************************************************************************
End of function usart_start_transmission
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: usart_start_reception
 * Description  : usart_start_reception
 * Arguments    : uint8_t * p_data
 *                uint32_t num
 * Return Value : none
 **********************************************************************************************************************/
void usart_start_reception(uint8_t * p_data, uint32_t num)
{
    /* Enable Receiver and Transmitter lines */
    USART_CH.Control(ARM_USART_CONTROL_RX, 1);

    /* Start reception. */
    USART_CH.Receive(p_data, num);
    return;
}
/***********************************************************************************************************************
End of function usart_start_reception
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: usart_bussy
 * Description  : usart_bussy
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void usart_bussy(void)
{
    ARM_USART_STATUS status;

    /* Check busy. */
    do
    {
        status = USART_CH.GetStatus();
    }
    while(1 == status.tx_busy);

    return;
}
/***********************************************************************************************************************
End of function usart_bussy
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: fputc
 * Description  : Retargets the C library printf function to the USART.
 * Arguments    : int ch
 *              : FILE *f
 * Return Value : none
 **********************************************************************************************************************/
int fputc(int ch, FILE *f)
{
    uint8_t buff[1];

    /* Cast char date to buffer*/
    buff[0] = (uint8_t)ch;

    usart_bussy();
    USART_CH.Send(buff, 1);

    return ch;
}
/***********************************************************************************************************************
End of function fputc
***********************************************************************************************************************/

#if defined(__GNUC__)                  /* GCC Compiler */
/***********************************************************************************************************************
 * Function Name: _write
 * Description  : Retargets the C library printf function to the USART for GCC.
 * Arguments    : int ch
 *              : FILE *f
 * Return Value : int
 **********************************************************************************************************************/
int _write(int file, char *data, int len)
{
    uint32_t i;
    FILE f;
    for(i=0;i<len;i++)
    {
        fputc(data[i], &f);
    }

    return len;
}
/***********************************************************************************************************************
End of function _write
***********************************************************************************************************************/
#endif


/***********************************************************************************************************************
 * Function Name: myusart_callback
 * Description  : myusart_callback
 * Arguments    : uint32_t event
 * Return Value : none
 **********************************************************************************************************************/
void myusart_callback(uint32_t event)
{
    switch (event)
    {
        case ARM_USART_EVENT_RECEIVE_COMPLETE:
            if (0 == g_prog_mode)
            {
                if ('\r' == g_receive_buffer[g_current_received_data])
                {
                    g_detect_command_flag = 1;
                    g_current_received_data = 0;
                }
                else
                {
                    g_current_received_data++;
                }
                if ((sizeof(g_receive_buffer)) == g_current_received_data)
                {
                    g_current_received_data = 0;
                }
                USART_CH.Receive(g_receive_buffer + g_current_received_data, 1);
            }
            else
            {
                g_detect_command_flag = 1;
            }
            break;
        case ARM_USART_EVENT_RX_OVERFLOW:
            __ASM volatile ("nop");
            break;
        case ARM_USART_EVENT_RX_TIMEOUT:
            __ASM volatile ("nop");
            break;
        case ARM_USART_EVENT_RX_BREAK:
            __ASM volatile ("nop");
            break;
        case ARM_USART_EVENT_RX_FRAMING_ERROR:
            __ASM volatile ("nop");
            break;
        case ARM_USART_EVENT_RX_PARITY_ERROR:
            __ASM volatile ("nop");
            break;
        default:
            __ASM volatile ("nop");
            break;
    }
    return;
}
/***********************************************************************************************************************
End of function myusart_callback
***********************************************************************************************************************/


/* End of File */
