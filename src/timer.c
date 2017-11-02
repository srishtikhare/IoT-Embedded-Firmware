/***********************************************************************************************//**
* File Name        : timer.c
* File Description : This file contains the functions to set TIMER0 in Blue Gecko
* Author           : Srishti Khare
* Date             : 09/29/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
*************************************************************************************************/
#include "main.h"
#include "em_timer.h"
#include "timer.h"
#include "gpio.h"
#include "sleep.h"
#include "em_cmu.h"

/**************************************************************************//**
 * @brief	Timer Interrupt Handler
 * @param	None
 * @return	None
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
	int intFlags;	//Variable to store the interrupting flags
	CORE_ATOMIC_IRQ_DISABLE();	//Disable interrupts to avoid nesting
	intFlags = TIMER_IntGet(TIMER0);    //Get the interrupting flags

	if(intFlags & TIMER_IF_OF)
		TIMER_IntClear(TIMER0, intFlags);   //Clear the interrupting flags

	unblockSleepMode(Energy_Mode_1);

	CORE_ATOMIC_IRQ_ENABLE();	//Re-enable interrupts
}
/**************************************************************************//**
 * @brief	Initialize timer
 * @param	None
 * @return	None
 *****************************************************************************/
void timer_init(void)
{
	/* Set TIMER0 parameters */
	TIMER_Init_TypeDef timerInit =
	{
	    .enable     = false,
	    .debugRun   = false,
	    .prescale   = timerPrescale1024,
	    .clkSel     = timerClkSelHFPerClk,
	    .fallAction = timerInputActionNone,
	    .riseAction = timerInputActionNone,
	    .mode       = timerModeUp,
	    .dmaClrAct  = false,
	    .quadModeX4 = false,
	    .oneShot    = true,
	    .sync       = false,
	};
	/* Initialize TIMER0 */
	TIMER_Init(TIMER0, &timerInit);

	/* Clear all TIMER interrupts*/
	TIMER_IntClear(TIMER0, TIMER_IFC_ICBOF3 | TIMER_IFC_ICBOF2 | TIMER_IFC_ICBOF1 | \
	                TIMER_IFC_ICBOF0 | TIMER_IFC_CC3 | TIMER_IFC_CC2 | TIMER_IFC_CC1 | \
	                TIMER_IFC_CC0| TIMER_IFC_DIRCHG | TIMER_IFC_UF | TIMER_IFC_OF);

	/* Enable overflow interrupt */
	TIMER_IntEnable(TIMER0, TIMER_IEN_OF);

	/* Enable TIMER0 interrupt vector in NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_SetPriority(TIMER0_IRQn, 2);
}
/**************************************************************************//**
 * @brief	Set timer top and counter values
 * @param	top value to set
 * @return	None
 *****************************************************************************/
void timer0_top_counter_set(int top)
{
	/* Set TIMER Top value */
	TIMER_TopSet(TIMER0, top);
	TIMER_CounterSet(TIMER0,0x00);
}
/**************************************************************************//**
 * @brief	Start timer
 * @param	top value to set
 * @return	None
 *****************************************************************************/
void Timer_Enable(int top)
{
	blockSleepMode(Energy_Mode_1);	//Set EM1 as lowest energy mode
	TIMER_TopSet(TIMER0, top);	//Reset TOP value for timer
	TIMER_CounterSet(TIMER0,0x00);	//Reset Counter value for timer
	TIMER_Enable(TIMER0,true);	//Start Timer
}
