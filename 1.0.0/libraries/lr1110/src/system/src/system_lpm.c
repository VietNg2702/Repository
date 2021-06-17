#include "../inc/system_lpm.h"
#include "../inc/system.h"

/**************************************************************************//**
* @brief Setup the MCU low power mode
*
* @retval       
******************************************************************************/

static st_lpm_sstby_cfg_t sstby_cfg;

#ifdef RE01_256KB_H
// MINPWON
#if (TX_BOARD_VERSION == 1)
void system_lpm_setup_rtc(void)
{
    int err = 0;
//    sstby_cfg.power_supply = LPM_OPE_TO_SSTBY_EXFPWON_VBB;         /* Transit from ALLPWON OPE to EXFPWON SSTBY VBB */
    sstby_cfg.power_supply = LPM_OPE_TO_SSTBY_MINPWON_VBB;         /* Transit from ALLPWON OPE to MINPWON SSTBY VBB */
    sstby_cfg.speed = LPM_SSTBY_SPEED_MODE_OFF;             /* Transit time is not shortened */
    sstby_cfg.wup   = LPM_SSTBY_WUP_IRQ7  | LPM_SSTBY_WUP_RTCALM ;  /* Wakeup interrupt : PORT_IRQ3 PORT_IRQ7  RTC_Alarm*/
    err = R_LPM_SSTBYModeSetup(&sstby_cfg);
    APP_ERR_HANDLER(err);
}
#elif (TX_BOARD_VERSION == 2)
void system_lpm_setup_rtc(void)
{
    int err = 0;
//    sstby_cfg.power_supply = LPM_OPE_TO_SSTBY_EXFPWON_VBB;         /* Transit from ALLPWON OPE to EXFPWON SSTBY VBB */
    sstby_cfg.power_supply = LPM_OPE_TO_SSTBY_MINPWON_VBB;         /* Transit from ALLPWON OPE to MINPWON SSTBY VBB */
    sstby_cfg.speed = LPM_SSTBY_SPEED_MODE_OFF;             /* Transit time is not shortened */
    sstby_cfg.wup   = LPM_SSTBY_WUP_IRQ9    | LPM_SSTBY_WUP_RTCALM ;  /* Wakeup interrupt : PORT_IRQ3 PORT_IRQ7  RTC_Alarm*/
    err = R_LPM_SSTBYModeSetup(&sstby_cfg);
    APP_ERR_HANDLER(err);
}
#endif
// EXFPWON
void system_lpm_setup_agt(void)
{
    int err = 0;
    sstby_cfg.power_supply = LPM_OPE_TO_SSTBY_EXFPWON_VBB;         /* Transit from ALLPWON OPE to EXFPWON SSTBY VBB */
//    sstby_cfg.power_supply = LPM_OPE_TO_SSTBY_MINPWON_VBB;         /* Transit from ALLPWON OPE to MINPWON SSTBY VBB */
    sstby_cfg.speed = LPM_SSTBY_SPEED_MODE_OFF;             /* Transit time is not shortened */
    sstby_cfg.wup   = LPM_SSTBY_WUP_IRQ3 | LPM_SSTBY_WUP_AGT1UD;  /* Wakeup interrupt : PORT_IRQ3 PORT_IRQ7  RTC_Alarm*/
    err = R_LPM_SSTBYModeSetup(&sstby_cfg);
    APP_ERR_HANDLER(err);
}

#else
void system_lpm_setup(void)
{
    int err = 0;
    /* set RAM retention area at SSTBY (MN added) */
    //R_LPM_RamRetentionSet(LPM_RAM_RETENTION_AREA0);

    /* SSTBY mode set up */
    sstby_cfg.ope_sstby = LPM_OPE_TO_SSTBY_EXFPWON_TO_EXFPWON;  /* Transit from EXPWN OPE to MINPWON SSTBY */
    sstby_cfg.sstby_ope = LPM_SSTBY_TO_OPE_EXFPWON_TO_EXFPWON;  /* Transit from EXFPWON SSTBY to EXPWON OPE */
//    sstby_cfg.ope_sstby = LPM_OPE_TO_SSTBY_MINPWON_TO_MINPWON;  /* Transit from EXPWN OPE to MINPWON SSTBY */
//    sstby_cfg.sstby_ope = LPM_SSTBY_TO_OPE_MINPWON_TO_MINPWON;  /* Transit from EXFPWON SSTBY to EXPWON OPE */
    sstby_cfg.wup       = LPM_SSTBY_WUP_AGT1UD|LPM_SSTBY_WUP_IRQ8; /* Wakeup interrupt : PORT_IRQ8, AGT1 Underflow */
    err = R_LPM_SSTBYModeSetup(&sstby_cfg);
    APP_ERR_HANDLER(err);
}
#endif

