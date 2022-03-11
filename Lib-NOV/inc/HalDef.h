/*
 * HalDef.h
 *
 *  Created on: 11.07.2016
 *********************************************
 *     c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef HALDEF_H_
#define HALDEF_H_

#include "si_toolchain.h"
#ifdef SK400
#include "SI_EFM8UB3_Register_Enums.h"
#endif
#ifdef SK202
#include "SI_EFM8BB3_Register_Enums.h"
#endif
#ifdef IO_LINK
#include "SI_EFM8UB3_Register_Enums.h"
#endif
#ifdef SK60USB
#include "SI_EFM8UB3_Register_Enums.h"
#endif



/*****************************************/
// switches for conditional compiling
//****************************************

//#define TESTWARE      // This is the final and Tested Firmware
//#define CORDIC        // comment out for MATHLIB trigonometrical function
#define I2C_VERSION   // comment out to use SPI-interface of magnetometer
//#define NO_WATCHDOG      // comment out to not use watchdog function

#define FAST_SINGLE_CH	// Only one I2C channel supported, but this one multiple times faster

#ifndef NDEBUG
#define TESTWARE      // comment out for final FIRMWARE
//#define CORDIC        // comment out for MATHLIB trigonometrical function
//#define I2C_VERSION   // comment out to use SPI-interface of magnetometer
#define NO_WATCHDOG      // comment out to not use watchdog function
//#define FAST_VERSION	// Very fast but stack consuming version
#endif
//*****************************************
#define START_IOLINK_FRAM       0x1000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
#define START_APP_FRAM          0x0000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
#define START_APP_FRAM_PAGE0    0x0000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
#define START_APP_FRAM_PAGE1    0x0800 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
#define START_APP_FRAM_PAGE2    0x1000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
#define START_APP_FRAM_PAGE3    0x1800 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)


#define SIZE_APP_FRAM           START_APP_FRAM_PAGE2 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
#define SIZE_IOLINK_FRAM        START_APP_FRAM_PAGE2 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
#define START_SCRATCHPAD_FRAM	START_APP_FRAM_PAGE3
#define START_SETTINGS_FRAM     START_APP_FRAM // StartAddress of FRAM-Page for Settings (Calibration)

#define  PB_NEW_EVENT (0x0f)
#define  PB_PRESENT (0x01)
#define  PB_ABSENT (0x00)

// IO-Pin Configuration
//*****************************************
#ifdef SK202
SI_SBIT(SCLK_IO, SFR_P1, 4);
SI_SBIT(MISO_IO, SFR_P0, 7);
SI_SBIT(MOSI_IO, SFR_P1, 3);
SI_SBIT(NCS_IO, SFR_P1, 1);

SI_SBIT(INT_DRDY, SFR_P0, 0);
SI_SBIT(PB_IN, SFR_P0, 1); // Switch
SI_SBIT(nLED_RED, SFR_P0, 2);// Led 1
SI_SBIT(nLED_GREEN, SFR_P0, 3);// Led 2
SI_SBIT(TX_B, SFR_P0, 4);
SI_SBIT(RX_B, SFR_P0, 5);
SI_SBIT(SPI_SCLK_IO, SFR_P0, 6);
SI_SBIT(SPI_MISO_IO, SFR_P0, 7);
SI_SBIT(SPI_MOSI_IO, SFR_P1, 0);
SI_SBIT(SPI_NCS_IO, SFR_P1, 1);
SI_SBIT(TRG, SFR_P1, 2);
SI_SBIT(I2C_SDA, SFR_P1, 3);
SI_SBIT(I2C_SCL, SFR_P1, 4);
SI_SBIT(TX_A, SFR_P1, 5);
SI_SBIT(AN_IN, SFR_P1, 6);
SI_SBIT(RX_A, SFR_P2, 0);
SI_SBIT(CTS_A, SFR_P2, 1);
SI_SBIT(RTS_A, SFR_P2, 2);
SI_SBIT(AN_OUT, SFR_P2, 3);
SI_SBIT(C2D, SFR_P3, 0);
#endif //SK202

#ifdef SK400
SI_SBIT(INT_DRDY, SFR_P0, 0);
SI_SBIT(PB_IN, SFR_P0, 1);     // Switch

