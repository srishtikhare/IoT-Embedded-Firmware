/***********************************************************************************************//**
* File Name        : adc.h
* File Description : This file contains the declaration of functions to set the ADC connected to
* 					 joystick, which is used for the ADC Energy Modes assignment.
* Author           : Srishti Khare
* Date             : 09/20/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
***************************************************************************************************/

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

	#include "main.h"
	#include "sleep.h"

	#ifdef EM_0
	int mode = 0;
	#endif

	#ifdef EM_1
	int mode = 1;
	#endif

	#ifdef EM_2
	int mode = 2;
	#endif

	#ifdef EM_3
	int mode = 3;
	#endif

	void ADC0_IRQHandler(void);
	void AdcSynctoAsync(void);
	void AdcSetup(void);
	void AdcInterrupt(void);
	uint32_t AdcRead(void);
	void LETIMER_Prescaler(void);
	void debounce_check(void);
	void debounce(void);

#endif /* SRC_ADC_H_ */
