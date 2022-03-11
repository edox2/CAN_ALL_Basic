//****************************************************************************//
// File:          cos301.c                                                    //
// Description:   Objects from CiA 301                                        //
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
// 29.06.2000  Initial version                                                //
// 10.11.2000  Added Online-Documentation for Doxygen                         //
// 29.01.2001  Moved several variables to cos_user.c                          //
// 30.10.2002  Changed callback structure to reduce code size                 //
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

#include "cos301.h"

#include "cos_emcy.h"            // Emergency service
#include "cos_mgr.h"             // CANopen Slave management functions
#include "cos_nmt.h"             // NMT service
#include "cos_nvm.h"             // Non-volatile memory address map
#include "cos_pdo.h"             // PDO service
#include "cos_sdo.h"             // SDO service
#include "cos_sync.h"            // SYNC service
#include "cos_time.h"            // time functions


/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/
uint8_t     ubIdx1001_ErrorRegisterG;     // error register
uint32_t    ulIdx1002_StatusRegisterG;    // status register

#if COS_DICT_OBJ_1010 > 0
uint8_t     ubCos301ParmSaveG;            // requested save operation
uint8_t     ubCos301ParmLoadG;            // requested load operation
#endif


#if COS_DICT_OBJ_1016 != 0
extern uint8_t    aubCosNmtHbConsNodeG[];
extern uint16_t   auwCosNmtHbConsTimeG[];
#endif

#if COS_DICT_OBJ_1020 == 1
static uint32_t   ulIdx1020_DateS;
static uint32_t   ulIdx1020_TimeS;
#endif

#if COS_DICT_OBJ_1021 > 0
uint8_t                 ubIdx1022_EdsTypeC;
extern const uint32_t   ulCosEdsSizeC;
extern const uint8_t    aubCosEdsFileC[];
#endif

#if COS_PDO_MAPPING > 0
#if COS_PDO_RCV_NUMBER > 0
extern CosPdoDynMapParm_ts atsRcvPdoDynMapParmG[];
#endif
#if COS_PDO_TRM_NUMBER > 0
extern CosPdoDynMapParm_ts atsTrmPdoDynMapParmG[];
#endif
#endif


/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// Cos301_ClearVerifyConfiguration()                                          //
// clear the verify configuration values                                      //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1020 == 1
void Cos301_ClearVerifyConfiguration(void)
{
   ulIdx1020_DateS = 0;
   ulIdx1020_TimeS = 0;
}
#endif


//----------------------------------------------------------------------------//
// Cos301_Idx1010()                                                           //
// store parameters                                                           //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1010 != 0
uint8_t Cos301_Idx1010(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint8_t  ubHandlerCodeT;
   uint8_t  ubLargestSubIndexT;
   uint32_t  ulValueT;

   //----------------------------------------------------------------
   // ubLargestSubIndexT must be set here according
   // to device capabilities
   //
   ubLargestSubIndexT = COS_DICT_OBJ_1010;

   //=== Todo: insert application specific code =====================
   //
   // the variable ulValueT must have the following
   // signature on write operation:
   // 65h,76h,61h,73h (evas in ASCII)
   // on read operation it defines the possibilities
   // of the device:
   // bit 0: 0 = device does not save param. on command
   //        1 = device saves param. on command
   // bit 1: 0 = device does not save param. autonom.
   //        1 = device saves param autonomously
   //
   ulValueT = 0x00000001;  // save on command

   //=== End of Todo ================================================


   //----------------------------------------------------------------
   // read access
   //
   if(ubReqCodeV == eSDO_READ_REQ)
   {

      //---------------------------------------------------
      // check the sub-index and respond to SDO request
      //
      if(ubSubIndexV == 0)
      {
         CosSdoCopyValueToMessage(  (void *) &ubLargestSubIndexT,
                                    CoDT_UNSIGNED8);
         ubHandlerCodeT = eCosSdo_READ1_OK;
      }
      else if(ubSubIndexV <= ubLargestSubIndexT)
      {
         CosSdoCopyValueToMessage(  (void *) &ulValueT, CoDT_UNSIGNED32);
         ubHandlerCodeT = eCosSdo_READ4_OK;
      }
      else
      {
         ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;
      }


   }
   //----------------------------------------------------------------
   // write access
   //
   else if ((ubReqCodeV == eSDO_WRITE_REQ_0) ||
            (ubReqCodeV == eSDO_WRITE_REQ_4))
   {
      //--------------------------------------------------------
      // check the sub-index and respond to SDO request
      //

      //--- sub-index 0 ----------------------------------------
      if(ubSubIndexV == 0)
      {
         ubHandlerCodeT = eCosSdo_ERR_ACCESS_RO;
      }
      //--- sub-index 1 - COS_DICT_OBJ_1010 --------------------
      else if(ubSubIndexV <= ubLargestSubIndexT)
      {
         //--- read value from SDO message ----------------
         CosSdoCopyMessageToValue( (void *) &ulValueT, CoDT_UNSIGNED32);

         //------------------------------------------------
         // check signature
         if(ulValueT == 0x65766173)
         {
            //---------------------------------------------
            // test if save operation is in progress
            //
            if(ubCos301ParmSaveG & 0x80)
            {
               ubHandlerCodeT = eCosSdo_ERR_DATA_STORE_STATE;
            }
            //---------------------------------------------
            // set variable that denotes the store
            // operation, the enumeration COS_PARM_e
            // complies with the sub-index
            //
            else
            {
               ubCos301ParmSaveG = ubSubIndexV;
               ubHandlerCodeT = eCosSdo_ERR_DEFER_RESPONSE;
            }
         }
         else
         {
            ubHandlerCodeT = eCosSdo_ERR_DATA_STORE;
         }
         // end of signature check
         //------------------------------------------------

      }
      //--- unsupported sub-index ------------------------------
      else
      {
         ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;
      }

   }
   else
   {
      ubHandlerCodeT = eCosSdo_ERR_DATATYPE;
   }

   return(ubHandlerCodeT);
}
#endif


