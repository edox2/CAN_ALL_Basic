/*
 * CRC.h
 *
 *  Created on: 01.04.2017
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef CRC_H_
#define CRC_H_

#include "stdint.h"

uint8_t calcCRC7(uint8_t Data, uint8_t crc);
uint8_t calcCRC8(uint8_t Data, uint8_t crc);
uint16_t calcCRC16(uint8_t Data, uint16_t crc);
uint16_t calcCRC16Array(uint16_t Length, uint8_t *Data, uint16_t crc);

#endif /* CRC_H_ */
