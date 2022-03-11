//****************************************************************************//
// File:          canopen.h                                                   //
// Description:   General definitions for CANopen                             //
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
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 17:09:58 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5697 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  CANOPEN_H_
#define  CANOPEN_H_

/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "compiler.h"            // datatype definitions


//-----------------------------------------------------------------------------
/*!
** \file    canopen.h
** \brief   General CANopen definitions
**
** The file canopen.h holds all definitions and structures that are
** necessary for a CANopen protocol stack.
*/


//-------------------------------------------------------------------
// identifier values for the pre-defined connection set
// the value used by the device is ID_BASE_XXX + node id
// direction has to be seen from the devices point of view
//
#define  ID_BASE_EMCY      0x080

#define  ID_BASE_PDO1_TX   0x180
#define  ID_BASE_PDO1_RX   0x200
#define  ID_BASE_PDO2_TX   0x280
#define  ID_BASE_PDO2_RX   0x300

#define  ID_BASE_SDO_TX    0x580
#define  ID_BASE_SDO_RX    0x600

#define  ID_BASE_NMT_ERR   0x700

//-------------------------------------------------------------------
/*!
** \enum    SdoCmd_e
** \brief   Command values for SDO transfer
**
*/
enum SdoCmd_e {

   eSDO_WRITE_RESP_SEG  = 0x00,  /*! response to segmented write request   */
   eSDO_WRITE_REQ_SEG   = 0x21,  /*! segmented write request               */
   eSDO_WRITE_REQ_0     = 0x22,  /*! write request, unspecified length     */
   eSDO_WRITE_REQ_4     = 0x23,  /*! write request, 4 bytes data           */
   eSDO_WRITE_REQ_3     = 0x27,  /*! write request, 3 bytes data           */
   eSDO_WRITE_REQ_2     = 0x2B,  /*! write request, 2 bytes data           */
   eSDO_WRITE_REQ_1     = 0x2F,  /*! write request, 1 byte data            */
   eSDO_WRITE_RESP      = 0x60,  /*! response to write request             */

   eSDO_READ_REQ        = 0x40,  /*! read request                          */
   eSDO_READ_RESP_SEG   = 0x41,  /*! read response, segmented data         */
   eSDO_READ_RESP_0     = 0x42,  /*! read response, unspecified length     */
   eSDO_READ_RESP_4     = 0x43,  /*! read response, 4 bytes data           */
   eSDO_READ_RESP_3     = 0x47,  /*! read response, 3 bytes data           */
   eSDO_READ_RESP_2     = 0x4B,  /*! read response, 2 bytes data           */
   eSDO_READ_RESP_1     = 0x4F,  /*! read response, 1 byte data            */

   eSDO_READ_REQ_SEG_0  = 0x60,  /*! read request, segmented data          */
   eSDO_READ_REQ_SEG_1  = 0x70,  /*! read request, segmented data          */

   eSDO_BLK_UP_REQ_0    = 0xA0,  /*! Init Block upload                     */
   eSDO_BLK_UP_REQ_1    = 0xA4,  /*! Init Block upload, CRC Info           */

   eSDO_BLK_DOWN_REQ_0  = 0xC0,  /*! Initiate Block download               */
   eSDO_BLK_DOWN_REQ_1  = 0xC2,  /*! Initiate Block download, size info    */
   eSDO_BLK_DOWN_REQ_2  = 0xC4,  /*! Initiate Block download, CRC info     */
   eSDO_BLK_DOWN_REQ_3  = 0xC6,  /*! Initiate Block download, CRC & size   */

   eSDO_ABORT           = 0x80   /*! abort transmission                    */
};


