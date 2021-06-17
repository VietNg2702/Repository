#include "../inc/system_rtc.h"
#include "../inc/system.h"

int rtc_bcd_to_dec (uint8_t to_convert);
uint8_t rtc_dec_to_bcd (uint8_t to_convert);

void rtc_prd_isr(void)  __attribute__ ((section(".ramfunc"))) ;
void rtc_alm_isr(void)  __attribute__ ((section(".ramfunc"))) ;

//static RTC_Event_t rtc_callback;
static bool is_rtc_initialized;


//void rtc_init (RTC_Event_t event_cb)
void rtc_init ()
{
  int i;

  R_LPM_ModuleStart(LPM_MSTP_RTC);

#ifdef RE01_256KB_H
  RTC->RCR4_b.RSKSTP = 1;
  RTC->RCR4_b.R32KMD = 0; // Calender / Binary
#else
  RTC->RCR4_b.RCKSEL &= ~(1UL);
#endif
  // Supply 6 cycles of the clock selected, for setup. 
  for (i = 0; i < 6; i++)
  {
    asm("nop");
  }
  
  RTC->RCR2_b.START &= ~(1UL);
  while(RTC->RCR2_b.START)
  {
    asm("nop");
  }
  
  RTC->RCR2_b.RESET |= 1UL;
  while(RTC->RCR2_b.RESET)
  {   
    asm("nop");
  }
  
  while (0x00u != RTC->RCR1)
  {
    /* Disable RTC interrupts */
    RTC->RCR1 = 0x00u;
  }
  
  /* Stop RTC counter */
  rtc_counter_run(RTC_COUNTER_STOP);
  
  /* Confirm that it has changed */
#ifdef RTC_CALENDER
  while (0 != RTC->RCR2_b.CNTMD)
  {
    /* Set RTC to calendar mode */
    RTC->RCR2_b.CNTMD = 0;
  }
#else
  while (1 != RTC->RCR2_b.CNTMD)
  {
	  /* Set RTC to binary mode */
	  RTC->RCR2_b.CNTMD = 1;
  }


#endif
  
  /* Clear alarms, capture, adjustment registers, and output enable */
  rtc_reset();
  
//  rtc_callback = event_cb;
  
  is_rtc_initialized = true;
}


void rtc_set_periodic (rtc_periodic_t freq, uint8_t priority)
{
  uint8_t tmp;
  
  if (is_rtc_initialized == false)
    return;
  
  /* NOTE: arguments validated before entering this routine */
  
  /* Set frequency */
  /* Note: Off can be any one of several values. It may not match the "off" value written. */
  tmp = RTC->RCR1_b.PES;
  if (RTC->RCR1_b.PES != freq)           // if setting needs to change
  {
    RTC->RCR1_b.PES = freq;            // write the setting
    while (RTC->RCR1_b.PES == tmp)     // loop while setting has not changed
    {
      /* Confirm that it has changed */
      asm("nop");
    }
  }
  
  
  /* Set interrupts */
  if (RTC_PERIODIC_OFF == freq)
  {
//    ICU->IELEN_b.RTCPRDEN = 0;
  }
  else
  {
    R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_RTC_PRD, 0x05, (system_int_cb_t)rtc_prd_isr);
    
    R_SYS_IrqStatusClear(SYSTEM_CFG_EVENT_NUMBER_RTC_PRD);
    
    R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_RTC_PRD, priority);
    R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_RTC_PRD);
    
//    ICU->IELEN_b.RTCPRDEN = 1;
    
    RTC->RCR1_b.PIE = 1;
  }
  
  return;
}

