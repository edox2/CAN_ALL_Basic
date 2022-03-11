/*
 * SpiBang.c
 *
 *  Created on: 14.02.2017
 *********************************************
 *    (c)2016-2017 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include "SpiBang.h"
#include "HalDef.h"

//-----------------------------------------------------------------------------
// SPI-Bus related little helpers
//-----------------------------------------------------------------------------

uint8_t ReadWriteByteSPI(uint8_t DataToSend) {
	uint8_t xdata index;
	uint8_t xdata DataRead = 0;

	for (index = 0x80; index > 0;) {
		SCLK_IO = 0;
		if (DataToSend & index)
			MOSI_IO = 1;
		else
			MOSI_IO = 0;
		SCLK_IO = 1;
		DataRead |= index & (((MISO_IO) == 0) ? 0 : 0xFF);
		index >>= 1;
	}
	return (DataRead);
}

//-----------------------------------------------------------------------------
void SPI_Delay(void)
{
  volatile uint16_t x;
  for(x = 0;x < DelayTime4SPI ;x)
    x++;

}

//-----------------------------------------------------------------------------
void SPI_ComeBusBus(void)   // Tewakle the bus to overcome a possible MLX90393 Bug
{
}


//-----------------------------------------------------------------------------
void SPI_Start(void)   // S-Phase
{
  NCS_IO = 0;
  SPI_Delay();
}
//-----------------------------------------------------------------------------
void SPI_Stop(void)   // P-Phase
{
  NCS_IO = 1;
}

//-----------------------------------------------------------------------------
void SPI_Write(uint8_t Data)   // W-Phase
{
	ReadWriteByteSPI(Data);
}

//-----------------------------------------------------------------------------
uint8_t SPI_Read(void)  // R-Phase
{
  return(ReadWriteByteSPI(0xff));
}

//-----------------------------------------------------------------------------
uint8_t SendSingleCommandSPI(uint8_t Command)
{
  uint8_t State;
  SPI_Start();                               // S-Phase
  SPI_Write(Command); // command to Write    // W-Phase
  State = SPI_Read(); // Read Data           // R-Phase
  SPI_Stop();                                // P-Phase
  return(State);

}

//-----------------------------------------------------------------------------
uint8_t WriteRegisterSPI(uint8_t Command, uint8_t Register, uint8_t DataHi, uint8_t DataLo)
{
  uint8_t State;

  SPI_Start();
  SPI_Write(Command);
  SPI_Write(DataHi);
  SPI_Write(DataLo);
  SPI_Write(Register);
  State = SPI_Read();
  SPI_Stop();
  return(State);
}

//-----------------------------------------------------------------------------
uint8_t WriteRegisterByteSPI(uint8_t Register, uint8_t Value)
{
  SPI_Start();                               // S-Phase
  SPI_Write(Register); // command to Write    // W-Phase
  SPI_Write(Value); // command to Write    // W-Phase
  SPI_Stop();                                // P-Phase
  return(SPI_PRESENT);
}

//-----------------------------------------------------------------------------
uint8_t ReadRegisterSPI(uint8_t Command, uint8_t Register, uint16_t *Dataword)
{
	uint8_t State;
	SI_UU16_t Value;

	SPI_Start();                               // S-Phase
	SPI_Write(Command); // command to Write    // W-Phase
	SPI_Write(Register); // command to Write    // W-Phase
	State = SPI_Read(); // Read Data           // R-Phase
	Value.u8[0] = SPI_Read(); // Read Data   // R-Phase
	Value.u8[1] = SPI_Read(); // Read Data   // R-Phase
	(*Dataword) = Value.u16;
	SPI_Stop();                                // P-Phase

	return(State);
}