SI_SBIT(SCLK_IO, SFR_P0, 2);
SI_SBIT(MISO_IO, SFR_P0, 3);
SI_SBIT(MOSI_IO, SFR_P0, 4);
SI_SBIT(NCS_IO, SFR_P0, 5);

SI_SBIT(TRG, SFR_P1, 0);

SI_SBIT(nLED_RED, SFR_P1, 1);  // Led 1
SI_SBIT(nLED_GREEN, SFR_P1, 2);  // Led 2

SI_SBIT(AUX_C, SFR_P2, 0);
SI_SBIT(C_BUS, SFR_P3, 1);

SI_SBIT(I2C_SDA, SFR_P0, 6);
SI_SBIT(I2C_SCL, SFR_P0, 7);
#endif //SK400

#ifdef SK60USB
SI_SBIT(SCLK_IO, SFR_P0, 0);
SI_SBIT(MISO_IO, SFR_P0, 1);
SI_SBIT(MOSI_IO, SFR_P0, 2);
SI_SBIT(I2C_SDA, SFR_P0, 3);
SI_SBIT(I2C_SCL, SFR_P0, 4);
SI_SBIT(INT_IO,  SFR_P0, 5);
SI_SBIT(INT_MAIN,SFR_P0, 6);
SI_SBIT(INT_RED, SFR_P0, 7);
SI_SBIT(TP2,     SFR_P1, 0);
SI_SBIT(TP3,     SFR_P1, 1);
SI_SBIT(TP4,     SFR_P1, 2);
SI_SBIT(TRG_MAIN,SFR_P1, 3);
SI_SBIT(TRG_RED, SFR_P1, 4);
SI_SBIT(NCS_IO,  SFR_P1, 5);
SI_SBIT(AUX_B,   SFR_P1, 6);
SI_SBIT(TP5,     SFR_P1, 7);
SI_SBIT(TP6,     SFR_P2, 0);
SI_SBIT(TP7,     SFR_P2, 1);
SI_SBIT(AUX_A,   SFR_P2, 2);
SI_SBIT(C_BUS,   SFR_P2, 3);
//SI_SBIT(AUX_C,   SFR_P3, 0);
#endif //SK60USB

//SI_SBIT(Ch2I2C_SDA, SFR_P1, 0);
//SI_SBIT(Ch2I2C_SCL, SFR_P1, 1);

SI_SBIT(I2C_SDA_Ch1, SFR_P0, 6);
SI_SBIT(I2C_SCL_Ch1, SFR_P0, 7);
SI_SBIT(I2C_SDA_Ch0, SFR_P0, 6);
SI_SBIT(I2C_SCL_Ch0, SFR_P0, 7);

#define I2C_Channel_Base (0)
#ifdef FAST_SINGLE_CH
#define I2C_Channel_Satellite (0)
#else
#define I2C_Channel_Satellite (1)
#endif

#ifdef IO_LINK
SI_SBIT(VCC_M, SFR_P0, 0);
SI_SBIT(INT_IO, SFR_P0, 1);
SI_SBIT(SCLK_IO, SFR_P0, 2);
SI_SBIT(MISO_IO, SFR_P0, 3);
SI_SBIT(MOSI_IO, SFR_P0, 4);
SI_SBIT(NCS_IO, SFR_P0, 5);
SI_SBIT(I2C_SDA, SFR_P0, 6);
SI_SBIT(I2C_SCL, SFR_P0, 7);
SI_SBIT(ID_IN, SFR_P1, 0);
SI_SBIT(AUX_A, SFR_P1, 1);
SI_SBIT(AUX_B, SFR_P1, 2);
SI_SBIT(AUX_C, SFR_P2, 0);
SI_SBIT(C_BUS, SFR_P2, 1);
SI_SBIT(PB_IN, SFR_P2, 1);

#define	BASE_ADDR_RED		0x1C
#define	WRITE_ADDR_RED		0x1C
#define	READ_ADDR_RED		0x1D

#define	BASE_ADDR_LED		0xDE
#define	WRITE_ADDR_LED		0xDE
#define	READ_ADDR_LED		0xDF

#define	BASE_ADDR_TWIST		0x18
#define	WRITE_ADDR_TWIST	0x18
#define	READ_ADDR_TWIST		0x19

#define	BASE_ADDR_TILT		0x3A
#define	WRITE_ADDR_TILT		0x3A
#define	READ_ADDR_TILT		0x3B

