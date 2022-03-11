//****************************************************************************//
// File:          cos_emcy.h                                                  //
// Description:   Emergency (EMCY) service for CANopen slave                  //
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
// 22.03.2002  Added Emergency code enumeration                               //
// 30.10.2002  Changed callback structure to reduce code size                 //
// 15.05.2003  Changed Emergency Error Codes from enumeration to defines      //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 17:09:58 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5697 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  COS_EMCY_H_
#define  COS_EMCY_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_defs.h"      // CANopen Slave definition file


//-----------------------------------------------------------------------------
/*!
** \file    cos_emcy.h
** \brief   CANopen Emergency functions
**
** Emergency objects are triggered by the occurrence of a device internal
** error situation and are transmitted from an emergency producer on the
** device. Emergency objects are suitable for interrupt type error alerts.
** An emergency object is transmitted only once per 'error event'. As long
** as no new errors occur on a device no further emergency objects must be
** transmitted.
** The emergency object may be received by zero or more emergency consumers.
** The reaction on the emergency consumer(s) is not specified.
**
**
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

//-------------------------------------------------------------------
/*!
** \defgroup EMCY_CODES Emergency error code
** \brief EMCY error codes
**
** List of pre-defined error codes for CosEmcySend() function.
** The following Emergency messages are issued by the CANopen Slave
** Protocol Stack:
** \li #EMCY_ERR_NONE
** \li #EMCY_ERR_COMMUNICATION
** \li #EMCY_ERR_CAN_OVERRUN
** \li #EMCY_ERR_CAN_ERROR_PASSIVE
** \li #EMCY_ERR_CAN_LIFEGUARD
** \li #EMCY_ERR_CAN_TX_COB_COLLISION
** \li #EMCY_ERR_PROTOCOL_PDO
** \li #EMCY_ERR_PROTOCOL_SYNC_LENGTH
**
*/


//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_NONE
**
** Error reset or no error
*/
#define  EMCY_ERR_NONE                          0x0000

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_GENERIC
**
** Generic error
*/
#define  EMCY_ERR_GENERIC                       0x1000

#define  EMCY_ERR_CURR_INP_SHORT_CIRCUIT        0x2110
#define  EMCY_ERR_CURR_OUT_OVERLOAD             0x2310
#define  EMCY_ERR_CURR_OUT_SHORT_CIRCUIT        0x2320
#define  EMCY_ERR_CURR_OUT_LOAD_DUMP            0x2330

#define  EMCY_ERR_VOLT_INP_HIGH                 0x3110
#define  EMCY_ERR_VOLT_INP_LOW                  0x3120

#define  EMCY_ERR_VOLT_INT_HIGH                 0x3210
#define  EMCY_ERR_VOLT_INT_LOW                  0x3220

#define  EMCY_ERR_VOLT_OUT_HIGH                 0x3310
#define  EMCY_ERR_VOLT_OUT_LOW                  0x3320

#define  EMCY_ERR_TEMP_GENERAL                  0x4000
#define  EMCY_ERR_TEMP_AMBIENT                  0x4100
#define  EMCY_ERR_TEMP_DEVICE                   0x4200

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_HW_GENERIC
**
** CANopen device hardware - generic error
*/
#define  EMCY_ERR_HW_GENERIC                    0x5000

#define  EMCY_ERR_HW_SELF_TEST                  0x5010
#define  EMCY_ERR_HW_AUTO_CALIBRATION           0x5020
#define  EMCY_ERR_HW_SENSOR_FAULT               0x5030
#define  EMCY_ERR_HW_SENSOR_FRACTION            0x5031
#define  EMCY_ERR_HW_SENSOR_SHORT_CIRCUIT       0x5032

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_SW_GENERIC
**
** CANopen device software - generic error
*/
#define  EMCY_ERR_SW_GENERIC                    0x6000

#define  EMCY_ERR_CALIBRATION                   0x6310
#define  EMCY_ERR_CALIBRATION_INPUT             0x6311
#define  EMCY_ERR_CALIBRATION_OUTPUT            0x6312
#define  EMCY_ERR_CALIBRATION_CONTROLLER        0x6320

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_MONITORING
**
** Monitoring - generic error
*/
#define  EMCY_ERR_MONITORING                    0x8000

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_COMMUNICATION
**
** Communication - generic
*/
#define  EMCY_ERR_COMMUNICATION                 0x8100

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_CAN_OVERRUN
**
** CAN overrun (objects lost)
*/
#define  EMCY_ERR_CAN_OVERRUN                   0x8110

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_CAN_ERROR_PASSIVE
**
** CAN in error passive mode
*/
#define  EMCY_ERR_CAN_ERROR_PASSIVE             0x8120

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_CAN_LIFEGUARD
**
** Life guard error or heartbeat error
*/
#define  EMCY_ERR_CAN_LIFEGUARD                 0x8130

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_CAN_BUSOFF_RECOVER
**
** Recovered from bus off
*/
#define  EMCY_ERR_CAN_BUSOFF_RECOVER            0x8140

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_CAN_TX_COB_COLLISION
**
** CAN-ID collision
*/
#define  EMCY_ERR_CAN_TX_COB_COLLISION          0x8150

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_PROTOCOL
**
** Protocol error - generic
*/
#define  EMCY_ERR_PROTOCOL                      0x8200

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_PROTOCOL_PDO
**
** PDO not processed due to length error
*/
#define  EMCY_ERR_PROTOCOL_PDO                  0x8210

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_PROTOCOL_PDO_EXCEED
**
** PDO length exceeded
*/
#define  EMCY_ERR_PROTOCOL_PDO_EXCEED           0x8220

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_PROTOCOL_MPDO
**
** DAM MPDO not processed, destination object not available
*/
#define  EMCY_ERR_PROTOCOL_PDO_MPDO             0x8230

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_PROTOCOL_SYNC_LENGTH
**
** Unexpected SYNC data length
*/
#define  EMCY_ERR_PROTOCOL_SYNC_LENGTH          0x8240

