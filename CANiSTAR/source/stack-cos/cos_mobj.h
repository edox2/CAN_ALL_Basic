//****************************************************************************//
// File:          cos_mobj.h                                                  //
// Description:   Manufacturer specific objects                               //
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
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 17:09:58 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5697 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  COS_MOBJ_H_
#define  COS_MOBJ_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_defs.h"      // CANopen Slave definition file


#ifndef  COS_MOB_MC
#define  COS_MOB_MC  0
#endif

#if COS_MOB_MC > 0
#include "mc_co_mobj.h"
#endif


//-----------------------------------------------------------------------------
/*!
** \file    cos_mobj.h
** \brief   Manufacturer specific objects
**
** This module implements the variables and callback functions for
** manufacturer specific objects. The header file and the corresponded
** implementation file provide an example. The code has to be adopted
** to the application by the manufacturer.
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





/*----------------------------------------------------------------------------*\
** variables of module for external use                                       **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
// the variable 'ubCosMob_Var2002G' has read/write access
//
extern uint8_t    ubCosMob_Var2002G;

//-------------------------------------------------------------------
// the variable 'uwCosMob_Var2003G' has read/write access
//
extern uint16_t   uwCosMob_Var2003G;

//-------------------------------------------------------------------
// the variable 'ulCosMob_Var2004G' has read/write access
//
extern uint32_t   ulCosMob_Var2004G;

//-------------------------------------------------------------------
// the variable 'uqCosMob_Var2005G' has read/write access
//
#if CPP_DATA_SIZE >= 64
extern uint64_t   uqCosMob_Var2005G;
#endif

//-------------------------------------------------------------------
// the string 'szCosMob_Str2008G' has read/write acccess
//
extern char	      szCosMob_Str2008G[];

/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*!
** \brief   Initialise manufacturer variables
** \sa      CosMgrParmInit()
**
** This function is used to initialise manufacturer variables with default
** values. The code provides an example and has to be adopted to the
** application by the manufacturer.
** <p>
** The function is called by the protocol stack (CosMgrParmInit()).
**
*/
void  CosMob_ParmInit(void);


/*!
** \brief   Load manufacturer variables from NV-memory
** \sa      CosMgrParmLoad()
**
** This function is used to load manufacturer variables with previously
** stored values.
** <p>
** The function is called by the protocol stack (CosMgrParmLoad()).
**
*/
uint8_t  CosMob_ParmLoad(void);


/*!
** \brief   Save manufacturer variables to NV-memory
** \sa      CosMgrParmSave()
**
** This function is used to save manufacturer variables to a
** non-volatile memory.
** <p>
** The function is called by the protocol stack (CosMgrParmSave()).
**
*/
uint8_t  CosMob_ParmSave(void);


/*!
** \brief   Index 2000h - Example function 1
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code
**
** This function provides an example for read/write access to an array
** of values (16 bit unsigned). The size of the array is defined via
** the constant value \e MOB2000_SIZE inside the implementation file.
**
** The function is called by the protocol stack.
** <p>
** The function returns a SDO response code defined by the #CosSdo_e
** enumeration.
**
*/
uint8_t  CosMob_Idx2000(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


/*!
** \brief   Index 2001h - Example function 2
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code
**
** This function provides an example for read/write access to value with
** limit check.
**
** The function is called by the protocol stack.
** <p>
** The function returns a SDO response code defined by the #CosSdo_e
** enumeration.
*/
uint8_t  CosMob_Idx2001(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


/*!
** \brief   Index 2004h - Example function 3
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code
**
** This function provides an example for read/write access to
** an object with deferred SDO response. The application is responsible
** to sent a valid response later.
**
** \code
** extern uint16_t uwMyValueG;
**
** void MySdoResponse(void)
** {
**    //---------------------------------------------------
**    // example for a read access
**    //
**    CosSdoCopyValueToMessage(  (void *) &uwMyValueG, CoDT_UNSIGNED16);
**    CosSdoResponse(eCosSdo_READ2_OK);
** }
** \endcode
*
** The function is called by the protocol stack.
** <p>
** The function returns a SDO response code defined by the #CosSdo_e
** enumeration.
*/
uint8_t  CosMob_Idx2006(uint8_t ubSubIndexV, uint8_t ubReqCodeV);

uint8_t  CosMob_Idx2007(uint8_t ubSubIndexV, uint8_t ubReqCodeV);

//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//


#endif   // COS_MOBJ_H_

