//****************************************************************************//
// File:          cos_emcy.c                                                  //
// Description:   Emergency (EMCY) service for CANopen slave                  //
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
// 18.04.2001  Do not send Emergency in Stopped Mode                          //
// 22.03.2002  Added Emergency code enumeration                               //
// 30.10.2002  Changed callback structure to reduce code size                 //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-20 14:29:41 +0100 (Do, 20. Feb 2014) $
// SVN  $Rev: 5729 $ --- $Author: koppe $
//------------------------------------------------------------------------------


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_emcy.h"

#include "cos_mgr.h"
#include "cos_nmt.h"       // include NMT services
#include "cos_sdo.h"       // include SDO services
#include "cos_time.h"      // include timing services
#include "cos301.h"        // include object dictionary



/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

uint32_t  ulCosEmcyIdentifierG;


#if COS_DICT_OBJ_1003 > 0
static uint32_t ulCosEmcyErrorFieldS[COS_DICT_OBJ_1003];
static uint8_t ubCosEmcyErrorCountS;
static uint8_t ubCosEmcyErrorNewPosS;
#endif

#if COS_DICT_OBJ_1015 > 0
uint16_t        uwCosEmcyInhibitTimeG;     // inhibit timer value for EMCY
static uint16_t uwCosEmcyInhibitTickS;     // ticks value for EMCY
#endif

//-------------------------------------------------------------------
// queue for EMCY data to be send
//
#if COS_DICT_OBJ_1014 > 1
static uint8_t ubCosEmcyHeadS;
static uint8_t ubCosEmcyTailS;
static uint8_t aubCosEmcyQueueS[COS_DICT_OBJ_1014][8];
#endif

/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CosEmcyAppendToErrorField()                                                //
// append data to pre-defined error field                                     //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1003 > 0
void CosEmcyAppendToErrorField(uint16_t uwEmcyCodeV, uint16_t uwAddInfoV)
{
   ubCosEmcyErrorCountS++;
   ubCosEmcyErrorNewPosS++;


   //----------------------------------------------------------------
   // check for overflow
   //
   if(ubCosEmcyErrorCountS > COS_DICT_OBJ_1003)
   {
      ubCosEmcyErrorCountS = COS_DICT_OBJ_1003;
   }

   if(ubCosEmcyErrorNewPosS > COS_DICT_OBJ_1003 - 1)
   {
      ubCosEmcyErrorNewPosS = 0;
   }


   //----------------------------------------------------------------
   // append to error field list
   //
   ulCosEmcyErrorFieldS[ubCosEmcyErrorNewPosS]   = uwAddInfoV;
   ulCosEmcyErrorFieldS[ubCosEmcyErrorNewPosS] <<= 16;
   ulCosEmcyErrorFieldS[ubCosEmcyErrorNewPosS]  |= uwEmcyCodeV;

}
#endif


//----------------------------------------------------------------------------//
// CosEmcyErrorField()                                                        //
// get data from pre-defined error field                                      //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1003 > 0
uint8_t CosEmcyErrorField(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint8_t  ubHandlerCodeT;
   int16_t  swListPositionT;
   uint8_t  ubResetValueT;


   //----------------------------------------------------------------
   // only read access for sub-indexes greater 0
   //
   if( (ubReqCodeV != eSDO_READ_REQ) && (ubSubIndexV != 0) )
   {
      ubHandlerCodeT = eCosSdo_ERR_ACCESS_RO;
      return(ubHandlerCodeT);
   }


   //----------------------------------------------------------------
   // handle sub-index 0
   //
   if(ubSubIndexV == 0)
   {

      //--------------------------------------------------------
      // read number of entries
      //
      if(ubReqCodeV == eSDO_READ_REQ)
      {
         CosSdoCopyValueToMessage(  (void *)&ubCosEmcyErrorCountS,
                                    CoDT_UNSIGNED8);
         ubHandlerCodeT = eCosSdo_READ1_OK;
      }

      //--------------------------------------------------------
      // reset error list when writing a value of 0
      //
      else if ((ubReqCodeV == eSDO_WRITE_REQ_0) ||
               (ubReqCodeV == eSDO_WRITE_REQ_1))
      {
         CosSdoCopyMessageToValue( (void *)&ubResetValueT, CoDT_UNSIGNED8);

         if(ubResetValueT > 0)
         {
            ubHandlerCodeT = eCosSdo_ERR_VALUE_HIGH;
         }
         else
         {
            ubCosEmcyErrorCountS    = 0;
            ubCosEmcyErrorNewPosS   = 0;
            ubHandlerCodeT = eCosSdo_WRITE_OK;
         }
      }
      else
      {
         ubHandlerCodeT = eCosSdo_ERR_DATATYPE;
      }

      return(ubHandlerCodeT);
   }


   //----------------------------------------------------------------
   // check if requested sub-index is available
   //
   if(ubSubIndexV > ubCosEmcyErrorCountS)
   {
      ubHandlerCodeT = eCosSdo_ERR_NO_SUB_INDEX;
   }
   else
   {
      //--------------------------------------------------------
      // get position of value in the list
      //
      swListPositionT = ubCosEmcyErrorNewPosS - ubSubIndexV + 1;
      if(swListPositionT < 0)
      {
         swListPositionT = swListPositionT + COS_DICT_OBJ_1003;
      }

      CosSdoCopyValueToMessage(  (void *) &ulCosEmcyErrorFieldS[swListPositionT],
                                 CoDT_UNSIGNED32);
      ubHandlerCodeT = eCosSdo_READ4_OK;
   }
   return(ubHandlerCodeT);
}
#endif


