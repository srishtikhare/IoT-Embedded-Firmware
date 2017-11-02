/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
#include "boards.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "aat.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#ifdef FEATURE_BOARD_DETECTED
#include "bspconfig.h"
#include "pti.h"
#endif

/* Device initialization header */
#include "InitDevice.h"

#ifdef FEATURE_SPI_FLASH
#include "em_usart.h"
#include "mx25flash_spi.h"
#endif /* FEATURE_SPI_FLASH */

#include "em_letimer.h"
#include "em_core.h"

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

#ifdef FEATURE_PTI_SUPPORT
static const RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;
#endif

/* Gecko configuration parameters (see gecko_configuration.h) */
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
  #ifdef FEATURE_PTI_SUPPORT
  .pti = &ptiInit,
  #endif
};

/* Flag for indicating DFU Reset must be performed */
uint8_t boot_to_dfu = 0;

//***********************************************************************************
// Include files
//***********************************************************************************

#include "em_device.h"
#include "em_chip.h"
#include "main.h"
#include "gpio.h"
#include "cmu.h"
#include "em_cmu.h"
#include "adc.h"

#include "sleep.h"
#include "letimer0.h"
#include "timer.h"
#include "bma280.h"
#include "i2c.h"

#define MODE 3

int sleep_block_counter[5];

int comparator_1_value=0, comparator_2_value=0;
uint32_t AdcRead_value;

int main(void)
{
	#ifdef FEATURE_SPI_FLASH
		/* Put the SPI flash into Deep Power Down mode for those radio boards where it is available */
		MX25_init();
		MX25_DP();
		/* We must disable SPI communication */
		USART_Reset(USART1);

	#endif /* FEATURE_SPI_FLASH */

	enter_DefaultMode_from_RESET();
	gecko_init(&config);

	mode = MODE;

	/* Initialize clocks */
	cmu_init();

	/* Initialize GPIO */
	gpio_init();

	/* Set up LETIMER0 with values */
	LETIMER0_call(LETIMER_Period);

	/* Initialize TIMER0 */
	timer_init();

	/* Initialize SPI */
	spi_init();

	/* Enable BMA280 in SUSPEND mode */
	BMA280_Enable();
	BMA280_Normal_to_Suspend();

	/* Set up ADC */
	AdcSetup();
	AdcInterrupt();
	AdcRead_value = AdcRead();

	while(1)
	{
		sleep();	//Enter sleep mode
	}

	return 0;
}
