/***********************************************************************************************//**
* File Name        : sleep.h
* File Description : This file contains the declaration of functions used to control the sleep
*                    routine of Blue Gecko in different Energy Modes
* Author           : Srishti Khare
* Date             : 09/13/2017
* Compiler         : arm-none-eabi-gcc
* Linker           : arm-none-eabi-ld
* Debugger         : arm-none-eabi-gdb
* Reference        : IoT Lecture 3 slides on D2L
* Reference        : IoT Lecture 3 slides on D2L
*************************************************************************************************/
#include "em_emu.h"
#include "em_core.h"

#ifndef SRC_SLEEP_H_
#define SRC_SLEEP_H_

	extern int sleep_block_counter[5];

    typedef enum {
		Energy_Mode_0,   /** Energy mode 0 */
		Energy_Mode_1,   /** Energy mode 1 */
		Energy_Mode_2,   /** Energy mode 2 */
		Energy_Mode_3,   /** Energy mode 3 */
		Energy_Mode_4,   /** Energy mode 4 */
	} sleepstate_enum;

	extern void sleep(void);
	extern void blockSleepMode(sleepstate_enum minimumMode);
	extern void unblockSleepMode(sleepstate_enum minimumMode);

#endif /* SRC_SLEEP_H_ */