void rtc_set_current_time (tm_t * p_current)
{
  uint8_t clock_state;
  volatile uint8_t dummy_byte;
  volatile uint16_t dummy_word;
  volatile uint8_t i;
  
  if (is_rtc_initialized == false)
    return;
  
  /* Note the clock state */
  clock_state = RTC->RCR2_b.START;
  
  /* Stop RTC counter */
  rtc_counter_run(RTC_COUNTER_STOP);
  
  /* Set for 24-hour mode. */
  RTC->RCR2_b.HR24 = 1;
  
  /* Set time */
  /* Set seconds. (0-59) */
  RTC->RSECCNT = rtc_dec_to_bcd((uint8_t) p_current->tm_sec);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RSECCNT;
  }
  
  /* Set minutes (0-59) */
  RTC->RMINCNT = rtc_dec_to_bcd((uint8_t) p_current->tm_min);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RMINCNT;
  }
  
  /* Set hours. (0-23) */
  RTC->RHRCNT = rtc_dec_to_bcd((uint8_t) p_current->tm_hour);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RHRCNT;
  }
  
  /* Set the date */
  /* Day of the week (0-6, 0=Sunday) */
  RTC->RWKCNT = rtc_dec_to_bcd((uint8_t) p_current->tm_wday);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RWKCNT;
  }
  
  /* Day of the month (1-31) */
  RTC->RDAYCNT = rtc_dec_to_bcd((uint8_t) p_current->tm_mday);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RDAYCNT;
  }
  
  /* Month. (1-12, 1=January) */
  RTC->RMONCNT = rtc_dec_to_bcd((uint8_t) (p_current->tm_mon + 1));
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RMONCNT;
  }
  
  /* Year. (00-99) */
  RTC->RYRCNT = (uint16_t) (rtc_dec_to_bcd((uint8_t) ((p_current->tm_year + 1900) % 100)));
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_word = RTC->RYRCNT;
  }
  
  /* Restore the clock */
  rtc_counter_run(clock_state);
  
  return;
}

void rtc_set_alarm_time (tm_t *p_alarm)
{
  volatile uint8_t dummy_byte;
  volatile uint16_t dummy_word;
  volatile uint8_t i;
  
  if (is_rtc_initialized == false)
    return;
  
  /* Set time */
  /* Set seconds. (0-59) */
  RTC->RSECAR &= 0x80u;
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RSECAR;
  }
  
  RTC->RSECAR |= rtc_dec_to_bcd((uint8_t) p_alarm->tm_sec);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RSECAR;
  }
  
  /* Set minutes (0-59) */
  RTC->RMINAR &= 0x80u;
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RMINAR;
  }
  
  RTC->RMINAR |= rtc_dec_to_bcd((uint8_t) p_alarm->tm_min);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RMINAR;
  }
  
  /* Set hours. (0-23) */
  RTC->RHRAR &= 0x80u;
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RHRAR;
  }
  
  RTC->RHRAR |= rtc_dec_to_bcd((uint8_t) p_alarm->tm_hour);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RHRAR;
  }
  
  /* Set the date */
  /* Day of the week (0-6, 0=Sunday) */
  RTC->RWKAR &= 0x80u;
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RWKAR;
  }
  
  RTC->RWKAR |= rtc_dec_to_bcd((uint8_t) p_alarm->tm_wday);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RWKAR;
  }
  
  /* Day of the month (1-31) */
  RTC->RDAYAR &= 0x80u;
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RDAYAR;
  }
  
  RTC->RDAYAR |= rtc_dec_to_bcd((uint8_t) p_alarm->tm_mday);
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RDAYAR;
  }
  
  /* Month. (1-12, 1=January) */
  RTC->RMONAR &= 0x80u;
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RMONAR;
  }
  
  RTC->RMONAR |= rtc_dec_to_bcd((uint8_t) (p_alarm->tm_mon + 1));
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_byte = RTC->RMONAR;
  }
  
  /* Year. (00-99) */
  RTC->RYRAR = (uint16_t) (rtc_dec_to_bcd((uint8_t) ((p_alarm->tm_year + 1900) % 100)));
  for (i = 0; i < RTC_DUMMY_READ; i++)
  {
    dummy_word = RTC->RYRAR;
  }
  
//  ICU->IELEN_b.RTCALMEN = state;
  return;
}



