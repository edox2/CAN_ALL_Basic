//****************************************************************************//
// File:          cos_lss.h                                                   //
// Description:   Layer Setting Services (LSS) for CANopen Slave              //
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
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 16.05.2002  Initial version                                                //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 17:09:58 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5697 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  COS_LSS_
#define  COS_LSS_


#include "cos_defs.h"      // CANopen Slave definition file


//-----------------------------------------------------------------------------
/*!
** \file    cos_lss.h
** \brief   Layer Setting Services (LSS Slave)
**
** &nbsp;<p>
** LSS offers the possibility to inquire and change the settings of certain
** parameters of the local layers on a CANopen module with LSS Slave
** capabilities by a CANopen module with LSS Master capabilities via the
** CAN Network. The following parameters can be inquired and/or changed by
** the use of LSS:
** \li   Node-ID of the CANopen Slave
** \li   Bit timing parameters of the physical layer (baud rate)
** \li   LSS address (Identity Object, Index 1018H)
**
** By using LSS a LSS Slave can be configured for a CANopen network without
** using any devices like DIP-switches for setting the parameters. There are
** several solutions available for LSS Slaves with and without a unique
** LSS-address or non-volatile storage.
*/


//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//


/*-------------------------------------------------------------------------*\
** Definitions                                                             **
**                                                                         **
\*-------------------------------------------------------------------------*/




//---------------------------------------------------------
/*!
** \enum    LssMode_e
** \brief   LSS state
**
*/
enum LssMode_e {

   eLSS_MODE_WAIT                   = 0x00,

   eLSS_MODE_SEL_VENDOR_ID          = 0x01,
   eLSS_MODE_SEL_PRODUCT_CODE       = 0x02,
   eLSS_MODE_SEL_REVISION_NUM       = 0x04,
   eLSS_MODE_SEL_SERIAL_NUM         = 0x08,

   eLSS_MODE_CONFIG                 = 0x0F
};


/*----------------------------------------------------------------------------*\
** Variables of module for external use                                       **
**                                                                            **
\*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

/*!
** \brief   Configure Bit timing
**
** By means of the Configure Bit Timing Parameters service the LSS Master
** sets the new bit timing on a LSS Slave.
*/
void  CosLssConfigureBitTiming(void);


/*!
** \brief   Configure Node ID
**
** By means of this service the LSS Master configures the NMT-address
** parameter of a LSS Slave. This service allows only one LSS Slave in
** configuration mode. The remote result parameter confirms the success
** or failure of the service. In case of a failure optionally the reason
** is confirmed.
*/
void  CosLssConfigureNodeId(void);


/*!
** \brief   Initialize the LSS service
**
** This function is called by the protocol stack to initialize the
** LSS service. The LSS service is only available in Stopped and
** Pre-Operational mode.
*/
void  CosLssInit(void);


/*!
** \brief   Inquiry services
** \param   ubServiceV service type
**
** The inquiry services are available only in configuration mode.
*/
void  CosLssInquiryService(uint8_t ubServiceV);


/*!
** \brief   Evaluate LSS messages
**
** This function is called by main message handler function
** CosMgrMessageHandler(). It evaluates the value of the first
** byte and calls the appropriate functions.
**
*/
void  CosLssMessageHandler(void);


/*!
** \brief   Store Configuration
**
** This function is used to store the configured parameters.
**
*/
void  CosLssStoreConfiguration(void);


/*!
** \brief   Switch Mode Global
**
** The Switch Mode Services control the mode attribute of a LSS Slave.
** LSS provides two ways to put a LSS Slave into configuration mode,
** Switch Mode Global and Switch Mode Selective. Switch Mode Selective
** switches exactly one LSS Slave between configuration and operation mode.
** Switch Mode Global switches all LSS Slaves between configuration and
** operation mode.
*/
void  CosLssSwitchModeGlobal(void);


/*!
** \brief   Switch Mode Selective
** \param   ubServiceV  service type
**
** The Switch Mode Services control the mode attribute of a LSS Slave.
** LSS provides two ways to put a LSS Slave into configuration mode,
** Switch Mode Global and Switch Mode Selective. Switch Mode Selective
** switches exactly one LSS Slave between configuration and operation mode.
** Switch Mode Global switches all LSS Slaves between configuration and
** operation mode.
*/
void  CosLssSwitchModeSelective(uint8_t ubServiceV);


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   // COS_LSS_

