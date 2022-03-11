//****************************************************************************//
// File:          cos_dict.c                                                  //
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
// SVN  $Date: 2014-04-21 15:43:13 +0200 (Mo, 21. Apr 2014) $
// SVN  $Rev: 5829 $ --- $Author: koppe $
//------------------------------------------------------------------------------



/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_dict.h"            // CANopen Slave dictionary header
#include "cos_emcy.h"            // Objects for emergency
#include "cos_nmt.h"             // Objects for NMT services
#include "cos_pdo.h"             // Objects for PDO services
#include "cos_sync.h"            // Objects for SYNC services
#include "cos_time.h"            // Objects for TIME services

#include "cos301.h"              // Objects from CiA 301


#if COS_DICT_MAN == 1
#include "cos_mobj.h"            // Manufacturer specific objects
#endif


#if COS_DS401_DI > 0
#include "cos401di.h"            // Objects from CiA 401, digital input
#endif

#if COS_DS401_DO > 0
#include "cos401do.h"            // Objects from CiA 401, digital output
#endif

#if COS_DS401_AI > 0
#include "cos401ai.h"            // Objects from CiA 401, analogue input
#endif

#if COS_DS401_AO > 0
#include "cos401ao.h"            // Objects from CiA 401, analogue output
#endif

#if COS_DS402 == 1
#include "cos402.h"              // Objects from CiA 402
#endif

#if COS_DS404_AI > 0
#include "cos404ai.h"            // Objects from CiA 404, analogue input
#endif

#if COS_DS404_AO > 0
#include "cos404ao.h"            // Objects from CiA 404, analogue output
#endif

#if COS_DS404_AL > 0
#include "cos404al.h"            // Objects from CiA 404, alarm
#endif

#if COS_DS406 > 0
#include "cos406.h"              // Objects from CiA 406
#endif

#if COS_DS418 > 0
#include "cos418.h"              // Objects from CiA 418
#endif

//-------------------------------------------------------------------
// Do not use this file for application profile CiA 417
//
#ifndef COS_DS417
#define COS_DS417    0
#endif
#if  COS_DS417 == 0

//-------------------------------------------------------------------
// global variables