#define	BASE_ADDR_PORT		0x40
#define	WRITE_ADDR_PORT		0x40
#define	READ_ADDR_PORT		0x41

#define	BASE_ADDR_ILLUM		0x64
#define	WRITE_ADDR_ILLUM	0x64
#define	READ_ADDR_ILLUM		0x65

#define	BASE_ADDR_VIBRA		0xB4
#define	WRITE_ADDR_VIBRA	0xB4
#define	READ_ADDR_VIBRA		0xB5

#define	BASE_ADDR_MAIN		0x1A
#define	WRITE_ADDR_MAIN		0x1A
#define	READ_ADDR_MAIN		0x1B

#define	BASE_ADDR_ADC1		0x90
#define	WRITE_ADDR_ADC1		0x90
#define	READ_ADDR_ADC1		0x91

#define	BASE_ADDR_ADC2		0x92
#define	WRITE_ADDR_ADC2		0x92
#define	READ_ADDR_ADC2		0x93

#define	BASE_ADDR_FRAM		0xA0
#define	WRITE_ADDR_FRAM		0xA0
#define	READ_ADDR_FRAM		0xA1

#define	BASE_ADDR_SENSOR1	0xBC
#define	WRITE_ADDR_SENSOR1	0xBC
#define	READ_ADDR_SENSOR1   0xBD

#define	BASE_ADDR_SENSOR2	0x3E
#define	WRITE_ADDR_SENSOR2	0x3E
#define	READ_ADDR_SENSOR2   0x3F

#endif //IO_LINK

// Counter Delay Loops
/*******************************************/

#ifdef FAST_SINGLE_CH
#define DelayTime4I2C	(1) // (2 = 1ms Clock)
#else
#define DelayTime4I2C	(0) // (2 = 1ms Clock)
#endif
#define DelayTime4SPI	(1)
#define DelayIteration4I2C	(10)
#ifdef IO_LINK
#define RESET_DELAY_3ms (850) //(1000 = 3,54ms)
#else
#define RESET_DELAY_3ms (500)
#endif

//Base-Addresses for I2C Devices
/*******************************************/
// Melexis MLX90393 on I2C BUS
#define	BASE_ADDR_DIAL		BASE_ADDR_A0Lo_A1Lo
#define	BASE_ADDR_DIAL_X	BASE_ADDR_A0Lo_A1Lo
#define	BASE_ADDR_DIAL_Y	BASE_ADDR_A0Hi_A1Hi
#define	BASE_ADDR_DIAL0		BASE_ADDR_A0Lo_A1Hi
#define	BASE_ADDR_LEVER  	BASE_ADDR_A0Hi_A1Lo

#define	UUID_POS (0xffc0) // Location in Flash of Unique serial Number from MCU Manufacturer
// #define START_ADDRESS          0x3800 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB1)
#define START_ADDRESS_FLASH     0x9800 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)

//#define START_IOLINK_FRAM       0x1000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
//#define START_APP_FRAM          0x0000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
//#define SIZE_APP_FRAM           0x1000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
//#define SIZE_IOLINK_FRAM        0x1000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
//#define START_SETTINGS_FRAM     START_APP_FRAM // StartAddress of FRAM-Page for Settings (Calibration)


//uint8_t UpdateComState();
void CountDelay(uint32_t count);
uint8_t UpdateButton1State(uint8_t *ButtonState);
uint8_t UpdateButton2State(uint8_t *ButtonState);
uint8_t UpdateButtonState(uint8_t *ButtonState);
uint8_t UpdateSignalState(uint8_t *SignalState, bool Signal);
uint8_t WriteByte (uint8_t Data);
uint8_t getCharModal(bool NonBlocking, uint8_t defaultChar);
void LoopDelay3ms(void);
void LoopDelay150ms(void);
void LoopDelayShort(void);
void WatchdogRefresh(void);
void WatchdogDisable(void);
void WatchdogEnable(void);
uint8_t getCharModalCom0(bool NonBlocking, uint8_t defaultChar);
uint8_t getCharModalCom1(bool NonBlocking, uint8_t defaultChar);
uint8_t WriteByteCom0 (uint8_t Data);
uint8_t WriteByteCom1 (uint8_t Data);
void putStrCom0(uint8_t outString[]);
void putStrCom1(uint8_t outString[]);
void clearCom0(void);

#endif /* HALDEF_H_ */
