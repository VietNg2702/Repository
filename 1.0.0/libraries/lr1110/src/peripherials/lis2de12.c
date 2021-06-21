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
* File Name    : lis2de12.c
* Description  : Device accessor to the LIS2DE12 Accelerometer
**********************************************************************************************************************/

#include "RE01_256KB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lr1110_user_config_mode.h"
#include "lr1110_user_config_io.h"
#include "system/inc/system.h"
#include "r_i2c_cmsis_api.h"
#include "r_lpm_api.h"
#include "lis2de12.h"

static char lis2de12_buffer[10];
static int16_t data16[7];
static int8_t bytes[10];

extern ARM_DRIVER_I2C Driver_I2C0;

#define SUCCESS (0)
#define FAIL    (-1)



int8_t lis2de12_read_bytes(uint8_t address, int8_t* dst, int8_t length)
{
    volatile int ret1, ret2;
    lis2de12_buffer[0]  = address;

    ret1 = system_i2c_write(&Driver_I2C0, LIS2DE12_I2C_ADD,  (uint8_t*)&lis2de12_buffer[0], 1 , true );
    APP_ERR_RETURN(ret1);

    system_time_wait_ms( 10 );

    ret2 = system_i2c_read(&Driver_I2C0, LIS2DE12_I2C_ADD,  (uint8_t*)dst, length , false );
    APP_ERR_RETURN(ret2);

    return SUCCESS;
}

int8_t lis2de12_write_byte(int8_t address, int8_t byte)
{
    volatile int ret1;
    lis2de12_buffer[0]  = address;
    lis2de12_buffer[1]  = byte;

	ret1 = system_i2c_write(&Driver_I2C0, LIS2DE12_I2C_ADD,  (uint8_t*)lis2de12_buffer, 2 , false );
    APP_ERR_RETURN(ret1);
    
    return SUCCESS;
}



/**************************************************************************//**
* @brief  Sensor data read request to IIS2DLPC
*
* @retval       
******************************************************************************/
int8_t lis2de12_check_id()
{
    volatile int ret3;
    bytes[0] = 0x0;
    
    ret3 = lis2de12_read_bytes(LIS2DE12_WHO_AM_I, &bytes[0], 1);
    APP_ERR_RETURN(ret3);

    if (bytes[0] == LIS2DE12_ID) 
    {
        return SUCCESS;
    }
	else 
    {
        return FAIL;
    }
}


int8_t lis2de12_data_fetch(accel_data_t * data)
{
    volatile int ret;

    ret = lis2de12_read_bytes(LIS2DE12_OUT_X_H, (int8_t*)data16, 6);
    APP_ERR_RETURN(ret);

    data->x = data16[0];
    data->y = data16[1];
    data->z = data16[2];

    return ret;
}


int8_t lis2de12_init()
{
    volatile int8_t ret;

    ret = lis2de12_check_id();

    //  ret = lis2de12_write_byte(LIS2DE12_CTRL_REG1, 0x1F);  // Sample 1Hz
    ret = lis2de12_write_byte(LIS2DE12_CTRL_REG1, 0x2F);  // Sample 10Hz
    //  ret = lis2de12_write_byte(LIS2DE12_CTRL_REG1, 0x7F);  // Sample 400Hz
    APP_ERR_RETURN(ret);

    ret = lis2de12_write_byte(LIS2DE12_CTRL_REG3, 0x40);  // IA1 interrupt
    APP_ERR_RETURN(ret);

    ret = lis2de12_write_byte(LIS2DE12_CTRL_REG5, 0x04);  // 4D enable Latch Disable
    //  ret = lis2de12_write_byte(LIS2DE12_CTRL_REG5, 0x00);  // 4D Disable Latch Disable
    APP_ERR_RETURN(ret);

    ret = lis2de12_write_byte(LIS2DE12_CTRL_REG6, 0x00);  // INT POLARITY
    APP_ERR_RETURN(ret);

    ret = lis2de12_write_byte(LIS2DE12_INT1_THS, 0x1F);  // 0.5g Threshold
    APP_ERR_RETURN(ret);

    ret = lis2de12_write_byte(LIS2DE12_INT1_CFG, 0x6A);  // 6D Mode is needed for motion detect
    APP_ERR_RETURN(ret);

    //  ret = lis2de12_write_byte(LIS2DE12_CTRL_REG1, 0x0F);  // Sleep
    //  APP_ERR_RETURN(ret);

    return SUCCESS;  
}


int8_t accelerometer_init()
{
	int8_t result;

    result = lis2de12_check_id();
    APP_ERR_RETURN(result);

    result = lis2de12_init();
    APP_ERR_RETURN(result);

    system_time_wait_ms( 100 );

    return SUCCESS;  
}


int8_t accelerometer_read(accel_data_t* p_accel_data)
{
	int8_t result;

    result = lis2de12_data_fetch(p_accel_data);
    APP_ERR_RETURN(result);

    return SUCCESS;  
}


