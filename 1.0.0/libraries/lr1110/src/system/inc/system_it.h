#ifndef __SYSTEM_IT_H
#define __SYSTEM_IT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "configuration.h"
#include "RE01_256KB.h"
#include "r_system_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler( void )  __attribute__ ((section(".ramfunc")));
void HardFault_Handler( void )  __attribute__ ((section(".ramfunc")));
void MemManage_Handler( void )  __attribute__ ((section(".ramfunc")));
void BusFault_Handler( void )  __attribute__ ((section(".ramfunc")));
void UsageFault_Handler( void )  __attribute__ ((section(".ramfunc")));
void SVC_Handler( void )  __attribute__ ((section(".ramfunc")));
void DebugMon_Handler( void )  __attribute__ ((section(".ramfunc")));
void PendSV_Handler( void )  __attribute__ ((section(".ramfunc")));
void SysTick_Handler( void )  __attribute__ ((section(".ramfunc")));
void EXTI4_IRQHandler( void )  __attribute__ ((section(".ramfunc")));
void EXTI15_10_IRQHandler( void ) __attribute__ ((section(".ramfunc")));
void DMA1_Channel7_IRQHandler( void ) __attribute__ ((section(".ramfunc")));
void DMA1_Channel6_IRQHandler( void ) __attribute__ ((section(".ramfunc")));
void LPTIM1_IRQHandler( void ) __attribute__ ((section(".ramfunc")));
void IRQ3_IRQHandler( void ) __attribute__ ((section(".ramfunc")));
void IRQ4_IRQHandler( void ) __attribute__ ((section(".ramfunc")));
void IRQ7_IRQHandler( void ) __attribute__ ((section(".ramfunc")));
void IRQ9_IRQHandler( void ) __attribute__ ((section(".ramfunc")));

#ifdef __cplusplus
}
#endif

#endif
