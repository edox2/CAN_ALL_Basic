/*
 * HighSideSwitch.h
 *
 *  Created on: 13.03.2022
 *      Author: benja
 */
#ifndef INC_HIGHSIDESWITCH_H_
#define INC_HIGHSIDESWITCH_H_

#include "stdint.h"

typedef struct {
  int16_t current;
  uint16_t voltage;
  uint16_t temperature;
} HighSideSwitchData;


#endif /* INC_HIGHSIDESWITCH_H_ */
