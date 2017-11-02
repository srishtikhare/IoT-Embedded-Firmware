/***********************************************************************************************//**
* File Name        : i2c.h
* File Description : This file contains the declaration of functions used for I2C
* Author           : Srishti Khare
* Date             : 10/06/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
* * Reference      : IoT Lecture 4 slides on D2L
*************************************************************************************************/
#include "em_i2c.h"

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

	#define SLAVE_ADDRESS 0x40
	#define WRITE 0x00
	#define READ 0x01
	#define MEASURE_TEMP_COMMAND 0xE3
	#define ACK 0x04
	#define NACK 0x08

	void i2c_init(void);
	uint16_t i2c_read_temperature(void);
	void delay_80ms(void);
	void i2c_enable(void);
	void i2c_disable(void);

	extern bool i2c_flag;

#endif /* SRC_I2C_H_ */
