//****************************************************************************//
// File:          cp_arch.h                                                   //
// Description:   CANpie architecture definitions                             //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// 53844 Troisdorf - Germany                                                  //
// www.microcontrol.net                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions         //
// are met:                                                                   //
// 1. Redistributions of source code must retain the above copyright          //
//    notice, this list of conditions, the following disclaimer and           //
//    the referenced file 'COPYING'.                                          //
// 2. Redistributions in binary form must reproduce the above copyright       //
//    notice, this list of conditions and the following disclaimer in the     //
//    documentation and/or other materials provided with the distribution.    //
// 3. Neither the name of MicroControl nor the names of its contributors      //
//    may be used to endorse or promote products derived from this software   //
//    without specific prior written permission.                              //
//                                                                            //
// Provided that this notice is retained in full, this software may be        //
// distributed under the terms of the GNU Lesser General Public License       //
// ("LGPL") version 3 as distributed in the 'COPYING' file.                   //
//                                                                            //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 23.07.2003  Initial version                                                //
//                                                                            //
//****************************************************************************//



#ifndef  CP_ARCH_H_
#define  CP_ARCH_H_


//------------------------------------------------------------------------------
// SVN  $Date: 2014-10-20 14:19:40 +0200 (Mo, 20. Okt 2014) $
// SVN  $Rev: 6289 $ --- $Author: koppe $
//------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/*!   \file    cp_arch.h
**    \brief   CANpie architecture definitions
**
**    This file does the setup for various targets
**
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/
#include "compiler.h"      // compiler definitions
#include "cp_cc.h"         // definitions for CAN controller / target

#include "mc_target.h"

/*----------------------------------------------------------------------------*\
** Definitions & Enumerations                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
// The symbol CP_TARGET defines the target for the CANpie sources.
//
#ifndef CP_TARGET
#error  Target (Symbol CP_TARGET) is not defined! Check file cp_cc.h!
#endif


/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*/
/*!
** \struct  CpPortLinux_s   cp_arch.h
** \brief   Port structure for Linux
**
*/
struct CpPortLinux_s {

   /*!   logical CAN interface number,
   **    first index is 0, value -1 denotes not assigned
   */
   int      slLogIf;

   /*!   physical CAN interface number,
   **    first index is 0, value -1 denotes not assigned
   */
   int      slPhyIf;

   /*!   CAN message queue number,
   **    first index is 0, value -1 denotes not assigned
   */
   int      slQueue;
};


/*----------------------------------------------------------------------------*/
/*!
** \struct  CpPortEmbedded_s   cp_arch.h
** \brief   Port structure for embedded target
**
*/
struct CpPortEmbedded_s {

   /*!   physical CAN interface number,
   **    first CAN channel (index) is 0
   */
   uint8_t     ubPhyIf;

   /*!   logical CAN interface number,
   **    first index is 0
   */
   uint8_t     ubLogIf;

};


/*-------------------------------------------------------------------*/
/*!
** \def  CP_TARGET
** \ingroup CP_CONF
**
** This symbol defines target controller for the CANpie driver.
** The value can be set in a Makefile or in the cp_cc.h file.
**
** \internal This strange re-definition is only for doxygen.
*/
#if      CP_TARGET == 1
#undef   CP_TARGET
#define  CP_TARGET   0
#endif

/*-------------------------------------------------------------------*/
/*  splint options                                                   */
/*@ -isoreserved                                                    @*/
/*-------------------------------------------------------------------*/

//---------------------------------------------------------------------
// Architecture definitions for generic MCU
//
#if CP_TARGET == MC_MCU_GENERIC

typedef struct CpPortLinux_s     CpPort_ts;

typedef int32_t                  CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_CHANNEL_MAX           2
#define CP_GLOBAL_RCV_ENABLE     0
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1
#define CP_USER                  1

#endif

//---------------------------------------------------------------------
// Architecture definitions for Linux
//
#if CP_TARGET == MC_OS_LINUX

typedef struct CpPortLinux_s     CpPort_ts;
typedef struct CpPortLinux_s     _TsCpPort;     // deprecated

typedef int32_t                  CpStatus_tv;
typedef int32_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_CHANNEL_MAX           2
#define CP_GLOBAL_RCV_ENABLE     0
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1
#define CP_USER                  1

#endif




//---------------------------------------------------------------------
// Architecture definitions for ATMEL CANary
//
#if CP_TARGET == CP_CC_CC01

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            14

