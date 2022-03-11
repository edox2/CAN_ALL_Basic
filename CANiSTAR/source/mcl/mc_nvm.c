//****************************************************************************//
// File:          mc_nvm.c                                                    //
// Description:   Functions for accessing an EEPROM                           //
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
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 16:48:44 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5696 $ --- $Author: koppe $
//------------------------------------------------------------------------------


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
// The MC_NVM_ADDR_PTR symbol defines the size of the address
// pointer:
// 1 = Byte value, range 0 .. 2^8 - 1
// 2 = Word value, range 0 .. 2^16 - 1
// 4 = Long value, range 0 .. 2^32 - 1
//
// The default is the word value range (16 bit).
//
#define  MC_NVM_ADDR_PTR      2


//-------------------------------------------------------------------
// maximum address of device (example)
//
#define  MC_NVM_ADDR_MAX      0xFF


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "mc_nvm.h"
#include <SI_C8051F550_Defs.h>
#include <SI_C8051F550_Register_Enums.h>
#include "F560_FlashPrimitives.h"
#include "F560_FlashUtils.h"



/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

static Status_tv  tvMcNvmStatusS;           // status of EEPROM



/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// McNvmBuildChecksum()                                                       //
// Build checksum over given address range                                    //
//----------------------------------------------------------------------------//
uint16_t  McNvmBuildChecksum( NvmAddr_tv tvStartAddressV,
                              NvmSize_tv tvDataCountV)
{
   register uint16_t  uwChecksumT = 0;
   uint16_t           uwValueT    = 0;


   //----------------------------------------------------------------
   // check the start address
   //
   if(tvStartAddressV % 2)
   {
      tvMcNvmStatusS = -eNVM_ERR_ADDRESS;
      return(uwChecksumT);
   }


   //----------------------------------------------------------------
   // the tvDataCountV parameter is given in bytes, but we access
   // a word here: the data count variable is divided by 2,
   // the start address must be an even address
   //
   tvDataCountV = tvDataCountV >> 1;
   while(tvDataCountV)
   {
      McNvmRead(tvStartAddressV, &uwValueT, 2);
      uwChecksumT = uwChecksumT ^ uwValueT;
      uwChecksumT = uwChecksumT + 5;
      tvStartAddressV = tvStartAddressV + 2;
      tvDataCountV--;
   }

   //----------------------------------------------------------------
   // operation success
   //
   tvMcNvmStatusS = eNVM_ERR_OK;

   return(uwChecksumT);
}




//----------------------------------------------------------------------------//
// McNvmEraseDevice()                                                         //
// Erase the complete memory                                                  //
//----------------------------------------------------------------------------//
Status_tv  McNvmEraseDevice(void)
{
   //----------------------------------------------------------------
   // operation success
   //
   tvMcNvmStatusS = eNVM_ERR_OK;

   return(tvMcNvmStatusS);
}


//----------------------------------------------------------------------------//
// McNvmGetStatus()                                                           //
// Return device / operation status                                           //
//----------------------------------------------------------------------------//
Status_tv  McNvmGetStatus(void)
{
   return(tvMcNvmStatusS);
}


//----------------------------------------------------------------------------//
// McNvmGetStorageMax()                                                       //
// Maximum storage capacity                                                   //
//----------------------------------------------------------------------------//
uint16_t  McNvmGetMax(void)
{
   return(MC_NVM_ADDR_MAX + 1);
}


//----------------------------------------------------------------------------//
// McNvmInit()                                                                //
// Initialise the driver                                                      //
//----------------------------------------------------------------------------//
Status_tv  McNvmInit(void)
{
   //----------------------------------------------------------------
   // disable chip-select of EEPROM (active high)
   //



   //----------------------------------------------------------------
   // clear data and clock lines
   //



   //----------------------------------------------------------------
   // clear device status
   //
   tvMcNvmStatusS = eNVM_ERR_OK;

   return(tvMcNvmStatusS);
}

//----------------------------------------------------------------------------//
// McNvmRead()                                                                //
// read tvSizeV bytes from the supplied address                               //
//----------------------------------------------------------------------------//
Status_tv McNvmRead(NvmAddr_tv tvAddressV, void * pvdDataV, NvmSize_tv tvSizeV)
{
   uint8_t *pubDataT;

   //----------------------------------------------------------------
   // check address value
   //
   if(tvAddressV + tvSizeV > MC_NVM_ADDR_MAX)
   {
      tvMcNvmStatusS = -eNVM_ERR_ADDRESS;
      return(tvMcNvmStatusS);
   }

   //----------------------------------------------------------------
   // read value from EEPROM
   //
   //
   pubDataT = pvdDataV;
   while(tvSizeV)
   {
	   (*pubDataT) = FLASH_ByteRead (tvAddressV);

//	   (*pubDataT) = 0x00;  // set to 0x00 in sample code
      pubDataT++;
      tvSizeV--;
   }


   //----------------------------------------------------------------
   // operation success
   //
   tvMcNvmStatusS = eNVM_ERR_OK;

   return(tvMcNvmStatusS);
}


//----------------------------------------------------------------------------//
// McNvmWrite()                                                               //
// Write data to given address                                                //
//----------------------------------------------------------------------------//
Status_tv McNvmWrite(NvmAddr_tv tvAddressV, void * pvdDataV,
                     NvmSize_tv tvSizeV)
{
   uint8_t *pubDataT;
   uint16_t tvAddressT = tvAddressV;

   //----------------------------------------------------------------
   // check address value
   //
   if(tvAddressV + tvSizeV > MC_NVM_ADDR_MAX)
   {
      tvMcNvmStatusS = -eNVM_ERR_ADDRESS;
      return(tvMcNvmStatusS);
   }


   //----------------------------------------------------------------
   // write data to EEPROM
   //
   //
   pubDataT = pvdDataV;
   while(tvSizeV)
   {
      //---------------------------------------------------
      //---------------------------------------------------
	  // read data from EEPROM here
	  //

	  FLASH_ByteWrite( tvAddressT,* pubDataT);

	  tvAddressT++;
	  pubDataT++;
      tvSizeV--;
   }


   //--- operation success ------------------------------------------
   tvMcNvmStatusS = eNVM_ERR_OK;

   return(tvMcNvmStatusS);
}




//----------------------------------------------------------------------------//
// McNvmWriteEnable()                                                         //
// Enable Write / Erase operations                                            //
//----------------------------------------------------------------------------//
Status_tv  McNvmWriteEnable(void)
{

   //----------------------------------------------------------------
   // enable chip-select of EEPROM
   //


   //----------------------------------------------------------------
   // write command byte to the EEPROM
   //


   //----------------------------------------------------------------
   // disable chip-select of EEPROM
   //


   //----------------------------------------------------------------
   // clear device status
   //
   tvMcNvmStatusS = eNVM_ERR_OK;
   return(tvMcNvmStatusS);
}


//----------------------------------------------------------------------------//
// McNvmWriteDisable()                                                        //
// Disable Write / Erase operations                                           //
//----------------------------------------------------------------------------//
Status_tv  McNvmWriteDisable(void)
{
   //----------------------------------------------------------------
   // enable chip-select of EEPROM
   //


   //----------------------------------------------------------------
   // write command byte to the EEPROM
   //


   //----------------------------------------------------------------
   // disable chip-select of EEPROM
   //


   //----------------------------------------------------------------
   // clear device status
   //
   tvMcNvmStatusS = eNVM_ERR_OK;
   return(tvMcNvmStatusS);
}



