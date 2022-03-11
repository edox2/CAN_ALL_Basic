/*
 * HalDef.c
 *
 *  Created on: 12.07.2016
 *********************************************
 *      (c)2016 SIGITRONIC SOFTWARE          *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
#include "HalDef.h"
#include <SI_EFM8UB3_Defs.h>
#include <SI_EFM8UB3_Register_Enums.h>

//#include <reg51.h>

volatile uint32_t x;



void CountDelay(uint32_t count) {
	for (x = 0; x < count; x)
		x++;
}

