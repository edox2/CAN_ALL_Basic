/*
 * HallDriverTLE493D_W2B6.h
 *
 *  Created on: 12.07.2016
 *********************************************
 *    (c)2018-2019 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
// Infineon TLE493D-W2B6-Ax 3D-Magnetic Sensor related stuff
#ifndef HALLSENSOR_TLE493D_H_
#define HALLSENSOR_TLE493D_H_

#include <si_toolchain.h>
#include "CalibrateMagnetics.h"

enum hallSensorState_t {
	HALLSENSOR_NOT_CALIBRATED,
	HALLSENSOR_CALIBRATED,
	HALLSENSOR_CALIB_CLEARED,
	HALLSENSOR_CALIB_CENTERPOINT,
	HALLSENSOR_CALIB_CENTERFIELD,
	HALLSENSOR_CALIB_EXTREMAS,
	HALLSENSOR_CALIB_XPOS,
	HALLSENSOR_CALIB_XNEG,
	HALLSENSOR_CALIB_YPOS,
	HALLSENSOR_CALIB_YNEG,
};

enum TLV493D_Mode_t {
	TLE493D_ModeLowPower = 0,
	TLE493D_ModeMasterControlled = 1,
	TLE493D_ModeFast = 3
};

enum TLV493D_LowPowerUpdateRate_t {
	TLE493D_Rate777Hz = 0x00,
	TLE493D_Rate97Hz = 0x20,
	TLE493D_Rate24Hz = 0x40,
	TLE493D_Rate12Hz = 0x60,
	TLE493D_Rate6Hz = 0x80,
	TLE493D_Rate3Hz = 0x0A,
	TLE493D_Rate0Hz4 = 0x0C,
	TLE493D_Rate0Hz05 = 0x0E
};

enum TLV493D_TempCompensation_t {
	TLE493D_TempCompNP0 = 0x00,
	TLE493D_TempCompN750 = 0x02,
	TLE493D_TempCompN1500 = 0x04,
	TLE493D_TempCompP350 = 0x06
};

enum TLV493D_Trigger_t {
	TLE493D_TriggerNotOnRead = 0x00,
	TLE493D_TriggerBeforeMSB = 0x10,
	TLE493D_TriggerAfterRef05 = 0x20
};


typedef struct {
	CalibAxisStruct xAxis;
	CalibAxisStruct yAxis;
	CalibAxisStruct zAxis;
	TemperatureStruct tValue;
	uint8_t Bx_00H;
	uint8_t By_01H;
	uint8_t Bz_02H;
	uint8_t Temp_03H;
	uint8_t Bx2_04H;
	uint8_t Temp2_05H;
	uint8_t Diag_06H;
	uint8_t XL_07H;
	uint8_t XH_08H;
	uint8_t YL_09H;
	uint8_t YH_0AH;
	uint8_t ZL_0BH;
	uint8_t ZH_0CH;
	uint8_t WU_0DH;
	uint8_t TMode_0EH;
	uint8_t TPhase_0FH;
	uint8_t Config_10H;
	uint8_t MOD1_11H;
	uint8_t Reserved_12H;
	uint8_t MOD2_13H;
	uint8_t Reserved_14H;
	uint8_t Reserved_15H;
	uint8_t Ver_16H;
	uint8_t LowPowerPeriod;
	uint8_t DisableTemperature;
	uint8_t Angular;
	uint8_t TriggerOption;
	uint8_t ShortRangeSensitivity;
	uint8_t TempCompensation;
	uint8_t Mode;
	uint8_t InterruptDisabled;
	uint8_t CollisionAvoidanceDisabled;
	uint8_t ProtocolOneByteReadEnabled;
	uint8_t BusAddress;
	uint8_t BusChannel;
	uint8_t FirstRun;
	uint8_t LastFrameCounter;
	uint8_t retry;
} TLE493D_BaseStruct;

#define TOGGLE_CBUS_SENSOR 0// Make successful internal update process visible on TOGGLE_PIN pin
#define TOGGLE_PIN C_BUS

#define	HALLSENSOR0_ADDR	(0x6A)
#define	HALLSENSOR1_ADDR	(0x44)
#define	HALLSENSOR2_ADDR	(0xF0)
#define	HALLSENSOR3_ADDR	(0x88)

#define I2C_READ_ERROR (0x01)
#define FRAME_COUNTER_ERROR (0x02)
#define POWERDOWN_3_ERROR (0x04)
#define POWERDOWN_0_ERROR (0x08)
#define TESTMODE_ERROR (0x10)
#define CONFIG_PARITY_ERROR (0x20)
#define FUSE_PARITY_ERROR (0x40)
#define BUS_PARITY_ERROR (0x80)

#define	SENSOR_TLE493D_W2B6_A0_ADDR	HALLSENSOR0_ADDR
#define	SENSOR_TLE493D_W2B6_A1_ADDR	HALLSENSOR1_ADDR
#define	SENSOR_TLE493D_W2B6_A2_ADDR	HALLSENSOR2_ADDR
#define	SENSOR_TLE493D_W2B6_A3_ADDR	HALLSENSOR3_ADDR

float CalcCelsiusDegreeSensor(TLE493D_BaseStruct *SensorData);
float CalcMicroTeslaHallSensor(int16_t VectorValue);
void ResetHallSensors(uint8_t Channel);
uint8_t GetHallSensorRegisters(TLE493D_BaseStruct *SensorData, bool Shortform);
uint8_t GetHallSensorOnce(TLE493D_BaseStruct *SensorData);
uint8_t InitHallSensor(TLE493D_BaseStruct *SensorData);
uint8_t GetMagnetSensor(TLE493D_BaseStruct *SensorData);


#endif /* HALLSENSOR_TLE493D_H_ */