//-------------------------------------------------------------------
// global variables
// The dictionary
// index, sub-index, attribute, data type, actValuePtr, CallbackPtr
//CP_CONST
CPP_CONST CosDicEntry_ts aDicTableG[] = {

   //--- Dummy entry: marks start of table, do not remove -
   {  0x0000, 0x00, 0x00   , 0x00, 0L                                },

   //--- Index 1000, device type --------------------------
   {  0x1000, 0x00, CoATTR_ACC_CONST   ,
      CoDT_UNSIGNED32      ,  (void *) &ulIdx1000_DeviceTypeC        },


   //--- Index 1001, error register -----------------------
   {  0x1001, 0x00, CoATTR_ACC_RO      ,
      CoDT_UNSIGNED8       ,  (void *) &ubIdx1001_ErrorRegisterG     },


   //--- Index 1002, manufacturer status register ---------
   {  0x1002, 0x00, CoATTR_ACC_RO      ,
      CoDT_UNSIGNED32      , (void *) &ulIdx1002_StatusRegisterG     },


   //--- Index 1003, pre-defined error field --------------
   #if COS_DICT_OBJ_1003 != 0
   {  0x1003, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosEmcyErrorField              },
   #endif


   //--- Index 1005, COB-ID SYNC --------------------------
   #if COS_DICT_OBJ_1005 == 1
   {  0x1005, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosSync_Idx1005                },
   #endif

   //--- Index 1006, Communication cycle period -----------
   #if COS_DICT_OBJ_1006 == 1
   {  0x1006, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosSync_Idx1006                },
   #endif

   //--- Index 1008, device name --------------------------
   #if COS_DICT_OBJ_1008 == 1
   {  0x1008, 0x00, CoATTR_ACC_CONST,
      CoDT_VISIBLE_STRING  , (void *) &ubIdx1008_DeviceNameC[0]      },
   #endif

   //--- Index 1009, hardware version ---------------------
   #if COS_DICT_OBJ_1009 == 1
   {  0x1009, 0x00, CoATTR_ACC_CONST,
      CoDT_VISIBLE_STRING  , (void *) &ubIdx1009_HwVersionC[0]       },
   #endif

   //--- Index 100A, software version ---------------------
   #if COS_DICT_OBJ_100A == 1
   {  0x100A, 0x00, CoATTR_ACC_CONST,
      CoDT_VISIBLE_STRING  , (void *) &ubIdx100A_SwVersionC[0]       },
   #endif


   //--- Index 100C, guard time ---------------------------
   #if COS_DICT_OBJ_100C == 1
   {  0x100C, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED16      , (void *) CosNmt_Idx100C                 },


   //--- Index 100D, life time ----------------------------
   {  0x100D, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED8       , (void *) CosNmt_Idx100D                 },
   #endif


   //--- Index 1010, store parameters ---------------------
   #if COS_DICT_OBJ_1010 != 0
   {  0x1010, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) Cos301_Idx1010                 },
   #endif

   //--- Index 1011, restore default parameters -----------
   #if COS_DICT_OBJ_1011 != 0
   {  0x1011, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) Cos301_Idx1011                 },
   #endif

   //--- Index 1012, TIME COB-ID --------------------------
   #if COS_DICT_OBJ_1012 > 0
   {  0x1012, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosTmr_Idx1012                 },
   #endif

   //--- Index 1013, High resolution time stamp -----------
   #if COS_DICT_OBJ_1013 > 0
   {  0x1013, 0x00, CoATTR_ACC_RO | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosTmr_Idx1013                 },
   #endif

   //--- Index 1014, EMCY COB-ID --------------------------
   #if COS_DICT_OBJ_1014 > 0
   {  0x1014, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosEmcyIdentifier              },
   #endif

   //--- Index 1015, EMCY inhibit time --------------------
   #if COS_DICT_OBJ_1015 > 0
   {  0x1015, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosEmcyInhibit                 },
   #endif

   //--- Index 1016, consumer heartbeat time --------------
   #if COS_DICT_OBJ_1016 != 0
   {  0x1016, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosNmt_Idx1016                 },
   #endif

   //--- Index 1017, producer heartbeat time --------------
   {  0x1017, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED16      , (void *) CosNmt_Idx1017                 },


   //--- Index 1018, identity object ----------------------
   {  0x1018, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) Cos301_Idx1018                 },

   //--- Index 1019, SYNC counter -------------------------
   #if COS_DICT_OBJ_1019 == 1
   {  0x1019, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosSync_Idx1019                },
   #endif

   //--- Index 1020, Verify configuration -----------------
   #if COS_DICT_OBJ_1020 == 1
   {  0x1020, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) Cos301_Idx1020                 },
   #endif

   //--- Index 1021, Store EDS ----------------------------
   #if COS_DICT_OBJ_1021 > 0
   {  0x1021, 0x00, CoATTR_ACC_RO | CoATTR_FUNCTION,
      CoDT_DOMAIN          , (void *) Cos301_Idx1021                 },
   #endif

   //--- Index 1022, Store format -------------------------
   #if COS_DICT_OBJ_1021 > 0
   {  0x1022, 0x00, CoATTR_ACC_RO,
      CoDT_UNSIGNED8       , (void *) &ubIdx1022_EdsTypeC            },
   #endif

   //--- Index 1029, Error behaviour object ---------------
   #if COS_DICT_OBJ_1029 > 0
   {  0x1029, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosNmt_Idx1029                 },
   #endif

   //--- Index 1400, Receive PDO 1 parameter --------------
   #if COS_PDO_RCV_NUMBER > 0
   {  0x1400, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoRcvComParam              },
   #endif

   //--- Index 1401, Receive PDO 2 parameter --------------
   #if COS_PDO_RCV_NUMBER > 1
   {  0x1401, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoRcvComParam              },
   #endif

   //--- Index 1402, Receive PDO 3 parameter --------------
   #if COS_PDO_RCV_NUMBER > 2
   {  0x1402, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoRcvComParam              },
   #endif

   //--- Index 1403, Receive PDO 4 parameter --------------
   #if COS_PDO_RCV_NUMBER > 3
   {  0x1403, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoRcvComParam              },
   #endif


   //--- Index 1600, Receive PDO 1 mapping ----------------
   #if COS_PDO_RCV_NUMBER > 0
   {  0x1600, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoMapParameter             },
   #endif

   //--- Index 1601, Receive PDO 2 mapping ----------------
    #if COS_PDO_RCV_NUMBER > 1
   {  0x1601, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoMapParameter             },
   #endif

   //--- Index 1602, Receive PDO 3 mapping ----------------
   #if COS_PDO_RCV_NUMBER > 2
   {  0x1602, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoMapParameter             },
   #endif

   //--- Index 1603, Receive PDO 4 mapping ----------------
   #if COS_PDO_RCV_NUMBER > 3
   {  0x1603, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoMapParameter             },
   #endif


   //--- Index 1800, Transmit PDO 1 parameter -------------
   #if COS_PDO_TRM_NUMBER > 0
   {  0x1800, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoTrmComParam              },
   #endif

   //--- Index 1801, Transmit PDO 2 parameter --------------
   #if COS_PDO_TRM_NUMBER > 1
   {  0x1801, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoTrmComParam              },
   #endif

   //--- Index 1802, Transmit PDO 3 parameter --------------
   #if COS_PDO_TRM_NUMBER > 2
   {  0x1802, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoTrmComParam              },
   #endif

   //--- Index 1803, Transmit PDO 4 parameter --------------
   #if COS_PDO_TRM_NUMBER > 3
   {  0x1803, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoTrmComParam              },
   #endif


   //--- Index 1A00, Transmit PDO 1 mapping ---------------
   #if COS_PDO_TRM_NUMBER > 0
   {  0x1A00, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoMapParameter             },
   #endif

   //--- Index 1A01, Transmit PDO 2 mapping ---------------
   #if COS_PDO_TRM_NUMBER > 1
   {  0x1A01, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoMapParameter             },
   #endif

   //--- Index 1A02, Transmit PDO 3 mapping ---------------
   #if COS_PDO_TRM_NUMBER > 2
   {  0x1A02, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoMapParameter             },
   #endif

   //--- Index 1A03, Transmit PDO 4 mapping ---------------
   #if COS_PDO_TRM_NUMBER > 3
   {  0x1A03, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosPdoMapParameter             },
   #endif


   //--- Index 1F80, NMT Startup behaviour ----------------
   #if COS_DICT_OBJ_1F80 > 0
   {  0x1F80, 0x00, CoATTR_ACC_RW | CoATTR_FUNCTION,
      CoDT_UNSIGNED32      , (void *) CosNmt_Idx1F80                 },
   #endif


   //--- include manufacturer specific objects ------------
   #if  COS_DICT_MAN == 1
   #include "cos_mobj.inc"
   #endif

   //--- include CiA 401, digital inputs ------------------
   #if  COS_DS401_DI > 0
   #include "cos401di.inc"
   #endif

   //--- include CiA 401, digital outputs -----------------
   #if  COS_DS401_DO > 0
   #include "cos401do.inc"
   #endif

   //--- include CiA 401, analogue inputs -----------------
   #if  COS_DS401_AI > 0
   #include "cos401ai.inc"
   #endif

   //--- include CiA 401, analogue outputs ----------------
   #if  COS_DS401_AO > 0
   #include "cos401ao.inc"
   #endif

   //--- include CiA 402 ----------------------------------
   #if  COS_DS402 == 1
   #include "cos402.inc"
   #endif

   //--- include CiA 404 ----------------------------------
   #if  (COS_DS404_AI != 0) || (COS_DS404_AO != 0) ||(COS_DS404_AL != 0)
   #include "cos404.inc"
   #endif

   //--- include CiA 406 ----------------------------------
   #if  COS_DS406 > 0
   #include "cos406.inc"
   #endif

   //--- include CiA 418 ----------------------------------
   #if  COS_DS418 > 0
   #include "cos418.inc"
   #endif

};



