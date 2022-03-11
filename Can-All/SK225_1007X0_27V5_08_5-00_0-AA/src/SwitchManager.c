/*
 * SwitchManager.c
 *
 *  Created on: 15.DEC.2016
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include <SwitchManager.h>
#include <LIMITS.H>
// Module Global Variables
int16_t xdata SwitchEntryPosX, SwitchEntryNegX, SwitchExitPosX, SwitchExitNegX;
int16_t xdata SwitchEntryPosY, SwitchEntryNegY, SwitchExitPosY, SwitchExitNegY;
uint8_t xdata SwitchPosX, SwitchPosY, SwitchNegX, SwitchNegY;

uint8_t xdata SwitchLevelEntry[16];
uint8_t xdata SwitchLevelExit[16];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
void InitTrendSwitch(void) {
	SwitchExitPosX = 14000;
	SwitchExitNegX = -SwitchExitPosX;
	SwitchEntryPosX = 20000;
	SwitchEntryNegX = -SwitchEntryPosX;

	SwitchExitPosY = SwitchExitPosX;
	SwitchExitNegY = SwitchExitNegX;
	SwitchEntryPosY = SwitchEntryPosX;
	SwitchEntryNegY = SwitchEntryNegX;

	SwitchNegX = 0;
	SwitchNegY = 0;
	SwitchPosX = 0;
	SwitchPosY = 0;
}
/*
void InitTrendSwitch(int16_t Margin)
{
	SwitchLevelEntryPos[0] = Margin
	SwitchExitPosX = 8000;
	SwitchExitNegX = -8000;
	SwitchEntryPosX = 12000;
	SwitchEntryNegX = -12000;

	SwitchExitPosY = 8000;
	SwitchExitNegY = -8000;
	SwitchEntryPosY = 12000;
	SwitchEntryNegY = -12000;

	SwitchNegX = 0;
	SwitchNegY = 0;
	SwitchPosX = 0;
	SwitchPosY = 0;
}
*/
void InitTrendSwitchVa(CalibAxisStruct *xAxisData, CalibAxisStruct *yAxisData) {
	SwitchExitPosX = (xAxisData->MaxScale / 6);
	SwitchExitNegX = 0 - SwitchExitPosX;
	SwitchEntryPosX = SwitchExitPosX * 2;
	SwitchEntryNegX = 0 - SwitchExitPosX * 2;

	SwitchExitPosY = (yAxisData->MaxScale / 6);
	SwitchExitNegY = 0 - SwitchExitPosY;
	SwitchEntryPosY = SwitchExitPosY * 2;
	SwitchEntryNegY = 0 - SwitchExitPosY * 2;
	SwitchNegX = 0;
	SwitchNegY = 0;
	SwitchPosX = 0;
	SwitchPosY = 0;
}

void InitTrendSwitchVb(CalibAxisStruct *xAxisData, CalibAxisStruct *yAxisData) {
	SwitchExitPosX = xAxisData->CenterMax
			+ ((xAxisData->MaxScale
					- (xAxisData->CenterMax - xAxisData->CenterMin)) / 6);
	SwitchExitNegX = -SwitchExitPosX;
	SwitchEntryPosX = xAxisData->CenterMin
			- ((xAxisData->MaxScale
					- (xAxisData->CenterMax - xAxisData->CenterMin)) / 6);
	;
	SwitchEntryNegX = -SwitchEntryPosX;

	SwitchExitPosY = yAxisData->CenterMax
			+ ((yAxisData->MaxScale
					- (yAxisData->CenterMax - yAxisData->CenterMin)) / 6);
	SwitchExitNegY = -SwitchExitPosY;
	SwitchEntryPosY = yAxisData->CenterMin
			- ((yAxisData->MaxScale
					- (yAxisData->CenterMax - yAxisData->CenterMin)) / 6);
	;
	SwitchEntryNegY = -SwitchEntryPosY;

	SwitchNegX = 0;
	SwitchNegY = 0;
	SwitchPosX = 0;
	SwitchPosY = 0;
}


