/*
 * IoLinkPhy.h
 *
 *  Created on: 12.07.2016
 *********************************************
 *    (c)2016-2017 SIGITRONIC SOFTWARE       *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

// IO-Link Phy HMT7742 or HMT7748 related stuff

#ifndef IOLINKPHY_H_
#define IOLINKPHY_H_

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

#define SPI_READ			0x01
#define SPI_WRITE   		0x00

#define LED_LEVEL_OFF_GREEN (0)    //!< LED off
#define LED_LEVEL_1_GREEN (0x10)      //!< ~0.5mA
#define LED_LEVEL_2_GREEN (0x20)      //!< ~1.0mA
#define LED_LEVEL_3_GREEN (0x30)      //!< ~1.5mA
#define LED_LEVEL_4_GREEN (0x40)      //!< ~2.0mA
#define LED_LEVEL_5_GREEN (0x50)      //!< ~2.5mA
#define LED_LEVEL_6_GREEN (0x60)      //!< ~3.0mA
#define LED_LEVEL_7_GREEN (0x70)      //!< ~3.5mA

#define LED_LEVEL_OFF_RED (0x00)    //!< LED off
#define LED_LEVEL_1_RED (0x01)      //!< ~0.5mA
#define LED_LEVEL_2_RED (0x02)      //!< ~1.0mA
#define LED_LEVEL_3_RED (0x03)      //!< ~1.5mA
#define LED_LEVEL_4_RED (0x04)      //!< ~2.0mA
#define LED_LEVEL_5_RED (0x05)      //!< ~2.5mA
#define LED_LEVEL_6_RED (0x06)      //!< ~3.0mA
#define LED_LEVEL_7_RED (0x07)      //!< ~3.5mA



enum LedLevel {
	LED_LEVEL_OFF,    //!< LED off
	LED_LEVEL_1,      //!< ~0.5mA
	LED_LEVEL_2,      //!< ~1.0mA
	LED_LEVEL_3,      //!< ~1.5mA
	LED_LEVEL_4,      //!< ~2.0mA
	LED_LEVEL_5,      //!< ~2.5mA
	LED_LEVEL_6,      //!< ~3.0mA
	LED_LEVEL_7,      //!< ~3.5mA
	LED_LEVEL_MAX = LED_LEVEL_7,
	LED_LEVEL_INVALID //!< not a LED level
};

enum MseqRegister {
	MSEQ_M2CNT_SHIFT = 2, MSEQ_OD_1 = 0 << 0, MSEQ_OD_2 = 1 << 0, MSEQ_OD_8 = 2
			<< 0
};

enum CfgRegister {
	CFG_NONE = 0,
	CFG_UVT_18_0V = 0 << 5,
	CFG_UVT_16_3V = 1 << 5,
	CFG_UVT_15_0V = 2 << 5,
	CFG_UVT_13_9V = 3 << 5,
	CFG_UVT_12_0V = 4 << 5,
	CFG_UVT_10_0V = 5 << 5,
	CFG_UVT_8_6V = 6 << 5,
	CFG_UVT_7_2V = 7 << 5,
	CFG_BD_38400 = 0 << 4,   //!< COM2
	CFG_BD_230400 = 1 << 4,   //!< COM3
	CFG_RF_ABS = 0 << 3,
	CFG_RF_REL = 1 << 3,
	CFG_S5V_SS = 0 << 0,
	CFG_S5V_3_3V = 2 << 0,
	CFG_S5V_5_0V = 3 << 0,
};

enum CtlRegister {
	CTL_NONE = 0,
	CTL_TRNS_MODE = 1 << 7,
	CTL_SCT_190MA = 0 << 4,
	CTL_SCT_210MA = 1 << 4,
	CTL_SCT_230MA = 2 << 4,
	CTL_SCT_250MA = 3 << 4,
	CTL_SCT_110MA = 4 << 4,
	CTL_SCT_130MA = 5 << 4,
	CTL_SCT_150MA = 6 << 4,
	CTL_SCT_170MA = 7 << 4,
	CTL_SGL_MODE = 1 << 3,
	CTL_IEN_MODE = 1 << 3,
	CTL_IOLINK_MODE = 0 << 2,
	CTL_DIO = 1 << 2,
	CTL_JOIN = 0 << 2,
	CTL_SIO_MODE = 1 << 2,
	CTL_HS = 1 << 1,
	CTL_LS = 1 << 0
};

enum LinkRegister {
	LINK_NONE = 0, LINK_CNT_MASK = 0x3C, LINK_CNT_SHIFT = 2,

	LINK_END = 1 << 1, LINK_SND = 1 << 0
};

enum StatusRegister {
	STATUS_NONE = 0, STATUS_RST = 1 << 7, STATUS_INT = 1 << 6, STATUS_UV = 1
			<< 5, STATUS_DINT = 1 << 4, STATUS_CHK = 1 << 3, STATUS_DAT = 1
			<< 2, STATUS_SSC = 1 << 1, STATUS_SOT = 1 << 0
};

enum TempRegister {
	TEMP_NONE = 0
};

enum DcDcRegister {
	DCDC_NONE = 0,
	DCDC_DIS = 1 << 7,
	DCDC_BYP = 1 << 6,
	DCDC_FSET_500kHz = 4 << 3,
	DCDC_FSET_625kHz = 5 << 3,
	DCDC_FSET_710kHz = 6 << 3,
	DCDC_FSET_830kHz = 7 << 3,
	DCDC_FSET_1000kHz = 0 << 3,
	DCDC_FSET_1250kHz = 1 << 3,
	DCDC_FSET_1670kHz = 2 << 3,
	DCDC_FSET_2000kHz = 3 << 3,
	DCDC_VSET_4V2 = 4,
	DCDC_VSET_4V5 = 5,
	DCDC_VSET_4V9 = 6,
	DCDC_VSET_5V4 = 7,
	DCDC_VSET_6V0 = 0,
	DCDC_VSET_6V7 = 1,
	DCDC_VSET_7V8 = 2,
	DCDC_VSET_9V5 = 3
};

enum DstatRegister {
	DSTAT_NONE = 0, DSTAT_LVL = 1 << 2, DSTAT_SSC = 1 << 1,
};


#ifdef HMT7748
//	USE_HMT 7748
enum Registers {
	REG_MSEQ = 0x00,
	REG_CFG = 0x01,
	REG_CTL = 0x02,
	REG_CCTL = 0x02,
	REG_DCTL = 0x03,
	REG_LINK = 0x04,
	REG_THERM = 0x05,
	REG_TEMP = 0x06,
	REG_LED = 0x07,
	REG_DCDC = 0x08,
	REG_DSTAT = 0x09,
	REG_STATUS = 0x0a,
	REG_FR0 = 0x10,
	REG_WRITE_BIT = 0x80
};
#else
// USE_HMT7742
enum Registers {
	REG_MSEQ = 0x00,
	REG_CFG = 0x01,
	REG_CTL = 0x02,
	REG_LINK = 0x03,
	REG_THERM = 0x04,
	REG_STATUS = 0x05,
	REG_LED = 0x06,
	REG_TEMP = 0x0c,
	REG_FR0 = 0x10,
	REG_WRITE_BIT = 0x80
};
#endif

uint8_t OneByteSPI(uint8_t DataToSend, uint8_t ReadOnly);



#endif /* IOLINKPHY_H_ */
