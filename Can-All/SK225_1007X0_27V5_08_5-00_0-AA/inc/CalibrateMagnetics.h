/*
 * CalibrateMagentics.c
 *
 *  Created on: 06.DEC.2016
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// Melexis MLX90393 related stuff
#ifndef CALIBRATEMAGNETICS_H_
#define CALIBRATEMAGNETICS_H_

#include <si_toolchain.h>

enum calibState_t {
	AXIS_NOT_CALIBRATED,
	AXIS_CALIBRATED,
	AXIS_CALIB_CLEARED,
	AXIS_CALIB_CENTERPOINT,
	AXIS_CALIB_CENTERFIELD,
	AXIS_CALIB_EXTREMAS,
	AXIS_CALIB_POS,
	AXIS_CALIB_NEG,
};

typedef struct {
	SI_UU16_t RawValue;
	SI_UU16_t FinalValue;
	float SlopeNeg;
	float SlopePos;
	int16_t Offset;
	int16_t MaxCal;
	int16_t CenterCal;
	int16_t MinCal;
	int16_t CenterMax;
	int16_t CenterMin;
	int16_t MaxScale;
	uint8_t SwapSign;
	enum calibState_t CalibrationState;
} CalibAxisStruct;

typedef struct {
	SI_UU16_t RawValue;
	SI_UU16_t FinalValue;
} TemperatureStruct;



bool IsMeterReset(uint8_t state);
bool IsMeterBurstMode(uint8_t state);
bool IsMeterSingleMode(uint8_t state);
bool IsMeterWOCMode(uint8_t state);
bool IsMeterError(uint8_t state);
bool IsMeterAbsent(uint8_t state);
bool CalibrationValid(CalibAxisStruct *AxisData);
bool CalibCenterDone(CalibAxisStruct *AxisData);
bool CalibExtremaDone(CalibAxisStruct *AxisData);
void ClearCalibration(CalibAxisStruct *AxisData);
void StoreCalibration(CalibAxisStruct *AxisData, uint16_t BaseLocation);
void RestoreCalibration(CalibAxisStruct *AxisData, uint16_t BaseLocation);
bool CalibrationValid(CalibAxisStruct *AxisData);
void TeachAxisGlobal(CalibAxisStruct *AxisData);
void TeachAxisCenter(CalibAxisStruct *AxisData);
void TeachAxisCenterbalanced(CalibAxisStruct *AxisData);
void ClearAxisStuct(CalibAxisStruct *AxisData);
void AdjustAxisGlobal(CalibAxisStruct *AxisData);
void AdjustAxisCenterbalanced(CalibAxisStruct *AxisData);
void AdjustAxisCenterbalancedClamped(CalibAxisStruct *AxisData, int16_t ClampPos, int16_t ClampNeg);
void AdjustAxisCenterSnapshot(CalibAxisStruct *AxisData);
void TeachAxisCenterSnapshot(CalibAxisStruct *AxisData);
int16_t CompensateGeometry(CalibAxisStruct *AxisDataX, CalibAxisStruct *AxisDataY, float StretchCoeff, int16_t ClampPos, int16_t ClampNeg);


#endif /* CALIBRATEMAGNETICS_H_ */