void rtc_enable_alarms (rtc_alarm_ctrl_t *p_alm_ctrl)
{
  if (is_rtc_initialized == false)
    return;
  
//  ICU->IELEN_b.RTCALMEN = 0;
  
  /* Alarm time enable setting */
  RTC->RSECAR_b.ENB  = (uint8_t)((true == p_alm_ctrl->sec) ? 1 : 0);
  RTC->RMINAR_b.ENB  = (uint8_t)((true == p_alm_ctrl->min) ? 1 : 0);
  RTC->RHRAR_b.ENB   = (uint8_t)((true == p_alm_ctrl->hour) ? 1 : 0);
  RTC->RDAYAR_b.ENB  = (uint8_t)((true == p_alm_ctrl->mday) ? 1 : 0);
  RTC->RMONAR_b.ENB  = (uint8_t)((true == p_alm_ctrl->mon) ? 1 : 0);
  RTC->RYRAREN_b.ENB = (uint8_t)((true == p_alm_ctrl->year) ? 1 : 0);
  RTC->RWKAR_b.ENB   = (uint8_t)((true == p_alm_ctrl->wday) ? 1 : 0);
  
  if (1 == RTC->RWKAR_b.ENB) // dummy read for waiting until set the value of RTC
  {
    asm("nop");
  }
  
  /* Alarm time setting definite waiting */
  R_SYS_SoftwareDelay((uint32_t)16, (e_system_delay_units_t)SYSTEM_DELAY_UNITS_MILLISECONDS);  //Approx.16ms (1/64Hz = 15.625ms)
  
  R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM, 0x06, (system_int_cb_t)rtc_alm_isr);
  
  R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM, p_alm_ctrl->int_priority);
  R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM);
  
//  ICU->IELEN_b.RTCALMEN = 1;
  
  RTC->RCR1_b.AIE = 1;
}

void rtc_disable_alarms(void)
{
  R_NVIC_DisableIRQ(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM);
  
  R_NVIC_ClearPendingIRQ(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM);
  
//  ICU->IELEN_b.RTCALMEN = 0;
  
  RTC->RCR1_b.AIE = 0;
}

void rtc_read_current (tm_t *p_current)
{
  
  uint16_t bcd_years; // Used for converting year.
  
  if (is_rtc_initialized == false)
    return;
  
  //do
  {
    /* Clear carry flag in ICU */
    //ICU.IR[IR_RTC_CUP].BIT.IR = 0;
    
    /* Read and convert RTC registers; mask off unknown bits and hour am/pm. */
    /* Seconds. (0-59) */
    p_current->tm_sec  = rtc_bcd_to_dec((uint8_t) (RTC->RSECCNT & 0x7fu));
    
    /* Minutes. (0-59) */
    p_current->tm_min  = rtc_bcd_to_dec((uint8_t) (RTC->RMINCNT & 0x7fu));
    
    /* Hours. (0-23) */
    p_current->tm_hour = rtc_bcd_to_dec((uint8_t) (RTC->RHRCNT & 0x3fu));
    
    /* Day of the month (1-31) */
    p_current->tm_mday = rtc_bcd_to_dec(RTC->RDAYCNT);
    
    /* Months since January (0-11) */
    p_current->tm_mon  = rtc_bcd_to_dec(RTC->RMONCNT) - 1;
    
    /* Years since 2000 */
    bcd_years = (uint16_t) RTC->RYRCNT;
    
    /* years years since 1900 (100-199) */
    p_current->tm_year = rtc_bcd_to_dec((uint8_t) (bcd_years & 0xFF)) + 100;
    
    /* Days since Sunday (0-6) */
    p_current->tm_wday = (int) (RTC->RWKCNT & 0x07u);
    
  } //while (1 == ICU.IR[IR_RTC_CUP].BIT.IR); //Reread if carry occurs during read
  
  return;
}

