/**********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MECHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2018-2021 Renesas Electronics Corporation. All rights reserved.    
**********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : lib_additional.c
* Description  : Additional common library
**********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lr1110_user_config_mode.h"
#include "RE01_256KB.h"
#include "r_system_api.h"
#include "r_lpm_api.h"
#include "lib_additional.h"

char* delComma(char *st)
{
    char c, *st0;
    st0=st;
    while ((c = *st) != '\0') 
    {
        if (c == ',') *st=' ';
        if (c == '\n' || c == '\r' ) {*st=' ';}
        st++;
    }

    return(st0);
}

char* delCommaSemicolon(char *st)
{
    char c, *st0;
    st0=st;
    while ((c = *st) != '\0')
    {
        if (c == ',' || c == ';') *st=' ';
        if (c == '\n' || c == '\r' ) {*st=' ';}
        st++;
    }

    return(st0);
}

char* delCRLF(char *st)
{
    char c, *st0;
    st0=st;
    while ((c = *st) != '\0') 
    {
        if (c == '\n' || c == '\r' ) {*st='\0';}
        st++;
    }

    return(st0);
}


uint8_t readHEX8(uint8_t* pos)
{
    volatile uint8_t x = 0;
    volatile uint8_t c;

    for (int8_t ic = 0; ic < 2; ic++ ) 
    {
        c = *pos;
        if (c == '\0') {break;}
        pos++;
        x = x << 4;
        x= x + hexd(c);
    }

    return (x);
}

// Ascii -> decimal
uint8_t hexd(uint8_t c0)
{
    uint8_t c = c0;
    if   (c <= '9') 
    {
        return (c - '0');
    }
    else if (c <= 'F') 
    {
        return (c + (uint8_t)10 - 'A');
    }
    else 
    {
        return (c + (uint8_t)10 - 'a');
    }
}


void wait_long()
{
    volatile int ic, jc;
    for (int ic = 0; ic < 1000; ic++) 
    {
        for (int jc = 0; jc < 100; jc++) 
        {

        }
    }
}


/***********************************************************************************************************************
*
* CRC8
* MSB first   x8 + x2 + x + 1
*
***********************************************************************************************************************/

uint8_t CRC8_calc(uint8_t *buff, uint16_t size)
{
	volatile uint8_t crc8;

	R_LPM_ModuleStart(LPM_MSTP_CRC);

	CRC->CRCCR0 = 0xC1;  // 8bit MSB first

    for(; size != 0; size--) 
    {
		CRC->CRCDIR_BY = *buff++;
	}

	crc8 = CRC->CRCDOR_BY;
	R_LPM_ModuleStop(LPM_MSTP_CRC);

    return crc8;
}


#define MSB_CRCS (0x07)  // x8 + x2 + x + 1

uint8_t CRC8_calc_software(uint8_t *buff, uint16_t size)
{
    volatile uint8_t crc8 = 0x00;

    for(; size != 0; size--) 
    {
        crc8 ^= *buff++;
        for(int8_t idx=0; idx < 8; idx++) 
        {
            if (crc8 & 0x80) 
            {
                crc8 <<=1;
                crc8 ^= MSB_CRCS;
            } 
            else 
            {
                crc8 <<=1;
            }
        }
    }

    return crc8;
}

