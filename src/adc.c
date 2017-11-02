/***********************************************************************************************//**
* File Name        : adc.c
* File Description : This file contains the functions to set the ADC connected to joystick, which is
*                    used for the ADC Energy Modes assignment.
* Author           : Srishti Khare
* Date             : 09/20/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
*************************************************************************************************/
#include "adc.h"
#include "em_device.h"
#include "em_adc.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "gpio.h"
#include "main.h"
#include "letimer0.h"
#include "bma280.h"
#include "sleep.h"
#include "i2c.h"
#include "temperature.h"
#include "em_gpio.h"

#define ADC_CMP_GT_VALUE 20	//define high value for compare window function
#define ADC_CMP_LT_VALUE 4000	//define low value for compare window function
#define ADC_BIAS 0x0001000F //value to set ADC to LOWACC

//Initialize variables to be used
int comparator_0_value;
double temperature_set_point = TEMPERATURE_SET_POINT;
bool joystick_input = false;
bool normal_mode_check = false;

/**************************************************************************//**
 * @brief	ADC0 IRQ Handler
 * @param	None
 * @return	None
 *****************************************************************************/
void ADC0_IRQHandler(void)	//Interrupt handler for ADc
{
	int intFlags;	//Variable to store the interrupting flags
	CORE_ATOMIC_IRQ_DISABLE();	//Disable interrupts to avoid nesting
	ADC0->SCANFIFOCLEAR = 0x0001;	//Clear FIFO
	ADC0->SINGLEFIFOCLEAR = 0x0001;
	intFlags = ADC_IntGet(ADC0);    //Get the interrupting flags
	ADC_IntClear(ADC0, intFlags);   //Clear the interrupting flags
	AdcRead_value = AdcRead();	//Read current ADC value for direction in
								//which joystick is pressed
	debounce_check();	//Perform debounce check

	if((AdcRead_value > 3450) && (AdcRead_value < 3550) && (!joystick_input))	//Joystick pressed in North direction
	{
		/* Transit BMA from SUSPEND to NORMAL mode */
		normal_mode_check = true;
		debounce();	//Perform software debounce routine
	}
	else if((AdcRead_value > 2000) && (AdcRead_value < 2050) && (!joystick_input))	//Joystick pressed in South direction
	{
		/* Transit BMA from NORMAL to SUSPEND mode */
		BMA280_Normal_to_Suspend();
		debounce();	//Perform software debounce routine
		/* Check if LED1 is set. If yes, clear it */
		if(GPIO_PinInGet(LED1_port, LED1_pin) == 1)
			GPIO_PinOutClear(LED1_port, LED1_pin);
	}
	else if((AdcRead_value > 3100) && (AdcRead_value < 3150) && (!joystick_input))	//Joystick pressed in East direction
	{
		/* While temperature set point is within 0-100 range, alter it */
		if(temperature_set_point + ADJUST_TEMPERATURE < MAX_TEMP)
			temperature_set_point += ADJUST_TEMPERATURE;
		debounce();	//Perform software debounce routine
	}
	else if((AdcRead_value > 2400) && (AdcRead_value < 2600) && (!joystick_input))	//Joystick pressed in West direction
	{
		/* While temperature set point is within 0-100 range, alter it */
		if(temperature_set_point - ADJUST_TEMPERATURE > MIN_TEMP)
			temperature_set_point -= ADJUST_TEMPERATURE;
		debounce();	//Perform software debounce routine
	}

	CORE_ATOMIC_IRQ_ENABLE();	//Re-enable interrupts
	/* Set BMA to NORMAL mode if the flag to check it is set */
	if(normal_mode_check == true)
	{
		BMA280_Suspend_to_Normal();
		BMA280_RegisterWrite(USART1,0x14,0xB6);
		BMA280_Enable();
		normal_mode_check = false;
	}
}
/**************************************************************************//**
 * @brief	Set ADC from synchronous to asynchronous
 * @param	None
 * @return	None
 *****************************************************************************/
void AdcSynctoAsync(void)
{
	int intFlags;
	intFlags = ADC_IntGet(ADC0);    //Get the interrupting flags
	ADC_IntDisable(ADC0, intFlags);   //Clear the interrupting flags

	ADC0->SCANFIFOCLEAR = 0x0001;	//Clear FIFO
	ADC0->SINGLEFIFOCLEAR = 0x0001;

	ADC0->CTRL |= 0x0080;	//set asynchronous clock for ADC0
}
/**************************************************************************//**
 * @brief	Setup ADC
 * @param	None
 * @return	None
 *****************************************************************************/
