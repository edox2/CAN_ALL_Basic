/*
 * IoLink.h
 *
 *  Created on: 12.07.2016
 *********************************************
 *      (c)2016 SIGITRONIC SOFTWARE          *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef IOLINK_H_
#define IOLINK_H_

#include "stdint.h"

#define IO_MSEQ_READ	(0x00)
#define IO_MSEQ_WRITE	(0x80)
#define IO_CFG_READ		(0x01)
#define IO_CFG_WRITE	(0x81)
#define IO_CCTL_READ	(0x02)
#define IO_CCTL_WRITE	(0x82)
#define IO_DCTL_READ	(0x03)
#define IO_DCTL_WRITE	(0x83)
#define IO_LINK_READ	(0x04)
#define IO_LINK_WRITE	(0x84)
#define IO_THERM_READ	(0x05)
#define IO_THERM_WRITE	(0x85)
#define IO_TEMP_READ	(0x06)
#define IO_TEMP_WRITE	(0x86)
#define IO_LED_READ		(0x07)
#define IO_LED_WRITE	(0x87)
#define IO_DCDC_READ	(0x08)
#define IO_DCDC_WRITE	(0x88)
#define IO_DSTAT_READ	(0x09)
#define IO_DSTAT_WRITE	(0x89)
#define IO_STATUS_READ	(0x0a)
#define IO_STATUS_WRITE	(0x8a)
#define IO_FR0_READ		(0x10)
#define IO_FR0_WRITE	(0x90)
#define IO_FR1_READ		(0x11)
#define IO_FR1_WRITE	(0x91)
#define IO_FR2_READ		(0x12)
#define IO_FR2_WRITE	(0x92)
#define IO_FR3_READ		(0x13)
#define IO_FR3_WRITE	(0x93)
#define IO_FR4_READ		(0x14)
#define IO_FR4_WRITE	(0x94)
#define IO_FR5_READ		(0x15)
#define IO_FR5_WRITE	(0x95)
#define IO_FR6_READ		(0x16)
#define IO_FR6_WRITE	(0x96)
#define IO_FR7_READ		(0x17)
#define IO_FR7_WRITE	(0x97)
#define IO_FR8_READ		(0x18)
#define IO_FR8_WRITE	(0x98)
#define IO_FR9_READ		(0x19)
#define IO_FR9_WRITE	(0x99)
#define IO_FR10_READ	(0x1a)
#define IO_FR10_WRITE	(0x9a)
#define IO_FR11_READ	(0x1b)
#define IO_FR11_WRITE	(0x9b)
#define IO_FR12_READ	(0x1c)
#define IO_FR12_WRITE	(0x9c)
#define IO_FR13_READ	(0x1d)
#define IO_FR13_WRITE	(0x9d)
#define IO_FR14_READ	(0x1e)
#define IO_FR14_WRITE	(0x9e)

uint8_t ReadWriteByteSPI(uint8_t DataToSend);

#endif /* IOLINK_H_ */
