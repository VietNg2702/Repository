/**
  ******************************************************************************
  * @file           : hdc2010.c
  * @Description	: Lib build over STM32 HAL to interface i2c with hcd2010 temperature and humidity sensor.
  *
  ******************************************************************************
  */

#include <stdint.h>
#include "hdc2010.h"

static hdc2010_i2c_handle_t *i2c_handle = NULL;


/**************************************************************************
* Function: hdc2010_initialize() 
* Description: Function to initialize hdc2010
* Parameter: read / write function pointer
* Return: None
***************************************************************************/
void hdc2010_initialize(hdc2010_i2c_handle_t *i2c)
{
    i2c_handle = i2c;
}

/**************************************************************************
* Function: openReg() 
* Description: Function to open a register
* Parameter: register
* Return: None
***************************************************************************/
void openReg(uint8_t reg)
{
    // Connect to HDC2010 point to specified register
    // Relinquish bus control
    i2c_handle->transmit(0x40, &reg, 1);
}

/**************************************************************************
* Function: readReg() 
* Description: Function to read data from register
* Parameter: reg: register
* Return: read data
***************************************************************************/
uint8_t readReg(uint8_t reg)
{
    uint8_t reading=0; 					// holds byte of read data

    i2c_handle->transmit(HDC2010_I2C_ADDR, &reg, 1);
    i2c_handle->receive(HDC2010_I2C_ADDR, &reading, 1);
}

/**************************************************************************
* Function: writeReg() 
* Description: Function to write data into register
* Parameter: reg: register
*            data: writed data
* Return: None
***************************************************************************/
void writeReg(uint8_t reg, uint8_t data)
{
    uint8_t data_tam[2];
    
    data_tam[0] = reg;     
    data_tam[1] = data;    //  8bit data
    i2c_handle->transmit(HDC2010_I2C_ADDR, data_tam, 2);  // data is the start pointer of our array
}

/**************************************************************************
* Function: readTemp() 
* Description: Function get data from sensor and return temperature
* Parameter: None
* Return: Return temperature
***************************************************************************/
float hdc2010_readTemp(void)
{
    uint8_t byte[2]={0};
    uint16_t temp;
    
    byte[0] = readReg(REG_TEMP_LOW);
    byte[1] = readReg(REG_TEMP_HIGH);
    temp = (unsigned int)(byte[1]) << 8 | (unsigned int) byte[0];
    
    return (float)(temp) * 165 / 65536 - 40;

}

/**************************************************************************
* Function: readHumidity() 
* Description: Function get data from sensor and return Humidity
* Parameter: None
* Return: Return Humidity
***************************************************************************/
float hdc2010_readHumidity(void)
{
    uint8_t byte[2];
    uint16_t humidity;
    
    byte[0] = readReg(REG_HUMID_LOW);
    byte[1] = readReg(REG_HUMID_HIGH);
    humidity = (unsigned int)byte[1] << 8 | byte[0];
    
    return (float)(humidity)/( 65536 )* 100;
}

/**************************************************************************
* Function: enableHeater() 
* Description: Function enable heater
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_enableHeater(void)
{
    uint8_t configContents;	//Stores current contents of config register
    
    configContents = readReg(REG_CONFIG);
    //set bit 3 to 1 to enable heater
    configContents = (configContents | 0x08);
    writeReg(REG_CONFIG, configContents);
}

/**************************************************************************
* Function: disableHeater() 
* Description: Function disable heater
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_disableHeater(void)
{
    uint8_t configContents;	//Stores current contents of config register
    
    configContents = readReg(REG_CONFIG);
    //set bit 3 to 0 to disable heater (all other bits 1)
    configContents = (configContents & 0xF7);
    writeReg(REG_CONFIG, configContents);
}

/**************************************************************************
* Function: setLowTemp() 
* Description: Function set low temperature threshold
* Parameter: temperature
* Return: None
***************************************************************************/
void hdc2010_setLowTemp(float temp)
{
    uint8_t temp_thresh_low;
    
    // Verify user is not trying to set value outside bounds
    if (temp < -40){
        temp = -40;
    }
    else if (temp > 125){
        temp = 125;
    }
    // Calculate value to load into register
    temp_thresh_low = (uint8_t)(256 * (temp + 40)/165);
    writeReg(REG_TEMP_THR_L, temp_thresh_low);
}

