#ifndef __I2C_BOARD_H__
#define __I2C_BOARD_H__

#include <stdint.h>
#include <stdbool.h>
#include "r_i2c_cmsis_api.h"
#include "r_system_api.h"

/*!
 * Operation Mode for the I2C
 */
typedef enum
{
    MODE_I2C = 0,
    MODE_SMBUS_DEVICE,
    MODE_SMBUS_HOST
}I2cMode;

/*!
 * I2C signal duty cycle
 */
typedef enum
{
    I2C_DUTY_CYCLE_2 = 0,
    I2C_DUTY_CYCLE_16_9
}I2cDutyCycle;

/*!
 * I2C select if the acknowledge in after the 7th or 10th bit
 */
typedef enum
{
    I2C_ACK_ADD_7_BIT = 0,
    I2C_ACK_ADD_10_BIT
}I2cAckAddrMode;

/*!
 * Internal device address size
 */
typedef enum
{
    I2C_ADDR_SIZE_8 = 0,
    I2C_ADDR_SIZE_16,
}I2cAddrSize;

#ifdef __cplusplus
extern "C" {
#endif

void system_i2c_init( ARM_DRIVER_I2C *i2c_obj0)  __attribute__ ((section(".ramfunc")));

void system_i2c_stop( ARM_DRIVER_I2C* i2c_obj)  __attribute__ ((section(".ramfunc")));

uint8_t system_i2c_write(ARM_DRIVER_I2C *i2c_obj0,  uint8_t deviceAddr,  uint8_t *buffer, uint8_t size , const bool repeated )  __attribute__ ((section(".ramfunc")));

uint8_t system_i2c_read(ARM_DRIVER_I2C *i2c_obj0,  uint8_t deviceAddr, uint8_t *buffer, uint8_t size , const bool repeated)  __attribute__ ((section(".ramfunc")));


#ifdef __cplusplus
}
#endif

#endif // __I2C_BOARD_H__
