/*
 * CalibrateMagnetics.c
 *
 *  Created on: 06.DEC.2016
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include <CalibrateMagnetics.h>
#include <LIMITS.H>
#include <MATH.H>

#define AXIS_HALF_SCALE (0x7fff)

volatile struct NotchPos xdata Positions[24];

void ClearAxisStuct(CalibAxisStruct *AxisData) {
	AxisData->CenterCal = 0;
	AxisData->CenterMax = INT_MIN;
	AxisData->CenterMin = INT_MAX;
	AxisData->FinalValue.s16 = 0;
	AxisData->MaxCal = INT_MIN;
	AxisData->MinCal = INT_MAX;
	AxisData->Offset = 0;
	AxisData->RawValue.s16 = 0;
	AxisData->SlopeNeg = 1.0f;
	AxisData->SlopePos = 1.0f; // Also used as SlopeGlobal
	AxisData->MaxScale = INT_MAX;
	AxisData->CalibrationState = AXIS_CALIB_CLEARED;
	AxisData->SwapSign = 0;
}

bool CalibCenterDone(CalibAxisStruct *AxisData) {
	if (AxisData->CalibrationState == AXIS_CALIB_CENTERFIELD) {
		AxisData->CalibrationState = AXIS_CALIB_EXTREMAS;
		return (true);
	}
	return (false);
}

bool CalibExtremaDone(CalibAxisStruct *AxisData) {
	if (AxisData->CalibrationState == AXIS_CALIB_EXTREMAS) {
		AxisData->CalibrationState = AXIS_CALIBRATED;
		return (true);
	}
	return (false);
}

void ClearCalibration(CalibAxisStruct *AxisData) {
	AxisData->CalibrationState = AXIS_NOT_CALIBRATED;
	ClearAxisStuct(AxisData);
}

//void StoreCalibration(CalibAxisStruct *AxisData, uint16_t BaseLocation);
//void RestoreCalibration(CalibAxisStruct *AxisData, uint16_t BaseLocation);

bool CalibrationValid(CalibAxisStruct *AxisData) {
	return ((AxisData->CalibrationState == AXIS_CALIBRATED) ? true : false);
}

void TeachAxisGlobal(CalibAxisStruct *AxisData) {
	AxisData->MaxCal =
			(AxisData->MaxCal > AxisData->RawValue.s16) ?
					AxisData->MaxCal : AxisData->RawValue.s16;
	AxisData->MinCal =
			(AxisData->MinCal < AxisData->RawValue.s16) ?
					AxisData->MinCal : AxisData->RawValue.s16;
	AxisData->SlopePos =
			(AxisData->MaxCal != AxisData->MinCal) ?
					(float) AxisData->MaxScale
							/ ((float) AxisData->MaxCal
									- (float) AxisData->MinCal) :
					1.0f; // use half scale of int to prevent overflow at lower temperatures
	AxisData->Offset = (AxisData->MaxCal + AxisData->MinCal) / 2;
	AdjustAxisGlobal(AxisData);

	// Update Calibration StateMachine
	switch (AxisData->CalibrationState) {
	/*		case AXIS_NOT_CALIBRATED:
	 break;
	 case AXIS_CALIBRATED:
	 break;
	 */case AXIS_CALIB_CLEARED:
		AxisData->CalibrationState = AXIS_CALIB_CENTERPOINT;
		break;
	case AXIS_CALIB_CENTERPOINT:
		AxisData->CalibrationState = AXIS_CALIB_CENTERFIELD;
		break;
		/*		case AXIS_CALIB_CENTERFIELD:
		 break;
		 case AXIS_CALIB_EXTREMAS:
		 break;
		 case AXIS_CALIB_POS:
		 break;
		 case AXIS_CALIB_NEG:
		 break;
		 default:
		 break;
		 */
	}

}

void AdjustAxisGlobal(CalibAxisStruct *AxisData) {
	AxisData->FinalValue.s16 = (int16_t) ((AxisData->RawValue.s16 - AxisData->Offset) * AxisData->SlopePos); 
}

void AdjustAxisCenterbalanced(CalibAxisStruct *AxisData) {
	if (AxisData->RawValue.s16 > AxisData->CenterCal) {
		AxisData->FinalValue.s16 = (int16_t) ((AxisData->RawValue.s16 - AxisData->CenterCal) * AxisData->SlopePos);
	}
	else
	{
		AxisData->FinalValue.s16 = (int16_t) ((AxisData->RawValue.s16 - AxisData->CenterCal) * AxisData->SlopeNeg);
	}
}

void AdjustAxisCenterbalancedClamped(CalibAxisStruct *AxisData, int16_t ClampPos, int16_t ClampNeg)
{
	if (AxisData->RawValue.s16 > AxisData->CenterCal)
	{
		AxisData->FinalValue.s16 = (int16_t) ((AxisData->RawValue.s16 - AxisData->CenterCal) * AxisData->SlopePos);

		if ((-1 > AxisData->FinalValue.s16) || (AxisData->FinalValue.s16 > ClampPos))
		{
			AxisData->FinalValue.s16 = ClampPos;
		}
	}
	else
	{
		AxisData->FinalValue.s16 = (int16_t) ((AxisData->RawValue.s16 - AxisData->CenterCal) * AxisData->SlopeNeg);

		if ((1 < AxisData->FinalValue.s16) || (AxisData->FinalValue.s16 < ClampNeg))
		{
			AxisData->FinalValue.s16 = ClampNeg;
		}
	}
}