/*----------------------------------------------------------------------------*\
** SDO abort codes                                                            **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
/*!
** \defgroup SDO_ERR  SDO error codes
** The SDO abort transfer service aborts the SDO upload service or
** SDO download service of an SDO referenced by its number. The
** reason is indicated by a 32-bit value.
*/

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_TOGGLE_BIT
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Toggle bit not altered.
*/
#define SDO_ERR_TOGGLE_BIT             (uint32_t)(0x05030000)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_TIMEOUT
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** SDO protocol timed out.
*/
#define SDO_ERR_TIMEOUT                (uint32_t)(0x05040000)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_COMMAND
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Client/server command specifier not valid or unknown.
*/
#define SDO_ERR_COMMAND                (uint32_t)(0x05040001)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_BLOCK_SIZE
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Invalid block size (block mode only).
*/
#define SDO_ERR_BLOCK_SIZE             (uint32_t)(0x05040002)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_BLOCK_SEQUENCE
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Invalid block sequence number (block mode only).
*/
#define SDO_ERR_BLOCK_SEQUENCE         (uint32_t)(0x05040003)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_BLOCK_CRC
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Invalid block CRC value (block mode only).
*/
#define SDO_ERR_BLOCK_CRC              (uint32_t)(0x05040004)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_MEMORY
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Out of memory.
*/
#define SDO_ERR_MEMORY                 (uint32_t)(0x05040005)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_ACCESS_UNSUPPORTED
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Unsupported access to an object.
*/
#define SDO_ERR_ACCESS_UNSUPPORTED     (uint32_t)(0x06010000)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_ACCESS_WO
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Attempt to read a write only object.
*/
#define SDO_ERR_ACCESS_WO              (uint32_t)(0x06010001)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_ACCESS_RO
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Attempt to write a read only object.
*/
#define SDO_ERR_ACCESS_RO              (uint32_t)(0x06010002)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_NO_OBJECT
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Object does not exist in the object dictionary.
*/
#define SDO_ERR_NO_OBJECT              (uint32_t)(0x06020000)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_MAPPING_OBJECT
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Object cannot be mapped to the PDO.
*/
#define SDO_ERR_MAPPING_OBJECT         (uint32_t)(0x06040041)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_MAPPING_LENGTH
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** The number and length of the objects to be mapped would exceed
** PDO length.
*/
#define SDO_ERR_MAPPING_LENGTH         (uint32_t)(0x06040042)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_GENERAL_PARAMETER
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** General parameter incompatibility reason.
*/
#define SDO_ERR_GENERAL_PARAMETER      (uint32_t)(0x06040043)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_GENERAL_DEVICE
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** General internal incompatibility in the device.
*/
#define SDO_ERR_GENERAL_DEVICE         (uint32_t)(0x06040047)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_HARDWARE
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Access failed due to an hardware error.
*/
#define SDO_ERR_HARDWARE               (uint32_t)(0x06060000)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_DATATYPE
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Data type does not match, length of service parameter does
** not match.
*/
#define SDO_ERR_DATATYPE               (uint32_t)(0x06070010)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_DATATYPE_HIGH
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Data type does not match, length of service parameter too high.
*/
#define SDO_ERR_DATATYPE_HIGH          (uint32_t)(0x06070012)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_DATATYPE_LOW
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Data type does not match, length of service parameter too low.
*/
#define SDO_ERR_DATATYPE_LOW           (uint32_t)(0x06070013)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_NO_SUB_INDEX
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Sub-index does not exist.
*/
#define SDO_ERR_NO_SUB_INDEX           (uint32_t)(0x06090011)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_VALUE_RANGE
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Invalid value for parameter (download only).
*/
#define SDO_ERR_VALUE_RANGE            (uint32_t)(0x06090030)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_VALUE_HIGH
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Value of parameter written too high (download only).
*/
#define SDO_ERR_VALUE_HIGH             (uint32_t)(0x06090031)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_VALUE_LOW
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Value of parameter written too low (download only).
*/
#define SDO_ERR_VALUE_LOW              (uint32_t)(0x06090032)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_VALUE_MIN_MAX
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Maximum value is less than minimum value.
*/
#define SDO_ERR_VALUE_MIN_MAX          (uint32_t)(0x06090036)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_SDO_CONNECTION
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Resource not available: SDO connection.
*/
#define SDO_ERR_SDO_CONNECTION         (uint32_t)(0x060A0023)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_GENERAL
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** General error.
*/
#define SDO_ERR_GENERAL                (uint32_t)(0x08000000)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_DATA_STORE
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Data cannot be transferred or stored to the application.
*/
#define SDO_ERR_DATA_STORE             (uint32_t)(0x08000020)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_DATA_STORE_LOCAL
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Data cannot be transferred or stored to the application because
** of local control.
*/
#define SDO_ERR_DATA_STORE_LOCAL       (uint32_t)(0x08000021)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_DATA_STORE_STATE
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Data cannot be transferred or stored to the application because
** of the present device state.
*/
#define SDO_ERR_DATA_STORE_STATE       (uint32_t)(0x08000022)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_OBJECT_DICTIONARY
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** Object dictionary dynamic generation fails or no object dictionary
** is present (e.g. object dictionary is generated from file and
** generation fails because of an file error).
*/
#define SDO_ERR_OBJECT_DICTIONARY      (uint32_t)(0x08000023)

