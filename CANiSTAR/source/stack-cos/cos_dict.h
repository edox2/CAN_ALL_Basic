//****************************************************************************//
// File:          cos_dict.h                                                  //
// Description:   Object dictionary for CANopen slave                         //
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
// 06.12.2000  Additional objects in dictionary                               //
// 05.02.2001  Bugfix in Search function                                      //
// 06.02.2003  Changed CosDictFindEntry() parameter structure                 //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-10-15 13:01:11 +0200 (Mi, 15. Okt 2014) $
// SVN  $Rev: 6274 $ --- $Author: tiderko $
//------------------------------------------------------------------------------


#ifndef  COS_DICT_H_
#define  COS_DICT_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_defs.h"      // CANopen Slave definition file

//-----------------------------------------------------------------------------
// Test for application profile CiA 417 (CANopen Lift) here and
// use another header file (cos_dict_lift.h) then
//
#if COS_DS417 > 0
#include "cos_dict_lift.h"
#else

//-----------------------------------------------------------------------------
/*!
** \file    cos_dict.h
** \brief   CANopen Slave Dictionary Functions
**
** This module holds the complete object dictionary of the CANopen slave.
** The implementation file cos_dict.c has a table (aDicTableG[]) with all
** necessary entries of the communication profile DS-301. Other dirctionary
** entries (device profile, manufacturer specific) are included in this
** table with external files (objXXX.tab).
**
*/


//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//


/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
// A dictionary entry can hold a pointer to data or a pointer to a
// SDO callback function. The SDO callback function is declared here.
//
#ifdef __MWERKS__
typedef uint8_t (*far SdoHandler_fn)(uint8_t ubSubIndexV, uint8_t ubReqCodeV);
#else
typedef uint8_t (* SdoHandler_fn)(uint8_t ubSubIndexV, uint8_t ubReqCodeV);
#endif


//-------------------------------------------------------------------
/*!
** \struct  CosDicEntry_s cos_dict.h
** \brief   Object Dictionary entry
**
** The structure CosDicEntry_s describes a single dictionary entry.
** Each entry is defined by the index, sub-index, access-attribute
** and data type. The pointer 'pActualValue' points to the data of
** the specified object or a callback function.
*/
struct CosDicEntry_s {

   /*!
   **    The index is a 16 bit value. Possible values (according to
   **    CANopen specification) are:
   **    \li   1000 - 1FFF (DS-301, Communication profile)
   **    \li   2000 - 5FFF (DS-301, vendor specific)
   **    \li   6000 - 9FFF (DS-4xx, device profile)
   */
   uint16_t     uwIndex;

   /*!
   **    The sub-index is a 8-bit value.
   */
   uint8_t     ubSubIndex;

   /*!
   **    The member attribute holds the access attribute defines in
   **    the enumeration CoACC, the PDO mapping information and the
   **    object search method.
   **    \li   Bit 0 .. Bit 3: object access (read/write/const)
   **    \li   Bit 4: set to 1 means PDO mapping possible
   **    \li   Bit 5: reserved
   **    \li   Bit 6: reserved
   **    \li   Bit 7: don't check sub-index for searching
   */
   uint8_t     ubAttribute;

   /*!
   **    The data type may be one of the definitions made in the
   **    enumeration CoDT.
   */
   uint8_t     ubDataType;

   /*!
   **    Pointer to the actual value or SDO callback.
   */
   #ifdef __MWERKS__
   void *far  pvdValue;
   #else
   void *      pvdValue;
   #endif
 };


typedef struct CosDicEntry_s  CosDicEntry_ts;


enum CosDict_e {

   eCosDict_FAIL_INDEX = 0,
   eCosDict_FAIL_SUBINDEX,
   eCosDict_FOUND_OBJECT

};

/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*!
** \brief   Find entry in object dictionary
** \param   uwIndexV    index of entry
** \param   ubSubIndexV sub-index of entry
** \param   pubStatusV  pointer to status
** \return  Pointer to dictionary entry
**
** This routine searches the dictionary for an entry defined by the parameters
** index and sub-index. On success, the pointer to the dictionary entry is
** returned. On failure the value 0L is returned. The status of the search rountine
** is copied to the location pubStatusV. The following status codes are valid:
**
** \li eCosDict_FOUND_OBJECT  : object was found (index and sub-index)
** \li eCosDict_FAIL_SUBINDEX : index was found, sub-index not present
** \li eCosDict_FAIL_INDEX    : object not found (no index)
**
*/
CPP_CONST CosDicEntry_ts * CosDictFindEntry( uint16_t uwIndexV,
                                             uint8_t ubSubIndexV,
                                             uint8_t *pubStatusV);


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//

#endif   // #if COS_DS417 > 0

#endif   // COS_DICT_H_