void rtc_read_alarm (tm_t *p_alarm)
{
  /* Used for converting year. */
  uint16_t bcd_years;
  
  if (is_rtc_initialized == false)
    return;
  
  /* Clear flag in ICU */
  //ICU.IR[IR_RTC_CUP].BIT.IR = 0;
  
  /* Read and convert RTC registers; mask off unknown bits and hour am/pm. */
  /* Seconds. (0-59) */
  p_alarm->tm_sec  = rtc_bcd_to_dec((uint8_t) (RTC->RSECAR & 0x7fu));
  
  /* Minutes. (0-59) */
  p_alarm->tm_min  = rtc_bcd_to_dec((uint8_t) (RTC->RMINAR & 0x7fu));
  
  /* Hours. (0-23) */
  p_alarm->tm_hour = rtc_bcd_to_dec((uint8_t) (RTC->RHRAR & 0x3fu));
  
  /* Day of the month (1-31) */
  p_alarm->tm_mday = rtc_bcd_to_dec((uint8_t) (RTC->RDAYAR & 0x3fu));
  
  /* Months since January (0-11) */
  p_alarm->tm_mon  = rtc_bcd_to_dec((uint8_t) (RTC->RMONAR & 0x1fu)) - 1;
  
  /* Years since 2000 (100-199)*/
  bcd_years = (uint16_t) RTC->RYRAR;
  
  /* RTC only supports years 0-99; years since 1900 */
  p_alarm->tm_year = rtc_bcd_to_dec((uint8_t) (bcd_years & 0xFF)) + 100;
  
  /* Days since Sunday (0-6) */
  p_alarm->tm_wday = (int) (RTC->RWKAR & 0x07u);
  return;
}

void rtc_counter_run (const uint8_t action)
{ 
  /* START bit is updated in synchronization with the next count source. */
  while (RTC->RCR2_b.START != action)
  {
    RTC->RCR2_b.START = action;
  }
}

void rtc_reset (void)
{
  RTC->RCR2_b.RESET |= 1UL;
  while(RTC->RCR2_b.RESET)
  {   
    asm("nop");
  }
}

void rtc_enable_ints (void)
{
  /* Enable RTC interrupts (PIE, CIE and AIE), not ICU yet */
    RTC->RCR1 = RTC_INT_ENABLE;
    while (RTC_INT_ENABLE != RTC->RCR1)
    {
        /* Confirm that it has changed */
        asm("nop");
    }
}

void rtc_disable_ints (void)
{
  
}


/***********************************************************************************************************************
* Function Name: Binary Counter
***********************************************************************************************************************/

void rtc_set_current_binary_time (uint32_t current_time)
{
  uint8_t clock_state;
  volatile uint8_t dummy_byte;
  volatile uint16_t dummy_word;
//  volatile uint8_t i;
  
  if (is_rtc_initialized == false)
    return;
  
  /* Note the clock state */
  clock_state = RTC->RCR2_b.START;
  
  /* Stop RTC counter */
  rtc_counter_run(RTC_COUNTER_STOP);
  
  RTC->BCNT0 = (uint8_t)(current_time & 0xFF);
  current_time = current_time >> 8;
  RTC->BCNT1 = (uint8_t)(current_time & 0xFF);
  current_time = current_time >> 8;
  RTC->BCNT2 = (uint8_t)(current_time & 0xFF);
  current_time = current_time >> 8;
  RTC->BCNT3 = (uint8_t)(current_time & 0xFF);
  
  /* Restore the clock */
  rtc_counter_run(clock_state);
  
  return;
}


uint32_t rtc_read_current_binary_time()
{
  uint32_t current_time;
  
  if (is_rtc_initialized == false)
    return 0;
  

  current_time = RTC->BCNT3;
  current_time = current_time << 8;
  current_time |= RTC->BCNT2;
  current_time = current_time << 8;
  current_time |= RTC->BCNT1;
  current_time = current_time << 8;
  current_time |= RTC->BCNT0;
  
  return current_time;
}



void rtc_set_alarm_binary_time (uint32_t alarm_time)
{
//  volatile uint8_t dummy_byte;
//  volatile uint16_t dummy_word;
//  volatile uint8_t i;
  
  if (is_rtc_initialized == false)
    return;
  
  RTC->BCNT0AR = (uint8_t)(alarm_time & 0xFF);
  alarm_time = alarm_time >> 8;
  RTC->BCNT1AR = (uint8_t)(alarm_time & 0xFF);
  alarm_time = alarm_time >> 8;
  RTC->BCNT2AR = (uint8_t)(alarm_time & 0xFF);
  alarm_time = alarm_time >> 8;
  RTC->BCNT3AR = (uint8_t)(alarm_time & 0xFF);

  
//  ICU->IELEN_b.RTCALMEN = state;
  return;
}