//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_PROTOCOL_RPDO
**
** RPDO timeout
*/
#define  EMCY_ERR_PROTOCOL_RPDO                 0x8250

#define  EMCY_ERR_INPUT_OVERLOAD                0xF001
#define  EMCY_ERR_OUTPUT_OVERLOAD               0xF002
#define  EMCY_ERR_LIMIT_EXCEEDED                0xF011
#define  EMCY_ERR_TARA_OVERFLOW                 0xF020
#define  EMCY_ERR_SELF_OPTIMISATION             0xF030


//-------------------------------------------------------------------
/*!
** \ingroup EMCY_CODES
** \def     EMCY_ERR_DEV_GENERAL
**
** Device specific - generic error
*/
#define  EMCY_ERR_DEV_GENERAL                   0xFF00
#define  EMCY_ERR_DEV_NV_MEM                    0xFF20



/*----------------------------------------------------------------------------*\
** Variables of module for external use                                       **
**                                                                            **
\*----------------------------------------------------------------------------*/

extern uint32_t ulCosEmcyIdentifierG;

#if COS_DICT_OBJ_1015 > 0
extern uint16_t uwCosEmcyInhibitTimeG;
#endif


/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-----------------------------------------------------------------------------
/*!
** \brief   Add emergency code to list
** \param   uwEmcyCodeV - Emergency error code
** \param   uwAddInfoV  - Additional information
**
** This function appends the parameters to the Pre-defined Error Field.
*/
void CosEmcyAppendToErrorField(uint16_t uwEmcyCodeV, uint16_t uwAddInfoV);


//-----------------------------------------------------------------------------
/*!
** \brief   Index 1003 - Pre-defined Error Field
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code (enumeration #CosSdo_e)
**
** The object at index 1003h holds the errors that have occurred on the
** device and have been signalled via the Emergency Object. In doing so
** it provides an error history.
**
** \li   The entry at sub-index 0 contains the number of actual errors that
**       are recorded in the array starting at sub-index 1.
** \li   Every new error is stored at sub-index 1, the older ones move down
**       the list.
** \li   Writing a 0 to sub-index 0 deletes the entire error history (empties
**       the array). Values higher than 0 are not allowed to write.
** \li   The error numbers are of type UNSIGNED32 and are composed of a 16 bit
**       error code and a 16 bit additional error information field which is
**       manufacturer specific. The error code is contained in the lower 2 bytes
**       (LSB) and the additional information is included in the upper 2 bytes
**       (MSB).
**
** If this object is supported it must consist of two entries at least.
** The length entry on sub-index 0h and at least one error entry at sub-index 1H.
** This function is called by the CANopen slave framework on
** reception on a SDO message (callback).
**
*/
#if COS_DICT_OBJ_1003 > 0
uint8_t  CosEmcyErrorField(uint8_t ubSubIndexV, uint8_t ubReqCodeV);
#endif


/*!
** \brief   Index 1014 - COB-ID Emergency Object
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code (enumeration #CosSdo_e)
**
** Index 1014h defines the COB-ID of the Emergency Object (EMCY).
** This function is called by the CANopen slave framework on
** reception on a SDO message (callback).
**
*/
uint8_t  CosEmcyIdentifier(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


/*!
** \brief   Index 1015 - Inhibit time Emergency Object
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code (enumeration #CosSdo_e)
**
** Index 1015h defines the inhibit time of the Emergency Object (EMCY).
** This function is called by the CANopen slave framework on
** reception on a SDO message (callback).
**
*/
uint8_t  CosEmcyInhibit(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


/*!
** \brief   Initialise Emergency service
**
** This function initialises the emergency service. It is
** called during the initialisation process of the CANopen
** slave (function CosMgrInit() in the file cos_mgr.c).
**
*/
void  CosEmcyInit(void);


/*!
** \brief   Send Emergency Message
** \param   uwEmcyCodeV          emergency code
** \param   pubCustomerCodeV     customer specific code
**
** This function is called by the application program
**
*/
void  CosEmcySend(uint16_t uwEmcyCodeV, uint8_t * pubCustomerCodeV);


void  CosEmcyTmrEvent(void);

//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//


#endif   // COS_EMCY_H_