/**************************************************************************
* Function: setHighTemp() 
* Description: Function set high temperature threshold
* Parameter: temperature
* Return: None
***************************************************************************/
void hdc2010_setHighTemp(float temp)
{
    uint8_t temp_thresh_high;
    
    // Verify user is not trying to set value outside bounds
    if (temp < -40)
    {
        temp = -40;
    }
    else if (temp > 125)
    {
        temp = 125;
    }
    // Calculate value to load into register
    temp_thresh_high = (uint8_t)(256 * (temp + 40)/165);
    writeReg(REG_TEMP_THR_H, temp_thresh_high);
}

/**************************************************************************
* Function: setHighHimidity() 
* Description: Function set low humidity threshold
* Parameter: humidity
* Return: None
***************************************************************************/
void hdc2010_setHighHumidity(float humid)
{
    uint8_t humid_thresh;
    
    // Verify user is not trying to set value outside bounds
    if (humid < 0)
    {
        humid = 0;
    }
    else if (humid > 100)
    {
        humid = 100;
    }
    // Calculate value to load into register
    humid_thresh = (uint8_t)(256 * (humid)/100);
    writeReg(REG_HUMID_THR_H, humid_thresh);
}

/**************************************************************************
* Function: setLowHimidity() 
* Description: Function set low humidity threshold
* Parameter: humidity
* Return: None
***************************************************************************/
void hdc2010_setLowHumidity(float humid)
{
    uint8_t humid_thresh;
    
    // Verify user is not trying to set value outside bounds
    if (humid < 0)
    {
        humid = 0;
    }
    else if (humid > 100)
    {
        humid = 100;
    }
    // Calculate value to load into register
    humid_thresh = (uint8_t)(256 * (humid)/100);
    writeReg(REG_HUMID_THR_L, humid_thresh);
}

/**************************************************************************
* Function: readLowHumidityThreshold() 
* Description: Function read low humidity threshold
* Parameter: None
* Return: humidity from the low threshold register
***************************************************************************/
float hdc2010_readLowHumidityThreshold(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_HUMID_THR_L);
    return (float)regContents * 100/256;
}

/**************************************************************************
* Function: readHighHumidityThreshold() 
* Description: Function read high humidity threshold
* Parameter: None
* Return: humidity from the high threshold register
***************************************************************************/
float hdc2010_readHighHumidityThreshold(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_HUMID_THR_H);
    return (float)regContents * 100/256;
}

/**************************************************************************
* Function: readLowTempThreshold() 
* Description: Function read low temperature threshold
* Parameter: None
* Return: temperature from the low threshold register
***************************************************************************/
float hdc2010_readLowTempThreshold(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_TEMP_THR_L);
    return (float)regContents * 165/256 - 40;
}

/**************************************************************************
* Function: readHighTempThreshold() 
* Description: Function read high temperature threshold
* Parameter: None
* Return: temperature from the high threshold register
***************************************************************************/
float hdc2010_readHighTempThreshold(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_TEMP_THR_H);
    return (float)regContents * 165/256 - 40;
}

/**************************************************************************
* Function: setTempRes() 
* Description: Function set temperature resolution
* Parameter: Resolution
* Return: None
***************************************************************************/
void hdc2010_setTempRes(int resolution)
{
    /* Upper two bits of the MEASUREMENT_CONFIG register controls
   the temperature resolution*/
    uint8_t configContents;
    
    configContents = readReg(REG_MEASUREMENT_CONFIG);
    switch(resolution)
    {
        case _14_BITS:
            configContents = (configContents & 0x3F);
            break;

        case _11_BITS:
            configContents = (configContents & 0x7F);
            configContents = (configContents | 0x40);
            break;

        case _9_BITS:
            configContents = (configContents & 0xBF);
            configContents = (configContents | 0x80);
            break;

        default:
            configContents = (configContents & 0x3F);
    }
    writeReg(REG_MEASUREMENT_CONFIG, configContents);
}


