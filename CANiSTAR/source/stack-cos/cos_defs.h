//****************************************************************************//
// File:          cos_defs.h                                                  //
// Description:   Global definitions, constants and structures for            //
//                CANopen Slave                                               //
//                                                                            //
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
// 26.06.2000  Initial version                                                //
// 06.11.2000  Added Online-Documentation for Doxygen                         //
// 22.03.2002  Changed message buffer layout                                  //
// 06.03.2006  Changed message buffer layout                                  //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-10-15 13:10:28 +0200 (Mi, 15. Okt 2014) $
// SVN  $Rev: 6275 $ --- $Author: koppe $
//------------------------------------------------------------------------------

#ifndef  COS_DEFS_H_
#define  COS_DEFS_H_

/*----------------------------------------------------------------------------*/
/* splint configuration                                                       */
/*@ +enumint                                                                 @*/
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// include the header files for CANpie                                        //
//----------------------------------------------------------------------------//
#include "cp_core.h"
#include "cp_msg.h"

//----------------------------------------------------------------------------//
// CANopen Slave configuration / definitions                                  //
//----------------------------------------------------------------------------//


#include "canopen.h"

#ifndef  COS_CONF_H_
#include <cos_conf.h>
#endif



//-----------------------------------------------------------------------------
/*!
** \file    cos_defs.h
** \brief   CANopen slave definitions
**
** The file cos_defs.h holds global definitions, constants and structures
** for the CANopen Slave. The files 'canopen.h' and 'cos_conf.h' are included
** by this header.
*/


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/
#define  COS_VERSION_MAJ            0x04000000
#define  COS_VERSION_MIN            0x00080000


/*----------------------------------------------------------------------------*\
** Message buffers                                                            **
**                                                                            **
\*----------------------------------------------------------------------------*/



//-------------------------------------------------------------------
/*!
** \enum    CosBuf_e
** \brief   Message buffer allocation
**
** Each CANopen service (i.e. identifier) is assigned to a specific
** message buffer in the CAN controller. For BasicCAN controllers
** the message buffers must be emulated in software (refer to CANpie).
*/
enum CosBuf_e {
   /*!
   ** NMT, Identifier 000h (Buffer 1)
   */
   eCosBuf_NMT = CP_BUFFER_1,

   /*!
   ** SYNC, Identifier 080h (Buffer 2)
   */
   eCosBuf_SYNC,

   /*!
   ** SDO Rx, Identifier 600h + NodeId (Buffer 3)
   */
   eCosBuf_SDO_RCV,

   /*!
   ** SDO Tx, Identifier 580h + NodeId (Buffer 4)
   */
   eCosBuf_SDO_TRM,


   #if COS_LSS_SUPPORT > 0
   /*!
   ** LSS Rx, Identifier 7E5h, (Buffer 5)
   */
   eCosBuf_LSS_RCV,

   /*!
   ** LSS Tx, Identifer 7E4h, (Buffer 6)
   */
   eCosBuf_LSS_TRM,
   #endif

   #if COS_SDO_CLIENT > 0
   /*!
   ** SDO Client Rx, Identifier 580h + Dest. NodeId
   */
   eCosBuf_SDOC_RCV,

   /*!
   ** SDO Client Tx, Identifier 600h + Dest. NodeId
   */
   eCosBuf_SDOC_TRM,
   #endif

   /*!
   ** Heartbeat producer, Nodeguarding (Buffer 7)
   */
   eCosBuf_NMT_ERR,


   /*!
   ** Heartbeat consumer, (Buffer 8)
   */
   #if COS_DICT_OBJ_1016 > 0
   eCosBuf_NMT_HBC,
   #endif

   /*!
   ** TIME producer / consumer, (Buffer 9)
   */
   #if COS_DICT_OBJ_1012 > 0
   eCosBuf_TIME = eCosBuf_NMT_ERR + COS_DICT_OBJ_1016 + 1,
   #endif

   /*!
   ** Buffer for first Receive PDO
   */
   #if COS_PDO_RCV_NUMBER > 0
   eCosBuf_PDO1_RCV = eCosBuf_NMT_ERR + COS_DICT_OBJ_1016 + COS_DICT_OBJ_1012 + 1,
   #endif

