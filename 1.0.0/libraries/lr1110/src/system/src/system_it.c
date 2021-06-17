#include "../inc/system_it.h"
#include "../inc/system.h"

int8_t irq3_event_flag;
int8_t irq4_event_flag;
int8_t irq7_event_flag;
int8_t irq9_event_flag;


/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler( void ) {}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler( void )
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while( 1 )
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler( void )
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while( 1 )
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler( void )
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while( 1 )
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler( void )
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while( 1 )
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler( void ) {}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler( void ) {}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler( void ) {}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
/*
  * @note   In the default implementation the System Timer (Systick) is used as source of time base.
  *         The Systick configuration is based on MSI clock, as MSI is the clock
  *         used after a system Reset and the NVIC configuration is set to Priority group 4.
  *         Once done, time base tick starts incrementing: the tick variable counter is incremented
  *         each 1ms in the SysTick_Handler() interrupt handler.
*/
void SysTick_Handler( void )
{
    system_time_IncreaseTicker( );
//    lv_tick_inc( 1 );
}

/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (EXTI), for the */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l4xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles external lines 10 to 15 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI4_IRQHandler( void )
{
}

/**
 * @brief  This function handles external lines 10 to 15 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI15_10_IRQHandler( void )
{
}

/**
 * @brief  This function handles DMA1 interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Channel7_IRQHandler( void )
{
}

/**
 * @brief  This function handles DMA1 interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Channel6_IRQHandler( void )
{
}

/**
 * @brief This function handles LPTIM1 global interrupt.
 */
void LPTIM1_IRQHandler( void )
{
}

void IRQ3_IRQHandler( void )
{
	irq3_event_flag = true;
}

//int8_t irq4_event_flag;
void IRQ4_IRQHandler( void )
{
	irq4_event_flag = true;
}

//int8_t irq7_event_flag;
void IRQ7_IRQHandler( void )
{
	irq7_event_flag = true;
}
//int8_t irq9_event_flag;
void IRQ9_IRQHandler( void )
{
	irq9_event_flag = true;
}
