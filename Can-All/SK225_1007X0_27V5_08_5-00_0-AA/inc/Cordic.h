/*
 * Cordic.h
 *
 *  Created on: 12.07.2016
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef CORDIC_H_
#define CORDIC_H_

#include "stdint.h"

uint16_t atan2_cordic(int16_t x, int16_t y);

#endif /* CORDIC_H_ */
