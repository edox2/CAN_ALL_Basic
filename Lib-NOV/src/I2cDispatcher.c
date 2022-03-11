/*
 * I2cBang.c
 *
 *  Created on: 24.06.2019
 *********************************************
 *    (c)2016-2019 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include "I2cDispatcher.h"
#include "HalDef.h"

volatile uint8_t xdata I2C_State = 0xf0;
volatile uint8_t DataToWrite[I2C_BUFFERSIZE];
volatile uint8_t DataToRead[I2C_BUFFERSIZE];


//-----------------------------------------------------------------------------
// I2C-Bus related little helpers
//-----------------------------------------------------------------------------
//volatile uint8_t xdata I2C_State = 0xf0;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void I2cDelay(void)
{
	volatile uint16_t x;
	for (x = 0; x < DelayTime4I2C; x)
		x++;
}

#ifndef FAST_SINGLE_CH

//-----------------------------------------------------------------------------
void I2C_SCL_Set(uint8_t Channel)
{
	switch (Channel)
	{
	case 0:
		I2C_SCL_Ch0 = 1;
		break;
	case 1:
		I2C_SCL_Ch1 = 1;
		break;
	case 2:
		I2C_SCL_Ch2 = 1;
		break;
	case 3:
		I2C_SCL_Ch3 = 1;
		break;

	}
}

//-----------------------------------------------------------------------------
void I2C_SCL_Clr(uint8_t Channel)
{
	switch (Channel)
	{
	case 0:
		I2C_SCL_Ch0 = 0;
		break;
	case 1:
		I2C_SCL_Ch1 = 0;
		break;
	case 2:
		I2C_SCL_Ch2 = 0;
		break;
	case 3:
		I2C_SCL_Ch3 = 0;
		break;
	}
}

//-----------------------------------------------------------------------------
void I2C_SDA_Set(uint8_t Channel)
{
	switch (Channel)
	{
	case 0:
		I2C_SDA_Ch0 = 1;
		break;
	case 1:
		I2C_SDA_Ch1 = 1;
		break;
	case 2:
		I2C_SDA_Ch2 = 1;
		break;
	case 3:
		I2C_SDA_Ch3 = 1;
		break;
	}
}

//-----------------------------------------------------------------------------
void I2C_SDA_Clr(uint8_t Channel)
{
	switch (Channel)
	{
	case 0:
		I2C_SDA_Ch0 = 0;
		break;
	case 1:
		I2C_SDA_Ch1 = 0;
		break;
	case 2:
		I2C_SDA_Ch2 = 0;
		break;
	case 3:
		I2C_SDA_Ch3 = 0;
		break;
	}
}

//-----------------------------------------------------------------------------
uint8_t I2C_SCL_Get(uint8_t Channel)
{
	uint8_t result;
	switch (Channel)
	{
	case 0:
		result = I2C_SCL_Ch0;
		break;
	case 1:
		result = I2C_SCL_Ch1;
		break;
	case 2:
		result = I2C_SCL_Ch2;
		break;
	case 3:
		result = I2C_SCL_Ch3;
		break;
	}
	return result;
}

//-----------------------------------------------------------------------------
uint8_t I2C_SDA_Get(uint8_t Channel)
{
	uint8_t result;
	switch (Channel)
	{
	case 0:
		result = I2C_SDA_Ch0;
		break;
	case 1:
		result = I2C_SDA_Ch1;
		break;
	case 2:
		result = I2C_SDA_Ch2;
		break;
	case 3:
		result = I2C_SDA_Ch3;
		break;
	}
	return result;
}

//-----------------------------------------------------------------------------
void I2cForceBits(uint8_t Channel, bool BitValue, uint8_t BitCount)
{
	I2cDelay();				//Start
	I2C_SCL_Set(Channel);
	I2cDelay();
	I2C_SDA_Set(Channel);
	I2cDelay();
	I2C_SDA_Clr(Channel);
	I2cDelay();
	I2C_SCL_Clr(Channel);
	I2cDelay();

	do {					// Number of DataBits
		if (BitValue)
			I2C_SDA_Set(Channel);
		else
			I2C_SDA_Clr(Channel);
		I2cDelay();
		I2C_SCL_Set(Channel);
		I2cDelay();
		I2C_SCL_Clr(Channel);
		I2cDelay();
		BitCount--;
	} while (BitCount);
	I2C_SDA_Set(Channel);

	I2C_SDA_Clr(Channel); 	// Stop
	I2cDelay();
	I2C_SCL_Set(Channel);
	I2cDelay();
	I2C_SDA_Set(Channel);
	I2cDelay();
}



//-----------------------------------------------------------------------------
void I2cStart(uint8_t Channel)   // S-Phase
{
	I2cDelay();
	I2C_SCL_Set(Channel);
	while (!I2C_SCL_Get(Channel)); // clock stretching
	I2cDelay();
	I2C_SDA_Set(Channel);
	I2cDelay();
	I2C_SDA_Clr(Channel);
	I2cDelay();
	I2C_SCL_Clr(Channel);
	I2cDelay();
}
//-----------------------------------------------------------------------------
void I2cStop(uint8_t Channel)   // P-Phase
{
	I2C_SDA_Clr(Channel);
	I2cDelay();
	I2C_SCL_Set(Channel);
	I2cDelay();
	I2C_SDA_Set(Channel);
	while (!I2C_SCL_Get(Channel)); // clock stretching
	I2cDelay();
}

//-----------------------------------------------------------------------------
void I2cSendAck(uint8_t Channel)   // A-Phase
{
	I2C_SCL_Clr(Channel);
	I2cDelay();
	I2C_SDA_Clr(Channel);
	I2cDelay();
	I2C_SCL_Set(Channel);
	while (!I2C_SCL_Get(Channel)); // clock stretching
	I2cDelay();
	I2C_SCL_Clr(Channel);
	I2cDelay();
}

//-----------------------------------------------------------------------------
void I2cSendNack(uint8_t Channel)   // A-Phase
{
	  I2C_SCL_Clr(Channel);
	  I2cDelay();
	  I2C_SDA_Set(Channel);
	  I2cDelay();
	  I2C_SCL_Set(Channel);
	  while (!I2C_SCL_Get(Channel)); // clock stretching
	  I2cDelay();
	  I2C_SCL_Clr(Channel);
	  I2cDelay();
}

//-----------------------------------------------------------------------------
uint8_t I2cGetAck(uint8_t Channel)    // A-Phase
{
  uint16_t  i;
  I2C_SCL_Set(Channel);
  for (i = 0; i <= DelayTime4I2C ; i++)
  {
    if (I2C_SDA_Get(Channel) == 0)
      i = DelayTime4I2C;
  }
  while (!I2C_SCL_Get(Channel)); // clock stretching
  I2cDelay();
  i = (I2C_SDA_Get(Channel) == 0)?1:0;
  I2C_SCL_Clr(Channel);
  I2cDelay();
  return (i);
}

//-----------------------------------------------------------------------------
void I2cWrite(uint8_t Channel, uint8_t Data)   // W-Phase
{
	uint8_t i = 0x80;
	do {
		//I2C_SDA = (Data & i) ? 1 : 0
		if (Data & i)
			I2C_SDA_Set(Channel);
		else
			I2C_SDA_Clr(Channel);
		I2cDelay();
		I2C_SCL_Set(Channel);
		while (!I2C_SCL_Get(Channel))
			; // clock stretching
		I2cDelay();
		I2C_SCL_Clr(Channel);
		I2cDelay();
		i >>= 1;
	} while (i);
	I2C_SDA_Set(Channel);
}

//-----------------------------------------------------------------------------
uint8_t I2cRead(uint8_t Channel)  // R-Phase
{

	uint8_t i = 0x80;
	uint8_t Data = 0;

	I2C_SDA_Set(Channel);
	do {
		I2cDelay();
		I2C_SCL_Set(Channel);
		while (!I2C_SCL_Get(Channel))
			; // clock stretching
		I2cDelay();
		Data |= i & ((I2C_SDA_Get(Channel) == 0) ? 0 : 0xFF);
		I2C_SCL_Clr(Channel);
		i >>= 1;
	} while (i);
	return (Data);
}


//------------------------------------------------------------------------------------------------

uint8_t AccessI2C(uint8_t Channel, uint8_t Address, uint16_t LengthWrite, uint8_t *DataToWriteOnly, uint16_t LengthReadOrWrite, uint8_t *DataToReadOrWrite, uint8_t MultipleStartMode)
{
	uint16_t Offset;

	//Pos.u16 = Position;
	I2cStart(Channel);
	I2cWrite(Channel, ((MultipleStartMode == I2C_MODE_NOREPEAT_START) && LengthReadOrWrite)? Address | READ_FLAG : Address); // Address to Write

	if (I2cGetAck(Channel) == 0) {
		I2cStop(Channel);
		return (I2C_ABSENT);
	}

	for (Offset = 0; Offset < LengthWrite; Offset++)
	{
		I2cWrite(Channel, *(DataToWriteOnly + Offset)); // command to Write     // W-Phase
		if (I2cGetAck(Channel) == 0) {
			I2cStop(Channel);                            // P-Phase
			return (I2C_ABSENT);
		} // A-Phase
	}

	if (MultipleStartMode == I2C_MODE_WRITE_ALL_SECTIONS){ // unusual case where two write buffers are more efficient
		for (Offset = 0; Offset < LengthReadOrWrite; Offset++)
		{
			I2cWrite(Channel, *(DataToReadOrWrite + Offset)); // command to Write     // W-Phase
			if (I2cGetAck(Channel) == 0) {
				I2cStop(Channel);                            // P-Phase
				return (I2C_ABSENT);
			} // A-Phase
		}
	}
	else
	{
		if (LengthReadOrWrite) // usual case where ReadWrite Buffer is intended to read data
		{
			if (MultipleStartMode == I2C_MODE_MULTIPLE_START)
			{
				I2cStart(Channel);                               // SR-Phase
				I2cWrite(Channel, Address | READ_FLAG); // Address // W-Phase
				if (I2cGetAck(Channel) == 0){
					  I2cStop(Channel);                          // P-Phase
					  return(I2C_ABSENT);
				} // A-Phase
			}
			for (Offset = 0; Offset < LengthReadOrWrite; Offset++)
			{
				*(DataToReadOrWrite + Offset) = I2cRead(Channel); // Read Data     // R-Phase
				if (Offset + 1 == LengthReadOrWrite)
				{
					I2cSendNack(Channel);
				}
				else
				{
					I2cSendAck(Channel);
				}
			}
		}
	}


	I2cStop(Channel);                                // P-Phase
	return (I2C_PRESENT);
}
#else

//-----------------------------------------------------------------------------
void I2C_SCL_Set(void)
{
		I2C_SCL_Ch0 = 1;
}

//-----------------------------------------------------------------------------
void I2C_SCL_Clr(void)
{
		I2C_SCL_Ch0 = 0;
}

//-----------------------------------------------------------------------------
void I2C_SDA_Set(void)
{
		I2C_SDA_Ch0 = 1;
}

//-----------------------------------------------------------------------------
void I2C_SDA_Clr(void)
{
		I2C_SDA_Ch0 = 0;
}

//-----------------------------------------------------------------------------
uint8_t I2C_SCL_Get(void)
{
	return I2C_SCL_Ch0;
}

//-----------------------------------------------------------------------------
uint8_t I2C_SDA_Get(void)
{
	return I2C_SDA_Ch0;
}

//-----------------------------------------------------------------------------
void I2cForceBits(uint8_t Channel, bool BitValue, uint8_t BitCount)
{
	if (Channel) return; // Only Channel0 supported in this fast version



	I2cDelay();				//Start
	I2C_SCL_Ch0 = 1;
	I2cDelay();
	I2C_SDA_Ch0 = 1;
	I2cDelay();
	I2C_SDA_Ch0 = 0;
	I2cDelay();
	I2C_SCL_Ch0 = 0;
	I2cDelay();

	do {					// Number of DataBits
		if (BitValue)
			I2C_SDA_Ch0 = 1;
		else
			I2C_SDA_Ch0 = 0;
		I2cDelay();
		I2C_SCL_Ch0 = 1;
		I2cDelay();
		I2C_SCL_Ch0 = 0;
		I2cDelay();
		BitCount--;
	} while (BitCount);
	I2C_SDA_Ch0 = 1;

	I2C_SDA_Ch0 = 0; 	// Stop
	I2cDelay();
	I2C_SCL_Ch0 = 1;
	I2cDelay();
	I2C_SDA_Ch0 = 1;
	I2cDelay();
}



//-----------------------------------------------------------------------------
void I2cStart(void)   // S-Phase
{
	I2cDelay();
	I2C_SCL_Ch0 = 1;
	while (!I2C_SCL_Ch0 ); // clock stretching
	I2cDelay();
	I2C_SDA_Ch0 = 1;
	I2cDelay();
	I2C_SDA_Ch0 = 0;
	I2cDelay();
	I2C_SCL_Ch0 = 0;
	I2cDelay();
}
//-----------------------------------------------------------------------------
void I2cStop(void)   // P-Phase
{
	I2C_SDA_Ch0 = 0;
	I2cDelay();
	I2C_SCL_Ch0 = 1;
	I2cDelay();
	I2C_SDA_Ch0 = 1;
	while (!I2C_SCL_Ch0 ); // clock stretching
	I2cDelay();
}

//-----------------------------------------------------------------------------
void I2cSendAck(void)   // A-Phase
{
	I2C_SCL_Ch0 = 0;
	I2cDelay();
	I2C_SDA_Ch0 = 0;
	I2cDelay();
	I2C_SCL_Ch0 = 1;
	while (!I2C_SCL_Ch0 ); // clock stretching
	I2cDelay();
	I2C_SCL_Ch0 = 0;
	I2cDelay();
}

//-----------------------------------------------------------------------------
void I2cSendNack(void)   // A-Phase
{
	  I2C_SCL_Ch0 = 0;
	  I2cDelay();
	  I2C_SDA_Ch0 = 1;
	  I2cDelay();
	  I2C_SCL_Ch0 = 1;
	  while (!I2C_SCL_Ch0 ); // clock stretching
	  I2cDelay();
	  I2C_SCL_Ch0 = 0;
	  I2cDelay();
}

//-----------------------------------------------------------------------------
uint8_t I2cGetAck(void)    // A-Phase
{
  uint16_t  i;
  I2C_SCL_Ch0 = 1;
  for (i = 0; i <= DelayTime4I2C ; i++)
  {
    if (I2C_SDA_Ch0  == 0)
      i = DelayTime4I2C;
  }
  while (!I2C_SCL_Ch0 ); // clock stretching
  I2cDelay();
  i = (I2C_SDA_Ch0  == 0)?1:0;
  I2C_SCL_Ch0 = 0;
  I2cDelay();
  return (i);
}

//-----------------------------------------------------------------------------
void I2cWrite(uint8_t Data)   // W-Phase
{
	uint8_t i = 0x80;
	do {
		//I2C_SDA = (Data & i) ? 1 : 0
		if (Data & i)
			I2C_SDA_Ch0 = 1;
		else
			I2C_SDA_Ch0 = 0;
		I2cDelay();
		I2C_SCL_Ch0 = 1;
		while (!I2C_SCL_Ch0 )
			; // clock stretching
		I2cDelay();
		I2C_SCL_Ch0 = 0;
		I2cDelay();
		i >>= 1;
	} while (i);
	I2C_SDA_Ch0 = 1;
}

//-----------------------------------------------------------------------------
uint8_t I2cRead(void)  // R-Phase
{

	uint8_t i = 0x80;
	uint8_t Data = 0;

	I2C_SDA_Ch0 = 1;
	do {
		I2cDelay();
		I2C_SCL_Ch0 = 1;
		while (!I2C_SCL_Ch0 )
			; // clock stretching
		I2cDelay();
		Data |= i & ((I2C_SDA_Ch0  == 0) ? 0 : 0xFF);
		I2C_SCL_Ch0 = 0;
		i >>= 1;
	} while (i);
	return (Data);
}


//------------------------------------------------------------------------------------------------

uint8_t AccessI2C(uint8_t Channel, uint8_t Address, uint16_t LengthWrite, uint8_t *DataToWriteOnly, uint16_t LengthReadOrWrite, uint8_t *DataToReadOrWrite, uint8_t MultipleStartMode)
{
	uint16_t Offset;

	if (Channel) // Only Channel0 supported in this fast version
		return (I2C_ABSENT);
	I2cStart();
	I2cWrite(((MultipleStartMode == I2C_MODE_NOREPEAT_START) && LengthReadOrWrite)? Address | READ_FLAG : Address); // Address to Write

	if (I2cGetAck() == 0) {
		I2cStop();
		return (I2C_ABSENT);
	}

	for (Offset = 0; Offset < LengthWrite; Offset++)
	{
		I2cWrite( *(DataToWriteOnly + Offset)); // command to Write     // W-Phase
		if (I2cGetAck() == 0) {
			I2cStop();                            // P-Phase
			return (I2C_ABSENT);
		} // A-Phase
	}

	if (MultipleStartMode == I2C_MODE_WRITE_ALL_SECTIONS){ // unusual case where two write buffers are more efficient
		for (Offset = 0; Offset < LengthReadOrWrite; Offset++)
		{
			I2cWrite(*(DataToReadOrWrite + Offset)); // command to Write     // W-Phase
			if (I2cGetAck() == 0) {
				I2cStop();                            // P-Phase
				return (I2C_ABSENT);
			} // A-Phase
		}
	}
	else
	{
		if (LengthReadOrWrite) // usual case where ReadWrite Buffer is intended to read data
		{
			if (MultipleStartMode == I2C_MODE_MULTIPLE_START)
			{
				I2cStart();                               // SR-Phase
				I2cWrite( Address | READ_FLAG); // Address // W-Phase
				if (I2cGetAck() == 0){
					  I2cStop();                          // P-Phase
					  return(I2C_ABSENT);
				} // A-Phase
			}
			for (Offset = 0; Offset < LengthReadOrWrite; Offset++)
			{
				*(DataToReadOrWrite + Offset) = I2cRead(); // Read Data     // R-Phase
				if (Offset + 1 == LengthReadOrWrite)
				{
					I2cSendNack();
				}
				else
				{
					I2cSendAck();
				}
			}
		}
	}


	I2cStop();                                // P-Phase
	return (I2C_PRESENT);
}
#endif


