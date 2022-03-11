/*
 * FramDriverMB85RC64TA.c
 *********************************************
 *    (c)2018-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// MB85RC64TA (64K (8K x 8) Bit FRAM Memory) Driver Lib


#include "FramDriverMB85RC64TA.h"
#include "I2cDispatcher.h"
#include "HalDef.h"
#include "CRC.h"

uint8_t FramWrite(uint8_t Channel, uint8_t BaseAddress, uint16_t Length, uint16_t Offset, uint8_t *Data)
{
	uint8_t state = I2C_PRESENT;
	uint16_t index = 0;

	if (Offset >= FRAM_SIZE)
	{
		state = I2C_INCONSISTENCY;
		Offset = FRAM_SIZE;
	}

	if (Length == 0)
	{
		return I2C_INCONSISTENCY;
	}

	if (Length > FRAM_SIZE)
	{
		state = I2C_INCONSISTENCY;
		Length = FRAM_SIZE;
	}

	if (Offset + Length >= FRAM_SIZE)
	{
		state = I2C_INCONSISTENCY;
		Length = FRAM_SIZE - Offset;
	}
	I2C_State = AccessI2C(Channel, BaseAddress, sizeof(Offset), (uint8_t*) &Offset, Length, Data, I2C_MODE_WRITE_ALL_SECTIONS);

	if (I2C_State == I2C_PRESENT)
	{
		return state;
	}
	return I2C_State;
}

uint8_t FramRead(uint8_t Channel, uint8_t BaseAddress, uint16_t Length, uint16_t Offset, uint8_t *Data)
{
	uint8_t state = I2C_PRESENT;
	if (Offset >= FRAM_SIZE)
	{
		state = I2C_INCONSISTENCY;
		Offset = FRAM_SIZE;
	}

	if (Length == 0)
	{
		return I2C_INCONSISTENCY;
	}

	if (Length > FRAM_SIZE)
	{
		state = I2C_INCONSISTENCY;
		Length = FRAM_SIZE;
	}

	if (Offset + Length >= FRAM_SIZE)
	{
		state = I2C_INCONSISTENCY;
		Length = FRAM_SIZE - Offset;
	}
	I2C_State = AccessI2C(Channel, BaseAddress, sizeof(Offset), (uint8_t*) &Offset, Length, Data, I2C_MODE_MULTIPLE_START);
//	I2C_State = ReadBytesI2C(BASE_ADDR_FRAM, Offset, Length, Data);
	if (I2C_State == I2C_PRESENT)
	{
		return state;
	}
	return I2C_State;
}



uint16_t BiStoreSettingFRAM(uint8_t Channel, uint8_t BaseAddress, uint16_t FramPos, uint8_t *ram, uint16_t numbytes, bool StoreToFram, uint16_t *CRC16)
{
   if (StoreToFram)
   {
   	   if (FramWrite(Channel, BaseAddress, numbytes, FramPos, ram))
   		   return 0;
   }
   else
   {
   	   if (FramRead(Channel, BaseAddress, numbytes, FramPos, ram))
			return 0;
   }

   * CRC16 = calcCRC16Array(numbytes, ram, * CRC16);
   return(FramPos + numbytes); // return starting point of next address in Settings
}


bool ClearAllSettingFram(uint8_t Channel, uint8_t BaseAddress, uint8_t FramPos)
{
   uint8_t dataB;

	if (FramRead(Channel, BaseAddress, 2, FramPos, &dataB))
			return false;

	if (dataB == 0x00)
		return false;

	dataB = 0x00;
    if (FramWrite(Channel, BaseAddress, 2, FramPos, &dataB))
   		   return false;
	return true;
}

