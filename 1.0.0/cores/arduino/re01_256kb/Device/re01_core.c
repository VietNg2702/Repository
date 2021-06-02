
#include "re01_core.h"
#include "RE01_256KB.h"
#include "r_core_cfg.h"
#include "r_system_api.h"
#include "r_lpm_api.h"

/* Start user code for include. */
#include "r_smip_api.h"
#include "r_image.h"
#include "nop_func.h"

/* End user code.*/

void re01_core_init(void)
{
    /**** It recommends use this part as template this device's start-up code. ****/
    /* If the API functions are allocated to RAM, this function must be executed first */
    /* It copies Driver codes, which change the allocation area from ROM to RAM, by using r_[module]_cfg.h */ 
    /* Attention: Every code Pin.c is always allocated to RAM. These functions are called from API.  */
    R_SYS_CodeCopy();
    /* Initialize System Function Driver. */
    /* This function needs to be called after R_SYS_CodeCopy function. */
    R_SYS_Initialize();
    /* Initialize LPM Function Driver. */
    /* This function needs to be called before call R_LPM_IOPowerSupplyModeSet function. */
    R_LPM_Initialize();
    /* Set Power Supply Open Control Register (VOCR register)                                      */
    /* If you doesn't clear bits which corresponding your using port domains, MCU cannot read      */
    /* correctly value of input signal.                                                            */ 
    /* The VOCR register is used for control such that, when power is not being supplied to        */
    /* the AVCC0, IOVCC0,IOVCC1.                                                                  ,*/
    /* Thie feature is mainly usign when the device is Energy Harvesting start-up.                 */
    /* Default value is "NOT propagated" signal into internal device.                              */
    /* Please change the value of argument with your target board                                  */
    R_LPM_IOPowerSupplyModeSet(0xF2);
    /**** End of template code.   ****/
}