static uint16_t uwDicTableSizeS = sizeof(aDicTableG) / sizeof(CosDicEntry_ts);



/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/



//----------------------------------------------------------------------------//
// CosDictFindEntry()                                                         //
// find entry in dictionary, divide-and-conquer routine                       //
//----------------------------------------------------------------------------//
#if COS_DICT_SEARCH_FAST == 1
CPP_CONST CosDicEntry_ts * CosDictFindEntry( uint16_t uwIndexV,
                                             uint8_t ubSubIndexV,
                                             uint8_t * pubStatusV)
{
   uint16_t  uwPosLeftT;
   uint16_t  uwPosRightT;
   uint16_t  uwPosEntryT;
   uint8_t   ubSearchStatusT = eCosDict_FAIL_INDEX;
   uint8_t   ubSearchRunT = 1;
   CPP_CONST CosDicEntry_ts *  ptsDicEntryT = 0L;


   //----------------------------------------------------------------
   // search algorithm uses "divide-and-conquer"
   // the table must be sorted in ascending order for
   // index and sub-index,
   // Search comparisons: lg(base2) N + 1
   //
   uwPosLeftT  = 1;
   uwPosRightT = uwDicTableSizeS - 1;

   while(ubSearchRunT) // && (uwIndexV > 0))
   {
      //--- get position in middle between L and R --------
      uwPosEntryT = (uwPosLeftT + uwPosRightT) / 2;

      //--- get pointer to dictionary entry ---------------
      ptsDicEntryT  = &(aDicTableG[uwPosEntryT]);


      //--- check the index -------------------------------
      if(uwIndexV < ptsDicEntryT->uwIndex)
      {
         uwPosRightT = uwPosEntryT - 1;
      }
      if (uwIndexV > ptsDicEntryT->uwIndex)
      {
         uwPosLeftT  = uwPosEntryT + 1;
      }


      //--- check the sub-index ---------------------------
      if(  (uwIndexV == ptsDicEntryT->uwIndex) &&
           !(ptsDicEntryT->ubAttribute & CoATTR_FUNCTION)  )
      {
         //-------------------------------------------
         // the index was found
         ubSearchStatusT = eCosDict_FAIL_SUBINDEX;

         if(ubSubIndexV < ptsDicEntryT->ubSubIndex)
         {
            uwPosRightT = uwPosEntryT - 1;
         }
         else
         {
            uwPosLeftT = uwPosEntryT + 1;
         }
      }


      //--------------------------------------------------------
      // now test the break condition
      //

      //--- index and sub-index match ---------------------
      if( (ptsDicEntryT->uwIndex == uwIndexV) &&
          (ptsDicEntryT->ubSubIndex == ubSubIndexV)      )
      {
         //-------------------------------------------
         // the object was found
         ubSearchStatusT = eCosDict_FOUND_OBJECT;
         ubSearchRunT = 0;
         break;
      }


      //--- index match and callback function -------------
      if( (ptsDicEntryT->uwIndex == uwIndexV) &&
          (ptsDicEntryT->ubAttribute & CoATTR_FUNCTION)  )
      {
         //-------------------------------------------
         // the object was found
         ubSearchStatusT = eCosDict_FOUND_OBJECT;
         ubSearchRunT = 0;
         break;
      }


      //--- no entry found --------------------------------
      if( uwPosRightT < uwPosLeftT )
      {
         ptsDicEntryT = 0L;
         ubSearchRunT = 0;
         break;
      }

   }

   //----------------------------------------------------------------
   // provide status information
   //
   if(pubStatusV)
   {
      *pubStatusV = ubSearchStatusT;
   }
   return(ptsDicEntryT);
}
#else


