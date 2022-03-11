//============================================================================//
// File:          mc_flash.h                                                  //
// Description:   Flash functions prototypes                                  //
// Author:        Johann Tiderko                                              //
// e-mail:        johann.tiderko@microcontrol.net                             //
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
// 10.11.2008  Initial version                                                //
//                                                                            //
//============================================================================//


//------------------------------------------------------------------------------
// SVN  $Date:2008-10-15 14:16:26 +0000 (Mi, 15 Okt 2008) $
// SVN  $Rev:1874 $ --- $Author:koppe $
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/*!
** \file    mc_flash.h
** \brief   MCL - erase and write flash memory
** \author  Johann Tiderko
** \author  johann.tiderko@microcontrol.net
**
** The file defines functions to erase a flash memory and to write data
** to a flash memory.
**
*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "compiler.h"


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/




//-------------------------------------------------------------------
/*!
** \enum    FLASH_ERR_e
** \brief   Error code for flash memory access
**
** The following error codes are used by the flash driver. The
** status can be checked via the function McFlashGetStatus().
*/
enum FLASH_ERR_e {
   /*! Device is operational, success of latest operation      */
   eFLASH_ERR_OK          = 0x00,

   /*! Hardware or initialisation failure of device            */
   eFLASH_ERR_INIT        = 0x01,

   /*! Address out of range                                    */
   eFLASH_ERR_ADDRESS     = 0x02,

   /*! Failure of read operation                               */
   eFLASH_ERR_READ        = 0x04,

   /*! Failure of write operation                              */
   eFLASH_ERR_WRITE       = 0x08,

   /*! Failure of erase operation                              */
   eFLASH_ERR_ERASE       = 0x10,

   /*! Failure of checksum operation                           */
   eFLASH_ERR_CHECKSUM    = 0x20,

   /*! Collision during read/write/erase operation             */
   eFLASH_ERR_COLLISION   = 0x40
};

/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/



//-------------------------------------------------------------------
/*!
** \brief Clear status of the last flash operation.
**
*/
void McFlashClrStatus(void);


//-------------------------------------------------------------------
/*!
** \brief   Get status of the last flash operation.
** \return  Status of the last flash operation.
**
*/
Status_tv McFlashGetStatus(void);


//-------------------------------------------------------------------
/*!
** \brief   Erase page of selected address.
** \param   ulFlashAddrV - Memory start address
** \param   ulSizeV - Memory size to erase
** \return  Status of flash operation
**
** This function is used to erase \a ulSizeV bytes of data, starting
** from address \a ulFlashAddrV.
*/
Status_tv McFlashErase(uint32_t ulFlashAddrV, uint32_t ulSizeV);


//-------------------------------------------------------------------
/*!
** \brief   Initialise flash of MCU
** \return  Status of flash operation
**
*/
Status_tv McFlashInit(void);



//-------------------------------------------------------------------
/*!
** \brief   Lock the Flash memory controller.
** \return  Status of flash operation
** \see     McFlashUnlock()
*/
Status_tv McFlashLock(void);



//-------------------------------------------------------------------
/*!
** \brief   Write data
** \param   ulAddressV - Flash memory address
** \param   pvdDataV   - Pointer to data
** \param   ulSizeV    - number of bytes to write
** \return  Status of flash operation
**
** This function is used to write data to a given flash memory
** location \c ulAddressV. The function checks the address range
** and the address alignment. In order to write to the flash
** memory, it has to be unlocked by calling McFlashUnlock()
** in advance.
**
*/
Status_tv McFlashWrite(uint32_t ulAddressV, void * pvdDataV, uint32_t ulSizeV);



//-------------------------------------------------------------------
/*!
** \brief   Unlock the Flash memory controller.
** \return  Status of flash operation
** \see     McFlashLock()
*/
Status_tv McFlashUnlock(void);
