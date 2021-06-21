#include "../inc/system_time.h"
#include "../inc/system.h"

volatile static uint32_t ticker = 0;

void system_time_init( void ) 
{  

}

void system_time_wait_ms( uint32_t time_in_ms ) 
{
	R_SYS_SoftwareDelay(time_in_ms, SYSTEM_DELAY_UNITS_MILLISECONDS);
}

void system_time_IncreaseTicker( void ) 
{ 
	ticker++; 
}

uint32_t system_time_GetTicker( void ) 
{ 
	return ticker; 
}
