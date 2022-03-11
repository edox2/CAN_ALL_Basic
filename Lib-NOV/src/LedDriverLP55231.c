/*
 * LedDriverLP55213.c
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// LP55231 Driver Lib

#include "LedDriverLP55231.h"
#include "I2cDispatcher.h"
#include "HalDef.h"
#include <string.h>

uint8_t LedSetSegmentRGB(uint8_t Channel, uint8_t BaseAddress, uint8_t LedRed, uint8_t LedGreen, uint8_t LedBlue, uint8_t Segment){

	uint8_t State = I2C_PRESENT;

	DataToWrite[0] = IllumD1_PWM;	   // First of Led PWM Registers
	State = (AccessI2C(Channel, BaseAddress, 1, &DataToWrite, 9, &DataToRead, I2C_MODE_MULTIPLE_START)) ? I2C_ABSENT : State;
	if (!State)
	{
		if (Segment < 8)
			memset(&DataToWrite[1], 0, 9);
		else
			memcpy(&DataToWrite[1], &DataToRead, 9);
		switch (Segment)
		{
			case 4:
			case 12:
				DataToWrite[1] =  LedGreen; // PWM D1 green
				DataToWrite[2] =  LedBlue; // PWM D2 blue
				DataToWrite[7] =  LedRed; // PWM D7 Red

				break;
			case 2:
			case 10:
				DataToWrite[3] =  LedGreen; // PWM D3 green
				DataToWrite[4] =  LedBlue; // PWM D4 blue
				DataToWrite[8] =  LedRed; // PWM D8 Red
				break;
			case 6:
			case 14:
				DataToWrite[1] =  LedGreen; // PWM D1 green
				DataToWrite[2] =  LedBlue; // PWM D2 blue
				DataToWrite[3] =  LedGreen; // PWM D3 green
				DataToWrite[4] =  LedBlue; // PWM D4 blue
				DataToWrite[7] =  LedRed; // PWM D7 Red
				DataToWrite[8] =  LedRed; // PWM D8 Red
				break;
			case 1:
			case 9:
				DataToWrite[5] =  LedGreen; // PWM D5 green
				DataToWrite[6] =  LedBlue; // PWM D6 blue
				DataToWrite[9] =  LedRed; // PWM D9 Red
				break;
			case 5:
			case 13:
				DataToWrite[1] =  LedGreen; // PWM D1 green
				DataToWrite[2] =  LedBlue; // PWM D2 blue
				DataToWrite[5] =  LedGreen; // PWM D5 green
				DataToWrite[6] =  LedBlue; // PWM D6 blue
				DataToWrite[7] =  LedRed; // PWM D7 Red
				DataToWrite[9] =  LedRed; // PWM D9 Red
				break;
			case 3:
			case 11:
				DataToWrite[3] =  LedGreen; // PWM D3 green
				DataToWrite[4] =  LedBlue; // PWM D4 blue
				DataToWrite[5] =  LedGreen; // PWM D5 green
				DataToWrite[6] =  LedBlue; // PWM D6 blue
				DataToWrite[8] =  LedRed; // PWM D8 Red
				DataToWrite[9] =  LedRed; // PWM D9 Red
				break;
			case 8:
				break;
			default:
				DataToWrite[1] =  LedGreen; // PWM D1 green
				DataToWrite[2] =  LedBlue; // PWM D2 blue
				DataToWrite[3] =  LedGreen; // PWM D3 green
				DataToWrite[4] =  LedBlue; // PWM D4 blue
				DataToWrite[5] =  LedGreen; // PWM D5 green
				DataToWrite[6] =  LedBlue; // PWM D6 blue
				DataToWrite[7] =  LedRed; // PWM D7 Red
				DataToWrite[8] =  LedRed; // PWM D8 Red
				DataToWrite[9] =  LedRed; // PWM D9 Red
				break;
		}
		if(memcmp(&DataToWrite[1], &DataToRead, 9))
			State = (AccessI2C(Channel, BaseAddress, 10, &DataToWrite, 0, &DataToRead, I2C_MODE_MULTIPLE_START)) ? I2C_ABSENT : State;
	}
	return (State);
}

uint8_t LedInitSetMaxCurrent(uint8_t Channel, uint8_t BaseAddress, uint8_t LedRed789, uint8_t LedGreen135, uint8_t LedBlue246)
{
	const uint8_t code ResetSettings[2] = {IllumRESET, 0xFF}; // Reset
	const uint8_t code EnableSettings[2] = {IllumENABLE_ENGINE_CNTRL1, 0x40}; // Chip Enable
	const uint8_t code MiscSettings[2] = {IllumMISC, 0x59}; // MISC Register Internal Oscillator On
	uint8_t State = I2C_PRESENT;
	State = (AccessI2C(Channel, BaseAddress, sizeof(ResetSettings), &ResetSettings, 0, &DataToRead, I2C_MODE_MULTIPLE_START)) ? I2C_ABSENT : State;
	State = (AccessI2C(Channel, BaseAddress, sizeof(EnableSettings), &EnableSettings, 0, &DataToRead, I2C_MODE_MULTIPLE_START)) ? I2C_ABSENT : State;
	State = (AccessI2C(Channel, BaseAddress, sizeof(MiscSettings), &MiscSettings, 0, &DataToRead, I2C_MODE_MULTIPLE_START)) ? I2C_ABSENT : State;
	State = (LedSetMaxCurrent(Channel, BaseAddress, LedRed789, LedGreen135, LedBlue246)) ?  I2C_ABSENT : State;
	return State;
}


uint8_t LedSetMaxCurrent(uint8_t Channel, uint8_t BaseAddress, uint8_t LedRed789, uint8_t LedGreen135, uint8_t LedBlue246)
{
	uint8_t State = I2C_PRESENT;
	DataToWrite[0] = IllumD1_CURRENT_CONTROL;	   // First of Led PWM Registers
	DataToWrite[1] = LedGreen135;
	DataToWrite[2] = LedBlue246;
	DataToWrite[3] = LedGreen135;
	DataToWrite[4] = LedBlue246;
	DataToWrite[5] = LedGreen135;
	DataToWrite[6] = LedBlue246;
	DataToWrite[7] = LedRed789;
	DataToWrite[8] = LedRed789;
	DataToWrite[9] = LedRed789;
	State = (AccessI2C(Channel, BaseAddress, 10, &DataToWrite, 0, &DataToRead, I2C_MODE_MULTIPLE_START)) ? I2C_ABSENT : State;
	return State;
}

uint8_t LedSetModeLogTC(uint8_t Channel, uint8_t BaseAddress, uint8_t LedRed789, uint8_t LedGreen135, uint8_t LedBlue246)
{
	uint8_t State = I2C_PRESENT;
	DataToWrite[0] = IllumD1_CONTROL;	   // First of Led PWM Registers
	DataToWrite[1] = LedGreen135;
	DataToWrite[2] = LedBlue246;
	DataToWrite[3] = LedGreen135;
	DataToWrite[4] = LedBlue246;
	DataToWrite[5] = LedGreen135;
	DataToWrite[6] = LedBlue246;
	DataToWrite[7] = LedRed789;
	DataToWrite[8] = LedRed789;
	DataToWrite[9] = LedRed789;
	State = (AccessI2C(Channel, BaseAddress, 10, &DataToWrite, 0, &DataToRead, I2C_MODE_MULTIPLE_START)) ? I2C_ABSENT : State;
	return (State);
}

uint8_t LedGetAdc(uint8_t Channel, uint8_t BaseAddress, uint8_t AdcInput, uint8_t * Value){
	// Simple Singleton approach; Extend when multiple devices of this type are in same program context. (e.g. [4] = {1,1,1,1});
	static uint8_t LastAdcInput = 0x1F;
	static uint8_t DataToWrite[2] = {IllumLED_TEST_CONTROL, 0xB1};
	uint8_t DataToRead[1];

	const uint8_t CmdGetAdc = IllumLED_TEST_ADC; // ADC Settings

	if (AdcInput != LastAdcInput) {
		DataToWrite[1] = AdcInput | 0xA0;
		I2C_State =  AccessI2C(Channel, BaseAddress, sizeof(DataToWrite), &DataToWrite, 0, &DataToRead, I2C_MODE_MULTIPLE_START);
		if (I2C_State)
			return I2C_State;
		LastAdcInput = AdcInput;

		CountDelay(RESET_DELAY_3ms);
	}
	I2C_State =  AccessI2C(Channel, BaseAddress, sizeof(CmdGetAdc), &CmdGetAdc, 1, &DataToRead, I2C_MODE_MULTIPLE_START);

	if (I2C_State)
		return I2C_State;
	* Value = DataToRead[0];
	return (I2C_State);
}

