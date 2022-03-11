//****************************************************************************//
// File:          cos_mobj.c                                                  //
// Description:   Example for manufacturer specific objects                   //
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
// 11.03.2002  Initial version                                                //
// 05.02.2003  Changed callback functions (return value)                      //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 17:09:58 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5697 $ --- $Author: koppe $
//------------------------------------------------------------------------------



/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_mobj.h"

#include "cos_emcy.h"      // include EMCY services
#include "cos_nvm.h"       // include NVM memory access
#include "cos_sdo.h"       // include SDO services



/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  MOB2000_SIZE   5

//-------------------------------------------------------------------
// these values are accessed via SDO callbacks, they are not
// mapped in a PDO
//
static uint16_t auwArray2000S[MOB2000_SIZE];
static uint8_t ubValue2001S;

//-------------------------------------------------------------------
// The values are accessed directly via a SDO. They may be mapped
// in a SDO.
//
uint8_t  ubCosMob_Var2002G;
uint16_t uwCosMob_Var2003G;
uint32_t ulCosMob_Var2004G;

#if CPP_DATA_SIZE >= 64
uint64_t uqCosMob_Var2005G;
#endif

static char szCosMob_Str2007S[24] = "Callback SDO Test";
static uint8_t ubCosMob_Size2007S    = 17;

char        szCosMob_Str2008G[64];


uint8_t ubSdoResponseG;





/*----------------------------------------------------------------------------*\
** Extern Variables                                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/




//----------------------------------------------------------------------------//
// CosMob_ParmInit()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosMob_ParmInit(void)
{
   uint8_t  ubCntT;

   //----------------------------------------------------------------
   // initialise variables
   //
   ubValue2001S      = 15;

   ubCosMob_Var2002G = 0x01;
   uwCosMob_Var2003G = 0x2345;
   ulCosMob_Var2004G = 0x6789ABCD;

   #if CPP_DATA_SIZE >= 64
   uqCosMob_Var2005G = 0x123456789ABCDEF0LL;
   #endif

   for(ubCntT = 0; ubCntT < MOB2000_SIZE; ubCntT++)
   {
      auwArray2000S[ubCntT] = 0;
   }



}


//----------------------------------------------------------------------------//
// CosMob_ParmLoad()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CosMob_ParmLoad(void)
{
   //----------------------------------------------------------------
   // read manufacturer objects from EEPROM
   //
   /*
   McNvmRead(eNVM_OBJ_2000, &uwMyVariableS, 2);
   */


   return(eCosErr_OK);
}


//----------------------------------------------------------------------------//
// CosMob_ParmSave()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CosMob_ParmSave(void)
{
   //----------------------------------------------------------------
   // write manufacturer objects to EEPROM
   //
   /*
   uint16_t  uwValueT = uwMyVariableS;


   McNvmWrite(eNVM_OBJ_2000, &uwValueT, 2);
   */

   return(eCosErr_OK);
}



//----------------------------------------------------------------------------//
// CosMob_Idx2000()                                                           //
// example for read/write access to an array of values                        //
//----------------------------------------------------------------------------//
uint8_t CosMob_Idx2000(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint8_t  ubHandlerCodeT;
   uint8_t  ubSubIdx0T = MOB2000_SIZE;
   uint16_t  uwValueT;


   //----------------------------------------------------------------
   // test the maximum sub-index
   //
   if(ubSubIndexV > MOB2000_SIZE)
   {
      //---------------------------------------------------
      // return here, we are out of the bounds of the
      // object
      //
      return(eCosSdo_ERR_NO_SUB_INDEX);
   }


   //----------------------------------------------------------------
   // read access
   //
   if(ubReqCodeV == eSDO_READ_REQ)
   {
      if(ubSubIndexV == 0)
      {
         CosSdoCopyValueToMessage(  (void *) &ubSubIdx0T, CoDT_UNSIGNED8);
         ubHandlerCodeT = eCosSdo_READ1_OK;
      }
      else
      {
         uwValueT = auwArray2000S[ubSubIndexV - 1];
         CosSdoCopyValueToMessage(  (void *) &uwValueT, CoDT_UNSIGNED16);
         ubHandlerCodeT = eCosSdo_READ2_OK;
      }

   }
   //----------------------------------------------------------------
   // write access
   //
   else if( (ubReqCodeV == eSDO_WRITE_REQ_0)||(ubReqCodeV == eSDO_WRITE_REQ_2) )
   {
      switch(ubSubIndexV)
      {
         case 0x00:
            //---------------------------------------------
            // no write access to sub-index 0
            //
            ubHandlerCodeT = eCosSdo_ERR_ACCESS_RO;
            break;

         default:
            CosSdoCopyMessageToValue(  (void *) &uwValueT, CoDT_UNSIGNED16);
            auwArray2000S[ubSubIndexV - 1] = uwValueT;
            ubHandlerCodeT = eCosSdo_WRITE_OK;
            break;
      }
   }

    //----------------------------------------------------------------
   // wrong access code
   //
   else
   {
      ubHandlerCodeT = eCosSdo_ERR_DATATYPE;
   }

   return(ubHandlerCodeT);

}


