//****************************************************************************//
// File:          cos_led.h                                                   //
// Description:   LED Manager for CANopen Slave                               //
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
// 17.03.2001  Initial version                                                //
// 19.08.2005  Changed COS_LED_PERIOD based on COS_TIMER_PERIOD               //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 17:09:58 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5697 $ --- $Author: koppe $
//------------------------------------------------------------------------------

#ifndef  COS_LED_H_
#define  COS_LED_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_defs.h"      // CANopen Slave definition file



//-----------------------------------------------------------------------------
/*!
** \file    cos_led.h
** \brief   LED Management
**
** The LED module supports two types of LEDs: a <b>Module LED</b> and a
** <b>Network LED</b>. The Module LED displays the state of the hardware,
** e.g. power supply, driver stage, etc. The Network LED displays the state
** of the CAN connection and the CANopen NMT state. Both LEDs should be
** bi-color LEDs. If not, the patterns for the blinking code may need to be
** adopted.
** <p>
** In the implementation, each LED gets two patterns - one for the red LED
** and one for the green LED (bi-color LED). Both patterns have a length of
** 32 bit. With a given default pattern shift rate of 50 ms (defined by
** the symbol COS_LED_PERIOD) the total pattern length can be 1,6 seconds.
** <p>
** Please note that this implementation is not fully compliant to DR-303-3.
** <p>
** The LED functionality is only compiled when the symbol #COS_LED_SUPPORT
** (see cos_conf.h) is set to a value greater 0.
** <p>
** The <b>Network LED</b> is autonomously run by the CANopen stack, i.e.
** no function has to be called by the user application.
** The following functions must be adopted to the target hardware for
** Network LED support:
** \li CosLedNetGreen()
** \li CosLedNetRed()
** <p>
** The <b>Module LED</b> must be driven by the user application, i.e.
** you have to call the CosLedModuleStatus() function somewhere.
** The following functions must be adopted to the target hardware for
** Module LED support:
** \li CosLedModGreen()
** \li CosLedModRed()
*/


//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


//---------------------------------------------------------
/*!
** \enum    CosLedMod_e
** \brief   Module LED states
**
** This enumeration defines all states for the module LED.
** The optional module LED displays the state of the
** hardware, not the network (i.e. CAN) state.
*/
enum  CosLedMod_e {
   /*! Module state is OK                                */
   eCosLedMod_OK = 0,

   /*! Hardware failure on baudrate setting              */
   eCosLedMod_FAIL_BAUD,

   /*! Hardware failure on address setting               */
   eCosLedMod_FAIL_ADDR,

   /*! Error occured that can be recovered               */
   eCosLedMod_RECOVER,

   /*! Application / hardware error 1                    */
   eCosLedMod_APP1,

   /*! Application / hardware error 2                    */
   eCosLedMod_APP2

};


//---------------------------------------------------------
/*!
** \enum    CosLedNet_e
** \brief   Network LED - NMT states
**
** This enumeration defines the NMT states for the green
** network LED. The definitions are used by the function
** CosLedNetworkStatus().
*/
enum  CosLedNet_e {
   /*! Module is in Stopped State                        */
   eCosLedNet_STOPPED = 0,

   /*! Module is in Pre-Operational State                */
   eCosLedNet_PREOPERATIONAL,

   /*! Module is in Operational State                    */
   eCosLedNet_OPERATIONAL,

   /*! Module is in Initialization State                 */
   eCosLedNet_INIT
};


//---------------------------------------------------------
/*!
** \enum    CosLedErr_e
** \brief   Network LED - error states
**
** This enumeration defines the CAN error states for the
** red network LED. The definitions are used by the function
** CosLedNetworkError().
*/
enum  CosLedErr_e {

   eCosLedErr_OK = 0,

   /*! CAN Bus is warn error active                      */
   eCosLedErr_BUS_WARN,

   /*! CAN Bus is error passive                          */
   eCosLedErr_BUS_PASSIVE,

   /*! CAN is Bus-Off                                    */
   eCosLedErr_BUS_OFF,

   /*! reserved blinkcode                                */
   eCosLedErr_RESERVED
};


//-------------------------------------------------------------------
// shortest on/off period for LED blinking in microseconds
//
#define  COS_LED_PERIOD       50000


/*----------------------------------------------------------------------------*\
** Variables of module for external use                                       **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/




/*!
** \brief   Initialize LED management
**
** This function is called by the protocol stack (CosMgrInit()).
** The function initializes the internal pattern variables.
*/
void  CosLedInit(void);


/*!
** \brief   Drive Red Module LED
** \param   btSwitchOnV - state of the LED
**
** According to the parameter <b>btSwitchOnV</b> the function switches
** the red Module LED on or off.
** \li   btSwitchOnV = 0: LED is switched off
** \li   btSwitchOnV = 1: LED is switched on
** <p>
** <b>This function must be adopted to the target hardware.</b>
*/
void  CosLedModRed(bool_t btSwitchOnV);


/*!
** \brief   Drive green Module LED
** \param   btSwitchOnV - state of the LED
**
** According to the parameter <b>btSwitchOnV</b> the function switches
** the green Module LED on or off.
** \li   btSwitchOnV = 0: LED is switched off
** \li   btSwitchOnV = 1: LED is switched on
** <p>
** <b>This function must be adopted to the target hardware.</b>
*/
void  CosLedModGreen(bool_t btSwitchOnV);


/*!
** \brief   Drive red Network LED
** \param   btSwitchOnV - state of the LED
**
** According to the parameter <b>btSwitchOnV</b> the function switches
** the red Network LED on or off.
** \li   btSwitchOnV = 0: LED is switched off
** \li   btSwitchOnV = 1: LED is switched on
** <p>
** <b>This function must be adopted to the target hardware.</b>
*/
void  CosLedNetRed(bool_t btSwitchOnV);


/*!
** \brief   Drive green Network LED
** \param   btSwitchOnV - state of the LED
**
** According to the parameter <b>btSwitchOnV</b> the function switches
** the green Network LED on or off.
** \li   btSwitchOnV = 0: LED is switched off
** \li   btSwitchOnV = 1: LED is switched on
** <p>
** <b>This function must be adopted to the target hardware.</b>
*/
void  CosLedNetGreen(bool_t btSwitchOnV);


/*!
** \brief   Module Status LED
** \param   ubStatusV - Module Status
**
** This function sets the pattern for the Module Status LED. Valid
** values for the parameter <b>ubStatusV</b> are given in the
** enumeration #CosLedMod_e.
*/
void  CosLedModuleStatus(uint8_t ubStatusV);


/*!
** \brief   Network Error LED
** \param   ubErrorV
**
** This function sets the network error value.
** Valid values for the parameter <b>ubErrorV</b> are given in the
** enumeration #CosLedErr_e.
*/
void  CosLedNetworkError(uint8_t ubErrorV);


/*!
** \brief   Update Network Status LED pattern
**
** This function sets the pattern for the Network Status LED.
*/
void  CosLedNetworkPattern(void);


/*!
** \brief   Network Status LED
** \param   ubStatusV
**
** This function sets the network status value.
** Valid values for the parameter <b>ubStatusV</b> are given in the
** enumeration #CosLedNet_e.
*/
void  CosLedNetworkStatus(uint8_t ubStatusV);


/*!
** \brief   Event handler for LED
**
** This function is called by the protocol stack (CosTmrEvent()).
** The function shifts the LED pattern within the given time period
** COS_LED_PERIOD and switches the LEDs on and off.
*/
void  CosLedTmrEvent(void);



//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//


#endif   // COS_LED_H_