//void rtc_enable_alarms_binary (rtc_alarm_ctrl_t *p_alm_ctrl)
void rtc_enable_alarms_binary (uint32_t alarm_mask)
{
  if (is_rtc_initialized == false)
    return;
  
//  ICU->IELEN_b.RTCALMEN = 0;
  
  /* Alarm time enable setting */
//  RTC->RSECAR_b.ENB  = (uint8_t)((true == p_alm_ctrl->sec) ? 1 : 0);
//  RTC->RMINAR_b.ENB  = (uint8_t)((true == p_alm_ctrl->min) ? 1 : 0);
//  RTC->RHRAR_b.ENB   = (uint8_t)((true == p_alm_ctrl->hour) ? 1 : 0);
//  RTC->RDAYAR_b.ENB  = (uint8_t)((true == p_alm_ctrl->mday) ? 1 : 0);
//  RTC->RMONAR_b.ENB  = (uint8_t)((true == p_alm_ctrl->mon) ? 1 : 0);
//  RTC->RYRAREN_b.ENB = (uint8_t)((true == p_alm_ctrl->year) ? 1 : 0);
//  RTC->RWKAR_b.ENB   = (uint8_t)((true == p_alm_ctrl->wday) ? 1 : 0);
  
  RTC->BCNT0AER = (uint8_t)alarm_mask & 0xFF;
  alarm_mask = alarm_mask >> 8;
  RTC->BCNT1AER = (uint8_t)alarm_mask & 0xFF;
  alarm_mask = alarm_mask >> 8;
  RTC->BCNT2AER = (uint16_t)alarm_mask & 0x00FF;
  alarm_mask = alarm_mask >> 8;
  RTC->BCNT3AER = (uint8_t)alarm_mask & 0x00FF;
  
  if (0xFF == (RTC->BCNT3AER & 0xFF)) // dummy read for waiting until set the value of RTC
  {
    asm("nop");
  }
  
  /* Alarm time setting definite waiting */
  R_SYS_SoftwareDelay((uint32_t)16, (e_system_delay_units_t)SYSTEM_DELAY_UNITS_MILLISECONDS);  //Approx.16ms (1/64Hz = 15.625ms)
  
  R_SYS_IrqEventLinkSet(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM, 0x06, (system_int_cb_t)rtc_alm_isr);
  
//  R_NVIC_SetPriority(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM, p_alm_ctrl->int_priority);
  R_NVIC_EnableIRQ(SYSTEM_CFG_EVENT_NUMBER_RTC_ALM);
  
//  ICU->IELEN_b.RTCALMEN = 1;
  
  RTC->RCR1_b.AIE = 1;
}




/***********************************************************************************************************************
* Function Name: rtc_bcd_to_dec
* Description  : Converts from binary coded decimal (BCD) to decimal
* Arguments    : to_convert -
*                    Value to convert.
* Return Value : Converted value.
***********************************************************************************************************************/
int rtc_bcd_to_dec (uint8_t to_convert)
{
  return (int) ((((to_convert & 0xF0) >> 4) * 10) + (to_convert & 0x0F));
}
/**********************************************************************************************************************
End of function rtc_bcd_to_dec
***********************************************************************************************************************/


/***********************************************************************************************************************
* Function Name: rtc_dec_to_bcd
* Description  : Converts from decimal to binary coded decimal (BCD)
* Arguments    : to_convert -
*                    Value to convert.
* Return Value : Converted value.
***********************************************************************************************************************/
uint8_t rtc_dec_to_bcd (uint8_t to_convert)
{
  return (uint8_t) ((((to_convert / 10) << 4) & 0xF0) | (to_convert % 10));
}
/**********************************************************************************************************************
End of function rtc_dec_to_bcd
***********************************************************************************************************************/

uint32_t rtc_global_count;

void rtc_prd_isr(void)
{
//  if (rtc_callback != NULL)
//    rtc_callback(1);
}

void rtc_alm_isr(void)
{
	rtc_global_count++;
//  if (rtc_callback != NULL)
//    rtc_callback(2);
}
