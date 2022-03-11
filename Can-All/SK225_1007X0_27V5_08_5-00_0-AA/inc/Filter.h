/*
 * Filter.c
 *
 *  Created on: 15.SEPT.2019
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef FILTER_H_
#define FILTER_H_

#include <si_toolchain.h>

#define FILTER_POWER (4) // FilterSize  = 2^FilterPower
//#define FILTER_LEN (1<<FILTER_POWER)


typedef struct {
	uint8_t FilterPos; // = (2^FILTER_POWER-1);
	int32_t FilterSum; // = 0;
	int32_t FilterRing[1<<FILTER_POWER];
	uint8_t FilterInitialized; //= false;
} RollingAverageFilterStruct;

typedef struct {
	uint8_t FilterPos; // = (2^FILTER_POWER-1);
	int32_t FilterSum; // = 0;
	int16_t FilterRing[1<<FILTER_POWER];
	uint8_t FilterInitialized; //= false;
} RollingAverageSignedFilterStruct;

typedef struct {
	uint8_t FilterPos; // = (2^FILTER_POWER-1);
	uint32_t FilterSum; // = 0;
	uint16_t FilterRing[1<<FILTER_POWER];
	uint8_t FilterInitialized; //= false;
} RollingAverageUnsignedFilterStruct;

int16_t FilterTake(RollingAverageFilterStruct * FilterCardridge);


void FilterReset(RollingAverageFilterStruct * FilterCardridge);
int32_t Filter(int32_t RawValue, RollingAverageFilterStruct * FilterCardridge);
bool FilterInitialized(RollingAverageFilterStruct * FilterCardridge);
int32_t FilterReadonly(RollingAverageFilterStruct * FilterCardridge);

void SignedFilterReset(RollingAverageSignedFilterStruct * FilterCardridge);
int16_t SignedFilter(int16_t RawValue, RollingAverageSignedFilterStruct * FilterCardridge);
bool SignedFilterInitialized(RollingAverageSignedFilterStruct * FilterCardridge);
int16_t SignedFilterReadonly(RollingAverageSignedFilterStruct * FilterCardridge);

void UnsignedFilterReset(RollingAverageUnsignedFilterStruct * FilterCardridge);
uint16_t UnsignedFilter(uint16_t RawValue, RollingAverageUnsignedFilterStruct * FilterCardridge);
bool UnsignedFilterInitialized(RollingAverageUnsignedFilterStruct * FilterCardridge);
uint16_t UnsignedFilterReadonly(RollingAverageUnsignedFilterStruct * FilterCardridge);

#endif /* FILTER_H_ */