uint8_t combineAxisToTrendSwitch(CalibAxisStruct *xAxisData,
		CalibAxisStruct *yAxisData) {

	static uint8_t xdata SwitchPosX = 0;
	static uint8_t xdata SwitchPosY = 0;
	static uint8_t xdata SwitchNegX = 0;
	static uint8_t xdata SwitchNegY = 0;


//	if (xAxisData->FinalValue.s16 > 0) {
		if (xAxisData->FinalValue.s16 > SwitchEntryPosX)
			SwitchPosX = 16; //was 4;
		if (xAxisData->FinalValue.s16 < SwitchExitPosX)
			SwitchPosX = 0;
//	} else {
		if (xAxisData->FinalValue.s16 < SwitchEntryNegX)
			SwitchNegX = 240; //was 1;
		if (xAxisData->FinalValue.s16 > SwitchExitNegX)
			SwitchNegX = 0;
//	}

//	if (yAxisData->FinalValue.s16 > 0) {
		if (yAxisData->FinalValue.s16 > SwitchEntryPosY)
			SwitchPosY = 1; //was 2;
		if (yAxisData->FinalValue.s16 < SwitchExitPosY)
			SwitchPosY = 0;
//	} else {
		if (yAxisData->FinalValue.s16 < SwitchEntryNegY)
			SwitchNegY = 15; //was 8;
		if (yAxisData->FinalValue.s16 > SwitchExitNegY)
			SwitchNegY = 0;
//	}
	return (SwitchPosY + SwitchPosX + SwitchNegY + SwitchNegX);
}


/*
uint8_t combineAxisToTrendSwitch(CalibAxisStruct *xAxisData,
		CalibAxisStruct *yAxisData) {
	if (xAxisData->FinalValue.s16 > 0) {
		if (xAxisData->FinalValue.s16 > SwitchEntryPosX)
			SwitchPosX = 4;
		if (xAxisData->FinalValue.s16 < SwitchExitPosX)
			SwitchPosX = 0;
	} else {
		if (xAxisData->FinalValue.s16 < SwitchEntryNegX)
			SwitchNegX = 1;
		if (xAxisData->FinalValue.s16 > SwitchExitNegX)
			SwitchNegX = 0;
	}

	if (yAxisData->FinalValue.s16 > 0) {
		if (yAxisData->FinalValue.s16 > SwitchEntryPosY)
			SwitchPosY = 2;
		if (yAxisData->FinalValue.s16 < SwitchExitPosY)
			SwitchPosY = 0;
	} else {
		if (yAxisData->FinalValue.s16 < SwitchEntryNegY)
			SwitchNegY = 8;
		if (yAxisData->FinalValue.s16 > SwitchExitNegY)
			SwitchNegY = 0;
	}
	return (SwitchPosY + SwitchPosX + SwitchNegY + SwitchNegX);
}
*/

/*

uint8_t combineAxisToTrendSwitch(CalibAxisStruct *xAxisData,
		CalibAxisStruct *yAxisData) {
	static uint8_t xdata SwitchPosX, SwitchPosY, SwitchNegX, SwitchNegY;


	if (xAxisData->FinalValue.s16 > 0) {
		if (xAxisData->FinalValue.s16 > SwitchEntryPosX)
			SwitchPosX = 16; //was 4;
		if (xAxisData->FinalValue.s16 < SwitchExitPosX)
			SwitchPosX = 0;
	} else {
		if (xAxisData->FinalValue.s16 < SwitchEntryNegX)
			SwitchNegX = 240; //was 1;
		if (xAxisData->FinalValue.s16 > SwitchExitNegX)
			SwitchNegX = 0;
	}

	if (yAxisData->FinalValue.s16 > 0) {
		if (yAxisData->FinalValue.s16 > SwitchEntryPosY)
			SwitchPosY = 1; //was 2;
		if (yAxisData->FinalValue.s16 < SwitchExitPosY)
			SwitchPosY = 0;
	} else {
		if (yAxisData->FinalValue.s16 < SwitchEntryNegY)
			SwitchNegY = 15; //was 8;
		if (yAxisData->FinalValue.s16 > SwitchExitNegY)
			SwitchNegY = 0;
	}
	return (SwitchPosY + SwitchPosX + SwitchNegY + SwitchNegX);
}

*/


