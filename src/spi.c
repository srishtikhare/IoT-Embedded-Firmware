/***********************************************************************************************//**
* File Name        : spi.c
* File Description : This file contains the functions to set the SPI using USART to access BMA280
* Author           : Srishti Khare
* Date             : 09/29/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
*************************************************************************************************/
#include "spi.h"
/**************************************************************************//**
 * @brief	Intialize SPI using USART
 * @param	None
 * @return	None
 *****************************************************************************/
void spi_init(void)
{
	/* Set USART parameters */
	USART_InitSync_TypeDef spiInit =
	{
		.enable = usartDisable,
		.refFreq = 0,
		.baudrate = 500000,
		.databits = usartDatabits8,
		.master = true,
		.msbf = true,
		.clockMode = usartClockMode0,
		.autoTx = false,
		.autoCsEnable = true,
		.autoCsHold = 1,
		.autoCsSetup = 1,
	};
	/* Initialize synchronous USART1*/
	USART_InitSync(USART1, &spiInit);

	/* Set USART1 registers*/
	USART1->ROUTELOC0 = USART_ROUTELOC0_CLKLOC_LOC11 | USART_ROUTELOC0_TXLOC_LOC11 | USART_ROUTELOC0_RXLOC_LOC11 | USART_ROUTELOC0_CSLOC_LOC11;
	USART1->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CSPEN;
	USART1->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

	/* Start USART1 */
	USART_Enable(USART1, usartEnable);
}
