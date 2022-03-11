/*
 * HalDef.h
 *
 *  Created on: 04.08.2019
 *********************************************
 *      (c)2019 SIGITRONIC SOFTWARE          *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef HALDEF_H_
#define HALDEF_H_

#include "SI_EFM8UB3_Register_Enums.h"
#include "si_toolchain.h"

SI_SBIT(I2C_SDA_Ch1, SFR_P0, 6);
SI_SBIT(I2C_SCL_Ch1, SFR_P0, 7);
SI_SBIT(I2C_SDA_Ch0, SFR_P0, 6);
SI_SBIT(I2C_SCL_Ch0, SFR_P0, 7);
SI_SBIT(PB_IN, SFR_P0, 1);


#define I2C_Channel_RCC (0)
#define I2C_Channel_Slave (1)

// Counter Delay Loops
/*******************************************/
#define DelayTime4I2C	(1)
#define DelayIteration4I2C	(10)
//Base-Addresses for I2C Devices
/*******************************************/

void CountDelay(uint32_t count);

#endif /* HALDEF_H_ */