uint8_t combineAxisToTrend(CalibAxisStruct *xAxisData, CalibAxisStruct *yAxisData, bool BipolarMode)
{
#define BRICK_COUNT (16)
#define CENTER_BAND (UINT_MAX / 3) // 3 was 5
#define TOP_BAND    (UINT_MAX / 8) // 8 was 32
#define BOTTOM_BAND (TOP_BAND)
#define BRICK_BAND  ((UINT_MAX - CENTER_BAND - TOP_BAND - BOTTOM_BAND) / BRICK_COUNT)
#define MASK_X_LEVEL	(0xf0)
#define MASK_Y_LEVEL	(0x0f)

static const int16_t xdata Transitions[BRICK_COUNT] = {
		INT_MIN + BOTTOM_BAND,
		INT_MIN + BOTTOM_BAND + (1 * BRICK_BAND),
		INT_MIN + BOTTOM_BAND + (2 * BRICK_BAND),
		INT_MIN + BOTTOM_BAND + (3 * BRICK_BAND),
		INT_MIN + BOTTOM_BAND + (4 * BRICK_BAND),
		INT_MIN + BOTTOM_BAND + (5 * BRICK_BAND),
		INT_MIN + BOTTOM_BAND + (6 * BRICK_BAND),
		INT_MIN + BOTTOM_BAND + (7 * BRICK_BAND),
		INT_MAX - TOP_BAND - (7 * BRICK_BAND),
		INT_MAX - TOP_BAND - (6 * BRICK_BAND),
		INT_MAX - TOP_BAND - (5 * BRICK_BAND),
		INT_MAX - TOP_BAND - (4 * BRICK_BAND),
		INT_MAX - TOP_BAND - (3 * BRICK_BAND),
		INT_MAX - TOP_BAND - (2 * BRICK_BAND),
		INT_MAX - TOP_BAND - (1 * BRICK_BAND),
		INT_MAX - TOP_BAND};

static const int8_t xdata UpwardLevel[BRICK_COUNT] = {
		0x99, // -7
		0x99, // -7
		0xaa,
		0xbb,
		0xcc,
		0xdd,
		0xee,
		0xff, // -1
		0x00, // 0
		0x11,
		0x22,
		0x33,
		0x44,
		0x55,
		0x66,
		0x77 // +7
};

static const int8_t xdata DownwardLevel[BRICK_COUNT] = {
		0x99, //-7
		0xaa,
		0xbb,
		0xcc,
		0xdd,
		0xee,
		0xff,
		0x00, // 0
		0x11, // +1
		0x22,
		0x33,
		0x44,
		0x55,
		0x66,
		0x77, // +7
		0x77  // +7
};

static const int8_t xdata UpwardSwitch[BRICK_COUNT] = {
		0xff, // -7
		0xff, // -7
		0xff, // -6
		0xff, // -5
		0xff, // -4
		0xff, // -3
		0xff, // -2 //
		0xff, // -1 //
		0x00, // 0
		0x00, // +1
		0x00, // +2
		0x00, // +3 //
		0x00, // +4 //
		0x11, // +5
		0x11, // +6
		0x11  // +7
};

static const int8_t xdata DownwardSwitch[BRICK_COUNT] = {
		0xff, // -7
		0xff, // -6
		0xff, // -5
		0x00, // -4 //
		0x00, // -3 //
		0x00, // -2
		0x00, // -1
		0x00, // 0
		0x11, // +1 //
		0x11, // +2 //
		0x11, // +3
		0x11, // +4
		0x11, // +5
		0x11, // +6
		0x11, // +7
		0x11  // +7
};

	static int16_t LastAnalogX = 0;
	static int16_t LastAnalogY = 0;
	static int16_t LastBipolarX = 0;
	static int16_t LastBipolarY = 0;
	uint8_t Index;
	uint8_t TempResult = 0;
	uint8_t TempSwitch = 0;

	//X-Path Upward
	if (xAxisData->FinalValue.s16 >= ((BipolarMode)? LastBipolarX : LastAnalogX))
	{
		Index = 0;
		while ((Transitions[Index] <= xAxisData->FinalValue.s16) && (Index < BRICK_COUNT-1))
		{
			Index++;
		};
		TempResult += UpwardLevel[Index] & MASK_X_LEVEL;
		TempSwitch += UpwardSwitch[Index] & MASK_X_LEVEL;
	}
	else //X-Path Downward
	{
		Index = 15;
		while ((Transitions[Index] >= xAxisData->FinalValue.s16) && (Index > 0))
		{
			Index--;
		};
		TempResult += DownwardLevel[Index] & MASK_X_LEVEL;
		TempSwitch += DownwardSwitch[Index] & MASK_X_LEVEL;
	}
	if (BipolarMode)
		LastBipolarX = xAxisData->FinalValue.s16;

	else
		LastAnalogX = xAxisData->FinalValue.s16;


	//Y-Path Upward
	if (yAxisData->FinalValue.s16 >= ((BipolarMode)? LastBipolarY : LastAnalogY))
	{
		Index = 0;
		while ((Transitions[Index] <= yAxisData->FinalValue.s16) && (Index < BRICK_COUNT-1))
		{
			Index++;
		};
		TempResult += UpwardLevel[Index] & MASK_Y_LEVEL;
		TempSwitch += UpwardSwitch[Index] & MASK_Y_LEVEL;
	}
	else //Y-Path Downward
	{
		Index = 15;
		while ((Transitions[Index] >= yAxisData->FinalValue.s16) && (Index > 0))
		{
			Index--;
		};
		TempResult += DownwardLevel[Index] & MASK_Y_LEVEL;
		TempSwitch += DownwardSwitch[Index] & MASK_Y_LEVEL;
	}
	if (BipolarMode)
		LastBipolarY = yAxisData->FinalValue.s16;

	else
		LastAnalogY = yAxisData->FinalValue.s16;

	return (BipolarMode)? TempSwitch : TempResult;
}















