/*
 * I2cDispatcher.h
 *
 *  Created on: 24.06.2019
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef I2CDISPATCHER_H_
#define I2CDISPATCHER_H_

#include "si_toolchain.h"

#define I2C_ERRORFLAG		0x10
#define I2C_ABSENT			0xff
#define I2C_PRESENT			0x00
#define I2C_ERROR			0xff
#define I2C_INIT			0xf0
#define I2C_HANGUP		    0xaa
#define I2C_INCONSISTENCY   0x20
#define I2C_CONVERSION		0x40
#define I2C_FATAL			0x55
#define I2C_PARITY			0x20

#define READ_FLAG			0x01

#define I2C_MODE_MULTIPLE_START     (1) // In most Cases the right choice: ReadOrWrite is treated as Read-Section
#define I2C_MODE_NOREPEAT_START     (0) // Special Case for unusual chips like infineon TLE493D and ReadOrWrite is treated as Read-Section
#define I2C_MODE_WRITE_ALL_SECTIONS (2) // ReadOrWrite-Section is treated as a second WriteBuffer (instead of readbuffer). If follows the other write buffer

#define I2C_BUFFERSIZE	(16)

extern uint8_t I2C_State;
extern uint8_t DataToWrite[I2C_BUFFERSIZE];
extern uint8_t DataToRead[I2C_BUFFERSIZE];


//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------
uint8_t AccessI2C(uint8_t Channel, uint8_t Address, uint16_t LengthWrite, uint8_t *DataToWrite, uint16_t LengthReadOrWrite, uint8_t *DataToReadOrWrite, uint8_t MultipleStartMode);
void I2cForceBits(uint8_t Channel, bool BitValue, uint8_t BitCount);

#endif /* I2CDISPATCHER_H_ */