#define CP_SMALL_CODE            1
#define CP_STATISTIC             0

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for ATMEL AT90CAN128
//
#if CP_TARGET == CP_CC_AT90CAN128

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            14

#define CP_SMALL_CODE            1
#define CP_STATISTIC             0

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for ATMEL AT91SAM7X
//
#if CP_TARGET == CP_CC_AT91SAM7X

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            8
#define CP_SMALL_CODE            1
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     8
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for ATMEL AT32UC3C family with 2 CAN
// interfaces
//
#if CP_TARGET == MC_FAM_AT32UC3C

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint32_t                 CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000
#endif

//---------------------------------------------------------------------
// Architecture definitions for Freescale ColdFire MCF523x
//
#if CP_TARGET == CP_CC_MCF523x

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef struct CpPortEmbedded_s  _TsCpPort;     // deprecated

typedef uint16_t                 CpStatus_tv;
typedef uint16_t                 _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            14

#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     1
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for Freescale ColdFire MCF5441x
//
#if CP_TARGET == CP_CC_MCF5441x

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef struct CpPortEmbedded_s  _TsCpPort;     // deprecated

typedef uint16_t                 CpStatus_tv;
typedef uint16_t                 _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16
#define CP_CHANNEL_MAX           2

#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     1
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for Freescale XGate
//
#if CP_TARGET == CP_CC_XGATE

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef struct CpPortEmbedded_s  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            0
#define CP_STATISTIC             0

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for Fujitsu LX16 family (340)
//
#if CP_TARGET == CP_CC_LX340

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            16

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1

#endif


//---------------------------------------------------------------------
// Architecture definitions for Fujitsu LX16 family (385)
//
#if CP_TARGET == CP_CC_LX385

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16

#define CP_SMALL_CODE            1
#define CP_STATISTIC             0

#endif


//---------------------------------------------------------------------
// Architecture definitions for Fujitsu LX16 family (495)
//
#if CP_TARGET == CP_CC_LX495

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            8

#define CP_SMALL_CODE            1
#define CP_STATISTIC             0

#endif


//---------------------------------------------------------------------
// Architecture definitions for Fujitsu FX16 340 family
//
#if CP_TARGET == CP_CC_FX340

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef struct CpPortEmbedded_s  _TsCpPort;		// deprecated


typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;	// deprecated


#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32

#define CP_CHANNEL_MAX           2
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#endif


//---------------------------------------------------------------------
// Architecture definitions for Infineon C164 family
//
#if CP_TARGET == CP_CC_C164

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            14

#define CP_SMALL_CODE            1
#define CP_STATISTIC             0

#define CP_GLOBAL_RCV_ENABLE     1
#define CP_GLOBAL_RCV_BUFFER     15
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for Infineon XC164 family
//
#if CP_TARGET == CP_CC_XC164

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef struct CpPortEmbedded_s  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32
#define CP_CAN_MSG_MACRO         0
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     31
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for Infineon C167 family
//
#if CP_TARGET == CP_CC_C167

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16

#define CP_SMALL_CODE            1
#define CP_STATISTIC             0

#endif

//---------------------------------------------------------------------
// Architecture definitions for LM3S2918
//
#if CP_TARGET == CP_CC_LM3S2918

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1
#endif


//---------------------------------------------------------------------
// Architecture definitions for Microchip 18Fxx8x
//
#if CP_TARGET == CP_CC_18Fxx8x

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16

#define CP_CHANNEL_MAX           1
#define CP_SMALL_CODE            1
#define CP_STATISTIC             0

#endif


//---------------------------------------------------------------------
// Architecture definitions for Microchip PIC24E
//
#if CP_TARGET == CP_CC_PIC24E

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef struct CpPortEmbedded_s  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16

#define CP_CHANNEL_MAX           2
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#endif

//---------------------------------------------------------------------
// Architecture definitions for Microchip PIC32MX7XX
//
#if CP_TARGET == CP_CC_PIC32MX7XX

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef struct CpPortEmbedded_s  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16

#define CP_CHANNEL_MAX           2
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#endif

//---------------------------------------------------------------------
// Architecture definitions for Nuvoton NUC140 family
//
#if CP_TARGET == MC_FAM_NUC140

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0

#endif

//---------------------------------------------------------------------
// Architecture definitions for NXP SJA1000
//
#if CP_TARGET == CP_CC_SJA1000

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16

#define CP_CHANNEL_MAX           1
#define CP_SMALL_CODE            1
#define CP_STATISTIC             0


#endif