/**************************************************************************
* Function: setHumidRes() 
* Description: Function set humidity resolution
* Parameter: Resolution
* Return: None
***************************************************************************/
void hdc2010_setHumidRes(int resolution)
{
    /*  Bits 5 and 6 of the MEASUREMENT_CONFIG register controls
    the humidity resolution*/
    uint8_t configContents;
    
    configContents = readReg(REG_MEASUREMENT_CONFIG);
    switch(resolution)
    {
        case _14_BITS:
            configContents = (configContents & 0xCF);
            break;

        case _11_BITS:
            configContents = (configContents & 0xDF);
            configContents = (configContents | 0x10);
            break;

        case _9_BITS:
            configContents = (configContents & 0xEF);
            configContents = (configContents | 0x20);
            break;

        default:
            configContents = (configContents & 0xCF);
    }
    writeReg(REG_MEASUREMENT_CONFIG, configContents);
}


/**************************************************************************
* Function: setMeasurementMode() 
* Description: Function set measurement mode
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_setMeasurementMode(int mode)
{
    /*  Bits 2 and 1 of the MEASUREMENT_CONFIG register controls
    the measurement mode  */
    uint8_t configContents;
    
    configContents = readReg(REG_MEASUREMENT_CONFIG);
    switch(mode)
    {
        case TEMP_AND_HUMID:
            configContents = (configContents & 0xF9);
            break;

        case TEMP_ONLY:
            configContents = (configContents & 0xFC);
            configContents = (configContents | 0x02);
            break;

        case HUMID_ONLY:
            configContents = (configContents & 0xFD);
            configContents = (configContents | 0x04);
            break;

        default:
            configContents = (configContents & 0xF9);
    }
    writeReg(REG_MEASUREMENT_CONFIG, configContents);
}

/**************************************************************************
* Function: triggerMeasurement() 
* Description: Function trigger measurement
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_triggerMeasurement(void)
{
    /*  Bit 0 of the MEASUREMENT_CONFIG register can be used
    to trigger measurements  */
    uint8_t configContents;
    
    configContents = readReg(REG_MEASUREMENT_CONFIG);
    configContents = (configContents | 0x01);
    writeReg(REG_MEASUREMENT_CONFIG, configContents);
}

/**************************************************************************
* Function: reset() 
* Description: Function to reset
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_reset(void)
{
    /*  Bit 7 of the CONFIG register can be used to trigger a
    soft reset  */
    uint8_t configContents;
    
    configContents = readReg(REG_CONFIG);
    configContents = (configContents | 0x80);
    writeReg(REG_CONFIG, configContents);
}

/**************************************************************************
* Function: enableInterrupt() 
* Description: Function to enable interrupt
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_enableInterrupt(void)
{
    /*  Bit 2 of the CONFIG register can be used to enable/disable
    the interrupt pin  */
    uint8_t configContents;
    
    configContents = readReg(REG_CONFIG);
    configContents = (configContents | 0x04);
    writeReg(REG_CONFIG, configContents);
}

/**************************************************************************
* Function: disableInterrupt() 
* Description: Function to disable interrupt
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_disableInterrupt(void)
{
    /*  Bit 2 of the CONFIG register can be used to enable/disable
    the interrupt pin  */
    uint8_t configContents;
    
    configContents = readReg(REG_CONFIG);
    configContents = (configContents & 0xFB);
    writeReg(REG_CONFIG, configContents);
}

/**************************************************************************
* Function: setRate() 
* Description: Function to set rate
* Parameter: Rate
* Return: None
***************************************************************************/
void hdc2010_setRate(int rate)
{
    /*  Bits 6-4  of the CONFIG register controls the measurement
    rate  */
    uint8_t configContents;
    
    configContents = readReg(REG_CONFIG);
    switch(rate)
    {
        case 0:
            configContents = (configContents & 0x8F);
            break;
        case 1:
            configContents = (configContents & 0x9F);
            configContents = (configContents | 0x10);
            break;
        case 2:
            configContents = (configContents & 0xAF);
            configContents = (configContents | 0x20);
            break;
        case 3:
            configContents = (configContents & 0xBF);
            configContents = (configContents | 0x30);
            break;
        case 4:
            configContents = (configContents & 0xCF);
            configContents = (configContents | 0x40);
            break;
        case 5:
            configContents = (configContents & 0xDF);
            configContents = (configContents | 0x50);
            break;
        case 6:
            configContents = (configContents & 0xEF);
            configContents = (configContents | 0x60);
            break;
        case 7:
            configContents = (configContents | 0x70);
            break;
        default:
            configContents = (configContents & 0x8F);
    }
    writeReg(REG_CONFIG, configContents);
}

