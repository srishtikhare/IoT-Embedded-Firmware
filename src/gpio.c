/***********************************************************************************************//**
* File Name        : gpio.c
* File Description : This file contains the function to set the GPIO pin connected to LED0, which is
*                    used for the Energy Modes assignment.
* Author           : Srishti Khare
* Date             : 09/13/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
*************************************************************************************************/
#include "gpio.h"
#include "main.h"
#include "em_gpio.h"
#include "bma280.h"
#include "i2c.h"

/* Define single and double tap values */
#define SINGLETAP 0x20
#define DOUBLETAP 0x10
bool i2c_flag = false;
/**************************************************************************//**
 * @brief	Initialize GPIO pins
 * @param	None
 * @return	None
 *****************************************************************************/
void gpio_init(void){

	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);

	GPIO_PinModeSet(ADC_port, ADC_pin, gpioModeDisabled, ADC_default);

	/*Over voltage disable for analog pin PortA pin 0*/
	GPIO->P[gpioPortA].OVTDIS = 0x0001;

	GPIO_PinModeSet(SPI_CS_port, SPI_CS_pin, gpioModePushPull, SPI_CS_default);
	GPIO_PinModeSet(SPI_SCLK_port, SPI_SCLK_pin, gpioModePushPull, SPI_SCLK_default);
	GPIO_PinModeSet(SPI_MOSI_port, SPI_MOSI_pin, gpioModePushPull, SPI_MOSI_default);
	GPIO_PinModeSet(SPI_MISO_port, SPI_MISO_pin, gpioModeInput, SPI_MISO_default);

	GPIO_PinModeSet(I2C_SCL_port, I2C_SCL_pin, gpioModeWiredAnd, I2C_SCL_default);
	GPIO_PinModeSet(I2C_SDA_port, I2C_SDA_pin, gpioModeWiredAnd, I2C_SDA_default);
	GPIO_PinModeSet(I2C_Bus_port, I2C_Bus_pin, gpioModePushPull, I2C_Bus_default);
}
/**************************************************************************//**
 * @brief	Initialize GPIO interrupt
 * @param	None
 * @return	None
 *****************************************************************************/
void gpio_interrupt_int(void)
{
	GPIO_IntClear(GPIO_IntGet());
	GPIO_PinModeSet(GPIO_INTERRUPT_port, GPIO_INTERRUPT_pin, gpioModeInput, GPIO_INTERRUPT_default);
	GPIO_IntConfig(GPIO_INTERRUPT_port, GPIO_INTERRUPT_pin, true, false, false);
	GPIO_IntEnable(GPIO_INT_ENABLE);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}
/**************************************************************************//**
 * @brief	GPIO Interrupt Handler
 * @param	None
 * @return	None
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	CORE_ATOMIC_IRQ_DISABLE();
	GPIO_IntClear(GPIO_INT_ENABLE);   //Clear the interrupting flags

	uint8_t rxdata = BMA280_RegisterRead(USART1,INT_STATUS_0);

	if(rxdata == SINGLETAP)	//If single tap occurred
	{
		i2c_enable();
		i2c_flag = true;
	}
	else if(rxdata == DOUBLETAP)	//If double tap occurred
	{
		i2c_disable();
		i2c_flag = false;
	}

	GPIO_IntEnable(GPIO_INT_ENABLE);	//Enable GPIO Interrupts
	CORE_ATOMIC_IRQ_ENABLE();	//Enable global interrupts
}
