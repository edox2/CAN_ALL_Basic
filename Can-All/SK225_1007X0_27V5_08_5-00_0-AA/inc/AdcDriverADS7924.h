/*
 * AdcDriver.h
 *********************************************
 *     (c)2016-2020 SIGITRONIC SOFTWARE      *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef ADCDRIVER_H_
#define ADCDRIVER_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <si_toolchain.h>

typedef struct {
	SI_UU16_t Channel0;
	SI_UU16_t Channel1;
	SI_UU16_t Channel2;
	SI_UU16_t Channel3;
} adcBaseStruct;


#define AdcMODECNTRL 	(0x00)
#define AdcINTCNTRL 	(0x01)
#define AdcDATA0_U		(0x82)
#define AdcDATA0_L		(0x03)
#define AdcDATA1_U		(0x84)
#define AdcDATA1_L		(0x05)
#define AdcDATA2_U		(0x86)
#define AdcDATA2_L		(0x07)
#define AdcDATA3_U		(0x88)
#define AdcDATA3_L		(0x09)
#define AdcULR0 		(0x0A)
#define AdcLLR0 		(0x0B)
#define AdcULR1 		(0x0C)
#define AdcLLR1 		(0x0D)
#define AdcULR2 		(0x0E)
#define AdcLLR2 		(0x0F)
#define AdcULR3 		(0x10)
#define AdcLLR3 		(0x11)
#define AdcINTCONFIG	(0x12)
#define AdcSLPCONFIG	(0x13)
#define AdcACQCONFIG	(0x14)
#define AdcPWRCONFIG	(0x15)
#define AdcRESET     	(0x16)

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

uint8_t AdcConvert(uint8_t Channel, uint8_t BaseAddress, adcBaseStruct * AnalogValues);
uint8_t AdcReset(uint8_t Channel, uint8_t BaseAddress);


#endif /* ADCDRIVER_H_ */
