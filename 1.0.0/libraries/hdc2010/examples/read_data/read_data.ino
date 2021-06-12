#include <wiring_i2c.h>
#include <hdc2010.h>

static hdc2010_i2c_handle_t i2c_handle;

static int32_t platform_i2c_transmit(uint8_t addr, const uint8_t *data, uint16_t len)
{
  i2c_transmit(ARDUINO_I2C0, addr, (uint8_t *)data, len);
  return 0;
}
static int32_t platform_i2c_receive(uint8_t addr, uint8_t *data, uint16_t len)
{
  i2c_receive(ARDUINO_I2C0, addr, data, len);
  return 0;
}

void setup() {
  i2c_initialize(ARDUINO_I2C0);
  i2c_handle.transmit = platform_i2c_transmit;
  i2c_handle.receive = platform_i2c_receive;
  hdc2010_initialize(&i2c_handle);

  hdc2010_reset();//Begin with a HDC2010(THS) reset
  // Configure Measurements
  hdc2010_setMeasurementMode(TEMP_AND_HUMID); // Set measurements to temperature and humidity
  hdc2010_setRate(ONE_HZ);                     // Set measurement frequency to 1 Hz
  hdc2010_setTempRes(_14_BITS);
  hdc2010_setHumidRes(_14_BITS);
  
}

void loop() {
  //begin HDC2010 sensor measuring
    hdc2010_triggerMeasurement();
    delay(1000);//delay for finish init HDC2010
    float temp = hdc2010_readTemp();
    float hum = hdc2010_readHumidity();
    delay(1000);

}
