/*
 * Filter.c
 *
 *  Created on: 15.SEPT.2019
 *********************************************
 *      (c)2020 SIGITRONIC SOFTWARE          *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include <Filter.h>


//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------


bool SignedFilterInitialized(RollingAverageSignedFilterStruct * FilterCardridge)
{
	return FilterCardridge->FilterInitialized;
}

void SignedFilterReset(RollingAverageSignedFilterStruct * FilterCardridge)
{
	FilterCardridge->FilterInitialized = false;
	FilterCardridge->FilterPos = ((1 << FILTER_POWER) - 1);
	FilterCardridge->FilterSum = 0;
}

int16_t SignedFilter(int16_t RawValue, RollingAverageSignedFilterStruct * FilterCardridge)
{
	if (FilterCardridge->FilterInitialized)
	{
		FilterCardridge->FilterSum -= FilterCardridge->FilterRing[FilterCardridge->FilterPos];
	}
	FilterCardridge->FilterRing[FilterCardridge->FilterPos] = RawValue;
	FilterCardridge->FilterSum += RawValue;
	FilterCardridge->FilterInitialized = (FilterCardridge->FilterPos) ? FilterCardridge->FilterInitialized : true;
	FilterCardridge->FilterPos--;
	FilterCardridge->FilterPos = FilterCardridge->FilterPos & ((1 << FILTER_POWER) - 1);
	return (int16_t)(FilterCardridge->FilterSum >> FILTER_POWER);
}

int16_t SignedFilterReadonly(RollingAverageSignedFilterStruct * FilterCardridge)
{
	return (int16_t)(FilterCardridge->FilterSum >> FILTER_POWER);
}


bool FilterInitialized(RollingAverageFilterStruct * FilterCardridge)
{
	return FilterCardridge->FilterInitialized;
}

void FilterReset(RollingAverageFilterStruct * FilterCardridge)
{
	FilterCardridge->FilterInitialized = false;
	FilterCardridge->FilterPos = ((1 << FILTER_POWER) - 1);
	FilterCardridge->FilterSum = 0;
}

int32_t Filter(int32_t RawValue, RollingAverageFilterStruct * FilterCardridge)
{
	if (FilterCardridge->FilterInitialized)
	{
		FilterCardridge->FilterSum -= FilterCardridge->FilterRing[FilterCardridge->FilterPos];
	}
	FilterCardridge->FilterRing[FilterCardridge->FilterPos] = RawValue;
	FilterCardridge->FilterSum += RawValue;
	FilterCardridge->FilterInitialized = (FilterCardridge->FilterPos) ? FilterCardridge->FilterInitialized : true;
	FilterCardridge->FilterPos--;
	FilterCardridge->FilterPos = FilterCardridge->FilterPos & ((1 << FILTER_POWER) - 1);
	return (int32_t)(FilterCardridge->FilterSum >> FILTER_POWER);
}

int16_t FilterTake(RollingAverageFilterStruct * FilterCardridge)
{
	int32_t SumSum = 0;
	uint8_t index;

	for (index = 0; index < (1 << FILTER_POWER); index++)
	{
		SumSum += FilterCardridge->FilterRing[index];
	}
	return (int16_t)(SumSum >> FILTER_POWER);
}

int32_t FilterReadonly(RollingAverageFilterStruct * FilterCardridge)
{
	return (int32_t)(FilterCardridge->FilterSum >> FILTER_POWER);
}


bool UnsignedFilterInitialized(RollingAverageUnsignedFilterStruct * FilterCardridge)
{
	return FilterCardridge->FilterInitialized;
}

void UnsignedFilterReset(RollingAverageUnsignedFilterStruct * FilterCardridge)
{
	FilterCardridge->FilterInitialized = false;
	FilterCardridge->FilterPos = ((1 << FILTER_POWER) - 1);
	FilterCardridge->FilterSum = 0;
}

uint16_t UnsignedFilter(uint16_t RawValue, RollingAverageUnsignedFilterStruct * FilterCardridge)
{
	if (FilterCardridge->FilterInitialized)
	{
		FilterCardridge->FilterSum -= FilterCardridge->FilterRing[FilterCardridge->FilterPos];
	}
	FilterCardridge->FilterRing[FilterCardridge->FilterPos] = RawValue;
	FilterCardridge->FilterSum += RawValue;
	FilterCardridge->FilterInitialized = (FilterCardridge->FilterPos) ? FilterCardridge->FilterInitialized : true;
	FilterCardridge->FilterPos--;
	FilterCardridge->FilterPos = FilterCardridge->FilterPos & ((1 << FILTER_POWER) - 1);
	return (int16_t)(FilterCardridge->FilterSum >> FILTER_POWER);
}

uint16_t UnsignedFilterReadonly(RollingAverageUnsignedFilterStruct * FilterCardridge)
{
	return (uint16_t)(FilterCardridge->FilterSum >> FILTER_POWER);
}