//----------------------------------------------------------------------------//
// Cos301_Idx1011()                                                           //
// restore parameters                                                         //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1011 != 0
uint8_t  Cos301_Idx1011(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint8_t  ubHandlerCodeT;
   uint8_t  ubLargestSubIndexT;
   uint32_t  ulValueT;

   //----------------------------------------------------------------
   // ubLargestSubIndexT must be set here according
   // to device capabilities
   //
   ubLargestSubIndexT = COS_DICT_OBJ_1011;

   //=== Todo: insert application specific code =====================
   //
   // the variable ulValueT must have the following
   // signature on write operation:
   // 64h,61h,6Fh,6Ch (daol in ASCII)
   // on read operation it defines the possibilities
   // of the device:
   // bit 0: 0 = device does not restore param. on command
   //        1 = device restores param. on command
   //
   ulValueT = 0x00000001;  // restore on command

   //
   //=== End of Todo ================================================

   //----------------------------------------------------------------
   // read access
   //
   if(ubReqCodeV == eSDO_READ_REQ)
   {

      //---------------------------------------------------
      // check the sub-index and respond to SDO request
      //
      if(ubSubIndexV == 0)
      {
         CosSdoCopyValueToMessage(  (void *) &ubLargestSubIndexT,
                                    CoDT_UNSIGNED8);
         ubHandlerCodeT = eCosSdo_READ1_OK;
      }
      else if(ubSubIndexV <= ubLargestSubIndexT)
      {
         CosSdoCopyValueToMessage(  (void *) &ulValueT, CoDT_UNSIGNED32);
         ubHandlerCodeT = eCosSdo_READ4_OK;
      }
      else
      {
         ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;
      }

   }
   //----------------------------------------------------------------
   // write access
   //
   else if ((ubReqCodeV == eSDO_WRITE_REQ_0) ||
            (ubReqCodeV == eSDO_WRITE_REQ_4))
   {
      //--------------------------------------------------------
      // check the sub-index and respond to SDO request
      //

      //--- sub-index 0 ----------------------------------------
      if(ubSubIndexV == 0)
      {
         ubHandlerCodeT = eCosSdo_ERR_ACCESS_RO;
      }
      //--- sub-index 1 - COS_DICT_OBJ_1011 --------------------
      else if(ubSubIndexV <= ubLargestSubIndexT)
      {
         //--- read value from SDO message ----------------
         CosSdoCopyMessageToValue( (void *) &ulValueT, CoDT_UNSIGNED32);

         //------------------------------------------------
         // check signature
         if(ulValueT == 0x64616F6C)
         {
            //---------------------------------------------
            // set variable that denotes the restore
            // operation, the enumeration COS_PARAMETER_e
            // complies with the sub-index
            //
            ubCos301ParmLoadG = ubSubIndexV;
            ubHandlerCodeT = eCosSdo_ERR_DEFER_RESPONSE;

         }
         else
         {
            ubHandlerCodeT = eCosSdo_ERR_DATA_STORE;
         }
         // end of signature check
         //------------------------------------------------

      }
      //--- unsupported sub-index ------------------------------
      else
      {
         ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;
      }

   }
   else
   {
      ubHandlerCodeT = eCosSdo_ERR_DATATYPE;
   }
   return(ubHandlerCodeT);

}
#endif


