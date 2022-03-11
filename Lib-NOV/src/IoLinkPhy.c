/*
 * IoLinkPhy.c
 *
 *  Created on: 12.07.2016
 *********************************************
 *    (c)2016-2017 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// IO-Link Phy HMT7742 related stuff

#include <IoLinkPhy.h>
#include "HalDef.h"
#include "SpiBang.h"


uint8_t OneByteSPI(uint8_t DataToSend, uint8_t ReadOnly) {
	uint8_t index;
	static uint8_t DataRead = 0x00;

	if (!ReadOnly) {
		for (index = 8; index > 0; index--) {
			SCLK_IO = 0x00;
			MOSI_IO = DataToSend & 0x80;
			DataToSend <<= 1; // DataToSend = DataToSend << 1;
			SCLK_IO = 0x01;
			DataToSend |= MISO_IO;
		}
		DataRead = DataToSend;
		return (DataToSend);
	}
	return (DataRead);
}
