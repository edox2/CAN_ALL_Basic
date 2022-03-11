/*
 * SwitchManager.c
 *
 *  Created on: 15.DEC.2016
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// CompassSchwitch related stuff
#ifndef SWITCHMANAGER_H_
#define SWITCHMANAGER_H_

#include <si_toolchain.h>
#include "CalibrateMagnetics.h"

void InitTrendSwitch(void);
void InitTrendSwitchVa(CalibAxisStruct *xAxisData, CalibAxisStruct *yAxisData);
void InitTrendSwitchVb(CalibAxisStruct *xAxisData, CalibAxisStruct *yAxisData);
uint8_t combineAxisToTrendSwitch(CalibAxisStruct *xAxisData, CalibAxisStruct *yAxisData);
uint8_t combineAxisToTrend(CalibAxisStruct *xAxisData, CalibAxisStruct *yAxisData, bool BipolarMode);

#endif /* SWITCHMANAGER_H_ */