//----------------------------------------------------------------------------//
// CosMob_Idx2001()                                                           //
// example for read/write access to value with limit check                    //
//----------------------------------------------------------------------------//
uint8_t CosMob_Idx2001(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint8_t  ubHandlerCodeT;
   uint8_t  ubValueT;


   //----------------------------------------------------------------
   // test the maximum sub-index
   //
   if(ubSubIndexV > 0)
   {
      //---------------------------------------------------
      // return here, we are out of the bounds of the
      // object
      //
      return(eCosSdo_ERR_NO_SUB_INDEX);
   }


   //----------------------------------------------------------------
   // read access
   //
   if(ubReqCodeV == eSDO_READ_REQ)
   {
      CosSdoCopyValueToMessage(  (void *) &ubValue2001S, CoDT_UNSIGNED8);
      ubHandlerCodeT = eCosSdo_READ1_OK;
   }

   //----------------------------------------------------------------
   // write access
   //
   else if( (ubReqCodeV == eSDO_WRITE_REQ_0)||(ubReqCodeV == eSDO_WRITE_REQ_1) )
   {
      CosSdoCopyMessageToValue(  (void *) &ubValueT, CoDT_UNSIGNED8);

      //---------------------------------------------------
      // in this example the valid value range is 10 .. 20
      //
      if(ubValueT > 20)
      {
         ubHandlerCodeT = eCosSdo_ERR_VALUE_HIGH;
      }
      else if(ubValueT < 10)
      {
         ubHandlerCodeT = eCosSdo_ERR_VALUE_LOW;
      }
      else
      {
         ubValue2001S = ubValueT;
         ubHandlerCodeT = eCosSdo_WRITE_OK;
      }
   }

   //----------------------------------------------------------------
   // wrong access code
   //
   else
   {
      ubHandlerCodeT = eCosSdo_ERR_DATATYPE;
   }

   return(ubHandlerCodeT);
}


//----------------------------------------------------------------------------//
// CosMob_Idx2006()                                                           //
// example for read/write access with deferred response                       //
//----------------------------------------------------------------------------//
uint8_t CosMob_Idx2006(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint8_t  ubHandlerCodeT;

   //----------------------------------------------------------------
   // test the maximum sub-index
   //
   if(ubSubIndexV > 0)
   {
      //---------------------------------------------------
      // return here, we are out of the bounds of the
      // object
      //
      return(eCosSdo_ERR_NO_SUB_INDEX);
   }


   //----------------------------------------------------------------
   // read access
   //
   if(ubReqCodeV == eSDO_READ_REQ)
   {
      ubHandlerCodeT = eCosSdo_ERR_DEFER_RESPONSE;
      ubSdoResponseG = 1;
   }

   //----------------------------------------------------------------
   // write access
   //
   else if( (ubReqCodeV == eSDO_WRITE_REQ_0)||(ubReqCodeV == eSDO_WRITE_REQ_1) )
   {
      ubHandlerCodeT = eCosSdo_ERR_DEFER_RESPONSE;
      ubSdoResponseG = 2;
   }

   //----------------------------------------------------------------
   // wrong access code
   //
   else
   {
      ubHandlerCodeT = eCosSdo_ERR_DATATYPE;
   }

   return(ubHandlerCodeT);
}


//----------------------------------------------------------------------------//
// CosMob_Idx2007()                                                           //
// example for read/write access to domain data                               //
//----------------------------------------------------------------------------//
uint8_t  CosMob_Idx2007(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint32_t  ulDataSizeT;
   uint8_t   ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;

   //----------------------------------------------------------------
   // test the maximum sub-index
   //
   if(ubSubIndexV > 0)
   {
      //---------------------------------------------------
      // return here, we are out of the bounds of the
      // object
      //
      return(ubHandlerCodeT);
   }


   //----------------------------------------------------------------
   // read access
   //
   if(ubReqCodeV == eSDO_READ_REQ)
   {
      //---------------------------------------------------
      // setup the data size to be transferred
      //
      ulDataSizeT = ubCosMob_Size2007S;

      //---------------------------------------------------
      // setup segmented SDO handler with data size
      // and pointer to data
      //
      CosSdoSegSetup(&szCosMob_Str2007S[0], ulDataSizeT);

      //---------------------------------------------------
      // inform SDO client about data size
      //
      CosSdoCopyValueToMessage(&ulDataSizeT, CoDT_INTEGER32);
      ubHandlerCodeT = eCosSdo_READ_SEG_OK;
   }
   //----------------------------------------------------------------
   // write access
   //
   else if(ubReqCodeV == eSDO_WRITE_REQ_SEG)
   {
      //--------------------------------------------------------
      // get the data size of the data to be uploaded
      //
      CosSdoCopyMessageToValue(&ulDataSizeT, CoDT_INTEGER32);

      //--------------------------------------------------------
      // check the data size and return an error if too large
      //
      if(ulDataSizeT > 24)
      {
         ubHandlerCodeT = eCosSdo_ERR_VALUE_RANGE;
      }
      else
      {
         //--------------------------------------------------------
         // the data size is OK, setup segmented SDO handler
         //
         ubCosMob_Size2007S = (uint8_t) ulDataSizeT;
         CosSdoSegSetup(&szCosMob_Str2007S, ulDataSizeT);
         ubHandlerCodeT = eCosSdo_WRITE_OK;
      }

   }
   //----------------------------------------------------------------
   // all data has been written by client, now check the data here
   //
   else if(ubReqCodeV == eSDO_WRITE_RESP_SEG)
   {
      //--------------------------------------------------------
      // don't allow the 'A' at the beginning for this
      // example
      //
      if(szCosMob_Str2007S[0] == 'A')
      {
         ubHandlerCodeT = eCosSdo_ERR_VALUE_RANGE;
      }
      else
      //--------------------------------------------------------
      // everything is fine: data is accepted
      //

      {
         ubHandlerCodeT = eCosSdo_WRITE_OK;
      }
   }
   return(ubHandlerCodeT);

}
