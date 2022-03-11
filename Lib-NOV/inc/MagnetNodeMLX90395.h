/*
 * MagentNodeMLX90395.h
 *
 *  Created on: 22.05.2019
 *********************************************
 *    (c)2016-2019 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// Melexis MLX90395 3-Axis Magnetometer-Node related stuff
#ifndef MAGNETNODE_H_
#define MAGNETNODE_H_

#include "CalibrateMagnetics.h"
#include <si_toolchain.h>

typedef struct {
	CalibAxisStruct xAxis;
	CalibAxisStruct yAxis;
	CalibAxisStruct zAxis;
	TemperatureStruct Temp;
	uint8_t BusAddress;
	uint8_t BusChannel;
	uint8_t Gain;
	uint8_t Resolution;
	uint8_t OSR;
	uint8_t Filter;
	uint8_t OperationMode;
	uint8_t ReadMode;
	uint8_t State;
	uint8_t Crc;
	SI_UU16_t Voltage;
	uint8_t AutoTempCompensation;
	SI_UU16_t ID1;
	SI_UU16_t ID2;
	SI_UU16_t ID3;
	uint8_t LastFrameCounter;
	uint8_t retry;
} MLX90395_BaseStruct;

#define	BASE_ADDR_A0Lo_A1Lo	(0x18)
#define	BASE_ADDR_A0Hi_A1Lo	(0x1A)
#define	BASE_ADDR_A0Lo_A1Hi	(0x1C)
#define	BASE_ADDR_A0Hi_A1Hi	(0x1E)

#define TOGGLE_CBUS_METERINIT 1// Make restart of Magentometer on TOGGLE_PIN pin
#define TOGGLE_CBUS_METER 1// Make successful internal update process visible on TOGGLE_PIN pin
#define TOGGLE_PIN C_BUS

#define STATE_BURST		0x80
#define STATE_WOC_CNT2	0x40
#define STATE_SM_CNT1	0x20
#define STATE_CNT0		0x10
#define STATE_CE_DED	0x08
#define STATE_SEC_OVF	0x04
#define STATE_RST		0x02
#define STATE_DRDY		0x01

#define METER_NO_ERROR (0x00)
#define METER_I2C_READ_ERROR (0x01)
#define METER_MEAS_COUNTER_ERROR (0x02)
#define METER_CRC_ERROR (0x04)
#define METER_COMMUNICAION_ERROR (0x08)
#define METER_OVERFLOW_ERROR (0x10)
#define METER_SINGLE_PARITY_ERROR (0x20)
#define METER_NOT_READY_ERROR (0x01)
#define METER_RESET_ERROR (0x80)

#define METER_SB_XY			0x16
#define METER_SB_ZXYT		0x1F

#define METER_SW_XY			0x26
#define METER_SM_XY			0x36
#define METER_RM_XY			0x46
#define METER_SM_ZXYT		0x3F
#define METER_RM_ZXYT		0x4F

#define METER_RR			0x50
#define METER_WR			0x60
#define METER_EX			0x80
#define METER_HR			0xD0
#define METER_HS			0xE0
#define METER_RT			0xF0
#define METER_NOP			0x00
#define REGISTER_80			0x80

#define CALIB_ADDR_00h_I2C 	(0x00<<1)
#define CALIB_ADDR_01h_I2C 	(0x01<<1)
#define CALIB_ADDR_02h_I2C 	(0x02<<1)
#define CALIB_ADDR_03h_I2C 	(0x03<<1)
#define CALIB_ADDR_04h_I2C 	(0x04<<1)
#define CALIB_ADDR_05h_I2C 	(0x05<<1)
#define CALIB_ADDR_06h_I2C 	(0x06<<1)
#define CALIB_ADDR_07h_I2C 	(0x07<<1)
#define CALIB_ADDR_08h_I2C 	(0x08<<1)
#define CALIB_ADDR_09h_I2C 	(0x09<<1)
#define CALIB_ADDR_0Ah_I2C 	(0x0A<<1)
#define CALIB_ADDR_0Bh_I2C 	(0x0B<<1)
#define CALIB_ADDR_0Ch_I2C 	(0x0C<<1)
#define CALIB_ADDR_0Dh_I2C 	(0x0D<<1)
#define CALIB_ADDR_0Eh_I2C 	(0x0E<<1)
#define CALIB_ADDR_0Fh_I2C 	(0x0F<<1)
#define CALIB_ADDR_10h_I2C 	(0x10<<1)

#define UID_ADDR_26h_I2C 	(0x26<<1)
#define UID_ADDR_27h_I2C 	(0x27<<1)
#define UID_ADDR_28h_I2C 	(0x28<<1)

#define CALIB_ADDR_00h_SPI 	(0x00<<2)
#define CALIB_ADDR_01h_SPI 	(0x01<<2)
#define CALIB_ADDR_02h_SPI 	(0x02<<2)
#define CALIB_ADDR_03h_SPI 	(0x03<<2)
#define CALIB_ADDR_04h_SPI 	(0x04<<2)
#define CALIB_ADDR_05h_SPI 	(0x05<<2)
#define CALIB_ADDR_06h_SPI 	(0x06<<2)
#define CALIB_ADDR_07h_SPI 	(0x07<<2)
#define CALIB_ADDR_08h_SPI 	(0x08<<2)
#define CALIB_ADDR_09h_SPI 	(0x09<<2)
#define CALIB_ADDR_0Ah_SPI 	(0x0A<<2)
#define CALIB_ADDR_0Bh_SPI 	(0x0B<<2)
#define CALIB_ADDR_0Ch_SPI 	(0x0C<<2)
#define CALIB_ADDR_0Dh_SPI 	(0x0D<<2)
#define CALIB_ADDR_0Eh_SPI 	(0x0E<<2)
#define CALIB_ADDR_0Fh_SPI 	(0x0F<<2)
#define CALIB_ADDR_10h_SPI 	(0x10<<2)

#define VALUE_RES_0 	(((0x0000)+(0x0000<<2)+(0x0000<<4)))
#define VALUE_RES_1 	(((0x0001)+(0x0001<<2)+(0x0001<<4)))
#define VALUE_RES_2 	(((0x0002)+(0x0002<<2)+(0x0002<<4)))
#define VALUE_RES_3 	(((0x0003)+(0x0003<<2)+(0x0003<<4)))
#define MASK_RES	 	(((0x0003)+(0x0003<<2)+(0x0003<<4))<<5)

#define VALUE_GAIN_0 	(0x0000)
#define VALUE_GAIN_1 	(0x0001)
#define VALUE_GAIN_2 	(0x0002)
#define VALUE_GAIN_3 	(0x0003)
#define VALUE_GAIN_4 	(0x0004)
#define VALUE_GAIN_5 	(0x0005)
#define VALUE_GAIN_6 	(0x0006)
#define VALUE_GAIN_7 	(0x0007)
#define VALUE_GAIN_8 	(0x0008)
#define VALUE_GAIN_9 	(0x0009)
#define VALUE_GAIN_10 	(0x000A)
#define VALUE_GAIN_11 	(0x000B)
#define VALUE_GAIN_12 	(0x000C)
#define VALUE_GAIN_13 	(0x000D)
#define VALUE_GAIN_14 	(0x000E)
#define VALUE_GAIN_15 	(0x000F)
#define MASK_GAIN   	(0x000F<<4)

#define GAIN_0_100	VALUE_GAIN_8
#define GAIN_0_125	VALUE_GAIN_9
#define GAIN_0_200	VALUE_GAIN_0
#define GAIN_0_200_2	VALUE_GAIN_11
#define GAIN_0_250	VALUE_GAIN_1
#define GAIN_0_250_2	VALUE_GAIN_12
#define GAIN_0_300	VALUE_GAIN_13
#define GAIN_0_333	VALUE_GAIN_2
#define GAIN_0_375	VALUE_GAIN_14
#define GAIN_0_400	VALUE_GAIN_3
#define GAIN_0_500	VALUE_GAIN_4
#define GAIN_0_500_2	VALUE_GAIN_15
#define GAIN_0_600	VALUE_GAIN_5
#define GAIN_0_667	VALUE_GAIN_10
#define GAIN_0_750	VALUE_GAIN_6
#define GAIN_1_000	VALUE_GAIN_7

#define VALUE_FILTER_0 	(0x0000)
#define VALUE_FILTER_1 	(0x0001)
#define VALUE_FILTER_2 	(0x0002)
#define VALUE_FILTER_3 	(0x0003)
#define VALUE_FILTER_4 	(0x0004)
#define VALUE_FILTER_5 	(0x0005)
#define VALUE_FILTER_6 	(0x0006)
#define VALUE_FILTER_7 	(0x0007)
#define MASK_FILTER 	(0x0007<<2)

#define VALUE_OSR_0 	(0x0000)
#define VALUE_OSR_1 	(0x0001)
#define VALUE_OSR_2 	(0x0002)
#define VALUE_OSR_3 	(0x0003)
#define MASK_OSR	 	(0x0003)

#define VALUE_OSR2_0 	(0x0000)
#define VALUE_OSR2_1 	(0x0001)
#define VALUE_OSR2_2 	(0x0002)
#define VALUE_OSR2_3 	(0x0003<<11)

#define VALUE_TCMP_EN_0 (0x0000)
#define VALUE_TCMP_EN_1 (0x0001)
#define MASK_TCMP_EN	(0x0001<<10)
#define VALUE_COMM_MODE_0 	(0x0000)
#define VALUE_COMM_MODE_1 	(0x0001)
#define VALUE_COMM_MODE_2 	(0x0002)
#define VALUE_COMM_MODE_3 	(0x0003)
#define MASK_COMM_MODE	 	(0x0003<<13)

float CalcMicroTeslaMeter(MLX90395_BaseStruct *MeterData, uint16_t VectorValue);
float CalcCelsiusDegreeMeter(MLX90395_BaseStruct *MeterData);

uint8_t GetMagnetometer(MLX90395_BaseStruct *MeterData);
uint8_t InitMeterI2C(MLX90395_BaseStruct *MeterData);
uint8_t GetMeterOnceI2C(MLX90395_BaseStruct *MeterData);
uint8_t SetOperationMeter(MLX90395_BaseStruct *MeterData);
uint8_t CheckForMeterI2C(MLX90395_BaseStruct *MeterData);

bool IsMeterReset(uint8_t state);
bool IsMeterBurstMode(uint8_t state);
bool IsMeterSingleMode(uint8_t state);
bool IsMeterWOCMode(uint8_t state);
bool IsMeterError(uint8_t state);
bool IsMeterAbsent(uint8_t state);

#endif /* MAGNETNODE_H_ */