/**************************************************************************
* Function: setInterruptPolarity() 
* Description: Function to set interrupt polarity
* Parameter: polarity
* Return: None
***************************************************************************/
void hdc2010_setInterruptPolarity(int polarity)
{
    /*  Bit 1 of the CONFIG register can be used to control the
    the interrupt pins polarity */
    uint8_t configContents;
    
    configContents = readReg(REG_CONFIG);
    switch(polarity)
    {
        case POLARITY_ACTIVE_LOW:
            configContents = (configContents & 0xFD);
            break;
        case POLARITY_ACTIVE_HIGH:
            configContents = (configContents | 0x02);
            break;
        default:
            configContents = (configContents & 0xFD);
    }
    writeReg(REG_CONFIG, configContents);
}

/**************************************************************************
* Function: setInterruptMode() 
* Description: Function to set interrupt mode
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_setInterruptMode(int mode)
{
    /*  Bit 0 of the CONFIG register can be used to control the
    the interrupt pin's mode */
    uint8_t configContents;
    
    configContents = readReg(REG_CONFIG);
    switch(mode)
    {
        case INTERRUPT_LEVEL_MODE:
            configContents = (configContents & 0xFE);
            break;

        case INTERRUPT_COMPARATOR_MODE:
            configContents = (configContents | 0x01);
            break;

        default:
            configContents = (configContents & 0xFE);
    }
    writeReg(REG_CONFIG, configContents);
}

/**************************************************************************
* Function: readInterruptStatus() 
* Description: Function to read interrupt status
* Parameter: None
* Return: Status
***************************************************************************/
uint8_t hdc2010_readInterruptStatus(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_INTERRUPT_DRDY);
    return regContents;
}


/**************************************************************************
* Function: clearMaxTemp() 
* Description: Function to clear the maximum temperature register
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_clearMaxTemp(void)
{
    writeReg(REG_TEMP_MAX, 0x00);
}

/**************************************************************************
* Function: clearMaxHumidity() 
* Description: Function to clear the maximum humidity register
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_clearMaxHumidity(void)
{
    writeReg(REG_HUMID_MAX, 0x00);
}

/**************************************************************************
* Function: readMaxTemp() 
* Description: Function to read the maximum temperature register
* Parameter: None
* Return: Temperature
***************************************************************************/
float hdc2010_readMaxTemp(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_TEMP_MAX);
    return (float)regContents * 165/256 - 40;

}

/**************************************************************************
* Function: readMaxHumidity() 
* Description: Function to read the maximum humidity register
* Parameter: None
* Return: None
***************************************************************************/
float hdc2010_readMaxHumidity(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_HUMID_MAX);
    return (float)regContents /256 * 100;
}

/**************************************************************************
* Function: enableThresholdInterrupt() 
* Description: Function to enable the interrupt pin for comfort zone operation
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_enableThresholdInterrupt(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_INTERRUPT_CONFIG);
    regContents = (regContents | 0x78);
    writeReg(REG_INTERRUPT_CONFIG, regContents);
}

/**************************************************************************
* Function: disableThresholdInterrupt() 
* Description: Function to disable the interrupt pin for comfort zone operation
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_disableThresholdInterrupt(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_INTERRUPT_CONFIG);
    regContents = (regContents & 0x87);
    writeReg(REG_INTERRUPT_CONFIG, regContents);
}


/**************************************************************************
* Function: enableDRDYInterrupt() 
* Description: Function to enable the interrupt pin for DRDY operation
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_enableDRDYInterrupt(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_INTERRUPT_CONFIG);
    regContents = (regContents | 0x80);
    writeReg(REG_INTERRUPT_CONFIG, regContents);
}

/**************************************************************************
* Function: disableDRDYInterrupt() 
* Description: Function to disable the interrupt pin for DRDY operation
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_disableDRDYInterrupt(void)
{
    uint8_t regContents;
    
    regContents = readReg(REG_INTERRUPT_CONFIG);
    regContents = (regContents & 0x7F);
    writeReg(REG_INTERRUPT_CONFIG, regContents);
}

/**************************************************************************
* Function: setTemperatureOffset() 
* Description: Function to set temperature offset
* Parameter: Value
* Return: None
***************************************************************************/
void hdc2010_setTemperatureOffset(uint8_t value)
{
    writeReg(REG_TEMP_OFFSET_ADJUST, value);
}

