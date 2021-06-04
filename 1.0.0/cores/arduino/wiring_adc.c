#include "wiring_adc.h"


#define PULSE_PRV_500MS      (0x4000-1)                    /* 500ms pulse cycle */
static volatile bool flag_done = false;

static void adc_callback(void)
{
    static uint16_t s_ad_data;
    flag_done = true;
    /* Get the value of an AD convert */
    //s_ad_data = ADC140->ADDR0;

}

static void adc_init(void)
{
	/* Lock ADC resource */
    if (0 != R_SYS_ResourceLock(SYSTEM_LOCK_S14AD))
    {
        /* If the result of the resource lock is an error, do not initialize */
        while(1);
    }
	 /* ELC is public module which is linked several events.
       Thus, normally ELC doesn't need to set a Module lock bit of its. */
    if (0 != R_SYS_ResourceLock(SYSTEM_LOCK_ELC))
    {
    /* If the result of the resource lock is an error, do not initialize */
       while(1);
    }

	/* Start module */
    R_LPM_ModuleStart(LPM_MSTP_S14AD);

    /* Start module ELC */
    R_LPM_ModuleStart(LPM_MSTP_ELC);

	/* ADCER   - A/D Control Extended Register
        b15     - ADRFMT - A/D Data Register Format Select - Select flush-right 
                                                             for the A/D data register format
        b2:b1   - ADPRC  - A/D Conversion Accuracy Select - A/D conversion is performed with 14-bit accuracy */
    ADC140->ADCER = 0x0006;

    /* ADCSR   - A/D Control Register
        b14:13  - ADCS - Scan Mode Select - Single-scan mode
        b12     - ADIE - Scan End Interrupt Enable - Disable ADC140_ADI interrupt
        b9      - TRGE - Trigger Start Enable - Disable A/D conversion to be started 
                                                by a synchronous or asynchronous trigger */
    ADC140->ADCSR = 0x0000;

    /* ADSTRGR - A/D Conversion Start Trigger Select Register
        b13:8   - TRSA[5:0] - A/D Conversion Start Trigger Select - ELC */
    ADC140->ADSTRGR = 0x3000;

    /* ADSSTRn - The ADSSTRn register sets the sampling time for analog input.(n = 0 ~ 6, L, T)
                  The relationship between the A/D sampling state register
                  and the associated channel is shown below.
                  ADSSTR0 - AN000 */
    ADC140->ADSSTR0 = 0x0F;

    /* Channel setting */
    ADC140->ADANSA0 |= 0x0001;

    /* Pin setting */
    R_S14AD_Pinset();                            /* AN028 : P506 */

    /* ADIE - Scan End Interrupt Enable - Enable ADC140_ADI interrupt */
    ADC140->ADCSR_b.ADIE = 1;

    /* ADC140_ADI - A/D scan end interrupt */
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_ADC140_ADI,0x08,adc_callback);
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_ADC140_ADI,3);
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_ADC140_ADI);
    R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_ADC140_ADI);
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_ADC140_ADI);

	/* TRGE - Trigger Start Enable - Enable A/D conversion to be started by a synchronous or asynchronous trigger */
    ADC140->ADCSR_b.TRGE = 1;

    /* ELSR8 - ELS - Event Link Select - Peripheral module is ADC14, Interrupt request source is AGT interrupt. */
    ELC->ELSR8_b.ELS = 0x11;
}

static void agt_init(void)
{
    /* Lock AGT0 resource */
    if (0 != R_SYS_ResourceLock(SYSTEM_LOCK_AGT0))
    {
        /* If the result of the resource lock is an error, do not initialize */
        while(1);
    }

    /* Start module */
    R_LPM_ModuleStart(LPM_MSTP_AGT0);

    /* AGTCR - AGT Control Register
    b7   - TCMBF - Compare Match B Flag - No match
    b6   - TCMAF - Compare Match A Flag - No match
    b5   - TUNDF - Underflow Flag - No underflow
    b4   - TEDGF - Active Edge Judgment Flag - No active edge received
    b3   - Reserved
    b2   - TSTOP - AGT Count Forced Stop - Invalid (writing 0 has no effect)
    b1   - TCSTF - AGT Count Status Flag - Counting is stopped
    b0   - TSTART - AGT Count Start - Stop counting */
    AGT0->AGTCR = 0x00;

    /* AGTMR1 - AGT Mode Register 1
    b7      - Reserved
    b6 : b4 - TCK    - Count Source - Divided clock AGTLCLK specified by CKS[2:0] bits in the AGTMR2 register
    b3      - TEDGPL - Edge Polarity - Single-edge
    b2 : b0 - TMOD   - Operating Mode - Timer mode */
    AGT0->AGTMR1 = 0x40;

    /* AGTMR2 - AGT Mode Register 2
    b7      - LPM - Low Power Consumption Mode Setting - Normal mode
    b6 : b3 - Reserved
    b2 : b0 - CKS - AGTSCLK/AGTLCLK Count Source Clock Frequency Division Ratio - 1/1 */
    AGT0->AGTMR2 = 0x00;

    /* AGTCMSR - AGT Compare Match Function Select Register
    b7   - Reserved
    b6   - TOPOLB - AGTOBn Polarity Select - Start AGTOBn output at low: Normal output
    b5   - TOEB - AGTOBn Output Enable - Disable AGTOBn output
    b4   - TCMEB - Compare Match B Register Enable - Disable the Compare Match B Register
    b3   - Reserved
    b2   - TOPOLA - AGTOAn Polarity Select - Start AGTOAn output at low: Normal output
    b1   - TOEA - AGTOAn Output Enable - Disable AGTOAn output
    b0   - TCMEA - Compare Match A Register Enable - Dsable the Compare Match A Register */
    AGT0->AGTCMSR = 0x00;

    /* AGT - 16-bit counter and reload register - 500ms pulse width */
    AGT0->AGT = PULSE_PRV_500MS;

    /* AGTCR - TSTART - AGT Count Start - Start counting */
    AGT0->AGTCR_b.TSTART = 1;

    /* Check the AGT Count Status Flag to start the timer */
    while (0 == AGT0->AGTCR_b.TCSTF);
        /* Waiting for count start */
        /* Intentionally empty braces. */

}   /* End of function agt_init() */