//----------------------------------------------------------------------------//
// Cos301_Idx1018()                                                           //
// identity object                                                            //
//----------------------------------------------------------------------------//
uint8_t Cos301_Idx1018(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint8_t  ubHandlerCodeT;
   uint32_t  ulSerNumT;


   //--- only read access -------------------------------------------
   if (ubReqCodeV != eSDO_READ_REQ)
   {
      ubHandlerCodeT = eCosSdo_ERR_ACCESS_RO;
      return(ubHandlerCodeT);
   }


   //--- test the sub-index -----------------------------------------
   switch (ubSubIndexV)
   {
      case 0:
         CosSdoCopyValueToMessage( (void *)&ubIdx1018_SubNumberC, CoDT_UNSIGNED8);
         ubHandlerCodeT = eCosSdo_READ1_OK;
         break;

      case 1:
         CosSdoCopyValueToMessage( (void *)&ulIdx1018_VendorIdC,
                                    CoDT_UNSIGNED32);
         ubHandlerCodeT = eCosSdo_READ4_OK;
         break;

      case 2:
         CosSdoCopyValueToMessage( (void *)&ulIdx1018_ProductCodeC,
                                    CoDT_UNSIGNED32);
         ubHandlerCodeT = eCosSdo_READ4_OK;
         break;

      case 3:
         CosSdoCopyValueToMessage( (void *)&ulIdx1018_RevisionNumC,
                                    CoDT_UNSIGNED32);
         ubHandlerCodeT = eCosSdo_READ4_OK;
         break;

      case 4:
         ulSerNumT = CosMgrGetSerialNumber();
         CosSdoCopyValueToMessage( (void *)&ulSerNumT, CoDT_UNSIGNED32);
         ubHandlerCodeT = eCosSdo_READ4_OK;
         break;

      default:
         ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;
         break;
   }

   return(ubHandlerCodeT);
}


//----------------------------------------------------------------------------//
// Cos301_Idx1020()                                                           //
// verify configuration                                                       //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1020 > 0
uint8_t Cos301_Idx1020(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint8_t  ubHandlerCodeT;
   uint8_t  ubSubNumberT = 2;

   //----------------------------------------------------------------
   // read access
   //
   if(ubReqCodeV == eSDO_READ_REQ)
   {
      switch (ubSubIndexV)
      {
         case 0:
            CosSdoCopyValueToMessage( (void *)&ubSubNumberT, CoDT_UNSIGNED8);
            ubHandlerCodeT = eCosSdo_READ1_OK;
            break;

         case 1:
            CosSdoCopyValueToMessage( (void *)&ulIdx1020_DateS,
                                       CoDT_UNSIGNED32);
            ubHandlerCodeT = eCosSdo_READ4_OK;
            break;

         case 2:
            CosSdoCopyValueToMessage( (void *)&ulIdx1020_TimeS,
                                       CoDT_UNSIGNED32);
            ubHandlerCodeT = eCosSdo_READ4_OK;
            break;

         default:
            ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;
            break;
      }
   }

   //----------------------------------------------------------------
   // write access:
   //
   else if( (ubReqCodeV == eSDO_WRITE_REQ_0) || (ubReqCodeV == eSDO_WRITE_REQ_4))
   {
      switch (ubSubIndexV)
      {
         case 0:
            ubHandlerCodeT = eCosSdo_ERR_ACCESS_RO;
            break;

         case 1:
            CosSdoCopyMessageToValue( (void *)&ulIdx1020_DateS,
                                       CoDT_UNSIGNED32);
            ubHandlerCodeT = eCosSdo_WRITE_OK;
            break;

         case 2:
            CosSdoCopyMessageToValue( (void *)&ulIdx1020_TimeS,
                                       CoDT_UNSIGNED32);
            ubHandlerCodeT = eCosSdo_WRITE_OK;
            break;

         default:
            ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;
            break;
      }
   }

   //----------------------------------------------------------------
   // wrong command specifier
   //
   else
   {
      ubHandlerCodeT = eCosSdo_ERR_DATATYPE;
   }

   return(ubHandlerCodeT);
}
#endif


//----------------------------------------------------------------------------//
// Cos301_Idx1021()                                                           //
// access to EDS                                                              //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1021 > 0
uint8_t  Cos301_Idx1021(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint32_t  ulDataSizeT;
   uint8_t * pubEdsFileT;
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
      ulDataSizeT = ulCosEdsSizeC;

      //---------------------------------------------------
      // setup segmented SDO handler with data size
      // and pointer to data
      //
      pubEdsFileT = (uint8_t *) &(aubCosEdsFileC[0]);
      CosSdoSegSetup(pubEdsFileT, ulDataSizeT);

      //---------------------------------------------------
      // inform SDO client about data size
      //
      CosSdoCopyValueToMessage(&ulDataSizeT, CoDT_INTEGER32);
      ubHandlerCodeT = eCosSdo_READ_SEG_OK;
   }
   //----------------------------------------------------------------
   // other commands are not allowed
   //
   else
   {
      ubHandlerCodeT = eCosSdo_ERR_COMMAND;
   }
   return(ubHandlerCodeT);

}
#endif


