//============================================================================//
// File:          mc_nvm.h                                                    //
// Description:   Functions for accessing Non-Volatile Memory (e.g. EEPROM)   //
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
// 08.03.2001  Initial version                                                //
// 26.09.2002  Minor modifications, sample implementation                     //
// 01.09.2006  API re-work, modification of address assignment                //
// 12.11.2008  API update                                                     //
//                                                                            //
//============================================================================//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-03-17 16:42:11 +0100 (Mo, 17. Mär 2014) $
// SVN  $Rev: 5761 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  MC_NVM_H_
#define  MC_NVM_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "compiler.h"


//-----------------------------------------------------------------------------
/*!
** \file    mc_nvm.h
** \brief   MCL - access to non-volatile memory
**
** For data access to a non-volatile memory (e.g. EEPROM) a simple API with
** read and write functions is provided.
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
// the typedef for the address is used to obtain both - optimal
// performance on different architectures as well as a wide
// address range
//
#ifndef  MC_NVM_ADDR_SIZE
#define  MC_NVM_ADDR_SIZE              2
#endif


#if MC_NVM_ADDR_SIZE == 1
/*!
** \typedef    NvmAddr_tv
** \brief      EEPROM address
*/
typedef  uint8_t           NvmAddr_tv;

/*!
** \typedef    NvmAddr_tv
** \brief      EEPROM size
*/
typedef  uint8_t           NvmSize_tv;

#endif

#if MC_NVM_ADDR_SIZE == 2
/*!
** \typedef    NvmAddr_tv
** \brief      EEPROM address
*/
typedef  uint16_t           NvmAddr_tv;

/*!
** \typedef    NvmSize_tv
** \brief      EEPROM size
*/
typedef  uint16_t           NvmSize_tv;

#endif

#if MC_NVM_ADDR_SIZE == 4
/*!
** \typedef    NvmAddr_tv
** \brief      EEPROM address
*/
typedef  uint32_t           NvmAddr_tv;

/*!
** \typedef    NvmAddr_tv
** \brief      EEPROM size
*/
typedef  uint32_t           NvmSize_tv;

#endif

//-------------------------------------------------------------------
/*!
** \enum    NVM_ERR_e
** \brief   Error code for NVM access
**
** The following error codes are used by the NVM driver. The
** status can be checked via the function McNvmGetStatus().
*/
enum NVM_ERR_e {
   /*! Device is operational, success of latest operation      */
   eNVM_ERR_OK          = 0x00,

   /*! Hardware or initialisation failure of device            */
   eNVM_ERR_INIT        = 0x01,

   /*! Address out of range                                    */
   eNVM_ERR_ADDRESS     = 0x02,

   /*! Failure of read operation                               */
   eNVM_ERR_READ        = 0x04,

   /*! Failure of write operation                              */
   eNVM_ERR_WRITE       = 0x08,

   /*! Failure of erase operation                              */
   eNVM_ERR_ERASE       = 0x10,

   /*! Failure of checksum operation                           */
   eNVM_ERR_CHECKSUM    = 0x20,

   /*! Collision during read / write operation                 */
   eNVM_ERR_COLLISION   = 0x40
};



/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/



//-------------------------------------------------------------------
/*!
** \brief   Build Checksum
** \param   tvStartAddressV - First address for checksum algorithm
** \param   tvDataCountV - Number of data for checksum algorithm
** \return  checksum value
**
** This function builds a checksum by adding the data from the memory location
** given by tvStartAddressV to tvStartAddressV + tvDataCountV. The value of
** tvDataCountV must be greater than 0.
*/
uint16_t  McNvmBuildChecksum(NvmAddr_tv tvStartAddressV, NvmSize_tv tvDataCountV);


//-------------------------------------------------------------------
/*!
** \brief   Clear device status
** \see     McNvmGetStatus()
**
** This function clears the device status / operation status.
**
*/
void  McNvmClearStatus(void);


//-------------------------------------------------------------------
/*!
** \brief   Erase non-volatile memory
** \return  Status code
**
** This function erases the complete non-volatile memory. Possible
** return values are defined in the enumeration #NVM_ERR_e.
*/
Status_tv  McNvmEraseDevice(void);


//-------------------------------------------------------------------
/*!
** \brief   Get device status
** \return  Status code
**
** This function returns the device status / operation status. Possible
** return values are defined in the enumeration #NVM_ERR_e. The device
** status is cleared with the function McNvmClearStatus().
**
*/
Status_tv  McNvmGetStatus(void);


//-------------------------------------------------------------------
/*!
** \brief   Get storage capacity
** \return  Storage size
**
** This function returns the maximum storage capacity of the device. The return
** value is the number of memory locations, independent of the data storage
** capabilities (BYTE or WORD) of the device. The highest memory address is
** then McNvmGetMax() - 1.
**
*/
NvmSize_tv  McNvmGetSize(void);


//-------------------------------------------------------------------
/*!
** \brief   Initialise Non-Volatile Memory
** \return  Status code
**
** This function is responsible for initialisation of the non-volatile
** memory. It has to be called prior to all other functions for
** non-volatile memory access.
** <p>
** Possible return values are defined in the enumeration #NVM_ERR_e.
**
*/
Status_tv	McNvmInit(void);



//-------------------------------------------------------------------
/*!
** \brief   Read data
** \param   tvAddressV - Memory read address
** \param   pvdDataV - Pointer to data
** \param   tvSizeV - number of bytes to be read
** \return  Status code
**
** This function is used to read data from a given memory location tvAddressV.
** The first memory location is address 0. The address points to a byte value
** location.
** <p>
** Possible return values are defined in the enumeration #NVM_ERR_e.
**
*/
Status_tv  McNvmRead(NvmAddr_tv tvAddressV, void * pvdDataV, NvmSize_tv tvSizeV);


//-------------------------------------------------------------------
/*!
** \brief   Enable Write Operation
** \return  Status code
** \see     McNvmWriteDisable()
**
** This function enables write-operation to the non-volatile memory,
** which are disabled by default.
** <p>
** Possible return values are defined in the enumeration #NVM_ERR_e.
*/
Status_tv  McNvmWriteEnable(void);


//-------------------------------------------------------------------
/*!
** \brief   Disable Write Operation
** \return  Status code
** \see     McNvmWriteEnable()
**
** This function disables write-operation on the non-volatile memory.
** <p>
** Possible return values are defined in the enumeration #NVM_ERR_e.
*/
Status_tv  McNvmWriteDisable(void);


//-------------------------------------------------------------------
/*!
** \brief   Write data
** \param   tvAddressV - Memory write address
** \param   pvdDataV - Pointer to data
** \param   tvSizeV - number of bytes to be stored
** \return  Status code
**
** This function is used to write data to a given memory location
** \a tvAddressV. The first memory location is address 0.
** <p>
** Possible return values are defined in the enumeration #NVM_ERR_e.
**
*/
Status_tv  McNvmWrite(NvmAddr_tv tvAddressV, void * pvdDataV, NvmSize_tv tvSizeV);




//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//


#endif   // MC_NVM_H_

