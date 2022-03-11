/*
 * AdcDriverADS7924.c
 *********************************************
 *    (c)2016-2022 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// ADS7924 Driver Lib


#include "AdcDriverADS7924.h"
#include "I2cDispatcher.h"

uint8_t AdcConvert(uint8_t Channel, uint8_t BaseAddress, adcBaseStruct * AnalogValues) {

	uint8_t ErrorCounter = 0;
	DataToWrite[0] = AdcMODECNTRL;
	DataToWrite[1] = 0x80;
	ErrorCounter += AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToWrite, I2C_MODE_MULTIPLE_START)? 1 : 0; // Awake Mode (Important to change to [Awake-Mode] first, before change to other mode!)
	DataToWrite[1] = 0x00;
	ErrorCounter += (AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToWrite, I2C_MODE_MULTIPLE_START))? 1 : 0; // Idle Mode

	DataToWrite[0] = AdcDATA0_U;
	ErrorCounter += AccessI2C(Channel, BaseAddress, 1, DataToWrite, 8, DataToWrite, I2C_MODE_MULTIPLE_START)? 1 : 0;// Idle Mode
	if (!ErrorCounter)
	{
		AnalogValues->Channel0.u8[0] = DataToWrite[0];
		AnalogValues->Channel0.u8[1] = DataToWrite[1];
		AnalogValues->Channel1.u8[0] = DataToWrite[2];
		AnalogValues->Channel1.u8[1] = DataToWrite[3];
		AnalogValues->Channel2.u8[0] = DataToWrite[4];
		AnalogValues->Channel2.u8[1] = DataToWrite[5];
		AnalogValues->Channel3.u8[0] = DataToWrite[6];
		AnalogValues->Channel3.u8[1] = DataToWrite[7];
	}

	DataToWrite[0] = AdcMODECNTRL;
	DataToWrite[1] = 0x80;
	ErrorCounter += AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToWrite, I2C_MODE_MULTIPLE_START)? 1 : 0; // Awake Mode (Important to change to [Awake-Mode] first, before change to other mode!)
	DataToWrite[1] = 0xCC;
	ErrorCounter += AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToWrite, I2C_MODE_MULTIPLE_START)? 1 : 0; // Auto Scan Mode
	return (ErrorCounter) ? I2C_ABSENT : I2C_PRESENT;
}

uint8_t AdcReset(uint8_t Channel, uint8_t BaseAddress) {

	DataToWrite[0] = AdcRESET;
	DataToWrite[1] = 0xAA; //10101010
	I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToWrite, I2C_MODE_MULTIPLE_START);// Idle Mode
	return (I2C_State);
}