#ifdef RE01_256KB_H
void system_lpm_prepare(void)
{
#if (TRACKER_TX == 1)
#else
        /* Set divider value for system clock and peripheral module clock */
        /* When transitioning from MINPWON to ALLPWON, the clock must be set to 4 MHz or less */
        /* before executing the WFE instruction due to limitation */
    int err = 0;
        err = R_SYS_SystemClockDividerSet(SYSTEM_CLOCK_DIV_8, SYSTEM_CLOCK_DIV_8);
        APP_ERR_HANDLER(err);
#endif
}

#else
void system_lpm_prepare_1500KB(void)
{
    int err = 0;

    /* Select Sub-osc clock as the system clock source */
    err = R_SYS_SystemClockSOSCSet();  // MN changed
//    err = R_SYS_SystemClockLOCOSet();
    APP_ERR_HANDLER(err);

    /* Stop HOCO oscillation */
    err = R_SYS_HighSpeedClockStop();
    APP_ERR_HANDLER(err);
    while(0x01 == (0x01 & R_SYS_OscStabilizationFlagGet()));

    /* Transit to SOSC-speed mode */
    err = R_SYS_32kHzSpeedModeSet();
    APP_ERR_HANDLER(err);

    /* Transit from NORMAL mode to VBB mode */
    err = R_LPM_BackBiasModeEntry();
    APP_ERR_HANDLER(err);
}
#endif
/**************************************************************************//**
* @brief Emter the MCU low power mode  
*
* @retval       
******************************************************************************/
//__attribute__ ((section(".ramfunc"))) void system_lpm_enter(void)

#ifdef RE01_256KB_H
void system_lpm_enter(void)
{
    int err = 0;
        /* Entry EXFPWON SSTBY VBB mode */
    err = R_LPM_SSTBYModeEntry();
    APP_ERR_HANDLER(err);
}

#else
void system_lpm_enter_1500KB(void)
{
    int err = 0;
    st_lpm_sstby_cfg_t sstby_cfg;

    /* Transit Power supply mode to EXFPWON to power off Flash power domain(ISO3) */
    err = R_LPM_PowerSupplyModeExfpwonSet();
    APP_ERR_HANDLER(err);

//    /* Transit Power supply mode to MINPWON to (Power off Peripheral power domain(ISO2)) */
//    err = R_LPM_PowerSupplyModeMinpwonSet();
//    APP_ERR_HANDLER(err);
    

    /* Entry SSTBY mode */
    R_LPM_SSTBYModeEntry();
}
#endif
/**************************************************************************//**
* @brief Exit the MCU low power mode      
*
* @retval       
******************************************************************************/
//__attribute__ ((section(".ramfunc"))) void system_lpm_exit(void)

#ifdef RE01_256KB_H
void system_lpm_exit(void)
{
#if (TRACKER_TX == 1)
#else
    int err = 0;
    /* Set divider value for system clock and peripheral module clock */
    /* Since the clock was set to 4MHz when transitioning from MINPWON to ALLPWON, */
    /* the clock setting was changed back to 32MHz */
    err = R_SYS_SystemClockDividerSet(SYSTEM_CLOCK_DIV_1, SYSTEM_CLOCK_DIV_1);
    APP_ERR_HANDLER(err);
#endif
}
#else
void system_lpm_exit_1500KB(void)
{  
    int err = 0;

    /* Transit Power supply mode to EXFPWON to power off Flash power domain(ISO3) */
//    err = R_LPM_PowerSupplyModeExfpwonSet();
//    APP_ERR_HANDLER(err);

    /* Transit Power supply mode to ALLPWON before starting code fetch from ROM (Power on Flash power domain(ISO3)) */
    err = R_LPM_PowerSupplyModeAllpwonSet();
    APP_ERR_HANDLER(err);

    /* Transit from VBB mode to NORMAL mode */
    R_LPM_BackBiasModeExit();
  
    /* Set the high-speed mode */
    err = R_SYS_HighSpeedModeSet();
    APP_ERR_HANDLER(err);

    /* Start HOCO oscillation */
    R_SYS_HighSpeedClockStart();
    while(0x00 == (0x01 & R_SYS_OscStabilizationFlagGet()));
  
    /* Change the system clock to HOCO */
    err = R_SYS_SystemClockHOCOSet();
    APP_ERR_HANDLER(err);
}
#endif



void system_lpm_wait_agt(void)
{
	system_lpm_prepare();
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM);
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ7);
	system_lpm_enter();
	system_lpm_exit();
}

void system_lpm_wait_rtc(void)
{
	system_lpm_prepare();
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_AGT1_AGTI);
        R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_PORT_IRQ3);
	system_lpm_enter();
	system_lpm_exit();
}

