/*
 * PersistSettings.h
 *
 *  Created on: 16.12.2016
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef PERSISTSETTINGS_H_
#define PERSISTSETTINGS_H_

#include "HalDef.h"

typedef uint16_t XRAMADDR;

bool ClearAllSettings(uint16_t baseAddr);
uint16_t BiStoreSettingFLASH(uint16_t flash, uint8_t *ram, uint16_t numbytes, bool StoreToFlash);
void GetSerial(uint16_t flash, uint8_t *ram, uint16_t numbytes);


#endif /* PERSISTSETTINGS_H_ */