//-------------------------------------------------------------------
/*!
** \def     SDO_ERR_NO_DATA
** \ingroup SDO_ERR
** \brief   SDO Abort Code
**
** No data available.
*/
#define SDO_ERR_NO_DATA                (uint32_t)(0x08000024)


//-----------------------------------------------------------------------------
/*!
** \enum    CoATTR
** \brief   Access Attributes for objects in dictionary
**
** Each object within the object dictionary can have the following access
** attributes:
** \li   Read/Write
** \li   Write Only
** \li   Read Only
** \li   Constant (value is Read Only)
**
** The member ubAttribute of the structure CosDicEntry_s is used for
** that purpose. By this enumeration the possible attributes are defined.
*/
enum CoATTR {
   CoATTR_ACC_RO     = 0x01,     /*! object is read-only                   */

   CoATTR_ACC_WO     = 0x02,     /*! object is write-only                  */

   CoATTR_ACC_RW     = 0x03,     /*! object supports read-write access     */

   CoATTR_ACC_CONST  = 0x05,     /*! object is constant                    */

   CoATTR_PDO_MAP    = 0x10,

   CoATTR_RES_1      = 0x20,

   CoATTR_RES_2      = 0x40,

   CoATTR_FUNCTION   = 0x80      /*! function callback                     */

};



//-----------------------------------------------------------------------------
// attributes for the object dictionary
//




//-----------------------------------------------------------------------------
/*!
** \enum    CoDT
** \brief   Data Type definitions for objects in dictionary
**
** Each object within the object dictionary has a specific data type.
** The member ubDataType of the structure CoStruct_DicEntry is used for
** that purpose. By this enumeration the possible data types are defined.
**
*/
enum CoDT {
   /*!
   ** data type:  boolean value
   */
   CoDT_BOOLEAN = 1,

   /*!
   ** data type:  Signed Integer, 1 byte
   */
   CoDT_INTEGER8,

   /*!
   ** data type:  Signed Integer, 2 bytes
   */
   CoDT_INTEGER16,

   /*!
   ** data type:  Signed Integer, 4 bytes
   */
   CoDT_INTEGER32,

   /*!
   ** data type:  Unsigned Integer, 1 byte
   */
   CoDT_UNSIGNED8,

   /*!
   ** data type:  Unsigned Integer, 2 bytes
   */
   CoDT_UNSIGNED16,

   /*!
   ** data type:  Unsigned Integer, 4 bytes
   */
   CoDT_UNSIGNED32,

   /*!
   ** data type:  Float, 32 bit according to IEEE
   */
   CoDT_REAL32,

   /*!
   ** data type:  character string
   */
   CoDT_VISIBLE_STRING,

   /*!
   ** data type:  octet string
   */
   CoDT_OCTET_STRING,

   /*!
   ** data type:  unicode string
   */
   CoDT_UNICODE_STRING,

   /*!
   ** data type:  time of day value
   */
   CoDT_TIME_OF_DAY,

