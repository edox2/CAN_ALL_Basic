//============================================================================//
// File:          mc_target.h                                                 //
// Description:   MCU target definitions                                      //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Junkersring 23                                                             //
// 53844 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11                                                     //
//                                                                            //
// The copyright to the computer program(s) herein is the property of         //
// MicroControl GmbH & Co. KG, Germany. The program(s) may be used            //
// and/or copied only with the written permission of MicroControl GmbH &      //
// Co. KG or in accordance with the terms and conditions stipulated in        //
// the agreement/contract under which the program(s) have been supplied.      //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 11.04.2014  Initial version                                                //
//                                                                            //
//============================================================================//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-10-24 13:22:36 +0200 (Fr, 24. Okt 2014) $
// SVN  $Rev: 6306 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  MC_TARGET_H_
#define  MC_TARGET_H_


//-----------------------------------------------------------------------------
/*!
** \file    mc_target.h
** \brief   MCL - MCU architecture definitions
**
** This file does the setup for the selected target MCU. The following
** symbols are defined here:
** \li  MC_MCU_SIZE_FLASH
** \li  MC_MCU_SIZE_RAM
**
** <p>
**
** \def MC_MCU_SIZE_FLASH
** The symbol \c MC_MCU_SIZE_FLASH defines the size of the internal flash
** in bytes.
**
** \def MC_MCU_SIZE_RAM
** The symbol \c MC_MCU_SIZE_RAM defines the size of the internal flash
** in bytes.
**
*/

/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "mc_target_defs.h"


