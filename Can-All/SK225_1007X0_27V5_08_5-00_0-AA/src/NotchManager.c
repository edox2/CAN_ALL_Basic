/*
 * NotchManager.c
 *
 *  Created on: 12.DEC.2016
 *********************************************
 *      (c)2016 SIGITRONIC SOFTWARE          *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include <NotchManager.h>
#include "LIMITS.H"
//#include "HalDef.h"

//#define AXIS_HALF_SCALE (0x7fff)

// Module Global Variables

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
bool addNotchComplete(uint16_t Angle, bool Reset) {
	static uint8_t notch = 0;

	if (Reset) // begin from scratch
	{
		notch = 0;
	}
	NotchPositions[notch].raw = Angle;
	notch = nextNotch(notch);
	if (notch) {
		return (false);
	} else {
		return (true); // array is complete now
	}
}

bool addNotchBackpack(int16_t rawX, int16_t rawY, bool Reset) {
	static uint8_t notch = 0;

	if (Reset) // begin from scratch
	{
		notch = 0;
	}
//	NotchBackpackPositions[notch].RawValueX.s16 = rawX;
//	NotchBackpackPositions[notch].RawValueY.s16 = rawY;

	NotchBackpackPositions[notch].CenterCalX = rawX;
	NotchBackpackPositions[notch].CenterCalY = rawY;

	NotchBackpackPositions[notch].MaxCalX = INT_MIN;
	NotchBackpackPositions[notch].MaxCalY = INT_MIN;
	NotchBackpackPositions[notch].MinCalX = INT_MAX;
	NotchBackpackPositions[notch].MinCalY = INT_MAX;



	notch = nextNotch(notch);
	if (notch) {
		return (false);
	} else {
		return (true); // array is complete now
	}
}


uint8_t prevNotch(uint8_t notch) {
	return ((notch) ? notch - 1 : NOTCH_COUNT - 1);
}

uint8_t nextNotch(uint8_t notch) {
	return ((notch >= NOTCH_COUNT - 1) ? 0 : notch + 1);
}

bool validateNotchList(void) {
	uint8_t index = 0;
	int32_t distance;

	for (index = 0; index < NOTCH_COUNT; index++) // Value of NotchPosition 0 is treated as Offset to achieve a virtual DiscontinuityPoint at Center of Notch 0
			{
		NotchPositions[index].center = NotchPositions[index].raw
				- NotchPositions[0].raw;
		NotchPositions[index].entry = NotchPositions[index].center;
		NotchPositions[index].exit = NotchPositions[index].center;
	}

	for (index = 0; index < NOTCH_COUNT; index++) {
		if (NotchPositions[nextNotch(index)].center
				> NotchPositions[index].center) {
			distance = (int32_t) (NotchPositions[nextNotch(index)].center
					- NotchPositions[index].center)
					/ (int32_t) NOTCH_THRESHOLD_MARGIN;
			NotchPositions[nextNotch(index)].entry =
					NotchPositions[index].center + (int16_t) distance;
			NotchPositions[index].exit = NotchPositions[nextNotch(index)].center
					- (int16_t) distance;
		} else {
			if (nextNotch(index)) {
				return (false);
			} else {
				distance = (int32_t) (NotchPositions[nextNotch(index)].center
						- NotchPositions[index].center)
						/ (int32_t) NOTCH_THRESHOLD_MARGIN;
				NotchPositions[nextNotch(index)].entry =
						NotchPositions[index].center + (int16_t) distance;
				NotchPositions[index].exit =
						NotchPositions[nextNotch(index)].center
								- (int16_t) distance;
			}
		}
	}
	return (true);
}

uint8_t getNotch(uint16_t Angle) {
	static uint8_t xdata NotchNow = 0;
	bool directionUp;
	uint8_t xdata StartNotch; // used to prevent endless loop in case of discontinuity in NotchTable during calibration
	uint16_t tempAngle;
	uint16_t tempEntry;
	uint16_t tempExit;

	Angle = AdjustToNotchDP(Angle);
	// check if gravity to current notch is existing
	StartNotch = NotchNow;

		tempAngle = Angle;
		tempEntry = NotchPositions[NotchNow].entry;
		tempExit = NotchPositions[NotchNow].exit;

	if (NotchPositions[NotchNow].exit < NotchPositions[NotchNow].entry)
	{
		tempAngle += 0x8000;
		tempEntry += 0x8000;
		tempExit += 0x8000;
	}

	if (tempExit > tempAngle)
		if (tempEntry < tempAngle)
			return (NotchNow);
		else
			directionUp = false;
	else
		directionUp = true;

	do // search for gravity in the right direction
	{
		NotchNow = (directionUp) ? nextNotch(NotchNow) : prevNotch(NotchNow);

		tempAngle = Angle;
		tempEntry = NotchPositions[NotchNow].entry;
		tempExit = NotchPositions[NotchNow].exit;

		if (NotchPositions[NotchNow].exit < NotchPositions[NotchNow].entry)
		{ // handle discontinuity point here
			tempAngle += 0x8000;
			tempEntry += 0x8000;
			tempExit += 0x8000;
		}
	}
	while (((tempExit < tempAngle) || (tempEntry > tempAngle))
			&& (StartNotch != NotchNow)); // Prevent endless Loop in case of corrupted notch table

	return (NotchNow);
}

uint16_t AdjustToNotchDP(uint16_t Angle) {
	return (Angle - NotchPositions[0].raw); // Set Virtual DiscontinuityPoint to Center of Notch=0
}

uint8_t getGravityPoint(uint16_t Angle) {
	static uint8_t xdata NotchNow = 0;
	bool directionUp;
	uint8_t xdata StartNotch; // used to prevent endless loop in case of discontinuity in NotchTable during calibration

	Angle = AdjustToNotchDP(Angle);

	// check if gravity to current notch is existing
	StartNotch = NotchNow;
	if (CalcExit(NotchNow) > Angle)
		if (((NotchNow) &&(CalcEntry(NotchNow) < Angle))
				|| ((!NotchNow) && (CalcEntry(NotchNow) > Angle))) // handle discontinuity point here
			return (NotchNow);
		else
			directionUp = false;
	else
		directionUp = true;

	do  // search for gravity in the right direction
		NotchNow = (directionUp) ? NotchNow + 1 : NotchNow - 1;
	while (((CalcExit(NotchNow) < Angle)
			|| ((!NotchNow) && (CalcEntry(NotchNow) < Angle))
			|| ((NotchNow) &&(CalcEntry(NotchNow) > Angle)) // handle discontinuity point here
	) && (StartNotch != NotchNow)); // Prevent endless Loop in case of corrupted notch table
	return (NotchNow);
}

uint16_t CalcEntry(uint8_t NotchNow) {
	SI_UU16_t entry;

	entry.u8[0] = NotchNow - 1;
	entry.u8[1] = 0x55;
	return (entry.u16);
}

uint16_t CalcExit(uint8_t NotchNow) {
	SI_UU16_t exit;

	exit.u8[0] = NotchNow;
	exit.u8[1] = 0xAA;
	return (exit.u16);
}
