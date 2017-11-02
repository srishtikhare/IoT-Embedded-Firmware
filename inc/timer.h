/***********************************************************************************************//**
* File Name        : timer.c
* File Description : This file contains the functions to set TIMER0 in Blue Gecko
* Author           : Srishti Khare
* Date             : 09/29/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
*************************************************************************************************/
#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

	void timer_init(void);
	void Timer_Enable(int top);
	void timer0_top_counter_set(int top);

#endif /* SRC_TIMER_H_ */
