/*
 * MagentNodeMLX90395.c
 *
 *  Created on: 05.05.2020
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include "MagnetNodeMLX90395.h"
#include "I2cDispatcher.h"
#include "HalDef.h"

/* Initializing the master */

volatile struct NotchPos xdata Positions[24];

float CalcMicroTeslaMeter(MLX90395_BaseStruct *MeterData, uint16_t VectorValue) {
	float xdata target = 0;
	static const float code uTfactor[8] = {0.805, 0.644, 0.483, 0.403, 0.322, 0.268, 0.215, 0.161};

	target = uTfactor[(MeterData->Gain & 0x07)];
	switch (MeterData->Resolution) {
	case VALUE_RES_0:
		target *= 1;
		break;
	case VALUE_RES_1:
		target *= 2;
		break;
	case VALUE_RES_2:
		target *= 4;
		break;
	case VALUE_RES_3:
		target *= 8;
		break;
	default:
		target *= 0;
		break;
	}
	return (target * VectorValue);
}


float CalcCelsiusDegreeMeter(MLX90395_BaseStruct *MeterData) {
    return (MeterData->Temp.RawValue.s16 / 50.0f);
}

uint8_t GetMagnetometer(MLX90395_BaseStruct *MeterData) {
#define MAX_ATTEMPTS (5)
	uint8_t ErrorState;
#if TOGGLE_CBUS_METERINIT == 1
	static bool togglePin = 0;
#endif

	ErrorState = GetMeterOnceI2C(MeterData);
	if (ErrorState) // Error indicated
	{
		if (MeterData->retry >= MAX_ATTEMPTS)
		{
#if TOGGLE_CBUS_METERINIT == 1
			TOGGLE_PIN = togglePin;
		    togglePin = !togglePin;
#endif
			if (!InitMeterI2C(MeterData)) // Something went wrong and we have to reinitalize the magnetometer
			{
				MeterData->retry = 0; //if sometheing went wrong again, we reinitalize next time again, otherwise we
			}// in worst case we end up in a watchdog-reset as we're spinning forever here
		}
		else
		{
			MeterData->retry++;  // increment here for any error indicated

			if ((MeterData->retry < 2 ) && (ErrorState & (METER_MEAS_COUNTER_ERROR + METER_NOT_READY_ERROR) == ErrorState)) // but if a non-fatal error occurs
			{
				ErrorState = I2C_PRESENT; // we hide a minimal amount of conflicts caused from oversampling, as this is desired for neatless data-delivery
			}

		}
	}
	else
	{
		MeterData->retry = 0;
	}
	if (!(MeterData->State & STATE_BURST)) // we have to start the next conversion if we're not already in burstmode
	{
		SetOperationMeter(MeterData); // If we fail here, we see it the next time we use GetMagentometer(), so no evaluation of errors here
	}
	return ErrorState;
 }