   /*!
   ** data type:  time difference value
   */
   CoDT_TIME_DIFFERENCE,

   CoDT_reserved01,

   CoDT_DOMAIN,

   CoDT_INTEGER24,

   CoDT_REAL64,

   CoDT_INTEGER40,

   CoDT_INTEGER48,

   CoDT_INTEGER56,

   /*!
   ** data type:  Signed Integer, 8 bytes
   */
   CoDT_INTEGER64,

   CoDT_UNSIGNED24,

   CoDT_reserved02,

   CoDT_UNSIGNED40,

   CoDT_UNSIGNED48,

   CoDT_UNSIGNED56,

   /*!
   ** data type:  Unsigned Integer, 8 bytes
   */
   CoDT_UNSIGNED64


};


typedef uint32_t (*pfnSdoHandler)(uint16_t, uint8_t, uint8_t, void *);


struct CoObject_s {
   uint16_t     uwIndex;
   uint8_t     ubSubIndex;

   /*    reserved entry for data alignment                           */
   uint8_t     ubRes0;

   uint16_t     uwRange;

   uint8_t     ubAttribute;

   /*!
   **    The data type may be one of the definitions made in the
   **    enumeration CoDT.
   */
   uint8_t     ubDataType;

   void *   pvdData;
   //uint32_t     (*pfnSdoHandler)(uint16_t, uint8_t, uint8_t, void *);

   uint32_t *	pulDataSize;

};

typedef struct CoObject_s  CoObject_ts;


//---------------------------------------------------------
/*!
** \enum    LssCmd_e
** \brief   LSS command specifiers
**
*/
enum LssCmd_e {

   /*! Switch mode global command ( 04h - 4dec)          */
   eLSS_CMD_MODE_GLOBAL             = 4,

   /*! Configure node-ID command (11h - 17dec)           */
   eLSS_CMD_CONFIG_NODE_ID          = 17,

   /*! Configure bit-timing command (13h - 19dec)        */
   eLSS_CMD_CONFIG_BIT_TIMING       = 19,

   eLSS_CMD_ACTIVE_BIT_TIMING       = 21,

   /*! Store configuration command (17h - 23dec)         */
   eLSS_CMD_CONFIG_STORE            = 23,

   eLSS_CMD_MODE_SEL_VENDOR_ID      = 64,
   eLSS_CMD_MODE_SEL_PRODUCT_CODE,
   eLSS_CMD_MODE_SEL_REVISION_NUM,
   eLSS_CMD_MODE_SEL_SERIAL_NUM,
   eLSS_CMD_MODE_SEL_RESPONSE,

   eLSS_CMD_IDENT_SLAVE_NCFG_REQ    = 76,
   eLSS_CMD_IDENT_SLAVE_NCFG_RES    = 80,

   eLSS_CMD_INQUIRE_VENDOR_ID       = 90,
   eLSS_CMD_INQUIRE_PRODUCT_CODE,
   eLSS_CMD_INQUIRE_REVISION_NUM,
   eLSS_CMD_INQUIRE_SERIAL_NUM,
   eLSS_CMD_INQUIRE_NODE_ID
};


//---------------------------------------------------------
/*!
** \enum    CoErrReg_e
** \brief   Error Register values
**
*/
enum CoErrReg_e {

   /*! generic error                                     */
   eCoErrReg_GENERIC                = 0x01,

   /*! current error                                     */
   eCoErrReg_CURRENT                = 0x02,

   /*! voltage error                                     */
   eCoErrReg_VOLTAGE                = 0x04,

   /*! temperature error                                 */
   eCoErrReg_TEMPERATURE            = 0x08,

   /*! communication error                               */
   eCoErrReg_COMMUNICATION          = 0x10,

   /*! device profile specific error                     */
   eCoErrReg_PROFILE                = 0x20,

   /*! manufacturer specific error                       */
   eCoErrReg_MANUFACTURER           = 0x80

};

#endif      // CANOPEN_H_
