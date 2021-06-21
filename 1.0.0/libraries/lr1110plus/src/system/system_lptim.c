#include "../inc/system_lptim.h"
#include "../inc/system.h"

#define AGT1_TIMER_COUNT 32  // 1 msec interval
#define AGTW0_TIMER_COUNT 0xFFFFFFFF  // 1 msec interval

static uint32_t agt_global_count;



void agt1_callback(void)  __attribute__ ((section(".ramfunc")));
void agt1_set_value( uint16_t value )  __attribute__ ((section(".ramfunc")));
uint16_t agt1_get_value()  __attribute__ ((section(".ramfunc")));
void agt1_start(uint16_t ticks) __attribute__ ((section(".ramfunc")));
void agt1_stop(void) __attribute__ ((section(".ramfunc")));

void agtw0_callback(void)  __attribute__ ((section(".ramfunc")));
void agtw0_set_value( uint32_t value )  __attribute__ ((section(".ramfunc")));
uint32_t agtw0_get_value()  __attribute__ ((section(".ramfunc")));
void agtw0_start(uint32_t ticks) __attribute__ ((section(".ramfunc")));
void agtw0_stop(void) __attribute__ ((section(".ramfunc")));

void agt1_callback()
{
    /* Underflow flag clear */
    AGT1->AGTCR_b.TUNDF = 0;
	agt_global_count++;
}

void agtw0_callback()
{
    /* Underflow flag clear */
    AGTW0->AGTCR_b.TUNDF = 0;
//	agt_global_count++;
}

void agt1_init(  )
{

	agt_global_count = 0;
    /* Release AGT1 module stop condition  */
    R_LPM_ModuleStart(LPM_MSTP_AGT1);

    /* AGT1 Setting */
    /* Stop AGT1 count */
    AGT1->AGTCR_b.TSTOP     = 1;
    while (AGT1->AGTCR_b.TCSTF == 1) { ; }		// wait till it stopped counting

    AGT1->AGTMR1_b.TMOD     = 0x00;             /* Operation mode   : Timer mode */
    AGT1->AGTMR1_b.TEDGPL   = 0;                /* Edge polarity    : Single-edge */
    AGT1->AGTMR1_b.TCK      = 0x06;             /* Count source     : AGTSCLK */
    AGT1->AGTMR2_b.CKS      = 0x05;             /* AGTSCLK division ratio   : 1/32 -> 1msec */
//    AGT1->AGTMR2_b.CKS      = 0x00;             /* AGTSCLK division ratio   : 1/1 */
//    AGT1->AGTMR2_b.CKS      = 0x06;             /* AGTSCLK division ratio   : 1/64 */

    AGT1->AGT = AGT1_TIMER_COUNT;

    /* Connect AGT1_AGTI interrupt signal to NVIC a channel */
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTI, 0x06, agt1_callback);

    /* Clear AGT1_AGTI IR flag in ICU */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTI);
    
    /* Setup NVIC for AGT1_AGTI interrupt */
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTI);
    
//    /* Set AGT1 to module stop condition */
//    /* If the count source is the sub-clock oscillator or LOCO, 
//       module stop bit of AGT1 must be set to 1 except when accessing the AGT1 registers */
//    R_LPM_ModuleStop(LPM_MSTP_AGT1);

    return;

}

void agtw0_init(  )
{
	agt_global_count = 0;
    /* Release AGTW0 module stop condition  */
    R_LPM_ModuleStart(LPM_MSTP_AGTW0);

    /* AGTW0 Setting */
    /* Stop AGTW0 count */
    AGTW0->AGTCR_b.TSTOP     = 1;
    while (AGTW0->AGTCR_b.TCSTF == 1) { ; }		// wait till it stopped counting

    AGTW0->AGTMR1_b.TMOD     = 0x00;             /* Operation mode   : Timer mode */
    AGTW0->AGTMR1_b.TEDGPL   = 0;                /* Edge polarity    : Single-edge */
    AGTW0->AGTMR1_b.TCK      = 0x06;             /* Count source     : AGTSCLK */
    AGTW0->AGTMR2_b.CKS      = 0x05;             /* AGTSCLK division ratio   : 1/32 ->  imsec*/
//    AGTW0->AGTMR2_b.CKS      = 0x00;             /* AGTSCLK division ratio   : 1/1 */
//    AGTW0->AGTMR2_b.CKS      = 0x06;             /* AGTSCLK division ratio   : 1/64 */

//    AGTW0->AGT = AGTW0_TIMER_COUNT;
    AGTW0->AGT = AGTW0_TIMER_COUNT;
    AGTW0->AGT = AGTW0_TIMER_COUNT;

    /* Connect AGTW0_AGTI interrupt signal to NVIC a channel */
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_AGTW0_AGTI, 0x15, agtw0_callback);

    /* Clear AGTW0_AGTI IR flag in ICU */
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_AGTW0_AGTI);
    
    /* Setup NVIC for AGTW0_AGTI interrupt */
