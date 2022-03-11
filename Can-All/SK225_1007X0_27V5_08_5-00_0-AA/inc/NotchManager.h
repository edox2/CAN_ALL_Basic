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
// SK400 related stuff
#ifndef NOTCHMANAGER_H_
#define NOTCHMANAGER_H_

#include <si_toolchain.h>

#ifndef NOTCH_COUNT
#define NOTCH_COUNT 			(20) // Up to 256 Notches possible
#endif // NOTCH_COUNT

#define NOTCH_THRESHOLD_MARGIN	(3) // Margin of 3 = 1/3 (~33%) of distance;  Margin of 4 = 1/4 (25%) of distance; Margin of 5 = 1/5 (20%) of distance;

uint8_t prevNotch(uint8_t notch);
uint8_t nextNotch(uint8_t notch);
bool validateNotchList(void);
uint8_t getNotch(uint16_t Angle);
bool addNotchComplete(uint16_t Angle, bool Reset);
uint16_t AdjustToNotchDP(uint16_t Angle);
uint16_t CalcEntry(uint8_t NotchNow);
uint16_t CalcExit(uint8_t NotchNow);
uint8_t getGravityPoint(uint16_t Angle);
bool addNotchBackpack(int16_t rawX, int16_t rawY, bool Reset);
struct NotchPos {
	uint16_t entry;
	uint16_t exit;
	uint16_t center;
	uint16_t raw;
};

struct NotchBackpackPos {
//	SI_UU16_t RawValueX;
//	SI_UU16_t FinalValueX;
//	float SlopeNegX;
//	float SlopePosX;
	int16_t MaxCalX;
	int16_t CenterCalX;
	int16_t MinCalX;
//	int16_t CenterMaxX;
//	int16_t CenterMinX;
//	SI_UU16_t RawValueY;
//	SI_UU16_t FinalValueY;
//	float SlopeNegY;
//	float SlopePosY;
	int16_t MaxCalY;
	int16_t CenterCalY;
	int16_t MinCalY;
//	int16_t CenterMaxY;
//	int16_t CenterMinY;
};


extern struct NotchPos xdata NotchPositions[NOTCH_COUNT];
extern struct NotchBackpackPos xdata NotchBackpackPositions[NOTCH_COUNT];

#endif /* NOTCHMANAGER_H_ */
