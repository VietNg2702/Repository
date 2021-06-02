/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  This code has been modified by SHCVN, Inc, in order to support
  the RE01 development boards.
*/
#define ARDUINO_MAIN
#include "Arduino.h"
#include "re01_core.h"
/***********************************************************************************************************************
* Function Name: main
* Revision     : 1.00
* Description  : main function. Please add the code for your system.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
int main()
{
  
    re01_core_init();
/*******************************************************************/
/**** Write user code for user init and system operations here. ****/
/*******************************************************************/

    setup();
    while(1)
    {
        loop();
		if (serialEventRun) serialEventRun();
    }

  return 0;
}

/* End of File */