   /*!
   ** Buffer for first Transmit PDO
   */
   eCosBuf_PDO1_TRM = eCosBuf_NMT_ERR + COS_DICT_OBJ_1016 + COS_PDO_RCV_NUMBER + 1,

   /*!
   ** Buffer for emergency service
   */
   eCosBuf_EMCY = eCosBuf_PDO1_TRM + COS_PDO_TRM_NUMBER

};

//-------------------------------------------------------------------
// Test if the buffer allocation fits in the scheme of the
// CANpie driver. If this error occurs, more buffers are allocated
// for the CANopen Slave stack than the CAN driver actually
// supports. Check the CAN driver settings and / or the settings
// of the CANopen Slave stack.
//
#define COS_BUFFER_MAX     (6 + COS_DICT_OBJ_1016 + COS_PDO_RCV_NUMBER + COS_PDO_TRM_NUMBER)
#if CP_BUFFER_MAX < COS_BUFFER_MAX
#error CAN buffer overflow, check CANpie driver / Slave stack configuration
#endif


/*----------------------------------------------------------------------------*\
** Data store / restore into non-volatile memory                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
/*!
** \enum    COS_PARM_e
** \brief   Parameter block for Store / Restore
**
** The following enumeration defines the parameter block that has
** been selected for a store / restore operation.
*/
enum COS_PARM_e {
   /*! no parameter block selected                             */
   eCOS_PARM_NONE = 0,

   /*! all parameters                                          */
   eCOS_PARM_ALL,

   /*! parameters from the communication profile               */
   eCOS_PARM_COM,

   /*! application parameters (device profile)                 */
   eCOS_PARM_APP,

   /*! manufacturer parameters                                 */
   eCOS_PARM_MAN
};


/*-------------------------------------------------------------------------*\
** Return code for SDO callback functions                                  **
**                                                                         **
\*-------------------------------------------------------------------------*/


//-------------------------------------------------------------------
/*!
** \enum    CosSdo_e
** \brief   Return code for SDO callbacks
**
** The following codes are used by callback functions for object
** dictionary access by Service Data Objects.
*/
enum CosSdo_e {

   /*!
   ** Generic read success (any number of bytes)
   */
   eCosSdo_READ_OK,

   /*!
   ** Read success, 1 byte of data in expedited SDO
   */
   eCosSdo_READ1_OK,

   /*!
   ** Read success, 2 bytes of data in expedited SDO
   */
   eCosSdo_READ2_OK,

   /*!
   ** Read success, 3 bytes of data in expedited SDO
   */
   eCosSdo_READ3_OK,

   /*!
   ** Read success, 4 bytes of data in expedited SDO
   */
   eCosSdo_READ4_OK,

   eCosSdo_READ_SEG_OK,

   /*!
   ** Write success
   */
   eCosSdo_WRITE_OK,

   /*!
   ** Toggle bit not alternated.
   */
   eCosSdo_ERR_TOGGLE_BIT = 20,

   /*!
   ** SDO protocol timed out.
   */
   eCosSdo_ERR_TIMEOUT,

   /*!
   ** Client/server command specifier not valid or unknown.
   */
   eCosSdo_ERR_COMMAND,

   /*!
   ** Invalid block size (block mode only).
   */
   eCosSdo_ERR_BLOCK_SIZE,

   /*!
   ** Invalid sequence number (block mode only).
   */
   eCosSdo_ERR_BLOCK_SEQUENCE,

   /*!
   ** CRC error (block mode only).
   */
   eCosSdo_ERR_BLOCK_CRC,

   /*!
   ** Out of memory.
   */
   eCosSdo_ERR_MEMORY,

   /*!
   ** Unsupported access to an object.
   */
   eCosSdo_ERR_ACCESS_UNSUPPORTED,

   /*!
   ** Attempt to read a write only object.
   */
   eCosSdo_ERR_ACCESS_WO,

   /*!
   ** Attempt to write a read only object.
   */
   eCosSdo_ERR_ACCESS_RO,

   /*!
   ** Object does not exist in the object dictionary.
   */
   eCosSdo_ERR_NO_OBJECT,

   /*!
   ** Object cannot be mapped to the PDO.
   */
   eCosSdo_ERR_MAPPING_OBJECT,

   /*!
   ** The number and length of the objects to be mapped would
   ** exceed PDO length.
   */
   eCosSdo_ERR_MAPPING_LENGTH,

