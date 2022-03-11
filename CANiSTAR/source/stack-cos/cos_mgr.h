//****************************************************************************//
// File:          cos_mgr.h                                                   //
// Description:   Init and control functions of the CANopen slave             //
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
// 27.06.2000  Initial version                                                //
// 07.11.2000  Added Online-Documentation for Doxygen                         //
// 16.05.2002  Added LSS support                                              //
// 09.09.2002  Added CAN Error Handler support / rename CosMgrMessageHandler  //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 17:09:58 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5697 $ --- $Author: koppe $
//------------------------------------------------------------------------------



#ifndef  COS_MGR_H_
#define  COS_MGR_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_defs.h"      // CANopen Slave definition file


//-----------------------------------------------------------------------------
/*!
** \file    cos_mgr.h
** \brief   CANopen Slave Management Functions
**
** This module holds all functions for the initialisation and control
** of the CANopen slave. Some functions of this module need to be
** adopted to the target platform:
** \li   CosMgrGetBaudrate()
** \li   CosMgrGetNodeAddress()
** \li   CosMgrGetSerialNumber()
** \li   CosMgrOnBusOff()
**
** These functions are located in the cos_user.c file.
**
*/


//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//


enum CosMgr_e {
   eCOS_MGR_INIT = 0,
   eCOS_MGR_RUN,
   eCOS_MGR_BUS_OFF,
   eCOS_MGR_STOP
};

#define  CO_CONF_SLAVE           0x0000
#define  CO_CONF_MASTER          0x0001

/*----------------------------------------------------------------------------*\
** Variables of module for external use                                       **
**                                                                            **
\*----------------------------------------------------------------------------*/
extern uint8_t       ubCosMgrNodeAddressG;
extern uint8_t       ubCosMgrBaudrateG;
#if CP_SMALL_CODE == 0
extern CpPort_ts     tsCanPortG;                // CAN interface
#endif
extern uint16_t      uwCosMgrConfigG;


/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/




/*!
** \brief   CAN error handler
** \param   ptsStateV    CANpie status information
** \return  CP_CALLBACK_PROCESSED
**
** This function is the main CAN error handler. It is installed as callback
** in the CANpie error handler service during initialisation. The function
** returns CP_CALLBACK_PROCESSED.
**
*/
uint8_t  CosMgrCanErrHandler(CpState_ts * ptsStateV);

/*!
** \brief   CAN receive message handler
** \param   ptsCanMsgV    pointer to CAN message structure
** \param   ubBufferIdxV  number of message buffer
** \return  CP_CALLBACK_PROCESSED
**
** This function implements the CAN receive message handler. It is installed
** as callback in the CANpie receive message handler service during
** initialisation. The function returns CP_CALLBACK_PROCESSED in order to
** prevent messages to be stored in the CANpie FIFO (if available).
** The parameter \c ubBufferIdxV defines the buffer in the CAN controller
** that caused the interrupt. The parameter \c ptsCanMsgV points to a CAN
** message structure which holds the following information:
** \arg Data Length Code
** \arg Remote Frame Flag
** \arg Buffer Overflow Flag
**
** All other fields in the CAN message structure are not used and may have
** undefined values.
**
*/
uint8_t  CosMgrCanRcvHandler(CpCanMsg_ts * ptsCanMsgV, uint8_t ubBufferIdxV);


/*!
** \brief   CAN transmit handler
** \param   ptsCanMsgV    pointer to CAN message structure
** \param   ubBufferIdxV  number of message buffer
** \return  CP_CALLBACK_PROCESSED
**
** This function is the CAN transmit message handler. It is installed
** as callback in the CANpie transmit message handler service during
** initialisation. The function returns CP_CALLBACK_PROCESSED in order to
** prevent messages to be stored in the CANpie FIFO (is available).
**
*/
uint8_t  CosMgrCanTrmHandler(CpCanMsg_ts * ptsCanMsgV, uint8_t ubBufferIdxV);


/*!
** \brief   Get module baudrate
** \return  Constant value for Baudrate
** \todo    This function has to be adopted to the hardware (cos_user.c)
**
** This routine checks for the baudrate. The code of this routine has
** to be adopted to the specific target. The function returns a constant
** for the baudrate that is specified in the CANpie cpconst.h file.
**
*/
uint8_t  CosMgrGetBaudrate(void);


/*!
** \brief   Get module address
** \return  Module ID
** \todo    This function has to be adopted to the hardware (cos_user.c)
**
** This routine checks for the module ID. The code of this routine has
** to be adopted to the specific target. The function returns a value
** in the range from 1 to 127.
**
*/
uint8_t  CosMgrGetNodeAddress(void);


