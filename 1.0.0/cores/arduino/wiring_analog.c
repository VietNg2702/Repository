#include "Arduino.h"
#include "wiring_analog.h"

uint32_t analogRead( uint32_t ulPin )
{
    return adc_read();
}