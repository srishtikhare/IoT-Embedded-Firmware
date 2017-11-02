/***********************************************************************************************//**
* File Name        : sleep.c
* File Description : This file contains the function to set the GPIO pin connected to LED0, which is
*                    used for the Energy Modes assignment.
* Author           : Srishti Khare
* Date             : 09/13/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
***************************************************************************************************/
#include "sleep.h"
/**************************************************************************//**
 * @brief	Sleep routine for Blue Gecko
 * @param	None
 * @return	None
 *****************************************************************************/
void sleep(void)	//sleep routine
{
	if(sleep_block_counter[0]>0)	//checks if sleep mode is selected in EM0
	{
		return;
	}
	else if(sleep_block_counter[1]>0)	//checks if sleep mode is selected in EM1
	{
		EMU_EnterEM1();	//if so, enters EM1
	}
	else if(sleep_block_counter[2]>0)	//checks if sleep mode is selected in EM2
	{
		EMU_EnterEM2(true);	//if so, enters EM2
	}
	else if(sleep_block_counter[3]>0)	//checks if sleep mode is selected in EM3
	{
		EMU_EnterEM3(true);	//if so, enters EM3
	}
	else
	{
		EMU_EnterEM3(true);
	}
	return;
}
/**************************************************************************//**
 * @brief	Set the energy/sleep mode in the MCU
 * @param	Minimum energy mode BG can go to
 * @return	None
 *****************************************************************************/
void blockSleepMode(sleepstate_enum minimumMode)	//sets the energy/sleep mode in the MCU
{
	CORE_ATOMIC_IRQ_DISABLE();	//disables atomic style interrupt
	sleep_block_counter[minimumMode]++;	//sets the variable for the energy/sleep mode
	CORE_ATOMIC_IRQ_ENABLE();	//enables atomic style interrupt
}
/**************************************************************************//**
 * @brief	Undo the energy/sleep mode in the MCU
 * @param	Minimum energy mode to be removed
 * @return	None
 *****************************************************************************/
void unblockSleepMode(sleepstate_enum minimumMode)	//clears the energy/sleep mode in the MCU
{
	CORE_ATOMIC_IRQ_DISABLE();	//disables atomic style interrupt
	if(sleep_block_counter[minimumMode]>0)
	{
		sleep_block_counter[minimumMode]--;	//clears the variable for the energy/sleep mode set initially
	}
	CORE_ATOMIC_IRQ_ENABLE();	//enables atomic style interrupt
}