//---------------------------------------------------------------------
// Architecture definitions for NXP LPC21x9
//
#if (CP_TARGET == CP_CC_LPC2119) || (CP_TARGET == CP_CC_LPC2129)

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_CHANNEL_MAX           2
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for NXP LPC176x family
//
#if CP_TARGET == MC_FAM_LPC176x

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_CHANNEL_MAX           2
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0

#endif


//---------------------------------------------------------------------
// Architecture definitions for NXP LPC177x / LPC178x
//
#if CP_TARGET == CP_CC_LPC177x_8x

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_CHANNEL_MAX           2
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for NXP LPC18xx family
//
#if CP_TARGET == CP_CC_LPC18xx

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_CHANNEL_MAX           2
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for Renesas R8C/34W family
//
#if CP_TARGET == CP_CC_R8C_34W

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            16

#define CP_CHANNEL_MAX           1
#define CP_SMALL_CODE            1
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif

//---------------------------------------------------------------------
// Architecture definitions for Silicon Laboratories C80551F040
//
#if CP_TARGET == CP_CC_C8051F040

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif

//---------------------------------------------------------------------
// Architecture definitions for SILABS C8051f550
//
#if CP_TARGET == CP_CC_C8051F550

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for ST STR71x
//
#if CP_TARGET == CP_CC_STR71x

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1
#endif

//---------------------------------------------------------------------
// Architecture definitions for ST STR91x
//
#if CP_TARGET == CP_CC_STR91x

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1
#endif

//---------------------------------------------------------------------
// Architecture definitions for ST STM32F103x
//
#if CP_TARGET == MC_FAM_STM32F103

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0

#endif


//---------------------------------------------------------------------
// Architecture definitions for ST STM32F105x and STM32F107x
//
#if   ((CP_TARGET == MC_FAM_STM32F105) || (CP_TARGET == MC_FAM_STM32F107))

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            0
#define CP_STATISTIC             1
#define CP_GLOBAL_RCV_ENABLE     0

#endif


//---------------------------------------------------------------------
// Architecture definitions for ST STM32F2xx
//
#if CP_TARGET == MC_FAM_STM32F2

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef struct CpPortEmbedded_s  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated


#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            0
#define CP_STATISTIC             1
#define CP_GLOBAL_RCV_ENABLE     0

#endif


//---------------------------------------------------------------------
// Architecture definitions for ST STM32F3xx
//
#if CP_TARGET == MC_FAM_STM32F3

typedef uint8_t                  CpPort_ts;
typedef uint8_t                  _TsCpPort;     // deprecated

typedef uint8_t                  CpStatus_tv;
typedef uint8_t                  _TvCpStatus;   // deprecated

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            1
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0

#endif


//---------------------------------------------------------------------
// Architecture definitions for ST STM32F4xx
//
#if CP_TARGET == MC_FAM_STM32F4

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            16

#define CP_SMALL_CODE            0
#define CP_STATISTIC             1
#define CP_GLOBAL_RCV_ENABLE     0

#endif


//---------------------------------------------------------------------
// Architecture definitions for ST ST10F2xx
//
#if CP_TARGET == CP_CC_ST10F2xx

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint8_t                  CpStatus_tv;

#define CP_AUTOBAUD              1
#define CP_BUFFER_MAX            32
#define CP_CHANNEL_MAX           2

#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     16
#define CP_GLOBAL_RCV_MASK       0x00000000

#endif


//---------------------------------------------------------------------
// Architecture definitions for Texas Instruments TMS320F280x
//
#if CP_TARGET == MC_FAM_DSP_TMS320F

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint16_t                 CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     31
#define CP_GLOBAL_RCV_MASK       0x00000000
#endif

//---------------------------------------------------------------------
// Architecture definitions for Texas Instruments TMS320 Piccolo
//
#if CP_TARGET == MC_FAM_DSP_PICCOLO

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint16_t                 CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32

#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#endif


//---------------------------------------------------------------------
// Architecture definitions for Texas Instruments TM4C123x family
// with 1 CAN interface
//
#if CP_TARGET == MC_FAM_TM4C123

typedef struct CpPortEmbedded_s  CpPort_ts;
typedef uint32_t                 CpStatus_tv;

#define CP_AUTOBAUD              0
#define CP_BUFFER_MAX            32
#define CP_SMALL_CODE            0
#define CP_STATISTIC             1

#define CP_GLOBAL_RCV_ENABLE     0
#define CP_GLOBAL_RCV_BUFFER     31
#define CP_GLOBAL_RCV_MASK       0x00000000
#endif


#endif   /* CP_ARCH_H_        */