static void adc_calibration(void)
{
    static bool flag_init = false;

    if(flag_init) return;

    flag_init = true;
    /* ADC - Addition Count Select - 1-time conversion */
    ADC140->ADADC_b.ADC = 0;

    /* AVEE - Average Mode Enable - Enable */
    ADC140->ADADC_b.AVEE = 1;

    /* ADSSTRn - The ADSSTRn register sets the sampling time for analog input.(n = 0 ~ 6, L, T)
                 The relationship between the A/D sampling state register
                 and the associated channel is shown below.
                 ADSSTR0 - AN000 */
    ADC140->ADSSTR0_b.SST = 0xFF;

    /* CALST - Performing calibration - Perform calibration at next A / D conversion start. */
    ADC140->ADCALC_b.CALST = 1;

    /* ADST - A/D Conversion Start - Starts A/D conversion process. */
    ADC140->ADCSR_b.ADST   = 1;

    while (1 == ADC140->ADCSR_b.ADST)
    {

        /* wait calibration */
        __NOP();
    }

    /* AVEE - Average Mode Enable - Disable */
    ADC140->ADADC_b.AVEE = 0;

}   /* End of function adc_calibration() */

static void adc_close(void)
{
    volatile uint16_t dummy;

    /* ADSTRGR - A/D Conversion Start Trigger Select Register
        b13:8   - TRSA[5:0] - A/D Conversion Start Trigger Select - Trigger source deselected state */
    ADC140->ADSTRGR_b.TRSA = 0x3F;

    /* ADIE - Scan End Interrupt Enable - Disable ADC140_ADI interrupt */
    ADC140->ADCSR_b.ADIE = 0;

    /* ELSR8 - ELS - Event Link Select - Disable event output for the associated peripheral module */
    ELC->ELSR8_b.ELS = 0x00;

    /* ADST - A/D Conversion Start - Stops A/D conversion process. */
    ADC140->ADCSR_b.ADST = 0;

    /* wait 2ADCLK(Wait due to dummy read of A/D register) */
    dummy = ADC140->ADCSR;
    dummy = ADC140->ADCSR;

    /* ADCSR   - A/D Control Register */
    ADC140->ADCSR = 0x0000;

    /* ADANSA0   - A/D Channel Select Register A0 */
    ADC140->ADANSA0 = 0x0000;

    /* Clear pin setting */
    R_S14AD_Pinclr();

    /* Set ModuleStop */
    R_LPM_ModuleStop(LPM_MSTP_S14AD);

    /* Resource lock release */
    R_SYS_ResourceUnlock(SYSTEM_LOCK_S14AD);

    /* ELC is public module which is linked several events.
       Thus, normally ELC doesn't need to set a Module unlock bit of its. */
    /* Set ModuleStpp ELC */
//    R_LPM_ModuleStop(LPM_MSTP_ELC);
    /* Resource lock release */
//    R_SYS_ResourceUnlock(SYSTEM_LOCK_ELC);

}   /* End of function adc_close() */

static void agt_close(void)
{

    /* AGTCR - AGT Count Forced Stop - Forcibly stop counting */
    AGT0->AGTCR_b.TSTOP = 1;

    /* AGTCR - AGT Control Register
    b7   - TCMBF - Compare Match B Flag - No match
    b6   - TCMAF - Compare Match A Flag - No match
    b5   - TUNDF - Underflow Flag - No underflow
    b4   - TEDGF - Active Edge Judgment Flag - No active edge received
    b3   - Reserved
    b2   - TSTOP - AGT Count Forced Stop 
    b1   - TCSTF - AGT Count Status Flag 
    b0   - TSTART - AGT Count Start - Stop counting  */
    AGT0->AGTCR = 0x00;

    /* AGTMR1 - AGT Mode Register 1 - initialize
    b7     - Reserved
    b6:b4  - TCK    - Count Source - PCLKB
    b3     - TEDGPL - Edge Polarity - Single-edge
    b2:b0  - TMOD   - Operating Mode -Timer mode  */
    AGT0->AGTMR1 = 0x00;

    /* AGTMR2 - AGT Mode Register 2 - initialize
    b7     - LPM - Low Power Consumption Mode Setting - Normal mode
    b6:b3  - Reserved
    b2:b0  - CKS - AGTSCLK/AGTLCLK Count Source Clock Frequency Division Ratio */
    AGT0->AGTMR2 = 0x00;

    /* Clear pin setting */
    R_AGT_Pinclr_CH0();

    /* Set ModuleStop */
    R_LPM_ModuleStop(LPM_MSTP_AGT0);

    /* Resource lock release */
    R_SYS_ResourceUnlock(SYSTEM_LOCK_AGT0);

}   /* End of function agt_close() */


uint32_t adc_read(void)
{
    adc_init();
    agt_init();
    adc_calibration();
    flag_done = false;
    while(!flag_done);
    adc_close();
    agt_close();

    return ADC140->ADDR0;
}
