/*
 * VibraDriver.h
 *********************************************
 *    (c)2016-2019 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef VIBRADRIVER_H_
#define VIBRADRIVER_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <si_toolchain.h>


#define VIBRA_CALIB_ERROR (0x01)

#define VibraREG00  (0x00)
#define VibraREG01  (0x01)
#define VibraREG02  (0x02)
#define VibraREG03  (0x03)
#define VibraREG04  (0x04)
#define VibraREG05  (0x05)
#define VibraREG06  (0x06)
#define VibraREG07  (0x07)
#define VibraREG08  (0x08)
#define VibraREG09  (0x09)
#define VibraREG0A  (0x0A)
#define VibraREG0B  (0x0B)
#define VibraREG0C  (0x0C)
#define VibraREG0D  (0x0D)
#define VibraREG0E  (0x0E)
#define VibraREG0F  (0x0F)
#define VibraREG10  (0x10)
#define VibraREG11  (0x11)
#define VibraREG12  (0x12)
#define VibraREG13  (0x13)
#define VibraREG14  (0x14)
#define VibraREG15  (0x15)
#define VibraREG16  (0x16)
#define VibraREG17  (0x17)
#define VibraREG18  (0x18)
#define VibraREG19  (0x19)
#define VibraREG1A  (0x1A)
#define VibraREG1B  (0x1B)
#define VibraREG1C  (0x1C)
#define VibraREG1D  (0x1D)
#define VibraREG1E  (0x1E)
#define VibraREG1F  (0x1F)
#define VibraREG20  (0x20)
#define VibraREG21  (0x21)
#define VibraREG22  (0x22)

#define VibraSTATUS									(0x00)
#define VibraMODE									(0x01)
#define VibraREAL_TIME_PLAYBACK_INPUT				(0x02)
#define VibraLIBRARY_SELECTION						(0x03)
#define VibraWAVEFORM_SEQUENCER1					(0x04)
#define VibraWAVEFORM_SEQUENCER2					(0x05)
#define VibraWAVEFORM_SEQUENCER3					(0x06)
#define VibraWAVEFORM_SEQUENCER4					(0x07)
#define VibraWAVEFORM_SEQUENCER5					(0x08)
#define VibraWAVEFORM_SEQUENCER6					(0x09)
#define VibraWAVEFORM_SEQUENCER7					(0x0A)
#define VibraWAVEFORM_SEQUENCER8					(0x0B)
#define VibraGO										(0x0C)
#define VibraOVERDRIVE_TIME_OFFSET					(0x0D)
#define VibraSUSTAIN_TIME_OFFSET_POSITIVE			(0x0E)
#define VibraSUSTAIN_TIME_OFFSET_NEGATIVE			(0x0F)
#define VibraBRAKE_TIME_OFFSET						(0x10)
#define VibraAUDIO_TO_VIBE_CONTROL					(0x11)
#define VibraAUDIO_TO_VIBE_MINIMUM_INPUT_LEVEL		(0x12)
#define VibraAUDIO_TO_VIBE_MAXIMUM_INPUT_LEVEL		(0x13)
#define VibraAUDIO_TO_VIBE_MINIMUM_OUTPUT_DRIVE		(0x14)
#define VibraAUDIO_TO_VIBE_MAXIMUM_OUTPUT_DRIVE		(0x15)
#define VibraRATED_VOLTAGE							(0x16)
#define VibraOVERDRIVE_CLAMP_VOLTAGE				(0x17)
#define VibraAUTO_CALIBRATION_COMPENSATION_RESULT	(0x18)
#define VibraAUTO_CALIBRATION_BACK_EMF_RESULT		(0x19)
#define VibraFEEDBACK_CONTROL						(0x1A)
#define VibraCONTROL1								(0x1B)
#define VibraCONTROL2								(0x1C)
#define VibraCONTROL3								(0x1D)
#define VibraCONTROL4								(0x1E)
#define VibraCONTROL5								(0x1F)
#define VibraLRA_OPEN_LOOP_PERIOD					(0x20)
#define VibraVOLTAGE_MONITOR						(0x21)
#define VibraLRA_RESONANCE_PERIOD					(0x22)


//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

uint8_t LraVibraGetState(uint8_t Channel, uint8_t BaseAddress, uint8_t *GoFlagState);
uint8_t LraVibraCalibrate(uint8_t Channel, uint8_t BaseAddress);
void LraVibraSetColdStartState(void);
uint8_t LraVibraSetPattern(uint8_t Channel, uint8_t BaseAddress, uint8_t *Pattern);
void LraVibraClearColdStartState(void);

#endif /* VIBRADRIVER_H_ */