uint8_t GetMeterOnceI2C(MLX90395_BaseStruct *MeterData) {

	static const uint8_t code CRCTable[256] =
	{
		 0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,   //line 00
		 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,   //line 01
		 0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,   //line 02
		 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,   //line 03
		 0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,   //line 04
		 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,   //line 05
		 0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,   //line 06
		 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,   //line 07
		 0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,   //line 08
		 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,   //line 09
		 0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,   //line 10
		 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,   //line 11
		 0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,   //line 12
		 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,   //line 13
		 0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,   //line 14
		 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,   //line 15
		 0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,   //line 16
		 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,   //line 17
		 0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,   //line 18
		 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,   //line 19
		 0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,   //line 20
		 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,   //line 21
		 0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,   //line 22
		 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,   //line 23
		 0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,   //line 24
		 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,   //line 25
		 0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,   //line 26
		 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,   //line 27
		 0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,   //line 28
		 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,   //line 29
		 0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,   //line 30
		 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3    //line 31
	};


	uint8_t crc = 0;
	uint8_t index;
	uint8_t ErrorCode = METER_NO_ERROR;
#if TOGGLE_CBUS_METER == 1
	static bool togglePin = 0;
#endif
	DataToWrite[0] = MeterData->ReadMode; // command to Write
	I2C_State =  AccessI2C(MeterData->BusChannel,MeterData->BusAddress, 1, DataToWrite, 12, DataToRead, I2C_MODE_MULTIPLE_START);
	if (I2C_State == I2C_PRESENT)
	{
		MeterData->State = DataToRead[0];
		MeterData->Crc = DataToRead[1];


		if (MeterData->State & STATE_SEC_OVF)
		{
			ErrorCode += METER_OVERFLOW_ERROR;
		}

		if (MeterData->State & STATE_CE_DED)
		{
			ErrorCode += METER_COMMUNICAION_ERROR;
		}

		if (!ErrorCode)
		{
			if (!(MeterData->State & STATE_DRDY))
			{
				ErrorCode += METER_NOT_READY_ERROR; // // non-fatal overrun-error (reads same data as last time the proper way)
			}

			if (MeterData->State & STATE_BURST)
			{
				if ((MeterData->State & (STATE_CNT0 + STATE_SM_CNT1 + STATE_WOC_CNT2)) == MeterData->LastFrameCounter)
				{
					ErrorCode += METER_MEAS_COUNTER_ERROR; // non-fatal overrun-error (reads same data as last time the proper way)
				}
				else
				{
					MeterData->LastFrameCounter =  MeterData->State & (STATE_CNT0 + STATE_SM_CNT1 + STATE_WOC_CNT2);
				}
			}
			for (index = 2; index < 12; index++)
			{
				crc = CRCTable[crc ^ DataToRead[index]];
			}
			if (crc == MeterData->Crc) // if CRC is verified to be good
			{	// actualize data only when verified to be vaild
				MeterData->xAxis.RawValue.u8[0] = DataToRead[2];
				MeterData->xAxis.RawValue.u8[1] = DataToRead[3];

				if (MeterData->xAxis.SwapSign)
				{
					MeterData->xAxis.RawValue.s16 = (MeterData->xAxis.RawValue.s16 == -32768)? 32767 : -MeterData->xAxis.RawValue.s16;
				}
				MeterData->yAxis.RawValue.u8[0] = DataToRead[4];
				MeterData->yAxis.RawValue.u8[1] = DataToRead[5];
				if (MeterData-> yAxis.SwapSign)
				{
					MeterData->yAxis.RawValue.s16 = (MeterData->yAxis.RawValue.s16 == -32768)? 32767 : -MeterData->yAxis.RawValue.s16;
				}
				MeterData->zAxis.RawValue.u8[0] = DataToRead[6];
				MeterData->zAxis.RawValue.u8[1] = DataToRead[7];
				if (MeterData->zAxis.SwapSign)
				{
					MeterData->zAxis.RawValue.s16 = (MeterData->zAxis.RawValue.s16 == -32768)? 32767 : -MeterData->zAxis.RawValue.s16;
				}
				MeterData->Temp.RawValue.u8[0] = DataToRead[8];
				MeterData->Temp.RawValue.u8[1] = DataToRead[9];
				MeterData->Voltage.u8[0] = DataToRead[10];
				MeterData->Voltage.u8[1] = DataToRead[11];
#if TOGGLE_CBUS_METER == 1
				TOGGLE_PIN = togglePin;
		    	togglePin = !togglePin;
#endif
			}
			else
			{
				ErrorCode += METER_CRC_ERROR;
			}
		}
	}
	else
	{
		ErrorCode += METER_I2C_READ_ERROR;
	}
	return ErrorCode;
}

uint8_t SetOperationMeter(MLX90395_BaseStruct *MeterData)
{
	DataToWrite[0] = REGISTER_80; 	// Register
	DataToWrite[1] = MeterData->OperationMode; 	 	// Command: Operation mode

	if (!AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 2, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START))
	{
		MeterData->State = DataToRead[0];
		return MeterData->State & (STATE_SEC_OVF + STATE_CE_DED);
	}
	return I2C_ABSENT;
}

uint8_t CheckForMeterI2C(MLX90395_BaseStruct *MeterData)
{
	DataToWrite[0] = REGISTER_80; 	// Register
	DataToWrite[1] = METER_EX; 	 	// Command: Exit
	return AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 2, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START);
}