//----------------------------------------------------------------------------//
// CosEmcyIdentifier()                                                        //
// get and set identifier for emergency service                               //
//----------------------------------------------------------------------------//
uint8_t  CosEmcyIdentifier(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   CpCanMsg_ts    tsCanMsgT;        // CAN message structure
   uint32_t       ulIdentifierT;    // identifier


   //----------------------------------------------------------------
   // only access to sub-index 0
   //
   if( ubSubIndexV != 0)
   {
      return(eCosSdo_ERR_NO_SUB_INDEX);
   }


   //----------------------------------------------------------------
   // handle read access
   //
   if( ubReqCodeV == eSDO_READ_REQ)
   {
      CosSdoCopyValueToMessage(  (void *) &ulCosEmcyIdentifierG,
                                 CoDT_UNSIGNED32);

      return(eCosSdo_READ4_OK);
   }


   //----------------------------------------------------------------
   // test write command code
   //
   if( !((ubReqCodeV == eSDO_WRITE_REQ_0) || (ubReqCodeV == eSDO_WRITE_REQ_4)) )
   {
      return(eCosSdo_ERR_DATATYPE);
   }


   //----------------------------------------------------------------
   // handle write access
   //
   CosSdoCopyMessageToValue( (void *)&ulIdentifierT, CoDT_UNSIGNED32);


   //--- check the value of this id -------------
   if( CosMgrIdCheck(ulIdentifierT) == 0 )
   {
      return(eCosSdo_ERR_VALUE_RANGE);
   }



   //----------------------------------------------------------------
   // setup identifier
   // mask bit 30 to 0
   //
   ulCosEmcyIdentifierG = ulIdentifierT & 0xBFFFFFFF;

   CpMsgClear(&tsCanMsgT);
   if( ulCosEmcyIdentifierG & 0x20000000)
   {
      CpMsgSetExtId(&tsCanMsgT, ulCosEmcyIdentifierG);
   }
   else
   {
      CpMsgSetStdId(&tsCanMsgT, (uint16_t) ulCosEmcyIdentifierG);
   }

   CpMsgSetDlc(&tsCanMsgT, 8);
   CpCoreBufferInit(&tsCanPortG, &tsCanMsgT, eCosBuf_EMCY, CP_BUFFER_DIR_TX);

   #if COS_DICT_OBJ_1020 == 1
   Cos301_ClearVerifyConfiguration();
   #endif

   return(eCosSdo_WRITE_OK);
}


