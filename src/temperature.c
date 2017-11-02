/***********************************************************************************************//**
* File Name        : temperature.c
* File Description : This file contains the functions to get slave temperature readings using I2C
* Author           : Srishti Khare
* Date             : 10/06/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
**************************************************************************************************/
#include "i2c.h"
#include "temperature.h"
/**************************************************************************//**
 * @brief	Read temperature value from slave using I2C
 * @param	None
 * @return	16-bit temperature value
 *****************************************************************************/
uint16_t temperature_read(void)
{
	/* Define variable */
	uint16_t temp_value = 0;
	/* Read temperature value into the variable */
	temp_value = i2c_read_temperature();
	/* Return read temperature value */
	return temp_value;
}
/**************************************************************************//**
 * @brief	Convert temperature value to degrees C
 * @param	16-bit temperature value
 * @return	Temperature value in degrees C (double)
 *****************************************************************************/
double convert_to_degrees_C(uint16_t data)
{
	/* Define variable */
	double value=0;
	/* Convert read temperature value into degrees C */
	value = data * 175.72;
	value /= 65536;
	value -= 46.85;
	/* Return converted value */
	return value;
}
