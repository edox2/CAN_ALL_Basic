/*
 * HallDriverTLE493D_W2B6.c
 *
 *  Created on: 12.07.2016
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include "HallDriverTLE493D_W2B6.h"
#include "I2cDispatcher.h"
#include "HalDef.h"

#define MASK_Bx_00H_Bx 				(0xFF)
#define MASK_By_01H_By 				(0xFF)
#define MASK_Bz_02H_Bz 				(0xFF)
#define MASK_Temp_03H_Temp 			(0xFF)
#define MASK_Bx2_04H_Bx  			(0xF0)
#define MASK_Bx2_04H_By   			(0x0F)
#define MASK_Temp2_05H_Temp  		(0xC0)
#define MASK_Temp2_05H_ID 			(0x30)
#define MASK_Temp2_05H_Bz  			(0x0F)
#define MASK_Diag_06H_P  			(0x80)
#define MASK_Diag_06H_FF  			(0x40)
#define MASK_Diag_06H_CF  			(0x20)
#define MASK_Diag_06H_T  			(0x10)
#define MASK_Diag_06H_PD3  			(0x08)
#define MASK_Diag_06H_PD0  			(0x04)
#define MASK_Diag_06H_FRM  			(0x03)
#define MASK_XL_07H_XL 				(0xFF)
#define MASK_XH_08H_XH 				(0xFF)
#define MASK_YL_09H_YL 				(0xFF)
#define MASK_YH_0AH_YH 				(0xFF)
#define MASK_ZL_0BH_ZL 				(0xFF)
#define MASK_ZH_0CH_ZH 				(0xFF)
#define MASK_WU_0DH_WA 				(0x80)
#define MASK_WU_0DH_WU 				(0x40)
#define MASK_WU_0DH_XH 				(0x38)
#define MASK_WU_0DH_XL 				(0x07)
#define MASK_TMode_0EH_TST 			(0xC0)
#define MASK_TMode_0EH_YH 			(0x38)
#define MASK_TMode_0EH_YL 			(0x07)
#define MASK_TPhase_0FH_PH 			(0xC0)
#define MASK_TPhase_0FH_ZH 			(0x38)
#define MASK_TPhase_0FH_ZL 			(0x07)
#define MASK_Config_10H_DT 			(0x80)
#define MASK_Config_10H_AM 			(0x40)
#define MASK_Config_10H_TRIG 		(0x30)
#define MASK_Config_10H_X2 			(0x08)
#define MASK_Config_10H_TL_mag		(0x06)
#define MASK_Config_10H_CP 			(0x01)
#define MASK_MOD1_11H_FP 			(0x80)
#define MASK_MOD1_11H_IICadr		(0x60)
#define MASK_MOD1_11H_PR 			(0x10)
#define MASK_MOD1_11H_CA 			(0x08)
#define MASK_MOD1_11H_INT 			(0x04)
#define MASK_MOD1_11H_MODE 			(0x03)
#define MASK_Reserved_12H_Reserved 	(0xFF)
#define MASK_MOD2_13H_PRD 			(0xE0)
#define MASK_MOD2_13H_Reserved 		(0x1F)
#define MASK_Reserved_14H_Reserved 	(0xFF)
#define MASK_Reserved_15H_Reserved 	(0xFF)
#define MASK_Ver_16H_Reserved 		(0xC0)
#define MASK_Ver_16H_Type 			(0x30)
#define MASK_Ver_16H_HWV 			(0x0F)

#define SENSOR_MOD1_LowPowerMode 			(0x00)
#define SENSOR_MOD1_MasterControlledMode 	(0x01)
#define SENSOR_MOD1_FastMode 				(0x03)

#define SENSOR_MOD1_FAST	(0x02)
#define SENSOR_MOD1_INT		(0x04)
#define SENSOR_MOD1_RES 	(0x18)
#define SENSOR_MOD1_IICADDR	(0x60)
#define SENSOR_MOD1_P		(0x80)

#define SENSOR_MOD2_T		(0x80)
#define SENSOR_MOD2_LP		(0x40)
#define SENSOR_MOD2_PT		(0x20)

#define SENSOR_TEMP_FRM 	(0x0C)
#define SENSOR_TEMP_CH 		(0x03)
#define SENSOR_BZ2_PD		(0x10)

#define SENSOR_READ_ALL_REGISTERS false
#define SENSOR_READ_SHORTFORM true

#define TRIGGER_BITS_NoAdcTrigger (0x00)
#define TRIGGER_BITS_AfterWrite	  (0x20)
#define TRIGGER_BITS_AfterRead    (0x80)



float CalcMicroTeslaHallSensor(int16_t VectorValue)
{
	return (0.098f * (float) (VectorValue / 16));
}

float CalcCelsiusDegreeSensor(TLE493D_BaseStruct *SensorData)
{
	return (((float)SensorData->tValue.RawValue.s16 / 16.0F - 1180.0F) * 0.24F + 25.0F);
}

uint8_t GetHallSensorRegisters(TLE493D_BaseStruct *SensorData, bool Shortform)
{
	{
		uint8_t DataToWrite[1] = {TRIGGER_BITS_AfterRead};
		static uint8_t DataToRead[23];
		uint8_t State;

		if (!Shortform){
			State = AccessI2C(SensorData->BusChannel, SensorData->BusAddress, sizeof(DataToWrite), DataToWrite, sizeof(DataToRead), DataToRead, I2C_MODE_NOREPEAT_START);
			SensorData->XL_07H = DataToRead[7];
			SensorData->XH_08H = DataToRead[8];
			SensorData->YL_09H = DataToRead[9];
			SensorData->YH_0AH = DataToRead[10];
			SensorData->ZL_0BH = DataToRead[11];
			SensorData->ZH_0CH = DataToRead[12];
			SensorData->WU_0DH = DataToRead[13];
			SensorData->TMode_0EH = DataToRead[14];
			SensorData->TPhase_0FH = DataToRead[15];
			SensorData->Config_10H = DataToRead[16];
			SensorData->MOD1_11H = DataToRead[17];
			SensorData->Reserved_12H = DataToRead[18];
			SensorData->MOD2_13H = DataToRead[19];
			SensorData->Reserved_14H = DataToRead[20];
			SensorData->Reserved_15H = DataToRead[21];
			SensorData->Ver_16H = DataToRead[22];
		}
		else
		{
			State = AccessI2C(SensorData->BusChannel, SensorData->BusAddress, sizeof(DataToWrite), DataToWrite, 7, DataToRead, I2C_MODE_NOREPEAT_START);
		}
		SensorData->Bx_00H = DataToRead[0];
		SensorData->By_01H = DataToRead[1];
		SensorData->Bz_02H = DataToRead[2];
		SensorData->Temp_03H = DataToRead[3];
		SensorData->Bx2_04H = DataToRead[4];
		SensorData->Temp2_05H = DataToRead[5];
		SensorData->Diag_06H = DataToRead[6];

		return (State);
	}
}

void ResetHallSensors(uint8_t Channel)
{
	volatile uint16_t x;
	I2cForceBits(Channel, true, 8);
	I2cForceBits(Channel, true, 8);
	I2cForceBits(Channel, false, 8);
	I2cForceBits(Channel, false, 8);

	for (x = 0; x < 60; x)                     // Delay of at least 30us
	{
		x++;
	}
}

uint8_t GetMagnetSensor(TLE493D_BaseStruct *SensorData) {
#define MAX_ATTEMPTS (5)
#define MAX_RESTART_ALL (3 * MAX_ATTEMPTS)
	uint8_t state;
	state = GetHallSensorOnce(SensorData);

	if (state) // Error indicated
	{

		if (SensorData->retry >= MAX_RESTART_ALL)
		{
			ResetHallSensors(SensorData->BusChannel); // Affects all Infineon TLE493D Magnet Sensors on the bus!
		}
		if (SensorData->retry >= MAX_ATTEMPTS)
		{   // Something went wrong and we have to reinitalize the magnetSensor
			if (!IsMeterError(InitHallSensor(SensorData)))
			{
				SensorData->retry = 0;
				state = I2C_ERROR; //indicate that we had a general reset of the sensor
			}
		}
		else
		{
			SensorData->retry++;
			while (!SensorData->retry); //Infinite loop to trigger Watchdog and cause hardware reset in case of overflow
			if (state == FRAME_COUNTER_ERROR) // Count this Error but don't state it as it is not a real error as long it shows not up permanently
			{
				state = I2C_PRESENT;
			}
		}
	}
	else
	{
		SensorData->retry = 0;
		state = I2C_PRESENT;
	}
	return state;
 }



uint8_t GetHallSensorOnce(TLE493D_BaseStruct *SensorData) {
	uint8_t state;
	SI_UU16_t Value;
	uint8_t Parity;
	uint8_t ErrorCode = 0;
#if TOGGLE_CBUS_SENSOR == 1
	static bool togglePin = 0;
#endif

	state = GetHallSensorRegisters(SensorData, SENSOR_READ_SHORTFORM);
	if (state == I2C_PRESENT)
	{
		Parity = SensorData->Bx_00H;
		Parity ^= SensorData->By_01H;
		Parity ^= SensorData->Bz_02H;
		Parity ^= SensorData->Temp_03H;
		Parity ^= SensorData->Bx2_04H;
		Parity ^= SensorData->Temp2_05H;
	    Parity ^= Parity >> 4;
	    Parity ^= Parity >> 2;
	    Parity ^= Parity >> 1;
	    Parity &= 1;
	    if (SensorData->Diag_06H & MASK_Diag_06H_P == Parity)
		{
			ErrorCode += BUS_PARITY_ERROR;
		}
	    if (!(SensorData->Diag_06H & MASK_Diag_06H_PD0))
	    	ErrorCode += POWERDOWN_0_ERROR; 	// After Startup
		if (!(SensorData->Diag_06H & MASK_Diag_06H_PD3))
			ErrorCode += POWERDOWN_3_ERROR;     // After Startup
		if (SensorData->Diag_06H & MASK_Diag_06H_T)
			ErrorCode += TESTMODE_ERROR;     // After Startup
		if (!(SensorData->Diag_06H & MASK_Diag_06H_CF))
			ErrorCode += CONFIG_PARITY_ERROR;  	// After Startup
		if (!(SensorData->Diag_06H & MASK_Diag_06H_FF))
			ErrorCode += FUSE_PARITY_ERROR;		// Sensor defective
		if (!ErrorCode) // Update data only when there is no evidence of an error in values
		{
			Value.u8[0] = SensorData->Bx_00H;
			Value.u8[1] = SensorData->Bx2_04H & MASK_Bx2_04H_Bx;
			SensorData->xAxis.RawValue.u16 = Value.u16;
			if (SensorData->xAxis.SwapSign)
			{
				SensorData->xAxis.RawValue.s16 = (SensorData->xAxis.RawValue.s16 == -32768)? 32767 : -SensorData->xAxis.RawValue.s16;
			}

			Value.u8[0] = SensorData->By_01H;
			Value.u8[1] = (SensorData->Bx2_04H & MASK_Bx2_04H_By) << 4;
			SensorData->yAxis.RawValue.u16 = Value.u16;
			if (SensorData->yAxis.SwapSign)
			{
				SensorData->yAxis.RawValue.s16 = (SensorData->yAxis.RawValue.s16 == -32768)? 32767 : -SensorData->yAxis.RawValue.s16;
			}

			Value.u8[0] = SensorData->Bz_02H;
			Value.u8[1] = (SensorData->Temp2_05H & MASK_Temp2_05H_Bz) << 4;
			SensorData->zAxis.RawValue.u16 = Value.u16;
			if (SensorData->zAxis.SwapSign)
			{
				SensorData->zAxis.RawValue.s16 = (SensorData->zAxis.RawValue.s16 == -32768)? 32767 : -SensorData->zAxis.RawValue.s16;
			}

			Value.u8[0] = SensorData->Temp_03H;
			Value.u8[1] = (SensorData->Temp2_05H & MASK_Temp2_05H_Temp) << 4;
			SensorData->tValue.RawValue.u16 = Value.u16;

			if ((SensorData->Diag_06H & MASK_Diag_06H_FRM) == SensorData->LastFrameCounter)
			{
				ErrorCode += FRAME_COUNTER_ERROR;
			}
			else
			{
				SensorData->LastFrameCounter = SensorData->Diag_06H & MASK_Diag_06H_FRM;
#if TOGGLE_CBUS_SENSOR == 1
				TOGGLE_PIN = togglePin;
				togglePin = !togglePin;
#endif
			}
		}
	}
	else
	{
		ErrorCode += I2C_READ_ERROR;
	}

	return ErrorCode;
}

uint8_t InitHallSensor(TLE493D_BaseStruct *SensorData) {

	// Do not touch this code unless you understand the manual of TLE493D!

	//SI_UU32_t xdata Parity;

	uint8_t DataToWrite[14];
	static uint8_t DataToRead[1];
	uint8_t State;
	uint8_t Parity;
	SensorData->retry++;

	if (SensorData->FirstRun) {
		if (GetHallSensorRegisters(SensorData, SENSOR_READ_ALL_REGISTERS) != I2C_PRESENT) {
			return (I2C_ABSENT);
		}
		SensorData->LastFrameCounter = SensorData->Diag_06H & MASK_Diag_06H_FRM;
	}
	SensorData->MOD1_11H &= ~MASK_MOD1_11H_IICadr;
	switch (SensorData->BusAddress)
	{
	case SENSOR_TLE493D_W2B6_A0_ADDR:
		SensorData->MOD1_11H |= 0x00;
		break;
	case SENSOR_TLE493D_W2B6_A1_ADDR:
		SensorData->MOD1_11H |= 0x20;
		break;
	case SENSOR_TLE493D_W2B6_A2_ADDR:
		SensorData->MOD1_11H |= 0x40;
		break;
	case SENSOR_TLE493D_W2B6_A3_ADDR:
		SensorData->MOD1_11H |= 0x60;
		break;
	}

	// Prepare rest of MOD1 according usecase

	SensorData->MOD1_11H &= ~MASK_MOD1_11H_MODE;
	SensorData->MOD1_11H |= (SensorData->Mode & MASK_MOD1_11H_MODE);

	SensorData->MOD1_11H = (SensorData->InterruptDisabled) ? 	SensorData->MOD1_11H | MASK_MOD1_11H_INT :	SensorData->MOD1_11H & ~MASK_MOD1_11H_INT;


	SensorData->MOD1_11H = (SensorData->CollisionAvoidanceDisabled) ? SensorData->MOD1_11H | MASK_MOD1_11H_CA :	SensorData->MOD1_11H & ~MASK_MOD1_11H_CA;
	SensorData->MOD1_11H = (SensorData->ProtocolOneByteReadEnabled) ? SensorData->MOD1_11H | MASK_MOD1_11H_PR :	SensorData->MOD1_11H & ~MASK_MOD1_11H_PR;

	SensorData->MOD2_13H &= ~MASK_MOD2_13H_PRD;
	SensorData->MOD2_13H |= (SensorData->LowPowerPeriod & MASK_MOD2_13H_PRD);

	SensorData->Config_10H = (SensorData->DisableTemperature) ? SensorData->Config_10H | MASK_Config_10H_DT :	SensorData->Config_10H & ~MASK_Config_10H_DT;
	SensorData->Config_10H = (SensorData->Angular) ? SensorData->Config_10H | MASK_Config_10H_AM :	SensorData->Config_10H & ~MASK_Config_10H_AM;
	SensorData->Config_10H = (SensorData->ShortRangeSensitivity) ? SensorData->Config_10H | MASK_Config_10H_X2 :	SensorData->Config_10H & ~MASK_Config_10H_X2;

	SensorData->Config_10H &= ~MASK_Config_10H_TRIG;
	SensorData->Config_10H |= (SensorData->TriggerOption & MASK_Config_10H_TRIG);

	SensorData->Config_10H &= ~MASK_Config_10H_TL_mag;
	SensorData->Config_10H |= (SensorData->TempCompensation & MASK_Config_10H_TL_mag);

	Parity = SensorData->MOD1_11H  & ~MASK_MOD1_11H_FP;
	Parity ^= SensorData->MOD2_13H & ~MASK_MOD2_13H_Reserved;
    Parity ^= Parity >> 4;
    Parity ^= Parity >> 2;
    Parity ^= Parity >> 1;
    Parity &= 1;
    SensorData->MOD1_11H = (!Parity) ? SensorData->MOD1_11H | MASK_MOD1_11H_FP :	SensorData->MOD1_11H & ~MASK_MOD1_11H_FP; // odd parity (Fuse Parity)

	Parity = SensorData->XL_07H;
	Parity ^= SensorData->XH_08H;
	Parity ^= SensorData->YL_09H;
	Parity ^= SensorData->YH_0AH;
	Parity ^= SensorData->ZL_0BH;
	Parity ^= SensorData->ZH_0CH;
	Parity ^= SensorData->WU_0DH & ~MASK_WU_0DH_WA;
	Parity ^= SensorData->TMode_0EH & ~MASK_TMode_0EH_TST;
	Parity ^= SensorData->TPhase_0FH & ~MASK_TPhase_0FH_PH;
	Parity ^= SensorData->Config_10H & ~MASK_Config_10H_CP;
    Parity ^= Parity >> 4;
    Parity ^= Parity >> 2;
    Parity ^= Parity >> 1;
    Parity &= 1;
    SensorData->Config_10H = (!Parity) ? SensorData->Config_10H | MASK_Config_10H_CP :	SensorData->Config_10H & ~MASK_Config_10H_CP; // odd parity (Wakeup and Configuration Parity)
/*
    DataToWrite[0] = 0x07 | TRIGGER_BITS_AfterWrite; //Register to start with write and TriggerBits
    DataToWrite[1] = SensorData->XL_07H;
    DataToWrite[2] = SensorData->XH_08H;
    DataToWrite[3] = SensorData->YL_09H;
    DataToWrite[4] = SensorData->YH_0AH;
    DataToWrite[5] = SensorData->ZL_0BH;
    DataToWrite[6] = SensorData->ZH_0CH;
    DataToWrite[7] = SensorData->WU_0DH;
    DataToWrite[8] = SensorData->TMode_0EH;
    DataToWrite[9] = SensorData->TPhase_0FH;
    DataToWrite[10] = SensorData->Config_10H;
    DataToWrite[11] = SensorData->MOD1_11H;
    DataToWrite[12] = SensorData->Reserved_12H;
    DataToWrite[13] = SensorData->MOD2_13H;
	State = AccessI2C(SensorData->BusChannel, SensorData->BusAddress, sizeof(DataToWrite), DataToWrite, 0, DataToRead, NOREPEAT_START);
*/

    DataToWrite[0] = 0x10 | TRIGGER_BITS_NoAdcTrigger; //Register to start with write and TriggerBits
    DataToWrite[1] = SensorData->Config_10H;
    DataToWrite[2] = SensorData->MOD1_11H;
    DataToWrite[3] = SensorData->Reserved_12H;
    DataToWrite[4] = SensorData->MOD2_13H;

	State = AccessI2C(SensorData->BusChannel, SensorData->BusAddress, 5, DataToWrite, 0, DataToRead, I2C_MODE_NOREPEAT_START);

	if (!State)
		SensorData->FirstRun = false;
	return (State);
}