//----------------------------------------------------------------------------//
// Cos301_ParmInit()                                                          //
// Initialise objects of DS-301 (index 1000h ... 1FFFh)                       //
//----------------------------------------------------------------------------//
void Cos301_ParmInit(void)
{
   #if (COS_DICT_OBJ_1016 > 0) || (COS_PDO_SUPPORT > 0)
   uint8_t  ubObjCntT;
   #endif
   #if (COS_PDO_SUPPORT > 0)
   uint16_t  uwIdentifierT;
   #endif

   ubIdx1001_ErrorRegisterG  = 0x00;         // global error register
   ulIdx1002_StatusRegisterG = 0x00000000;   // global status register


   //----------------------------------------------------------------
   // SYNC default identifier
   #if COS_SYNC_SUPPORT == 1
   ulCosSyncIdentifierG = 0x80;
   #endif

   //----------------------------------------------------------------
   // SYNC default cycle time
   //
   #if COS_DICT_OBJ_1006 > 0
   uwCosSyncCycleTimeG = 0;
   #endif

   //----------------------------------------------------------------
   // SYNC default counter
   //
   #if COS_DICT_OBJ_1019 > 0
   ubCosSyncCntMaxG = 0;
   #endif


   //----------------------------------------------------------------
   // EMCY default identifier
   //
   ulCosEmcyIdentifierG = ID_BASE_EMCY + ubCosMgrNodeAddressG;

   //----------------------------------------------------------------
   // EMCY inhibit time
   //
   #if COS_DICT_OBJ_1015 > 0
   uwCosEmcyInhibitTimeG = 0;
   #endif

   //----------------------------------------------------------------
   // TIME default identifier
   //
   #if COS_DICT_OBJ_1012 > 0
   ulCosTimeIdentifierG = 0x80000100;
   #endif

   //----------------------------------------------------------------
   // node guarding
   //
   #if COS_DICT_OBJ_100C == 1
   uwCosNmtGuardTimeG         = 0;
   ubCosNmtGuardFactorG       = 0;
   #endif


   //----------------------------------------------------------------
   // heartbeat consumer time
   //
   #if COS_DICT_OBJ_1016 > 0
   for(ubObjCntT = 0; ubObjCntT < COS_DICT_OBJ_1016; ubObjCntT++)
   {
      auwCosNmtHbConsTimeG[ubObjCntT] = 0x0000;
      aubCosNmtHbConsNodeG[ubObjCntT] = 0x00;
   }
   #endif


   //----------------------------------------------------------------
   // heartbeat time
   //
   uwCosNmtHeartTimeG = 0;

   //----------------------------------------------------------------
   // verify configuration, object 1020h
   //
   #if COS_DICT_OBJ_1020 == 1
   ulIdx1020_DateS = 0;
   ulIdx1020_TimeS = 0;
   #endif

   //----------------------------------------------------------------
   // EDS store format, object 1022h
   //
   #if   COS_DICT_OBJ_1021 == 1
   ubIdx1022_EdsTypeC = 0x00;
   #elif COS_DICT_OBJ_1021 == 2
   ubIdx1022_EdsTypeC = 0x80;
   #endif


   //----------------------------------------------------------------
   // error behaviour, object 1029
   //
   #if COS_DICT_OBJ_1029 > 0
   for(ubObjCntT = 0; ubObjCntT < COS_DICT_OBJ_1029; ubObjCntT++)
   {
      aubCosNmtErrorBehaviorG[ubObjCntT] = 0;
   }
   #endif


   //----------------------------------------------------------------
   // startup behaviour, object 1F80
   //
   #if COS_DICT_OBJ_1F80 > 0
   ubCosNmtStartupG = 0;
   #endif

   //----------------------------------------------------------------
   // Transmit PDO communication parameter
   //
   #if COS_PDO_TRM_NUMBER > 0
   for(ubObjCntT = 0; ubObjCntT < COS_PDO_TRM_NUMBER; ubObjCntT++)
   {
      uwIdentifierT = (0x0100 * ubObjCntT);
      uwIdentifierT = uwIdentifierT + ID_BASE_PDO1_TX + ubCosMgrNodeAddressG;

      atsTrmPdoComG[ubObjCntT].ulIdentifier     = 0x40000000 | uwIdentifierT;
      atsTrmPdoComG[ubObjCntT].ubTransType      = 0xFF;
      atsTrmPdoComG[ubObjCntT].ubSyncCount      = 0x00;

      #if COS_PDO_INHIBIT == 1
      atsTrmPdoComG[ubObjCntT].uwInhibitTime    = 0x0000;
      atsTrmPdoComG[ubObjCntT].uwInhibitTick    = 0x0000;
      #endif

      atsTrmPdoComG[ubObjCntT].uwEventTime      = 0x0000;
      atsTrmPdoComG[ubObjCntT].uwEventTick      = 0x0000;

      #if COS_PDO_SYNC_START == 1
      atsTrmPdoComG[ubObjCntT].ubSyncStartValue = 0;
      atsTrmPdoComG[ubObjCntT].ubSyncStartFlag  = 0;
      #endif
   }
   #endif


   //--- Receive PDO communication parameter ----------------
   #if COS_PDO_RCV_NUMBER > 0
   for(ubObjCntT = 0; ubObjCntT < COS_PDO_RCV_NUMBER; ubObjCntT++)
   {
      uwIdentifierT = (0x0100 * ubObjCntT);
      uwIdentifierT = uwIdentifierT + ID_BASE_PDO1_RX + ubCosMgrNodeAddressG;

      atsRcvPdoComG[ubObjCntT].ulIdentifier  = 0x40000000 | uwIdentifierT;
      atsRcvPdoComG[ubObjCntT].ubTransType   = 0xFF;
      atsRcvPdoComG[ubObjCntT].ubSyncCount   = 0x00;

      #if COS_PDO_INHIBIT == 1
      atsRcvPdoComG[ubObjCntT].uwInhibitTime = 0x0000;
      atsRcvPdoComG[ubObjCntT].uwInhibitTick = 0x0000;

      #endif
      atsRcvPdoComG[ubObjCntT].uwEventTime   = 0x0000;
      atsRcvPdoComG[ubObjCntT].uwEventTick   = 0x0000;
   }
   #endif

   //----------------------------------------------------------------
   // do user specific setup
   //
   CosPdoComSetup();
}