//----------------------------------------------------------------------------//
// CosEmcyInhibit()                                                           //
// get and set inhibit time for emergency service                             //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1015 > 0
uint8_t  CosEmcyInhibit(uint8_t ubSubIndexV, uint8_t ubReqCodeV)
{
   uint16_t  uwTimeValueT;

   //----------------------------------------------------------------
   // only access to sub-index 0
   //
   if( ubSubIndexV != 0)
   {
      return(eCosSdo_ERR_NO_SUB_INDEX);
   }


   //----------------------------------------------------------------
   // handle read access
   //
   if( ubReqCodeV == eSDO_READ_REQ)
   {
      uwTimeValueT = CosTmrCalcTime( uwCosEmcyInhibitTimeG);
      uwTimeValueT = uwTimeValueT * 10;
      CosSdoCopyValueToMessage(  (void *) &uwTimeValueT,
                                 CoDT_UNSIGNED16);

      return(eCosSdo_READ2_OK);
   }


   //----------------------------------------------------------------
   // test write command code
   //
   if( !((ubReqCodeV == eSDO_WRITE_REQ_0) || (ubReqCodeV == eSDO_WRITE_REQ_2)) )
   {
      return(eCosSdo_ERR_DATATYPE);
   }


   //----------------------------------------------------------------
   // handle write access
   //
   CosSdoCopyMessageToValue( (void *)&uwCosEmcyInhibitTimeG, CoDT_UNSIGNED16);

   //----------------------------------------------------------------
   // convert to milliseconds
   //
   uwCosEmcyInhibitTimeG = uwCosEmcyInhibitTimeG / 10;

   uwCosEmcyInhibitTimeG = CosTmrCalcTicks(uwCosEmcyInhibitTimeG);

   #if COS_DICT_OBJ_1020 == 1
   Cos301_ClearVerifyConfiguration();
   #endif

   return(eCosSdo_WRITE_OK);
}
#endif


//----------------------------------------------------------------------------//
// CosEmcyInit()                                                              //
// Initialise emergency message service                                       //
//----------------------------------------------------------------------------//
void  CosEmcyInit(void)
{
   CpCanMsg_ts       tsCanMsgT;     // CAN message structure


   CpMsgClear(&tsCanMsgT);

   //----------------------------------------------------------------
   // setup identifier
   //
   if( ulCosEmcyIdentifierG & 0x20000000)
   {
      CpMsgSetExtId(&tsCanMsgT, ulCosEmcyIdentifierG);
   }
   else
   {
      CpMsgSetStdId(&tsCanMsgT, (uint16_t) ulCosEmcyIdentifierG);
   }

   CpMsgSetDlc(&tsCanMsgT, 8);
   CpCoreBufferInit(&tsCanPortG, &tsCanMsgT, eCosBuf_EMCY, CP_BUFFER_DIR_TX);


   //----------------------------------------------------------------
   // reset pre-defined error list
   //
   #if COS_DICT_OBJ_1003 > 0

   for(  ubCosEmcyErrorCountS = 0;
         ubCosEmcyErrorCountS < COS_DICT_OBJ_1003;
         ubCosEmcyErrorCountS++  )
   {
      ulCosEmcyErrorFieldS[ubCosEmcyErrorCountS] = 0x00000000;
   }

   ubCosEmcyErrorCountS    = 0;
   ubCosEmcyErrorNewPosS   = 0;

   #endif

   #if COS_DICT_OBJ_1014 > 1
   ubCosEmcyHeadS = 0;
   ubCosEmcyTailS = 0;
   #endif

   #if COS_DICT_OBJ_1015 > 0
   uwCosEmcyInhibitTimeG = 0;
   uwCosEmcyInhibitTickS = 0;
   #endif
}


