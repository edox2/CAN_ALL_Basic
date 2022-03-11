/*
 * SpiBang.h
 *********************************************
 *    (c)2016-2017 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef SPIBANG_H_
#define SPIBANG_H_

#include "si_toolchain.h"


#define SPI_ERRORFLAG		0x10
#define SPI_ABSENT			0xff
#define SPI_PRESENT			0x00
#define SPI_ERROR			0xff
#define SPI_INIT			0xf0


#define READ_FLAG			0x01

extern uint8_t SPI_State;


//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------
uint8_t WriteRegisterByteSPI(uint8_t Command, uint8_t Value);
uint8_t WriteRegisterSPI(uint8_t Command, uint8_t Register, uint8_t DataHi, uint8_t DataLo);
uint8_t ReadRegisterSPI(uint8_t Command, uint8_t Register, uint16_t *Dataword);
uint8_t SendSingleCommandSPI(uint8_t Command);


void SPI_ComeBusBus(void);
void SPI_Delay(void);
void SPI_Start(void);
void SPI_Stop(void);
void SPI_Write(uint8_t Data);
uint8_t SPI_Read(void);
uint8_t ReadWriteByteSPI(uint8_t DataToSend);

#endif /* SPIBANG_H_ */