uint8_t InitMeterI2C(MLX90395_BaseStruct *MeterData) {
	SI_UU16_t xdata D_Value;
	uint8_t ErrorState = I2C_PRESENT;

	MeterData->retry++;
	DataToWrite[0] = REGISTER_80; 	// Register
	DataToWrite[1] = METER_EX; 	 	// Command: Exit
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 2, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;
	LoopDelay3ms(); // Delay for Melexis to accomplish Power-up Reset

	DataToWrite[1] = METER_RT; 	 	// Command: Reset
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 2, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;
	LoopDelay3ms(); // Delay for Melexis to accomplish Power-up Reset

	DataToWrite[0] = UID_ADDR_26h_I2C; //METER_RR
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 1, DataToWrite, 6, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;
	MeterData->ID1.u8[0] = DataToRead[0];
	MeterData->ID1.u8[1] = DataToRead[1];
	MeterData->ID2.u8[0] = DataToRead[2];
	MeterData->ID2.u8[1] = DataToRead[3];
	MeterData->ID3.u8[0] = DataToRead[4];
	MeterData->ID3.u8[1] = DataToRead[5];

	DataToWrite[0] = CALIB_ADDR_00h_I2C; //METER_RR
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 1, DataToWrite, 2, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;
	//D_Value.u16 = 0x005C; // The Default for this configuration Register0
	D_Value.u8[0] = DataToRead[0];
	D_Value.u8[1] = DataToRead[1];
	D_Value.u16 &= ~MASK_GAIN;
	D_Value.u16 |= (MASK_GAIN & (((uint16_t) MeterData->Gain) << 4));
	DataToWrite[1] = D_Value.u8[0];
	DataToWrite[2] = D_Value.u8[1];
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 3, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;//METER_WR

	DataToWrite[0] = CALIB_ADDR_01h_I2C; //METER_RR
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 1, DataToWrite, 2, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;
	//D_Value.u16 = 0xE400; // The Default for this configuration Register1 (I2C-Mode Only!)
	D_Value.u8[0] = DataToRead[0];
	D_Value.u8[1] = DataToRead[1];
	D_Value.u16 &= ~MASK_TCMP_EN;
	D_Value.u16 |= (MASK_TCMP_EN & (((uint16_t) MeterData->AutoTempCompensation) << 10));
	DataToWrite[1] = D_Value.u8[0];
	DataToWrite[2] = D_Value.u8[1];
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 3, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;

	DataToWrite[0] = CALIB_ADDR_02h_I2C; //METER_RR
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 1, DataToWrite, 2, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;
    //D_Value.u16 = 0x03DA; // The Default for this configuration Register2
	D_Value.u8[0] = DataToRead[0];
	D_Value.u8[1] = DataToRead[1];
	D_Value.u16 &= ~MASK_RES;
	D_Value.u16 |= (MASK_RES & (((uint16_t) MeterData->Resolution) << 5));
	D_Value.u16 &= ~MASK_FILTER;
	D_Value.u16 |= (MASK_FILTER & (((uint16_t) MeterData->Filter) << 2));
	D_Value.u16 &= ~MASK_OSR;
	D_Value.u16 |= (MASK_OSR & (((uint16_t) MeterData->OSR) << 0));
	DataToWrite[1] = D_Value.u8[0];
	DataToWrite[2] = D_Value.u8[1];
	ErrorState = (AccessI2C(MeterData->BusChannel, MeterData->BusAddress, 3, DataToWrite, 1, DataToRead, I2C_MODE_MULTIPLE_START))? I2C_ABSENT : ErrorState;//METER_WR
	ErrorState = (SetOperationMeter(MeterData))? I2C_ABSENT : ErrorState;
	return ErrorState;
}

bool IsMeterReset(uint8_t state) {
	if (state != I2C_ERROR) {
		return ((state & STATE_RST) ? true : false);
	}
	return (false);
}
bool IsMeterBurstMode(uint8_t state) {
	if (state != I2C_ERROR) {
		return ((state & STATE_BURST) ? true : false);
	}
	return (false);
}
bool IsMeterSingleMode(uint8_t state) {
	if (state != I2C_ERROR) {
		return ((state & STATE_SM_CNT1) ? true : false);
	}
	return (false);
}
bool IsMeterWOCMode(uint8_t state) {
	if (state != I2C_ERROR) {
		return ((state & STATE_WOC_CNT2) ? true : false);
	}
	return (false);
}
bool IsMeterError(uint8_t state) {
	return ((state & STATE_CE_DED) ? true : false);
}
bool IsMeterAbsent(uint8_t state) {
	if (state != I2C_ERROR) {
		return (false);
	}
	return (true);
}

