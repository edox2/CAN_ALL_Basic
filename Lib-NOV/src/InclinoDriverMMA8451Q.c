/*
 *InclinoDriverMMA8451Q.c
 *
 *  Created on: 11.07.2016
 *********************************************
 *    (c)2016-2019 SIGITRONIC SOFTWARE       *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 *  NXP (freescale) MMA8451Q related stuff
 */

#include "InclinoDriverMMA8451Q.h"
#include "I2cDispatcher.h"

uint8_t TiltGetStatus(uint8_t Channel, uint8_t BaseAddress,	SI_UU16_t *xAxis, SI_UU16_t *yAxis, SI_UU16_t *zAxis) 
{
	static uint8_t xdata IcoldStart = 1;

	if (IcoldStart) 
	{
		// Initialize Accelerometer here
		DataToWrite[0] = TiltWHO_AM_I; //BASE_ADDR_TILT
		I2C_State = AccessI2C(Channel, BaseAddress, 1, DataToWrite, 1,
				DataToRead, I2C_MODE_MULTIPLE_START); // Idle Mode
		if (DataToRead[0] == 0x1A) {
			DataToWrite[0] = TiltCTRL_REG1;
			DataToWrite[1] = 0x00;
			I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0,
					DataToRead, I2C_MODE_MULTIPLE_START); // Go to Active Mode
			IcoldStart = 0;
			return I2C_State;
		}
		return I2C_ABSENT;
	} 
	else 
	{
		DataToWrite[0] = TiltSTATUS_F_STATUS;
		I2C_State = AccessI2C(Channel, BaseAddress, 1, DataToWrite, 6, DataToRead, I2C_MODE_MULTIPLE_START); // Go to Active Mode
		xAxis->u8[0] = DataToRead[0];
		xAxis->u8[1] = DataToRead[1];
		yAxis->u8[0] = DataToRead[2];
		yAxis->u8[1] = DataToRead[3];
		zAxis->u8[0] = DataToRead[4];
		zAxis->u8[1] = DataToRead[5];
	}
	return I2C_State;
}