//----------------------------------------------------------------------------//
// CosDictFindEntry()                                                         //
// find entry in dictionary, linear search                                    //
//----------------------------------------------------------------------------//
CPP_CONST CosDicEntry_ts * CosDictFindEntry( uint16_t uwIndexV,
                                             uint8_t ubSubIndexV,
                                             uint8_t * pubStatusV)
{
   uint16_t                   uwPosEntryT;
   uint8_t                    ubSearchStatusT = eCosDict_FAIL_INDEX;
   CPP_CONST CosDicEntry_ts * ptsDicEntryT = 0L;


   //----------------------------------------------------------------
   // search algorithm for linear search
   //
   for(uwPosEntryT = 1; uwPosEntryT < uwDicTableSizeS; uwPosEntryT++)
   {

      //--- get pointer to dictionary entry --------------------
      ptsDicEntryT  = (CosDicEntry_ts *) &(aDicTableG[uwPosEntryT]);


      //--------------------------------------------------------
      // now test the break condition
      //

      //--- index match -----------------------------------
      if( ptsDicEntryT->uwIndex == uwIndexV )
      {
         //-------------------------------------------
         // the index was found
         ubSearchStatusT = eCosDict_FAIL_SUBINDEX;

         //--- sub-index match -----------------------
         if( ptsDicEntryT->ubSubIndex == ubSubIndexV )
         {
            //-----------------------------------
            // the object was found
            ubSearchStatusT = eCosDict_FOUND_OBJECT;
            *pubStatusV = ubSearchStatusT;
            return(ptsDicEntryT);
         }


         //--- callback function match ---------------
         if( ptsDicEntryT->ubAttribute & CoATTR_FUNCTION )
         {
            //-----------------------------------
            // the object was found
            ubSearchStatusT = eCosDict_FOUND_OBJECT;
            *pubStatusV = ubSearchStatusT;
            return(ptsDicEntryT);
         }
      }
   }

   *pubStatusV = ubSearchStatusT;
   return(0L);

}
#endif


#endif   // COS_DS417 == 0