/*!
** \brief   Get Serial Number of module
** \return  Serial Number
** \todo    This function has to be adopted to the hardware (cos_user.c)
**
** This routine returns the Serial Number of the module, which must be
** a unique value for a product family. The Serial Number is used within
** in Identity Object (Index 1018hex).
**
*/
uint32_t  CosMgrGetSerialNumber(void);


/*!
** \brief   Check identifier
** \param   ulIdentifierV - Identifier value
** \return  1 if identifier accepted, 0 if rejected
**
** This routine checks for a restricted CAN-ID.  Such a restricted
** CAN-ID shall not be used as a CAN-ID by any configurable
** communication object, neither for SYNC, TIME, EMCY, PDO, and SDO.
** The restricted CAN-IDs are:
** \li      0 (000h) - NMT
** \li      1 (001h) - reserved
** \li    113 (071h) - 127 (07Fh) reserved
** \li    257 (101h) - 384 (180h) reserved
** \li   1409 (581h) - 1535 (5FFh) default SDO (tx)
** \li   1537 (601h) - 1663 (67Fh) default SDO (rx)
** \li   1760 (6E0h) - 1791 (6FFh) reserved
** \li   1793 (701h) - 1919 (77Fh) NMT Error Control
** \li   2020 (780h) - 2047 (7FFh) reserved
**
** The function is called by the stack upon change of COB-IDs. The
** identifier value is tested for the lower 29 bits. The upper
** 3 bits of the identifier value are masked inside this function
** (since they have a special meaning in CANopen).
**
*/
uint8_t  CosMgrIdCheck(uint32_t ulIdentifierV);


/*!
** \brief   Initialise the CANopen Slave
** \param   ubCanIfV  physical CAN interface
** \param   uwConfigV initial configuration
** \return  Error Code
**
** The function returns CosErr_OK on success. It is responsible
** for initialising the CANopen state machine and calling the
** LED management. 
**
*/
uint8_t CosMgrInit(uint8_t ubCanIfV, uint16_t uwConfigV);


/*!
** \brief   Perform NVM access
**
**
*/
void CosMgrNvmOperation(void);


/*!
** \brief   Handle Bus-Off condition
**
**
*/
void CosMgrOnBusOff(void);


/*!
** \brief   Initialise parameters
** \see     CosMgrParmLoad()
**
** This function sets default values for 
** the objects in DS-301, DS-4xx and the manufacturer profile area.
**
*/
void CosMgrParmInit(void);


/*!
** \brief   Load Parameters
** \return  Error code
** \see     CosMgrParmSave()
**
** This function reads data from a non-volatile memory and sets up
** the objects in DS-301, DS-4xx and the manufacturer profile area.
**
*/
uint8_t CosMgrParmLoad(void);


/*!
** \brief   Save Parameters
** \param   ubGroupV    Defines the parameter group
** \return  Error code
** \see     CosMgrParmLoad()
**
** This function performs data storing of a CANopen slave. The
** parameter <b>ubGroupV</b> defines what parameters are stored:
**
** \li   eCOS_PARM_ALL - all parameters of the device
** \li   eCOS_PARM_COM - communication parameters (DS-301)
** \li   eCOS_PARM_APP - application parameters (device profile)
** \li   eCOS_PARM_MAN - manufacturer specific parameters
**
*/
uint8_t CosMgrParmSave(uint8_t ubGroupV);


uint8_t CosMgrProcess(void);


void CosMgrProcessMsg(void);

/*!
** \brief   Update parameter of device profile
** \param   uwIndexV     parameter index
** \param   ubSubIndexV  parameter sub-index
**
** This function is called by the stack when a parameter of a device
** profile is modified. It can be used to control the interaction
** between the CANopen slave stack and the application. The
** implementation of this function depends on the application.
*/
void CosMgrProfileUpdate(uint16_t uwIndexV, uint8_t ubSubIndexV);


/*!
** \brief   Release the CANopen Slave protocol stack
** \return  Error Code
**
** This routine stops the CANopen protocol stack.
** The function returns CosErr_OK on success.
**
*/
uint8_t CosMgrRelease(void);


/*!
** \brief   Start the CANopen Slave protocol stack
** \param   ubBaudSelV initial baudrate
** \param   ubNodeIdV device node-ID (NID)
** \return  Error Code
**
** This routine starts the CANopen protocol stack on the given
** baudrate and node-ID. The initialisation routines from
** other CANopen services are called and the Bootup message is sent.
** The function returns CosErr_OK on success.
**
*/
uint8_t CosMgrStart(uint8_t ubBaudSelV, uint8_t ubNodeIdV);


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   // COS_MGR_H_

