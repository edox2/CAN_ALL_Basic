/*
 * FramDriverMB85RC64TA.h
 *********************************************
 *      (c)2018 SIGITRONIC SOFTWARE          *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// MB85RC64TA Driver Lib

#ifndef FRAMDRIVER_H_
#define FRAMDRIVER_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <si_toolchain.h>


#define FRAM_SIZE (0x2000)

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

uint8_t FramWrite(uint8_t Channel, uint8_t BaseAddress, uint16_t Length, uint16_t Offset, uint8_t *Data);

uint8_t FramRead(uint8_t Channel, uint8_t BaseAddress, uint16_t Length, uint16_t Offset, uint8_t *Data);

uint16_t BiStoreSettingFRAM(uint8_t Channel, uint8_t BaseAddress, uint16_t FramPos, uint8_t *ram, uint16_t numbytes, bool StoreToFram, uint16_t *CRC16);

#endif /* FRAMDRIVER_H_ */