//----------------------------------------------------------------------------//
// Cos301_ParmLoad()                                                          //
// load objects of DS-301 (index 1000h ... 1FFFh)                             //
//----------------------------------------------------------------------------//
uint8_t  Cos301_ParmLoad(void)
{
   #if COS_DICT_OBJ_1010 > 0

   #if (COS_DICT_OBJ_1016 > 0) || (COS_PDO_SUPPORT > 0) || (COS_DICT_OBJ_1029 > 0)
   uint8_t  ubObjCntT;           // counter for object number
   #endif

   #if COS_PDO_SUPPORT > 0
   uint8_t  ubPdoOffsetT;        // address offset for PDO parameter
   uint32_t ulIdValueT;          // temporary identifier value
   #endif

   #if COS_MOB_MC_STORE_COB_ID > 0
   uint16_t uwCobIdStoreT;       // COB-ID storage behaviour
   uint8_t  ubOldNodeAddressT;
   #endif


   #if COS_DICT_OBJ_1016 > 0
   uint16_t uwHbConsTimeT;       // heartbeat consumer time
   uint8_t  ubHbConsNodeT;       // heartbeat consumer node ID
   uint32_t ulHbConsValueT;
   #endif

   //----------------------------------------------------------------
   // get COB-ID storage behaviour
   // Format | 15 .... 8|7 .. 2| 1..0 |
   //        | Address  | res. |Format|
   //
   // The address is required for eCosCOB_ID_Store_PREDEF. If the
   // value in the lower byte is > eCosCOB_ID_Store_OFFSET, then
   // the EEPROM has not been initialised properly. IN this case
   // we set the default value (eCosCOB_ID_Store_KEEP) here.
   //
   #if COS_MOB_MC_STORE_COB_ID > 0
   McNvmRead(eNVM_301_COB_ID_STORE_U16, &uwCobIdStoreT, 2);
   ubOldNodeAddressT = uwCobIdStoreT >> 8;
   uwCobIdStoreT = uwCobIdStoreT & 0x00FF;
   if( uwCobIdStoreT  > eCosCOB_ID_Store_OFFSET)
   {
      uwCobIdStoreT = eCosCOB_ID_Store_KEEP;
   }
   #endif


   //----------------------------------------------------------------
   // SYNC identifier
   //
   #if COS_SYNC_SUPPORT == 1
   McNvmRead(eNVM_301_OBJ_1005_U32, &ulCosSyncIdentifierG, 4);
   #endif

   //----------------------------------------------------------------
   // SYNC cycle time
   //
   #if COS_DICT_OBJ_1006 > 0
   McNvmRead(eNVM_301_OBJ_1006_U16, &uwCosSyncCycleTimeG, 2);
   #endif

   //----------------------------------------------------------------
   // SYNC counter
   //
   #if COS_DICT_OBJ_1019== 1
   McNvmRead(eNVM_301_OBJ_1019_U08, &ubCosSyncCntMaxG, 1);
   #endif


   //----------------------------------------------------------------
   // Node guarding
   //
   #if COS_DICT_OBJ_100C == 1
   McNvmRead(eNVM_301_OBJ_100C_U16, &uwCosNmtGuardTimeG  , 2);
   McNvmRead(eNVM_301_OBJ_100D_U08, &ubCosNmtGuardFactorG, 1);
   #endif

   //----------------------------------------------------------------
   // EMCY identifier
   //
   McNvmRead(eNVM_301_OBJ_1014_U32, &ulCosEmcyIdentifierG, 4);

   //----------------------------------------------------------------
   // EMCY inhibit time
   //
   #if COS_DICT_OBJ_1015 > 0
   McNvmRead(eNVM_301_OBJ_1015_U16, &uwCosEmcyInhibitTimeG, 2);
   #endif


   //----------------------------------------------------------------
   // heartbeat consumer time
   //
   #if COS_DICT_OBJ_1016 > 0
   for(ubObjCntT = 0; ubObjCntT < COS_DICT_OBJ_1016; ubObjCntT++)
   {
      McNvmRead(eNVM_301_OBJ_1016_1_U32 + (ubObjCntT * 4), &ulHbConsValueT, 4);

      uwHbConsTimeT = (uint16_t) ulHbConsValueT;
      ubHbConsNodeT = (uint8_t) (ulHbConsValueT >> 16);

      //---------------------------------------------------
      // the time is stored in ticks, we need to convert
      // it in milliseconds for the setup routine of
      // heartbeat consumer
      //
      uwHbConsTimeT = CosTmrCalcTime(uwHbConsTimeT);
      CosNmtSetHeartbeatCons(ubObjCntT, ubHbConsNodeT, uwHbConsTimeT);
   }
   #endif


   //----------------------------------------------------------------
   // heartbeat producer time
   //
   McNvmRead(eNVM_301_OBJ_1017_U16, &uwCosNmtHeartTimeG, 2);


   //----------------------------------------------------------------
   // verify configuration, object 1020h
   //
   #if COS_DICT_OBJ_1020 == 1
   McNvmRead(eNVM_301_OBJ_1020_1_U32, &ulIdx1020_DateS, 4);
   McNvmRead(eNVM_301_OBJ_1020_2_U32, &ulIdx1020_TimeS, 4);
   #endif

   //----------------------------------------------------------------
   // error behaviour, object 1029
   //
   #if COS_DICT_OBJ_1029 > 0
   for(ubObjCntT = 0; ubObjCntT < COS_DICT_OBJ_1029; ubObjCntT++)
   {
      McNvmRead(  eNVM_301_OBJ_1029_1_U08 + ubObjCntT,
                  &(aubCosNmtErrorBehaviorG[ubObjCntT]),
                  1);
   }
   #endif


   //----------------------------------------------------------------
   // if COS_MOB_STORE_COB_ID is defined, check ID calculation
   //
   #if COS_MOB_MC_STORE_COB_ID > 0
   switch(uwCobIdStoreT)
   {
      case eCosCOB_ID_Store_KEEP:
         // keep the already loaded value
         break;

      case eCosCOB_ID_Store_PREDEF:
         // calculate according to pre-defined connection set
         // if the address has changed
         if(ubOldNodeAddressT != ubCosMgrNodeAddressG)
         {
            ulCosEmcyIdentifierG = ID_BASE_EMCY + ubCosMgrNodeAddressG;
         }
         break;

      case eCosCOB_ID_Store_OFFSET:
         // add an current node id to the stored value
         ulCosEmcyIdentifierG += ubCosMgrNodeAddressG;
         break;

      default:

         break;
   }
   #endif




   //----------------------------------------------------------------
   // read Receive PDO communication parameter
   //
   #if COS_PDO_RCV_NUMBER > 0
   for(ubObjCntT = 0; ubObjCntT < COS_PDO_RCV_NUMBER; ubObjCntT++)
   {
      ubPdoOffsetT = 12 * ubObjCntT;    // one parameter set has 12 bytes
      McNvmRead(eNVM_301_RCV_PDO_ID_U32 + ubPdoOffsetT, &ulIdValueT, 4);

      //----------------------------------------------------------------
      // if COS_MOB_STORE_COB_ID is defined, check ID calculation
      //
      #if COS_MOB_MC_STORE_COB_ID > 0
      switch(uwCobIdStoreT)
      {
         case eCosCOB_ID_Store_KEEP:
            // keep the already loaded value
            break;

         case eCosCOB_ID_Store_PREDEF:
            // calculate according to pre-defined connection set
            // if the address has changed
            if(ubOldNodeAddressT != ubCosMgrNodeAddressG)
            {
               ulIdValueT = (0x0100 * ubObjCntT);
               ulIdValueT = ulIdValueT + ID_BASE_PDO1_RX + ubCosMgrNodeAddressG;
               ulIdValueT = ulIdValueT | 0x40000000;
            }
            break;

         case eCosCOB_ID_Store_OFFSET:
            // add an current node id to the stored value
            ulIdValueT += ubCosMgrNodeAddressG;
            break;

         default:

            break;
      }
      #endif

      atsRcvPdoComG[ubObjCntT].ulIdentifier  = ulIdValueT;
      McNvmRead(  eNVM_301_RCV_PDO_TYP_U08 + ubPdoOffsetT,
                 &(atsRcvPdoComG[ubObjCntT].ubTransType), 1);
      atsRcvPdoComG[ubObjCntT].ubSyncCount   = 0;

      #if COS_PDO_INHIBIT == 1
      atsRcvPdoComG[ubObjCntT].uwInhibitTime = 0;
      atsRcvPdoComG[ubObjCntT].uwInhibitTick = 0;
      #endif
      McNvmRead(  eNVM_301_RCV_PDO_TMR_U16 + ubPdoOffsetT,
                  &(atsRcvPdoComG[ubObjCntT].uwEventTime), 2);
      atsRcvPdoComG[ubObjCntT].uwEventTick   = 0;

      //----------------------------------------------------------------
      // read variable PDO mapping parameters
      //
      #if COS_PDO_MAPPING > 0
      ubPdoOffsetT = 18 * ubObjCntT;    // one parameter set has 18 bytes

      // read all parameters of the corresponding transmit PDO
      McNvmRead(  eNVM_301_RCV_PDO_DYNMAP + ubPdoOffsetT,
                  &(atsRcvPdoDynMapParmG[ubObjCntT].ubSize), 18);
      #endif
   }
   #endif


   //--------------------------------------------------------
   // read Transmit PDO communication parameter
   //
   #if COS_PDO_TRM_NUMBER > 0
   for(ubObjCntT = 0; ubObjCntT < COS_PDO_TRM_NUMBER; ubObjCntT++)
   {
      ubPdoOffsetT = 12 * ubObjCntT;    // one parameter set has 12 bytes

      McNvmRead(eNVM_301_TRM_PDO_ID_U32 + ubPdoOffsetT, &ulIdValueT, 4);

      //----------------------------------------------------------------
      // if COS_MOB_STORE_COB_ID is defined, check ID calculation
      //
      #if COS_MOB_MC_STORE_COB_ID > 0
      switch(uwCobIdStoreT)
      {
         case eCosCOB_ID_Store_KEEP:
            // keep the already loaded value
            break;

         case eCosCOB_ID_Store_PREDEF:
            // calculate according to pre-defined connection set
            // if the address has changed
            if(ubOldNodeAddressT != ubCosMgrNodeAddressG)
            {
               ulIdValueT = (0x0100 * ubObjCntT);
               ulIdValueT = ulIdValueT + ID_BASE_PDO1_TX + ubCosMgrNodeAddressG;
               ulIdValueT = ulIdValueT | 0x40000000;
            }
            break;

         case eCosCOB_ID_Store_OFFSET:
            // add an current node id to the stored value
            ulIdValueT += ubCosMgrNodeAddressG;
            break;

         default:

            break;
      }
      #endif

      atsTrmPdoComG[ubObjCntT].ulIdentifier  = ulIdValueT;
      McNvmRead(  eNVM_301_TRM_PDO_TYP_U08 + ubPdoOffsetT,
                  &(atsTrmPdoComG[ubObjCntT].ubTransType),
                  1);
      atsTrmPdoComG[ubObjCntT].ubSyncCount   = 0;

      #if COS_PDO_INHIBIT == 1
      atsTrmPdoComG[ubObjCntT].uwInhibitTime = 0;
      atsTrmPdoComG[ubObjCntT].uwInhibitTick = 0;
      #endif

      McNvmRead(  eNVM_301_TRM_PDO_TMR_U16 + ubPdoOffsetT,
                  &(atsTrmPdoComG[ubObjCntT].uwEventTime),
                  2);
      atsTrmPdoComG[ubObjCntT].uwEventTick   = atsTrmPdoComG[ubObjCntT].uwEventTime;


      //----------------------------------------------------------------
      // read variable PDO mapping parameters
      //
      #if COS_PDO_MAPPING > 0
      ubPdoOffsetT = 18 * ubObjCntT;    // one parameter set has 18 bytes

      // read all parameters of the corresponding transmit PDO
      McNvmRead(  eNVM_301_TRM_PDO_DYNMAP + ubPdoOffsetT,
                  &(atsTrmPdoDynMapParmG[ubObjCntT].ubSize), 18);
      #endif
   }
   #endif



   //----------------------------------------------------------------
   // startup behaviour, object 1F80
   //
   #if COS_DICT_OBJ_1F80 > 0
   McNvmRead(eNVM_301_OBJ_1F80_U32, &ubCosNmtStartupG, 1);
   #endif

   #endif
   return(0);
}