// Don't use AGTW0 interrupt
//    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_AGTW0_AGTI);
    
//    /* Set AGTW0 to module stop condition */
//    /* If the count source is the sub-clock oscillator or LOCO, 
//       module stop bit of AGTW0 must be set to 1 except when accessing the AGTW0 registers */
//    R_LPM_ModuleStop(LPM_MSTP_AGTW0);

    return;

}


void agt1_start(uint16_t ticks)
{

    /* Release AGT1 module stop condition  */
    R_LPM_ModuleStart(LPM_MSTP_AGT1);
    AGT1->AGT = ticks;

    /* Start AGT1 count */
    AGT1->AGTCR_b.TSTART    = 1;
    
    /* Wait until the count starts */
    while(0 == AGT1->AGTCR_b.TCSTF);

//    /* Enable the low power mode */
//    AGT1->AGTMR2_b.LPM      = 1;

#ifdef RE01_1500KB_H
    /* Set AGTW0 to module stop condition */
    /* If the count source is the sub-clock oscillator or LOCO, 
       module stop bit of AGTW0 must be set to 1 except when accessing the AGTW0 registers	*/
           R_LPM_ModuleStop(LPM_MSTP_AGTW1);
#endif
    return;
}

void agtw0_start(uint32_t ticks)
{
    /* Release AGTW0 module stop condition  */
    R_LPM_ModuleStart(LPM_MSTP_AGTW0);

    AGTW0->AGT = ticks;

    /* Start AGTW0 count */
    AGTW0->AGTCR_b.TSTART    = 1;
    
    /* Wait until the count starts */
    while(0 == AGTW0->AGTCR_b.TCSTF);

//    /* Enable the low power mode */
//    AGTW0->AGTMR2_b.LPM      = 1;
    
#ifdef RE01_1500KB_H
    /* Set AGTW0 to module stop condition */
    /* If the count source is the sub-clock oscillator or LOCO, 
       module stop bit of AGTW0 must be set to 1 except when accessing the AGTW0 registers	*/
           R_LPM_ModuleStop(LPM_MSTP_AGTW0);
#endif
    return;
}


void agt1_stop()
{
    /* Release AGT1 module stop condition  */
    R_LPM_ModuleStart(LPM_MSTP_AGT1);

    /* Disable the low power mode */
    AGT1->AGTMR2_b.LPM       = 0;
    
    /* Stop AGT1 count */
    AGT1->AGTCR_b.TSTART     = 0;
    
    /* Wait until the count stops */ //MN added
    while(1 == AGT1->AGTCR_b.TCSTF);

    /* Set AGT1 to module stop condition */
    /* If the count source is the sub-clock oscillator or LOCO, 
       module stop bit of AGT1 must be set to 1 except when accessing the AGT1 registers */
    R_LPM_ModuleStop(LPM_MSTP_AGT1);

    return;
}

void agtw0_stop()
{
    /* Release AGTW0 module stop condition  */
    R_LPM_ModuleStart(LPM_MSTP_AGTW0);

    /* Disable the low power mode */
    AGTW0->AGTMR2_b.LPM       = 0;
    
    /* Stop AGTW0 count */
    AGTW0->AGTCR_b.TSTART     = 0;
    
    /* Wait until the count stops */ //MN added
    while(1 == AGTW0->AGTCR_b.TCSTF);

    /* Set AGTW0 to module stop condition */
    /* If the count source is the sub-clock oscillator or LOCO, 
       module stop bit of AGTW0 must be set to 1 except when accessing the AGTW0 registers */
    R_LPM_ModuleStop(LPM_MSTP_AGTW0);

    return;
}

void agt1_set_value( uint16_t value )
{
    AGT1->AGT = value & 0xFFFF;
}

void agtw0_set_value( uint32_t value )
{
    AGTW0->AGT = value;
}

uint16_t agt1_get_value( )
{
	uint16_t value16;
    value16 = AGT1->AGT;
	return value16;
}

uint32_t agtw0_get_value( )
{
	uint32_t value32;
    value32 = AGTW0->AGT;
	return value32;
}



void system_lptim_init( )
{
	agtw0_init();
}

void system_lptim_set_and_run( uint32_t ticks )
{
	agtw0_init();
	agtw0_start(ticks);
}

void system_lptim_stop( )
{
	agtw0_stop();
}

uint32_t system_lptim_get( )
{
	return (0xFFFFFFFF - agtw0_get_value());
}

void system_int_timer_init( )
{
	agt1_init();
}


void system_int_timer_set_and_run( uint16_t ticks )
{
	agt1_init();
	agt1_start(ticks);
}


void system_int_timer_stop( )
{
	agt1_stop();
}

uint16_t system_int_timer_get( )
{
	return (0xFFFF - agt1_get_value());
}