   /*!
   ** General parameter incompatibility reason.
   */
   eCosSdo_ERR_GENERAL_PARAMETER,

   /*!
   ** General internal incompatibility in the device.
   */
   eCosSdo_ERR_GENERAL_DEVICE,

   /*!
   ** Access failed due to an hardware error.
   */
   eCosSdo_ERR_HARDWARE,

   /*!
   ** Data type does not match or length of service parameter
   ** does not match.
   */
   eCosSdo_ERR_DATATYPE,

   /*!
   ** Data type does not match or length of service parameter
   ** too high.
   */
   eCosSdo_ERR_DATATYPE_HIGH,

   /*!
   ** Data type does not match or length of service parameter
   ** too low.
   */
   eCosSdo_ERR_DATATYPE_LOW,

   /*!
   ** Sub-index does not exist.
   */
   eCosSdo_ERR_NO_SUB_INDEX,

   /*!
   ** Value range of parameter exceeded (only for write access).
   */
   eCosSdo_ERR_VALUE_RANGE,

   /*!
   ** Value of parameter written too high (download only).
   */
   eCosSdo_ERR_VALUE_HIGH,

   /*!
   ** Value of parameter written too low (download only).
   */
   eCosSdo_ERR_VALUE_LOW,

   /*!
   ** Maximum value is less than minimum value.
   */
   eCosSdo_ERR_VALUE_MIN_MAX,

   /*!
   ** General SDO error.
   */
   eCosSdo_ERR_GENERAL,

   /*!
   ** Data cannot be transferred or stored to the application.
   */
   eCosSdo_ERR_DATA_STORE,

   /*!
   ** Data cannot be transferred or stored to the application
   ** because of local control.
   */
   eCosSdo_ERR_DATA_STORE_LOCAL,

   /*!
   ** Data cannot be transferred or stored to the application because of
   ** the present device state.
   */
   eCosSdo_ERR_DATA_STORE_STATE,

   /*!
   ** Object dictionary dynamic generation fails or no object
   ** dictionary is present (e.g. object dictionary is generated
   ** from file and generation fails because of an file error).
   */
   eCosSdo_ERR_OBJECT_DICTIONARY,

   /*!
   ** Data actually not available for access
   */
   eCosSdo_ERR_NO_DATA,

   /*!
   ** Do not send a SDO response by the stack. The application must
   ** send the SDO response manually.
   */
   eCosSdo_ERR_DEFER_RESPONSE,

   eCosSdo_ERR_MAX
};




/*-------------------------------------------------------------------------*\
** Error codes                                                             **
**                                                                         **
\*-------------------------------------------------------------------------*/

//---------------------------------------------------------
/*!
** \enum    CosErr_e
** \brief   Error Codes of protocol stack
**
** All error codes of the CANopen slave protocol stack have
** the prefix 'eCosErr_'.
**
*/
enum CosErr_e {

   /*!
   ** No error
   */
   eCosErr_OK = 0,

   eCosErr_CAN_INIT,

   /*!
   ** Node is not initialised
   */
   eCosErr_NODE_INIT = 0x10,

   /*!
   ** Reset request for Node
   */
   eCosErr_NODE_RESET,

   /*!
   ** Node ID (device address) is out of range
   */
   eCosErr_VALUE_NODE_ID = 0x30,

   /*!
   ** Baudrate is out of range
   */
   eCosErr_VALUE_BAUDRATE,

   /*!
   ** Identifier is out of range
   */
   eCosErr_VALUE_IDENTIFIER,

   //<! failure of parameter load operation
   eCosErr_PARM_LOAD = 0x40,

   // failure of parameter save operation
   eCosErr_PARM_SAVE
};


//---------------------------------------------------------
/*!
** \enum    CosCOB_ID_Store_e
** \brief   COB-ID storage behaviour
**
**
*/
enum CosCOB_ID_Store_e {

   /*!
   ** keep stored data
   */
   eCosCOB_ID_Store_KEEP = 0,

   /*!
   ** use pre-defined connection set
   */
   eCosCOB_ID_Store_PREDEF,

   /*!
   ** use offset
   */
   eCosCOB_ID_Store_OFFSET

};

#endif   // COS_DEFS_H_