void AdcSetup(void)
{
	/* Enable ADC clock */
	CMU_ClockEnable(cmuClock_ADC0, true);

	/* Base the ADC configuration on the default setup. */
	ADC_Init_TypeDef init =
	{
		.ovsRateSel = adcOvsRateSel2,              /* 2x oversampling (if enabled). */
		.warmUpMode = adcWarmupNormal,             /* ADC shutdown after each conversion. */
		.timebase = ADC_TimebaseCalc(0),  /* Use HW default value. */
		.prescale = ADC_PrescaleCalc(400000, 0),     /* Use HW default value. */
		.tailgate = false,                       /* Do not use tailgate. */
		.em2ClockConfig = adcEm2ClockOnDemand,               /* ADC disabled in EM2 */
	};
	ADC_Init(ADC0, &init);

	ADC_InitSingle_TypeDef sInit =
	{
		.prsSel = adcPRSSELCh0,            /* PRS ch0 (if enabled). */
		.acqTime = adcAcqTime32,             /* 1 ADC_CLK cycle acquisition time. */
		.reference = adcRefVDD,              /* 1.25V internal reference. */
		.resolution = adcRes12Bit,             /* 12 bit resolution. */
		.posSel = adcPosSelAPORT3XCH8,     /* Select node BUS0XCH0 as posSel */
		.negSel = adcNegSelVSS,            /* Select VSS as negSel */
		.diff = false,                   /* Single ended input. */
		.prsEnable = false,                   /* PRS disabled. */
		.leftAdjust = false,                   /* Right adjust. */
		.rep = true,                   /* Deactivate conversion after one scan sequence. */
		.singleDmaEm2Wu = false,                   /* No EM2 DMA wakeup from single FIFO DVL */
		.fifoOverwrite = true,                    /* Discard new data on full FIFO. */
	};
	ADC_InitSingle(ADC0, &sInit);
	/* Set ADC0 registers */
	ADC0->SCANCTRL |= _ADC_SCANCTRL_REF_VDD;
	ADC0->SCANCTRLX |= _ADC_SCANCTRLX_VREFSEL_VDDXWATT;

	ADC0->SINGLECTRL |=  ADC_SINGLECTRL_CMPEN;
	ADC0->CMPTHR = _ADC_CMPTHR_RESETVALUE;
	ADC0->CMPTHR = (ADC_CMP_GT_VALUE << _ADC_CMPTHR_ADGT_SHIFT) +
	                     (ADC_CMP_LT_VALUE << _ADC_CMPTHR_ADLT_SHIFT);
	ADC0->BIASPROG |= ADC_BIAS;

	AdcSynctoAsync();	//Convert ADC clock from synchronous to asynchronous

	/*Enable Interrupts*/
	ADC_IntClear(ADC0, ADC_IFC_PROGERR | ADC_IFC_VREFOV | ADC_IFC_SCANCMP |
	            ADC_IFC_SINGLECMP | ADC_IFC_SCANUF | ADC_IFC_SINGLEUF | ADC_IFC_SCANOF | ADC_IFC_SINGLEOF);	//Clear ADC Interrupts
	ADC_IntEnable(ADC0, ADC_IEN_SINGLECMP );	//Set ADC Interrupts
	blockSleepMode(mode);	//Set sleep mode
	NVIC_EnableIRQ(ADC0_IRQn);	//Set up global interrupt for ADC
	CORE_ATOMIC_IRQ_ENABLE();	//Enable global interrupts
}
/**************************************************************************//**
 * @brief	Setup ADC interrupt
 * @param	None
 * @return	None
 *****************************************************************************/
void AdcInterrupt(void)
{
	blockSleepMode(mode);
	CORE_ATOMIC_IRQ_ENABLE();	//Re-enable interrupts
	NVIC_EnableIRQ(ADC0_IRQn);
}

/***************************************************************************//**
 * @brief	Perform one ADC conversion
 * @param	None
 * @return	ADC conversion result
 ******************************************************************************/
uint32_t AdcRead(void)	//ADC read function
{
  ADC_Start(ADC0, adcStartSingle);	//start the ADC
  while ( (ADC0->STATUS & ADC_STATUS_SINGLEDV) == 0 ) {
  }
  return ADC_DataSingleGet(ADC0);	//return ADC value
}

/**************************************************************************//**
 * @brief	Check if debounce occurred
 * @param	None
 * @return	None
 *****************************************************************************/
void debounce_check(void)	//check for debounce
{
	if(joystick_input)	//if joystick has been pressed
	{
		joystick_input = false;	//turn joystick input to false
		ADC0->CMPTHR = (ADC_CMP_GT_VALUE << _ADC_CMPTHR_ADGT_SHIFT) +
					   (ADC_CMP_LT_VALUE << _ADC_CMPTHR_ADLT_SHIFT);	//Exchange the higher and lower limit of compare																		//window function concept
	}
}
/**************************************************************************//**
 * @brief	Debounce routine
 * @param	None
 * @return	None
 *****************************************************************************/
void debounce(void)	//software debounce routine
{
	joystick_input = true;	//set joystick input to true
	ADC0->CMPTHR = (ADC_CMP_LT_VALUE << _ADC_CMPTHR_ADGT_SHIFT) +
				   (ADC_CMP_GT_VALUE << _ADC_CMPTHR_ADLT_SHIFT);	//Swap back the higher and lower limit of compare																	//window function concept
}

