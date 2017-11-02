/***********************************************************************************************//**
* File Name        : temperature.h
* File Description : This file contains the declaration of functions used to get slave temperature
* readings using I2C
* Author           : Srishti Khare
* Date             : 10/06/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
* * Reference      : IoT Lecture 4 slides on D2L
*************************************************************************************************/
#ifndef SRC_TEMPERATURE_H_
#define SRC_TEMPERATURE_H_

	#define TEMPERATURE_SET_POINT 15
	#define ADJUST_TEMPERATURE 5	//define how much to increment on period by
	#define MAX_TEMP 50	//define maximum allowable on period value
	#define MIN_TEMP 0	//define minimum allowable on period value

	uint16_t temperature_read(void);
	double convert_to_degrees_C(uint16_t data);
#endif /* SRC_TEMPERATURE_H_ */
