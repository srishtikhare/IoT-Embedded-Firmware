/***********************************************************************************************//**
* File Name        : i2c.c
* File Description : This file contains the functions to set I2C
* Author           : Srishti Khare
* Date             : 10/06/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
*************************************************************************************************/
#include "i2c.h"
#include "gpio.h"
#include "sleep.h"
#include "em_i2c.h"

/* TIMER0 TOP value macro for 80 ms delay */
#define TIMER_TOP_80_ms 978
/**************************************************************************//**
 * @brief	Initialize I2C0
 * @param	None
 * @return	None
 *****************************************************************************/
void i2c_init(void)
{
	//clear buffers in I2C slave
	for (int i=0; i<9; i++)
	{
		GPIO_PinOutClear(I2C_SCL_port, I2C_SCL_pin);
		GPIO_PinOutSet(I2C_SCL_port, I2C_SCL_pin);
	}

	// Set up SCL
	I2C0->ROUTEPEN = I2C0->ROUTEPEN | I2C_ROUTEPEN_SCLPEN;	\
	I2C0->ROUTELOC0 = (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SCLLOC_MASK))
				| I2C_ROUTELOC0_SCLLOC_LOC14;
	// Set up SDA
	I2C0->ROUTEPEN = I2C0->ROUTEPEN | I2C_ROUTEPEN_SDAPEN;
	I2C0->ROUTELOC0 = (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SDALOC_MASK))	\
				| I2C_ROUTELOC0_SDALOC_LOC16;

	const I2C_Init_TypeDef i2c_config =
	{
		.enable			= false, //Enable I2C peripheral when init completed.
		.master 		= true,
		.refFreq		= 0,
		.freq			= I2C_FREQ_STANDARD_MAX, //(Max) I2C bus frequency to use
		.clhr			= i2cClockHLRStandard //Clock low/high ratio control. 4:4
	};
	/* Initialize I2C0 */
	I2C_Init(I2C0, &i2c_config);

	//reset the I2C bus
	/* Exit the BUSY state. The I2C will be in this state out of RESET. */
	if(I2C0->STATE & I2C_STATE_BUSY)
	{
		I2C0->CMD = I2C_CMD_ABORT;
	}
}
/**************************************************************************//**
 * @brief	Use I2C to take temperature readings from slave
 * @param	None
 * @return	14-bit temperature value
 *****************************************************************************/
uint16_t i2c_read_temperature(void)
{
	/* Initialize receive buffers*/
	uint16_t rxdata=0;
	uint8_t rxdata_temp=0;

	/* Send slave address + write bit to slave */
	I2C0->TXDATA = (SLAVE_ADDRESS << 1) | WRITE;
	I2C0->CMD = I2C_CMD_START;

	/* Wait for ACK */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC = I2C_IFC_ACK;

	/* Send command to read temperature */
	I2C0->TXDATA = MEASURE_TEMP_COMMAND;
	I2C0->CMD = I2C_CMD_START;
	while((I2C0->IF & I2C_IF_ACK) == 0);
	I2C0->IFC = I2C_IFC_ACK;

	/* Send slave address + read bit to slave */
	I2C0->TXDATA = (SLAVE_ADDRESS << 1) | READ;
	I2C0->CMD = I2C_CMD_START;	//Sr

	/* Receive MSB byte from slave */
	while((I2C0->IF & I2C_IF_ACK) == 0);
	while((I2C0->IF & I2C_IF_RXDATAV) == 0);
	rxdata = I2C0->RXDATA;
	I2C0->IFC = I2C_IFC_ACK;
	rxdata = rxdata << 8;

	/* Sebnd ACK */
	I2C0->CMD = ACK;
	while((I2C0->IF & I2C_IF_RXDATAV) == 0);

	/* Receive LSB byte from slave and adjust data to 14-bit standards */
	rxdata_temp = I2C0->RXDATA;
	rxdata_temp = rxdata_temp >> 2;
	rxdata = rxdata | rxdata_temp;

	/* Send NACK */
	I2C0->CMD = NACK;	//send NACK
	/* Send STOP bit */
	I2C0->CMD = I2C_CMD_STOP;

	/* Return received data */
	return rxdata;
}
/**************************************************************************//**
 * @brief	Routine to provide 80 ms delay while enabling I2C
 * @param	None
 * @return	None
 *****************************************************************************/
void delay_80ms(void)
{
	/* Enable global interrupts */
	CORE_ATOMIC_IRQ_ENABLE();
	/* Set TIMER Top value */
	timer0_top_counter_set(TIMER_TOP_80_ms);
	/* Start timer */
	Timer_Enable(TIMER_TOP_80_ms);
	/* Put Blue Gecko in sleep */
	sleep();
	/* Initialize GPIO interrupt */
	gpio_interrupt_int();
	/* Start GPIO interrupt */
	GPIO_IntEnable(GPIO_INT_ENABLE);
}
/**************************************************************************//**
 * @brief	Enable I2C for use
 * @param	None
 * @return	None
 *****************************************************************************/
void i2c_enable(void)
{
	GPIO_PinOutSet(I2C_Bus_port, I2C_Bus_pin);
	delay_80ms();
	i2c_init();
	I2C_Enable(I2C0,true);
}
/**************************************************************************//**
 * @brief	Disable I2C
 * @param	None
 * @return	None
 *****************************************************************************/
void i2c_disable(void)
{
	GPIO_PinOutClear(I2C_Bus_port, I2C_Bus_pin);
	I2C_Enable(I2C0,false);
}
