/*
 * LraDriver.c
 *********************************************
 *     (c)2016-2019 SIGITRONIC SOFTWARE      *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// DRV2605L Driver Lib

#include <VibraDriverDRV2605L.h>
#include "I2cDispatcher.h"

static uint8_t xdata VcoldStart = 1;

void LraVibraSetColdStartState(void)
{
	VcoldStart = 1;
}

void LraVibraClearColdStartState(void)
{
	VcoldStart = 0;
}

uint8_t LraVibraCalibrate(uint8_t Channel, uint8_t BaseAddress)
{
	uint8_t ErrorState = I2C_PRESENT;
	// Initialize I2C Vibra here
    DataToWrite[0] = VibraREG01;

    //Write the MODE register (address 0x01) to value 0x00 to remove the device from standby mode.
    DataToWrite[1] = 0x00;
	I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
	ErrorState = (I2C_State)? I2C_State : ErrorState;
	// Put Driver Out of Standby to AutoCaliration Mode
    DataToWrite[1] = 0x07;
	I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
	ErrorState = (I2C_State)? I2C_State : ErrorState;

	DataToWrite[0] = VibraREG16;
    DataToWrite[1] = 85; //70 1.4V RMS //90: Rated Voltage 1.8V RMS @f0=235Hz  // (before 85; // Rated Voltage 2.1Vrms)
    DataToWrite[2] = 107; // 84: 1.8V //89: Open Loop Clamping Voltage 1.9V for Overdrive // (before 107;// Clamping Voltage 2.1V (for Overdrive))
	I2C_State = AccessI2C(Channel, BaseAddress, 3, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
	ErrorState = (I2C_State)? I2C_State : ErrorState;


	DataToWrite[0] = VibraREG1A;
    DataToWrite[1] = 0xA8; // See Auto Calibration Description (LRA)          (Register 0x1A)
    DataToWrite[2] = 0x86; // See Auto Calibration Description (LRA) f0=235Hz (Register 0x1B)
    DataToWrite[3] = 0xF5; // See Auto Calibration Description                (Register 0x1C)
	I2C_State = AccessI2C(Channel, BaseAddress, 4, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
	ErrorState = (I2C_State)? I2C_State : ErrorState;

	DataToWrite[0] = VibraREG0C;
    DataToWrite[1] = 0x01; // Set GO-Bit to Start Auto Calibration
	I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
	ErrorState = (I2C_State)? I2C_State : ErrorState;

	if (!I2C_State) // only enter the loop when there is chance to get the chip answering as it was doing so before
		do //Check GO-Bit on VibraREG0C to disappear
		{
	 		I2C_State = AccessI2C(Channel, BaseAddress, 1, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START);
		} while ((I2C_State == 0) && (DataToRead[0] & 0x01));

	ErrorState = (I2C_State)? I2C_State : ErrorState;

	// Diagnostics *** optional things begin here ***
	DataToWrite[0] = VibraREG01;
    DataToWrite[1] = 0x06; // Put Driver to Diagnostic Mode
	I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
	ErrorState = (I2C_State)? I2C_State : ErrorState;

	DataToWrite[0] = VibraREG0C;
    DataToWrite[1] = 0x01; // Set GO-Bit to Start Diagnostics
	I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
	ErrorState = (I2C_State)? I2C_State : ErrorState;

    if (!I2C_State) // only enter the loop when there is chance to get the chip answering as it was doing so before
    	do //Check GO-Bit on VibraREG0C to disappear
    	{
     		I2C_State = AccessI2C(Channel, BaseAddress, 1, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START);
    	} while ((I2C_State == 0) && (DataToRead[0] & 0x01));

    ErrorState = (I2C_State)? I2C_State : ErrorState;

	DataToWrite[0] = VibraREG00;

	//Check Diagnostic state on VibraREG00
	if (!ErrorState) // only enter the loop when there is chance to get the chip answering as it was doing so before
	{
		I2C_State = AccessI2C(Channel, BaseAddress, 1, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START);
		ErrorState = (I2C_State || (DataToRead[0] & (0x08 + 0x02 + 0x01))) ? (I2C_State || (DataToRead[0] & (0x08 + 0x02 + 0x01))) : ErrorState;

		// Diagnostics *** optional things end here ***
    	DataToWrite[0] = VibraREG03;
        DataToWrite[1] = 0x06; // Select the LRA-Library (Closed Loop Patterns optimized for LRA)
        DataToWrite[2] = 0;
        DataToWrite[3] = 0;
    	I2C_State = AccessI2C(Channel, BaseAddress, 4, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
    	ErrorState = (I2C_State)? I2C_State : ErrorState;

        DataToWrite[0] = VibraREG01;

        //Write the MODE register (address 0x01) to value 0x00 to remove the device from standby mode.
        DataToWrite[1] = 0x00;
    	I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
    	ErrorState = (I2C_State)? I2C_State : ErrorState;
    	if (!ErrorState)
    		VcoldStart = 0;
	}
	return ErrorState;
}


uint8_t LraVibraSetPattern(uint8_t Channel, uint8_t BaseAddress, uint8_t *Pattern)
{
	uint8_t Offset;
	uint8_t ErrorState = I2C_ABSENT;
	if (!VcoldStart)
	{

		DataToWrite[0] = VibraREG04;
		for (Offset = 0; Offset < 8; Offset++)
		{
			DataToWrite[Offset + 1] = *(Pattern + Offset);
		}
 		I2C_State = AccessI2C(Channel, BaseAddress, 9, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);

		DataToWrite[0] = VibraREG0C;

		if (*(Pattern))
		{
			DataToWrite[1] = 0x01;// Set GO-Bit
		}
		else
		{
			DataToWrite[1] = 0x00;// Clear GO-Bit
		}
 		I2C_State = AccessI2C(Channel, BaseAddress, 2, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
 		ErrorState = (I2C_State)? I2C_State : ErrorState;
	}
	return (ErrorState);
}

uint8_t LraVibraGetState(uint8_t Channel, uint8_t BaseAddress, uint8_t *GoFlagState)
{
	DataToWrite[0] = VibraREG00;
	I2C_State = AccessI2C(Channel, BaseAddress, 1, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START);
	if (!I2C_State && (DataToRead[0] & 0xE0)){
		DataToWrite[0] = VibraREG0C;
		I2C_State = AccessI2C(Channel, BaseAddress, 1, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START);
		*GoFlagState = DataToRead[0];
	}
	return I2C_State;
}

