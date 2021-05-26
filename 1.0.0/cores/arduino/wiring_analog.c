#include "Arduino.h"
#include "wiring_analog.h"

 

static int _readResolution = 10;
static int _writeResolution = 8;



void analogReadResolution(int res) {
	_readResolution = res;
}

void analogWriteResolution(int res) {
	_writeResolution = res;
}

void analogWritePhase(uint32_t pin, uint32_t phase)
{

}
      
void analogOutputInit( void )
{

}

void analogWrite(uint32_t pin, uint32_t ulValue)
{

 
}

uint32_t analogRead(uint32_t pin)
{
  return 0;
}

  