//----------------------------------------------------------------------------//
// CosEmcySend()                                                              //
// transmit emergency message                                                 //
//----------------------------------------------------------------------------//
void  CosEmcySend(uint16_t uwEmcyCodeV, uint8_t * pubCustomerCodeV)
{
   uint8_t  aubEmcyDataT[8];
   uint8_t  ubDataCntT;
   uint16_t  uwAddInfoT = 0;


   //--- check for service disabled ---------------------------------
   if(ulCosEmcyIdentifierG & 0x80000000) return;


   //--- copy emergency error code ----------------------------------
   aubEmcyDataT[0] = (uint8_t) (uwEmcyCodeV & 0x00FF);
   aubEmcyDataT[1] = (uint8_t) (uwEmcyCodeV >> 8);


   //--- insert the error register ----------------------------------
   aubEmcyDataT[2] = ubIdx1001_ErrorRegisterG;


   //----------------------------------------------------------------
   // copy customer error code, if the pointer is 0L, the
   // message is filled with zeros
   //
   if(pubCustomerCodeV == 0L)
   {
      //---------------------------------------------------
      // fill with zero
      //
      for(ubDataCntT = 3; ubDataCntT < 8; ubDataCntT++)
      {
         aubEmcyDataT[ubDataCntT] = 0x00;
      }

      //---------------------------------------------------
      // manufacturer error code for object 1003
      // is set to 0
      //
      uwAddInfoT = 0x0000;
   }

   else
   {
      //---------------------------------------------------
      // copy manufacturer error code (5 bytes)
      //
      for(ubDataCntT = 3; ubDataCntT < 8; ubDataCntT++)
      {
         aubEmcyDataT[ubDataCntT] = *pubCustomerCodeV;

         //--- save manufacturer error code ----------
         if(ubDataCntT == 3)
         {
            uwAddInfoT = *pubCustomerCodeV;
            uwAddInfoT = uwAddInfoT << 8;
         }
         if(ubDataCntT == 4)
         {
            uwAddInfoT = uwAddInfoT | (*pubCustomerCodeV);
         }

         pubCustomerCodeV++;
      }
   }


   //----------------------------------------------------------------
   // store message in list
   //
   //----------------------------------------------------------------
   #if COS_DICT_OBJ_1003 > 0
   CosEmcyAppendToErrorField(uwEmcyCodeV, uwAddInfoT);
   #endif


   //----------------------------------------------------------------
   // Do FIFO operation or send immediately
   //
   //----------------------------------------------------------------
   #if COS_DICT_OBJ_1014 > 1

   //----------------------------------------------------------------
   // test if FIFO is full:
   //
   if( (ubCosEmcyHeadS == 0) && (ubCosEmcyTailS == COS_DICT_OBJ_1014 - 1) )
   {
      return;
   }

   if(ubCosEmcyHeadS == ubCosEmcyTailS + 1)
   {
      return;
   }



   //----------------------------------------------------------------
   // FIFO is not full, insert the message
   //
   for(ubDataCntT = 0; ubDataCntT < 8; ubDataCntT++)
   {
      aubCosEmcyQueueS[ubCosEmcyTailS][ubDataCntT] = aubEmcyDataT[ubDataCntT];
   }

   //----------------------------------------------------------------
   // increment tail position
   //
   ubCosEmcyTailS += 1;
   if(ubCosEmcyTailS == COS_DICT_OBJ_1014)
   {
      ubCosEmcyTailS = 0;
   }
   #else
   //----------------------------------------------------------------
   // check for operational state
   //
   if(CosNmtGetNodeState() == NODE_STATE_STOPPED) return;

   //-----------------------------------------------------------------
   // get message from FIFO and copy into CAN message buffer
   //
   CpCoreBufferSetData( &tsCanPortG, eCosBuf_EMCY,
                        &aubEmcyDataT[0] );

   //-----------------------------------------------------------------
   // send the message
   //
   CpCoreBufferSend(&tsCanPortG, eCosBuf_EMCY);

   #endif

}


//----------------------------------------------------------------------------//
// CosEmcyTmrEvent()                                                          //
// transmit emergency message                                                 //
//----------------------------------------------------------------------------//
void  CosEmcyTmrEvent(void)
{
   #if COS_DICT_OBJ_1014 > 1
   //----------------------------------------------------------------
   // test inhibit timer value for EMCY
   //
   #if COS_DICT_OBJ_1015 > 0
   if(uwCosEmcyInhibitTickS > 0)
   {
      uwCosEmcyInhibitTickS--;
      return;
   }
   #endif

   //----------------------------------------------------------------
   // check for operational state
   //
   if(CosNmtGetNodeState() == NODE_STATE_STOPPED) return;

   //-----------------------------------------------------------------
   // test if FIFO is empty
   //
   if(ubCosEmcyHeadS == ubCosEmcyTailS)
   {
      return;
   }

   //-----------------------------------------------------------------
   // get message from FIFO and copy into CAN message buffer
   //
   CpCoreBufferSetData( &tsCanPortG, eCosBuf_EMCY,
                        &aubCosEmcyQueueS[ubCosEmcyHeadS][0] );

   //--- send the message -------------------------------------------
   CpCoreBufferSend(&tsCanPortG, eCosBuf_EMCY);


   //----------------------------------------------------------------
   // increment head position
   //
   ubCosEmcyHeadS += 1;
   if(ubCosEmcyHeadS == COS_DICT_OBJ_1014)
   {
      ubCosEmcyHeadS = 0;
   }

   //----------------------------------------------------------------
   // reload inhibit timer value for EMCY
   //
   #if COS_DICT_OBJ_1015 > 0
   uwCosEmcyInhibitTickS = uwCosEmcyInhibitTimeG;
   #endif

   #endif   // COS_DICT_OBJ_1014 > 1
}