/*
uint8_t combineAxisToDetents(CalibAxisStruct *xAxisData,
		CalibAxisStruct *yAxisData) {

	uint8_t micro

	if (xAxisData->FinalValue > 0) {
		if (xAxisData->FinalValue > SwitchEntryPosX)
			SwitchPosX = 4;
		if (xAxisData->FinalValue < SwitchExitPosX)
			SwitchPosX = 0;
	} else {
		if (xAxisData->FinalValue < SwitchEntryNegX)
			SwitchNegX = 1;
		if (xAxisData->FinalValue > SwitchExitNegX)
			SwitchNegX = 0;
	}

	if (yAxisData->FinalValue > 0) {
		if (yAxisData->FinalValue > SwitchEntryPosY)
			SwitchPosY = 2;
		if (yAxisData->FinalValue < SwitchExitPosY)
			SwitchPosY = 0;
	} else {
		if (yAxisData->FinalValue < SwitchEntryNegY)
			SwitchNegY = 8;
		if (yAxisData->FinalValue > SwitchExitNegY)
			SwitchNegY = 0;
	}

	return (SwitchPosY + SwitchPosX + SwitchNegY + SwitchNegX);

	Deflection = 0;
	if (SwitchDirection & 1)
	{
		Deflection += 0x07;
	}
	if (SwitchDirection & 2)
	{
		Deflection += 0x70;

	}
	if (SwitchDirection & 4)
	{
		Deflection += 0x09;
	}
	if (SwitchDirection & 8)
	{
		Deflection += 0x90;
	}
}


*/