/*----------------------------------------------------------------------------*\
** Definitions & Enumerations                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//#define MC_TARGET MC_MCU_MC9S12XDP512

//-------------------------------------------------------------------
// The symbol MC_TARGET defines the microcontroller
//
#ifndef MC_TARGET
#error  Target (Symbol MC_TARGET) is not defined!
#endif


/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
// Generic MCU                                                                //
//----------------------------------------------------------------------------//
#if (MC_TARGET == MC_MCU_GENERIC)

#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00008000)

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_GENERIC

#endif


//----------------------------------------------------------------------------//
// ATMEL AT32UC3C family                                                      //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_AT32UC3C)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_AT32UC3C

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if (MC_TARGET == MC_MCU_AT32UC3C064C)
//---------------------------------------------------------
// 64 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_AT32UC3C0128C)
//---------------------------------------------------------
// 128 kByte Flash, 32 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00008000)
#endif

#if (MC_TARGET == MC_MCU_AT32UC3C0256C)
//---------------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if (MC_TARGET == MC_MCU_AT32UC3C0512C)
//---------------------------------------------------------
// use processor include file from AVR
//
#include <avr32/uc3c0512c.h>

//---------------------------------------------------------
// 512 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#endif


//----------------------------------------------------------------------------//
// Freescale MC9S12X family                                                   //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_MC9S12X)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_MC9S12X

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      CP_CC_XGATE
#define CP_CHANNEL_MAX                 4

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if (MC_TARGET == MC_MCU_MC9S12XDP512)
//---------------------------------------------------------
// 64 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#endif

//----------------------------------------------------------------------------//
// Nuvoton NUC140 family                                                      //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_NUC140)

//---------------------------------------------------------
// include target dependent header files
//
#include "NUC1xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_NUC140

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_NUC140LC1CN) || (MC_TARGET == MC_MCU_NUC140RC1CN))
//---------------------------------------------------------
// 32 kByte Flash, 4 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00008000)
#define MC_MCU_SIZE_RAM                (0x00001000)
#endif

#if ((MC_TARGET == MC_MCU_NUC140LD2CN) || (MC_TARGET == MC_MCU_NUC140RD2CN))
//---------------------------------------------------------
// 64 kByte Flash, 8 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00002000)
#endif

#if ((MC_TARGET == MC_MCU_NUC140LE3CN) || (MC_TARGET == MC_MCU_NUC140RE3CN))
//---------------------------------------------------------
// 128 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_NUC140VE3CN)
//---------------------------------------------------------
// 128 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#endif


//----------------------------------------------------------------------------//
// NXP LPC176x family                                                         //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_LPC176x)

//---------------------------------------------------------
// include target dependent header files
//
#include "LPC17xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_LPC176x

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_LPC1764FBD100))
//---------------------------------------------------------
// 128 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if ((MC_TARGET == MC_MCU_LPC1765FET100) || (MC_TARGET == MC_MCU_LPC1765FBD100))
//---------------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if ((MC_TARGET == MC_MCU_LPC1766FBD100))
//---------------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if ((MC_TARGET == MC_MCU_LPC1768UK) || (MC_TARGET == MC_MCU_LPC1768FET100))
//---------------------------------------------------------
// 512 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#if ((MC_TARGET == MC_MCU_LPC1768FBD100) || (MC_TARGET == MC_MCU_LPC1769FBD100))
//---------------------------------------------------------
// 512 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif


#endif

//----------------------------------------------------------------------------//
// ST STM32F103 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F103)

//---------------------------------------------------------
// Define line the MCU belongs to, so corresponding
// register and defines are available
//
#ifndef STM32F10X_HD
#define STM32F10X_HD                   1
#endif

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f10x.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F103

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_STM32F103RC) || (MC_TARGET == MC_MCU_STM32F103VC))
//------------------------------------------------
// 256 kByte Flash, 48 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x0000C000)
#endif

//------------------------------------------------
// 384 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F103RD) || (MC_TARGET == MC_MCU_STM32F103VD))
#define MC_MCU_SIZE_FLASH              (0x00060000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 512 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F103ZC) || (MC_TARGET == MC_MCU_STM32F103ZD))
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F105 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F105)

//---------------------------------------------------------
// Define MCU product line, so corresponding
// register and definitions are available
//
#ifndef STM32F10X_CL
#define STM32F10X_CL                   1
#endif

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f107_macro.h"
#include "stm32f10x.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F105

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if ((MC_TARGET == MC_MCU_STM32F105R8) || (MC_TARGET == MC_MCU_STM32F105V8))
//------------------------------------------------
// 64 kByte Flash, 64 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 128 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F105RB) || (MC_TARGET == MC_MCU_STM32F105VB))
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F105RC) || (MC_TARGET == MC_MCU_STM32F105VC))
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F107 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F107)

//---------------------------------------------------------
// Define MCU product line, so corresponding
// register and definitions are available
//
#ifndef STM32F10X_CL
#define STM32F10X_CL                   1
#endif

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f107_macro.h"
#include "stm32f10x.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F107

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2


//------------------------------------------------
// 128 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F107RB) || (MC_TARGET == MC_MCU_STM32F107VB))
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

//------------------------------------------------
// 256 kByte Flash, 64 kByte RAM
//
#if ((MC_TARGET == MC_MCU_STM32F107RC) || (MC_TARGET == MC_MCU_STM32F107VC))
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00010000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F373 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F373)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f3xx_macro.h"
#include "stm32f37x.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F3

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//---------------------------------------------------------
// Type dependent size of Flash and SRAM memory
//
#if (MC_TARGET == MC_MCU_STM32F373C8) || (MC_TARGET == MC_MCU_STM32F373R8)
//------------------------------------------------
// 64 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373V8)
//---------------------------------------------------------
// 64 kByte Flash, 16 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00010000)
#define MC_MCU_SIZE_RAM                (0x00004000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373CB) || (MC_TARGET == MC_MCU_STM32F373RB)
//---------------------------------------------------------
// 128 kByte Flash, 24 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00006000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373VB)
//---------------------------------------------------------
// 128 kByte Flash, 24 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00006000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373CC) || (MC_TARGET == MC_MCU_STM32F373RC)
//---------------------------------------------------------
// 256 kByte Flash, 32 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00008000)
#endif

#if (MC_TARGET == MC_MCU_STM32F373VC)
//---------------------------------------------------------
// 256 kByte Flash, 32 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00008000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F405 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F405)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f4xx_macro.h"
#include "stm32f4xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F4

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type dependent size of Flash and SRAM memory
//
#if (MC_TARGET == MC_MCU_STM32F405RG) || (MC_TARGET == MC_MCU_STM32F405OG)
//-----------------------------------------------
// 1024 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F405VG) || (MC_TARGET == MC_MCU_STM32F405ZG)
//-----------------------------------------------
// 1024 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F405OE)
//-----------------------------------------------
// 512 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F407 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F407)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f4xx_macro.h"
#include "stm32f4xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F4

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Type dependent size of Flash and SRAM memory
//
#if (MC_TARGET == MC_MCU_STM32F407VE) || (MC_TARGET == MC_MCU_STM32F407ZE)
//-----------------------------------------------
// 512 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F407IE)
//-----------------------------------------------
// 512 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00080000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F407VG) || (MC_TARGET == MC_MCU_STM32F407ZG)
//-----------------------------------------------
// 1024 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#if (MC_TARGET == MC_MCU_STM32F407IG)
//-----------------------------------------------
// 1024 kByte Flash, 192 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00030000)
#endif

#endif


//----------------------------------------------------------------------------//
// ST STM32F437 family                                                        //
//----------------------------------------------------------------------------//
#if ((MC_TARGET & MC_FAM_MASK) == MC_FAM_STM32F437)

//---------------------------------------------------------
// include target dependent header files
//
#include "stm32f4xx_macro.h"
#include "stm32f4xx.h"

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_STM32F4

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//---------------------------------------------------------
// Target depending core macros
//
#define MC_SWAP16(x)                   (uint16_t)(__REV16((uint32_t) (x) ))
#define MC_SWAP32(x)                   (uint32_t)(__REV  ((uint32_t) (x) ))

//---------------------------------------------------------
// Type depending Flash and SRAM sizes
//
#if (MC_TARGET == MC_MCU_STM32F437VG) || (MC_TARGET == MC_MCU_STM32F437ZG)
//-----------------------------------------------
// 1024 kByte Flash, 256 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00040000)
#endif

#if (MC_TARGET == MC_MCU_STM32F437IG)
//-----------------------------------------------
// 1024 kByte Flash, 256 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00100000)
#define MC_MCU_SIZE_RAM                (0x00040000)
#endif

#if (MC_TARGET == MC_MCU_STM32F437VI) || (MC_TARGET == MC_MCU_STM32F437ZI)
//-----------------------------------------------
// 2048 kByte Flash, 256 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00200000)
#define MC_MCU_SIZE_RAM                (0x00040000)
#endif

#if (MC_TARGET == MC_MCU_STM32F437AI) || (MC_TARGET == MC_MCU_STM32F437II)
//-----------------------------------------------
// 2048 kByte Flash, 256 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00200000)
#define MC_MCU_SIZE_RAM                (0x00040000)
#endif

#endif

//----------------------------------------------------------------------------//
// TI DSP Fixed Point TMS320F280x/1X family                                   //
//----------------------------------------------------------------------------//
#if (MC_TARGET == MC_MCU_TMS320F2808)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_DSP_TMS320F

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 2

//-----------------------------------------------
// 128 kByte Flash, 36 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00009000)

#endif

//----------------------------------------------------------------------------//
// TI DSP Piccolo family                                                      //
//----------------------------------------------------------------------------//
#if (MC_TARGET == MC_MCU_TMS320F28035)

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_DSP_PICCOLO

//---------------------------------------------------------
// Assign target specific CANpie parameters
//
#define CP_TARGET                      MC_MCU_FAMILY
#define CP_CHANNEL_MAX                 1

//-----------------------------------------------
// 128 kByte Flash, 20 kByte RAM
//
#define MC_MCU_SIZE_FLASH              (0x00020000)
#define MC_MCU_SIZE_RAM                (0x00005000)

#endif


//----------------------------------------------------------------------------//
// TI TM4C123x family                                                         //
//----------------------------------------------------------------------------//

#if (MC_TARGET == MC_MCU_TM4C123GH6PGE)

//---------------------------------------------------------
// use processor include file from TIVA-C library
//
#include <inc/tm4c123gh6pge.h>

//---------------------------------------------------------
// assign the MCU family
//
#define MC_MCU_FAMILY                  MC_FAM_TM4C123

//---------------------------------------------------------
// 256 kByte Flash, 32 kByte RAM, 2 kByte EEPROM
//
#define MC_MCU_SIZE_EEPROM             (0x00000800)
#define MC_MCU_SIZE_FLASH              (0x00040000)
#define MC_MCU_SIZE_RAM                (0x00008000)

//---------------------------------------------------------
// CANpie specific configuration
//
#define CP_TARGET                      MC_FAM_TM4C123
#define CP_CHANNEL_MAX                 2

#endif

//----------------------------------------------------------------------------//
// OS: Linux                                                                  //
//----------------------------------------------------------------------------//

#if (MC_TARGET == MC_OS_LINUX)

//---------------------------------------------------------
// CANpie specific configuration
//
#define CP_TARGET                      MC_OS_LINUX

#endif

#endif   // MC_TARGET_H_