void AdjustAxisCenterSnapshot(CalibAxisStruct *AxisData) {
	AxisData->RawValue.s16 -= AxisData->Offset;
}

void TeachAxisCenterSnapshot(CalibAxisStruct *AxisData) {
	AxisData->Offset = AxisData->RawValue.s16 - AxisData->CenterCal;
}

void TeachAxisCenterbalanced(CalibAxisStruct *AxisData) {
	AxisData->MaxCal =
			(AxisData->MaxCal > AxisData->RawValue.s16) ?
					AxisData->MaxCal : AxisData->RawValue.s16;
	AxisData->MinCal =
			(AxisData->MinCal < AxisData->RawValue.s16) ?
					AxisData->MinCal : AxisData->RawValue.s16;

	AxisData->SlopePos =
			(AxisData->MaxCal != AxisData->CenterCal) ?
					((float) AxisData->MaxScale / 2.0f)
							/ ((float) AxisData->MaxCal
									- (float) AxisData->CenterCal) :
					1.0f;
	AxisData->SlopeNeg =
			(AxisData->MinCal != AxisData->CenterCal) ?
					((float) AxisData->MaxScale / 2.0f)
							/ ((float) AxisData->CenterCal
									- (float) AxisData->MinCal) :
					1.0f;

	AdjustAxisCenterbalanced(AxisData);
	// Update Calibration StateMachine
	switch (AxisData->CalibrationState) {
	case AXIS_CALIB_CENTERPOINT:
	case AXIS_CALIB_CENTERFIELD:
		AxisData->CalibrationState = AXIS_CALIB_EXTREMAS;
		break;
	}
}


int16_t CompensateGeometry(CalibAxisStruct *AxisDataX, CalibAxisStruct *AxisDataY, float StretchCoeff, int16_t ClampPos, int16_t ClampNeg)
{
	int16_t CorrectorXY;
	int16_t NewAxisValue;

	// Geometry compensation
	// prevent ill posed problem by order of multiplications
	CorrectorXY = (int16_t)(StretchCoeff * sqrt((float)abs(AxisDataX->FinalValue.s16) * (float)abs(AxisDataY->FinalValue.s16)));
	if (AxisDataY->FinalValue.s16 > 0)
	{
		NewAxisValue = (AxisDataY->FinalValue.s16 + CorrectorXY);
		NewAxisValue = (NewAxisValue > ClampPos)? ClampPos : (NewAxisValue < 0) ? ClampPos : NewAxisValue;
	}
	else
	{
		NewAxisValue = (AxisDataY->FinalValue.s16 - CorrectorXY);
		NewAxisValue = (NewAxisValue < ClampNeg)? ClampNeg : (NewAxisValue > 0) ? ClampNeg : NewAxisValue;
	}
	AxisDataY->FinalValue.s16 = NewAxisValue;


	if (AxisDataX->FinalValue.s16 > 0)
	{
		NewAxisValue = (AxisDataX->FinalValue.s16 + CorrectorXY);
		NewAxisValue = (NewAxisValue > ClampPos)? ClampPos : (NewAxisValue < 0) ? ClampPos : NewAxisValue;
	}
	else
	{
		NewAxisValue = (AxisDataX->FinalValue.s16 - CorrectorXY);
		NewAxisValue = (NewAxisValue < ClampNeg)? ClampNeg : (NewAxisValue > 0) ? ClampNeg : NewAxisValue;
	}
	AxisDataX->FinalValue.s16 = NewAxisValue;
	return CorrectorXY;
}


void TeachAxisCenter(CalibAxisStruct *AxisData) {
	AxisData->CenterMax =
			(AxisData->CenterMax > AxisData->RawValue.s16) ?
					AxisData->CenterMax : AxisData->RawValue.s16;
	AxisData->CenterMin =
			(AxisData->CenterMin < AxisData->RawValue.s16) ?
					AxisData->CenterMin : AxisData->RawValue.s16;
	AxisData->CenterCal = AxisData->CenterMax / 2 + AxisData->CenterMin / 2;
	//Keep MaxCal and MinCal and Offset in order too
	AxisData->MaxCal =
			(AxisData->MaxCal > AxisData->CenterMax) ?
					AxisData->MaxCal : AxisData->CenterMax;
	AxisData->MinCal =
			(AxisData->MinCal < AxisData->CenterMin) ?
					AxisData->MinCal : AxisData->CenterMin;

	AxisData->Offset = (AxisData->MaxCal + AxisData->MinCal) / 2;

	// Update Calibration StateMachine
	switch (AxisData->CalibrationState) {
	/*		case AXIS_NOT_CALIBRATED:
	 break;
	 case AXIS_CALIBRATED:
	 break;
	 */case AXIS_CALIB_CLEARED:
		AxisData->CalibrationState = AXIS_CALIB_CENTERPOINT;
		break;
	case AXIS_CALIB_CENTERPOINT:
		AxisData->CalibrationState = AXIS_CALIB_CENTERFIELD;
		break;
		/*		case AXIS_CALIB_CENTERFIELD:
		 break;
		 case AXIS_CALIB_EXTREMAS:
		 break;
		 case AXIS_CALIB_POS:
		 break;
		 case AXIS_CALIB_NEG:
		 break;
		 default:
		 break;
		 */
	}
}

