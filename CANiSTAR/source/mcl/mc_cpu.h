//============================================================================//
// File:          mc_cpu.h                                                    //
// Description:   CPU control functions                                       //
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
// 04.02.2004  Initial version                                                //
//                                                                            //
//============================================================================//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-03-17 16:42:11 +0100 (Mo, 17. Mär 2014) $
// SVN  $Rev: 5761 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  MC_CPU_H_
#define  MC_CPU_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "compiler.h"



//-----------------------------------------------------------------------------
/*!
** \file    mc_cpu.h
** \brief   MCL - CPU control
**
**
*/

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//#define MAXADCCHANNEL (0x3f)
//extern uint16_t  auwADC_ConvertedValueS[MAXADCCHANNEL + 1];


/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
/*!
** \brief   Initialise CPU
** \todo    Adopt to target system
**
** Perform basic initialisation of the CPU / MCU. This includes:
** \li IRQ vector setup
** \li Stack setup
** \li Clock initialisation
** \li Memory interface setup
**
** As this function is very close to the application (and used controller)
** it has to be adopted by the user.
*/
void McCpuInit(void);

uint8_t McGetPushbuttonState(void);

int8_t McGetChannelId(void);

uint32_t McGetSerialNumber(void);

uint16_t  McAdcStartConversion(uint8_t channel);



//------------------------------------------------------------------------------
/*!
** \brief   Get clock frequency
** \return  clock frequency value in Hz
**
*/
uint32_t McCpuGetClock(void);




#endif   // #define  MC_CPU_H_
