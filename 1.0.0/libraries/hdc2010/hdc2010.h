/**
  ******************************************************************************
  * @file           : hdc2010.h
  * @Description	: Lib build over STM32 HAL to interface i2c with hcd2010 temperature and humidity sensor.
  *
  ******************************************************************************
  */

#ifndef __HDC2010_H_
#define __HDC2010_H_

#include <stddef.h>

#ifdef __cplusplus
 extern "C" {
#endif

// Define HDC2010
#define HDC2010_I2C_ADDR 	(0x40<<1)
#define HDC2010_IRQ_PIN 	6

//  the measurement resolution
#define _14_BITS 0
#define _11_BITS 1
#define _9_BITS  2

//  the measurement mode
#define TEMP_AND_HUMID 0
#define TEMP_ONLY	   1
#define HUMID_ONLY	   2

// the the interrupt pins polarity
#define POLARITY_ACTIVE_LOW		0
#define POLARITY_ACTIVE_HIGH	1

// the interrupt pin's mode
#define INTERRUPT_LEVEL_MODE		0
#define INTERRUPT_COMPARATOR_MODE	1

// the sample rate
#define MANUAL			0
#define TWO_MINS		1
#define ONE_MINS		2
#define TEN_SECONDS		3
#define	FIVE_SECONDS	4
#define ONE_HZ			5
#define TWO_HZ			6
#define FIVE_HZ			7

//Define Register Map
#define REG_TEMP_LOW 				0x00
#define REG_TEMP_HIGH 				0x01
#define REG_HUMID_LOW 				0x02
#define REG_HUMID_HIGH 				0x03
#define REG_INTERRUPT_DRDY 			0x04
#define REG_TEMP_MAX 				0x05
#define REG_HUMID_MAX 				0x06
#define REG_INTERRUPT_CONFIG 		0x07
#define REG_TEMP_OFFSET_ADJUST 		0x08
#define REG_HUM_OFFSET_ADJUST 		0x09
#define REG_TEMP_THR_L 				0x0A
#define REG_TEMP_THR_H 				0x0B
#define REG_HUMID_THR_L 			0x0C
#define REG_HUMID_THR_H 			0x0D
#define REG_CONFIG 					0x0E
#define REG_MEASUREMENT_CONFIG 		0x0F
#define REG_MID_L 					0xFC
#define REG_MID_H 					0xFD
#define REG_DEVICE_ID_L 			0xFE
#define REG_DEVICE_ID_H 			0xFF

typedef int32_t (*hdc2010_transmit_t)( uint8_t, const uint8_t *, uint16_t);
typedef int32_t (*hdc2010_receive_t)( uint8_t, uint8_t *, uint16_t);

typedef struct
{
  /** Component mandatory fields **/
  hdc2010_transmit_t  transmit;
  hdc2010_receive_t   receive;
} hdc2010_i2c_handle_t;


/**************************************************************************
* Function: hdc2010_initialize() 
* Description: Function to initialize hdc2010
* Parameter: read / write function pointer
* Return: None
***************************************************************************/
void hdc2010_initialize(hdc2010_i2c_handle_t *i2c);

/**************************************************************************
* Function: readTemp() 
* Description: Function get data from sensor and return temperature
* Parameter: None
* Return: Return temperature
***************************************************************************/
float hdc2010_readTemp(void);					// Returns the temperature in degrees C

/**************************************************************************
* Function: readHumidity() 
* Description: Function get data from sensor and return Humidity
* Parameter: None
* Return: Return Humidity
***************************************************************************/
float hdc2010_readHumidity(void);				// Returns the relative humidity

/**************************************************************************
* Function: enableHeater() 
* Description: Function enable heater
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_enableHeater(void);				// Enables the heating element

/**************************************************************************
* Function: disableHeater() 
* Description: Function disable heater
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_disableHeater(void);				// Disables the heating element

/**************************************************************************
* Function: setLowTemp() 
* Description: Function set low temperature threshold
* Parameter: temperature
* Return: None
***************************************************************************/
void hdc2010_setLowTemp(float temp);			// Sets low threshold temperature (in c)

/**************************************************************************
* Function: setHighTemp() 
* Description: Function set high temperature threshold
* Parameter: temperature
* Return: None
***************************************************************************/
void hdc2010_setHighTemp(float temp);			// Sets high threshold temperature (in c)

/**************************************************************************
* Function: setHighHimidity() 
* Description: Function set low humidity threshold
* Parameter: humidity
* Return: None
***************************************************************************/
void hdc2010_setHighHumidity(float humid);		// Sets high Humiditiy threshold

/**************************************************************************
* Function: setLowHimidity() 
* Description: Function set low humidity threshold
* Parameter: humidity
* Return: None
***************************************************************************/
void hdc2010_setLowHumidity(float humid);		// Sets low Humidity threshold

/**************************************************************************
* Function: readLowHumidityThreshold() 
* Description: Function read low humidity threshold
* Parameter: None
* Return: humidity from the low threshold register
***************************************************************************/
float hdc2010_readLowHumidityThreshold(void);	// Returns contents of low humidity threshold register

/**************************************************************************
* Function: readHighHumidityThreshold() 
* Description: Function read high humidity threshold
* Parameter: None
* Return: humidity from the high threshold register
***************************************************************************/
float hdc2010_readHighHumidityThreshold(void);	// Returns contents of high humidity threshold register

/**************************************************************************
* Function: readLowTempThreshold() 
* Description: Function read low temperature threshold
* Parameter: None
* Return: temperature from the low threshold register
***************************************************************************/
float hdc2010_readLowTempThreshold(void);		// Returns contents of low temperature threshold register (in C)

/**************************************************************************
* Function: readHighTempThreshold() 
* Description: Function read high temperature threshold
* Parameter: None
* Return: temperature from the high threshold register
***************************************************************************/
float hdc2010_readHighTempThreshold(void);		// Returns contents of high temperature threshold register (in C)

/**************************************************************************
* Function: triggerMeasurement() 
* Description: Function trigger measurement
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_triggerMeasurement(void);			// Triggers a manual temperature/humidity reading

/**************************************************************************
* Function: reset() 
* Description: Function to reset
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_reset(void); 						// Triggers a software reset

/**************************************************************************
* Function: enableInterrupt() 
* Description: Function to enable interrupt
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_enableInterrupt(void);				// Enables the interrupt/DRDY pin

/**************************************************************************
* Function: disableInterrupt() 
* Description: Function to disable interrupt
* Parameter: Mode
* Return: None
***************************************************************************/
void hdc2010_disableInterrupt(void); 			// Disables the interrupt/DRDY pin (High Z)

/**************************************************************************
* Function: readInterruptStatus() 
* Description: Function to read interrupt status
* Parameter: None
* Return: Status
***************************************************************************/
uint8_t hdc2010_readInterruptStatus(void); 		// Reads the status of the interrupt register

/**************************************************************************
* Function: clearMaxTemp() 
* Description: Function to clear the maximum temperature register
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_clearMaxTemp(void);				// Clears the Maximum temperature register

/**************************************************************************
* Function: clearMaxHumidity() 
* Description: Function to clear the maximum humidity register
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_clearMaxHumidity(void);			// Clears the Maximum humidity register

/**************************************************************************
* Function: readMaxTemp() 
* Description: Function to read the maximum temperature register
* Parameter: None
* Return: Temperature
***************************************************************************/
float hdc2010_readMaxTemp(void); 				// Reads the maximum temperature register

/**************************************************************************
* Function: readMaxHumidity() 
* Description: Function to read the maximum humidity register
* Parameter: None
* Return: None
***************************************************************************/
float hdc2010_readMaxHumidity(void);			// Reads the maximum humidity register

/**************************************************************************
* Function: enableThresholdInterrupt() 
* Description: Function to enable the interrupt pin for comfort zone operation
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_enableThresholdInterrupt(void);	// Enables high and low temperature/humidity interrupts

/**************************************************************************
* Function: disableThresholdInterrupt() 
* Description: Function to disable the interrupt pin for comfort zone operation
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_disableThresholdInterrupt(void);	// Disables high and low temperature/humidity interrupts

/**************************************************************************
* Function: enableDRDYInterrupt() 
* Description: Function to enable the interrupt pin for DRDY operation
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_enableDRDYInterrupt(void);			// Enables data ready interrupt

/**************************************************************************
* Function: disableDRDYInterrupt() 
* Description: Function to disable the interrupt pin for DRDY operation
* Parameter: None
* Return: None
***************************************************************************/
void hdc2010_disableDRDYInterrupt(void);		// Disables data ready interrupt

/**************************************************************************
* Function: setTempRes() 
* Description: Function set temperature resolution
* Parameter: Resolution
*	0 - 14 bit
*	1 - 11 bit
*	2 - 9 bit
*	default - 14 bit	
* Return: None
***************************************************************************/		
void hdc2010_setTempRes(int resolution);

/**************************************************************************
* Function: setHumidRes() 
* Description: Function set humidity resolution
* Parameter: Resolution
*	0 - 14 bit
*	1 - 11 bit
*	2 - 9 bit
*	default - 14 bit
* Return: None
***************************************************************************/
void hdc2010_setHumidRes(int resolution);

/**************************************************************************
* Function: setMeasurementMode() 
* Description: Function set measurement mode
* Parameter: Mode
*	0 - Temperature and Humidity
*	1 - Temperature only
*	2 - Humidity only
*	default - Temperature & Humidity
* Return: None
***************************************************************************/
void hdc2010_setMeasurementMode(int mode);

/**************************************************************************
* Function: setRate() 
* Description: Function to set rate
* Parameter: Rate
*	0 - Manual
*	1 - reading every 2 minutes
*	2 - reading every minute
*	3 - reading every ten seconds
*	4 - reading every 5 seconds
*	5 - reading every second
*	6 - reading at 2Hz
*	7 - reading at 5Hz
*	default - Manual
* Return: None
***************************************************************************/
void hdc2010_setRate(int rate);

/**************************************************************************
* Function: setInterruptPolarity() 
* Description: Function to set interrupt polarity
* Parameter: Polarity
*	0 - Active Low
*	1 - Active High
*	default - Active Low
* Return: None
***************************************************************************/
void hdc2010_setInterruptPolarity(int polarity);

/**************************************************************************
* Function: setInterruptMode() 
* Description: Function to set interrupt mode
* Parameter: Mode
*	0 - Level sensitive
*	1 - Comparator mode
*	default - Level sensitive
* Return: None
***************************************************************************/
void hdc2010_setInterruptMode(int polarity);

/**************************************************************************
* Function: setTemperatureOffset() 
* Description: Function to set temperature offset
* Parameter: Value
* Return: None
***************************************************************************/
void hdc2010_setTemperatureOffset(uint8_t value);

/**************************************************************************
* Function: openReg() 
* Description: Function to open a register
* Parameter: register
* Return: None
***************************************************************************/
void openReg(uint8_t reg); 	    			// Points to a given register

/**************************************************************************
* Function: readReg() 
* Description: Function to read data from register
* Parameter: reg: register
* Return: read data
***************************************************************************/
uint8_t readReg(uint8_t reg);				// Reads a given register, returns 1 byte

/**************************************************************************
* Function: writeReg() 
* Description: Function to write data into register
* Parameter: reg: register
*            data: writed data
* Return: None
***************************************************************************/
void writeReg(uint8_t reg, uint8_t data); 	// Writes a byte of data to one register

#ifdef __cplusplus
}
#endif

#endif
