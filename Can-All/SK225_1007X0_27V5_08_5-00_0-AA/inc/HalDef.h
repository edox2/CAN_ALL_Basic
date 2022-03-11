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

#include "SI_EFM8UB3_Register_Enums.h"
#include "si_toolchain.h"

#define  PB_NEW_EVENT (0x0f)
#define  PB_EDGE (0x03)
#define  PB_PRESENT (0x01)
#define  PB_ABSENT (0x00)
#define	 UUID_POS (0xffc0) // Location in Flash of Unique serial Number from MCU Manufacturer

#define RESET_DELAY_3ms (500)

extern uint8_t I2C_State;

extern bool VERS_100750;
extern bool VERS_100750V8;

void LoopDelay3ms(void);
uint8_t UpdateButtonState(uint8_t *ButtonState);
uint8_t UpdateComState();
void CountDelay(uint32_t count);


#define	BASE_ADDR_ILLUM		0x64
#define	WRITE_ADDR_ILLUM	0x64
#define	READ_ADDR_ILLUM		0x65

// IO-Pin Configuration
/*****************************************/

//SI_SBIT(AUX_E_100750,   SFR_P2, 0);
SI_SBIT(VER_1,   SFR_P0, 6);
SI_SBIT(VER_2,   SFR_P0, 7);
SI_SBIT(VER_3,   SFR_P1, 0);

SI_SBIT(DBG_LED,   SFR_P1, 1);

SI_SBIT(PB_IN,   SFR_P1, 2);
//SI_SBIT(C_BUS,   SFR_P2, 1);

SI_SBIT(I2C_SDA_Ch1, SFR_P0, 2);
SI_SBIT(I2C_SCL_Ch1, SFR_P0, 3);
SI_SBIT(I2C_SDA_Ch0, SFR_P0, 0);
SI_SBIT(I2C_SCL_Ch0, SFR_P0, 1);

SI_SBIT(I2C_SDA_Ch3, SFR_P0, 2);
SI_SBIT(I2C_SCL_Ch3, SFR_P0, 3);
SI_SBIT(I2C_SDA_Ch2, SFR_P0, 0);
SI_SBIT(I2C_SCL_Ch2, SFR_P0, 1);

//SI_SBIT(I2C_SDA_Ch3, SFR_P0, 6);
//SI_SBIT(I2C_SCL_Ch3, SFR_P0, 7);
//SI_SBIT(I2C_SDA_Ch2, SFR_P1, 0);
//SI_SBIT(I2C_SCL_Ch2, SFR_P1, 1);

#define I2C_Channel_Base_100750 (0)
#define I2C_Channel_Satellite_100750 (1)
#define I2C_Channel_Base_100700 (2)
#define I2C_Channel_Satellite_100700 (3)

// Counter Delay Loops
/*******************************************/
#define DelayTime4I2C	(1)
#define DelayIteration4I2C	(10)
//Base-Addresses for I2C Devices
/*******************************************/
// Melexis MLX90393 on I2C BUS
#define	BASE_ADDR_DIAL_X	BASE_ADDR_A0Lo_A1Lo
//#define	WRITE_ADDR_DIAL_X	0x18
//#define	READ_ADDR_DIAL_X	0x19

#define	BASE_ADDR_DIAL_Y	BASE_ADDR_A0Hi_A1Hi
//#define	WRITE_ADDR_DIAL_Y	0x1E
//#define	READ_ADDR_DIAL_Y	0x1F

#define	BASE_ADDR_DIAL0		BASE_ADDR_A0Lo_A1Hi
//#define	WRITE_ADDR_LEVER	0x1A
//#define	READ_ADDR_LEVER		0x1B

#define	BASE_ADDR_LEVER  	BASE_ADDR_A0Hi_A1Lo
//#define	WRITE_ADDR_DIAL_0	0x1C
//#define	READ_ADDR_DIAL_0	0x1D

#define START_ADDRESS_CALIB_FLASH          0x9000 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)
#define SIZE_CALIB_FLASH			 0x0800 // Size of FLASH-Pages for Settings (make sure it dont't conflicts with USB-Bootloader of EFM8UB3 starting at 0x9A00)
#define PAGE_SIZE_FLASH 			 0x0200 // Size if a single FLASH-Page that will be erased at once

//#define START_ADDRESS_FLASH          0x9800 // StartAddress of FLASH-Page for Settings (Dont't conflicts with USB-Bootloader EFM8UB3)

#endif /* HALDEF_H_ */