//----------------------------------------------------------------------------//
// Cos301_ParmSave()                                                          //
// save objects of DS-301 (index 1000h ... 1FFFh)                             //
//----------------------------------------------------------------------------//
uint8_t  Cos301_ParmSave(void)
{
   #if COS_DICT_OBJ_1010 > 0

   #if COS_DICT_OBJ_1016 > 0
   uint32_t  ulHbConsValueT;
   #endif

   #if (COS_PDO_SUPPORT > 0) || (COS_DICT_OBJ_1016 > 0)
   uint8_t  ubObjCntT;           // counter for object number
   #endif

   #if COS_PDO_SUPPORT > 0
   uint8_t  ubPdoOffsetT;        // address offset for PDO parameter
   #endif




   //----------------------------------------------------------------
   // SYNC identifier
   //
   #if COS_SYNC_SUPPORT == 1
   McNvmWrite(eNVM_301_OBJ_1005_U32, &ulCosSyncIdentifierG, 4);
   #endif

   //----------------------------------------------------------------
   // SYNC cycle time
   //
   #if COS_DICT_OBJ_1006 > 0
   McNvmWrite(eNVM_301_OBJ_1006_U16, &uwCosSyncCycleTimeG, 2);
   #endif

   //----------------------------------------------------------------
   // SYNC counter
   //
   #if COS_DICT_OBJ_1019== 1
   McNvmWrite(eNVM_301_OBJ_1019_U08, &ubCosSyncCntMaxG, 1);
   #endif

   //----------------------------------------------------------------
   // Node guarding
   //
   #if COS_DICT_OBJ_100C == 1
   McNvmWrite(eNVM_301_OBJ_100C_U16, &uwCosNmtGuardTimeG  , 2);
   McNvmWrite(eNVM_301_OBJ_100D_U08, &ubCosNmtGuardFactorG, 1);
   #endif


   //----------------------------------------------------------------
   // EMCY identifier
   //
   McNvmWrite(eNVM_301_OBJ_1014_U32, &ulCosEmcyIdentifierG, 4);

   //----------------------------------------------------------------
   // EMCY inhibit time
   //
   #if COS_DICT_OBJ_1015 > 0
   McNvmWrite(eNVM_301_OBJ_1015_U16, &uwCosEmcyInhibitTimeG, 2);
   #endif


   //----------------------------------------------------------------
   // heartbeat consumer time
   //
   #if COS_DICT_OBJ_1016 > 0
   for(ubObjCntT = 0; ubObjCntT < COS_DICT_OBJ_1016; ubObjCntT++)
   {

      ulHbConsValueT = aubCosNmtHbConsNodeG[ubObjCntT];
      ulHbConsValueT = ulHbConsValueT << 16;
      ulHbConsValueT = ulHbConsValueT | auwCosNmtHbConsTimeG[ubObjCntT];

      //--------------------------------------------------------
      // the time is stored in ticks
      //
      McNvmWrite(eNVM_301_OBJ_1016_1_U32 + (ubObjCntT * 4), &ulHbConsValueT, 4);

   }
   #endif

   //----------------------------------------------------------------
   // heartbeat producer time
   //
   McNvmWrite(eNVM_301_OBJ_1017_U16, &uwCosNmtHeartTimeG, 2);

   //----------------------------------------------------------------
   // verify configuration, object 1020h
   //
   #if COS_DICT_OBJ_1020 == 1
   McNvmWrite(eNVM_301_OBJ_1020_1_U32, &ulIdx1020_DateS, 4);
   McNvmWrite(eNVM_301_OBJ_1020_2_U32, &ulIdx1020_TimeS, 4);
   #endif


   //----------------------------------------------------------------
   // error behaviour, object 1029
   //
   #if COS_DICT_OBJ_1029 > 0
   for(ubObjCntT = 0; ubObjCntT < COS_DICT_OBJ_1029; ubObjCntT++)
   {
      McNvmWrite( eNVM_301_OBJ_1029_1_U08 + ubObjCntT,
                  &(aubCosNmtErrorBehaviorG[ubObjCntT]),
                  1);
   }
   #endif


   //--------------------------------------------------------
   // write Receive PDO communication parameter
   //
   #if COS_PDO_RCV_NUMBER > 0
   for(ubObjCntT = 0; ubObjCntT < COS_PDO_RCV_NUMBER; ubObjCntT++)
   {
      ubPdoOffsetT = 12 * ubObjCntT;    // one parameter set has 12 bytes

      McNvmWrite( eNVM_301_RCV_PDO_ID_U32 + ubPdoOffsetT,
                  &(atsRcvPdoComG[ubObjCntT].ulIdentifier),
                  4);

      McNvmWrite( eNVM_301_RCV_PDO_TYP_U08 + ubPdoOffsetT,
                  &(atsRcvPdoComG[ubObjCntT].ubTransType),
                  1);

      McNvmWrite( eNVM_301_RCV_PDO_TMR_U16 + ubPdoOffsetT,
                  &(atsRcvPdoComG[ubObjCntT].uwEventTime),
                  2);

      //----------------------------------------------------------------
      // write variable PDO mapping parameters
      //
      #if COS_PDO_MAPPING > 0
      ubPdoOffsetT = 18 * ubObjCntT;    // one parameter set has 18 bytes

      // write all parameters of the corresponding transmit PDO
      McNvmWrite(  eNVM_301_RCV_PDO_DYNMAP + ubPdoOffsetT,
                  &(atsRcvPdoDynMapParmG[ubObjCntT].ubSize), 18);
      #endif

   }
   #endif

   //--------------------------------------------------------
   // write Transmit PDO communication parameter
   //
   #if COS_PDO_TRM_NUMBER > 0
   for(ubObjCntT = 0; ubObjCntT < COS_PDO_TRM_NUMBER; ubObjCntT++)
   {
      ubPdoOffsetT = 12 * ubObjCntT;    // one parameter set has 12 bytes

      McNvmWrite( eNVM_301_TRM_PDO_ID_U32 + ubPdoOffsetT,
                  &(atsTrmPdoComG[ubObjCntT].ulIdentifier),
                  4);

      McNvmWrite( eNVM_301_TRM_PDO_TYP_U08 + ubPdoOffsetT,
                  &(atsTrmPdoComG[ubObjCntT].ubTransType),
                  1);

      McNvmWrite( eNVM_301_TRM_PDO_TMR_U16 + ubPdoOffsetT,
                  &(atsTrmPdoComG[ubObjCntT].uwEventTime),
                  2);

      //----------------------------------------------------------------
      // write variable PDO mapping parameters
      //
      #if COS_PDO_MAPPING > 0
      ubPdoOffsetT = 18 * ubObjCntT;    // one parameter set has 18 bytes

      // write all parameters of the corresponding transmit PDO
      McNvmWrite(  eNVM_301_TRM_PDO_DYNMAP + ubPdoOffsetT,
                  &(atsTrmPdoDynMapParmG[ubObjCntT].ubSize), 18);
      #endif
   }
   #endif



   //----------------------------------------------------------------
   // startup behaviour, object 1F80
   //
   #if COS_DICT_OBJ_1F80 > 0
   McNvmWrite(eNVM_301_OBJ_1F80_U32, &ubCosNmtStartupG, 1);
   #endif

   #endif   //    #if COS_DICT_OBJ_1010 > 0

   return(0);
}


