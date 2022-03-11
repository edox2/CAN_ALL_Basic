//****************************************************************************//
// File:          cos_lss.c                                                   //
// Description:   Layer Setting Services (LSS) for CANopen Slave              //
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
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 16.05.2002  Initial version                                                //
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

#include "cos_lss.h"             // CANopen Slave definition file

#include "cos301.h"              // Objects from DS-301
#include "cos_mgr.h"             // CANopen Slave management functions
#include "cos_nvm.h"             // Non-volatile memory support

#if COS_LSS_SUPPORT > 0          // only used if LSS service enabled




/*----------------------------------------------------------------------------*\
** Variables of module for internal use                                       **
**                                                                            **
\*----------------------------------------------------------------------------*/
static uint8_t ubCosLssModeS;
static uint8_t ubCosLssBaudrateS;
static uint8_t ubCosLssNodeIdS;
static uint8_t aubCosLssRcvDataS[8];
static uint8_t aubCosLssTrmDataS[8];

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  ID_LSS_RCV     0x07E5   // identifier for reception (from LSS master)
#define  ID_LSS_TRM     0x07E4   // identifier for transmission (to LSS master)


/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CosLssConfigureNodeId()                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLssConfigureBitTiming(void)
{
   uint16_t  uwNewBitTimingT;
   uint8_t  ubDataCntT;          // counter for response message
   uint8_t  ubErrorCodeT = 1;    // default is unsupported bit-timing


   //----------------------------------------------------------------
   // test the new bit-timing
   //
   if( aubCosLssRcvDataS[1] == 0x00)
   {
      //--- look for entries out of CiA bit-timing table --
      uwNewBitTimingT = aubCosLssRcvDataS[2];
      if(uwNewBitTimingT <= 9)
      {
         //-------------------------------------------
         // adjust to internal representation, i.e.
         // the CP_BAUD enumeration
         //
         if (uwNewBitTimingT != 9)
         {
            ubCosLssBaudrateS = 8 - (uint8_t)uwNewBitTimingT;
         } 
         else
         {
            ubCosLssBaudrateS = (uint8_t)uwNewBitTimingT;
         }

         ubErrorCodeT = 0;
      }
   }


   //----------------------------------------------------------------
   // prepare response message
   //
   aubCosLssTrmDataS[0] = eLSS_CMD_CONFIG_BIT_TIMING;
   aubCosLssTrmDataS[1] = ubErrorCodeT;
   for(ubDataCntT = 2; ubDataCntT < 8; ubDataCntT++)
   {
      aubCosLssTrmDataS[ubDataCntT] = 0x00;
   }

   //----------------------------------------------------------------
   // send response message
   //
   CpCoreBufferSetData(&tsCanPortG, eCosBuf_LSS_TRM, &aubCosLssTrmDataS[0]);
   CpCoreBufferSend(&tsCanPortG, eCosBuf_LSS_TRM);
}


//----------------------------------------------------------------------------//
// CosLssConfigureNodeId()                                                    //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLssConfigureNodeId(void)
{
   uint8_t  ubNewNodeIdT;
   uint8_t  ubDataCntT;          // counter for response message
   uint8_t  ubErrorCodeT = 1;    // default is wrong node ID


   //----------------------------------------------------------------
   // test the new node ID
   //
   ubNewNodeIdT = aubCosLssRcvDataS[1];

   if( (ubNewNodeIdT > 0x00) && (ubNewNodeIdT < 0x80) )
   {
      ubCosLssNodeIdS = ubNewNodeIdT;
      ubErrorCodeT = 0;
   }

   if( ubNewNodeIdT == 0xFF )
   {
      ubCosLssNodeIdS = ubNewNodeIdT;
      ubErrorCodeT = 0;
   }


   //----------------------------------------------------------------
   // prepare response message
   //
   aubCosLssTrmDataS[0] = eLSS_CMD_CONFIG_NODE_ID;
   aubCosLssTrmDataS[1] = ubErrorCodeT;

   for(ubDataCntT = 2; ubDataCntT < 8; ubDataCntT++)
   {
      aubCosLssTrmDataS[ubDataCntT] = 0x00;
   }

   //----------------------------------------------------------------
   // send response message
   //
   CpCoreBufferSetData(&tsCanPortG, eCosBuf_LSS_TRM, &aubCosLssTrmDataS[0]);
   CpCoreBufferSend(&tsCanPortG, eCosBuf_LSS_TRM);
}


//----------------------------------------------------------------------------//
// CosLssInit()                                                               //
// initialize the LSS module                                                  //
//----------------------------------------------------------------------------//
void  CosLssInit(void)
{
   CpCanMsg_ts    tsCanMsgT;     // CAN message structure


   //----------------------------------------------------------------
   // LSS transmit buffer
   //
   CpCoreBufferRelease(&tsCanPortG, eCosBuf_LSS_TRM);
   CpMsgClear(&tsCanMsgT);
   CpMsgSetStdId(&tsCanMsgT, ID_LSS_TRM);
   CpMsgSetDlc(&tsCanMsgT, 8);
   CpCoreBufferInit(&tsCanPortG, &tsCanMsgT, eCosBuf_LSS_TRM, CP_BUFFER_DIR_TX);


   //----------------------------------------------------------------
   // LSS receive buffer
   //
   CpCoreBufferRelease(&tsCanPortG, eCosBuf_LSS_RCV);
   CpMsgClear(&tsCanMsgT);
   CpMsgSetStdId(&tsCanMsgT, ID_LSS_RCV);
   CpMsgSetDlc(&tsCanMsgT, 8);
   CpCoreBufferInit(&tsCanPortG, &tsCanMsgT, eCosBuf_LSS_RCV, CP_BUFFER_DIR_RX);


   //----------------------------------------------------------------
   // setup default node-id and baudrate
   //
   ubCosLssNodeIdS   = ubCosMgrNodeAddressG;    // node address
   McNvmRead(eNVM_305_BAUDRATE_U08, &ubCosLssBaudrateS, 1);


   //----------------------------------------------------------------
   // default mode is LSS waiting
   //
   ubCosLssModeS = eLSS_MODE_WAIT;
}


//----------------------------------------------------------------------------//
// CosLssInquiryService()                                                     //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLssInquiryService(uint8_t ubServiceV)
{
   uint32_t  ulDataT;       // 32-bit value from object 1018h

   //----------------------------------------------------------------
   // get the data for the requested service
   //
   switch(ubServiceV)
   {
      case eLSS_CMD_INQUIRE_VENDOR_ID:
         ulDataT = ulIdx1018_VendorIdC;
         break;

      case eLSS_CMD_INQUIRE_PRODUCT_CODE:
         ulDataT = ulIdx1018_ProductCodeC;
         break;

      case eLSS_CMD_INQUIRE_REVISION_NUM:
         ulDataT = ulIdx1018_RevisionNumC;
         break;

      case eLSS_CMD_INQUIRE_SERIAL_NUM:
         ulDataT = CosMgrGetSerialNumber();
         break;

      case eLSS_CMD_INQUIRE_NODE_ID:
         ulDataT = (uint32_t) ubCosMgrNodeAddressG;
         break;

      default:
         ulDataT = 0;
         break;
   }

   //----------------------------------------------------------------
   // prepare response message
   //
   aubCosLssTrmDataS[0] = ubServiceV;
   aubCosLssTrmDataS[1] = (uint8_t)(ulDataT);
   aubCosLssTrmDataS[2] = (uint8_t)(ulDataT >> 8);
   aubCosLssTrmDataS[3] = (uint8_t)(ulDataT >> 16);
   aubCosLssTrmDataS[4] = (uint8_t)(ulDataT >> 24);
   aubCosLssTrmDataS[5] = 0x00;
   aubCosLssTrmDataS[6] = 0x00;
   aubCosLssTrmDataS[7] = 0x00;


   //----------------------------------------------------------------
   // send response message
   //
   CpCoreBufferSetData(&tsCanPortG, eCosBuf_LSS_TRM, &aubCosLssTrmDataS[0]);
   CpCoreBufferSend(&tsCanPortG, eCosBuf_LSS_TRM);

}


//----------------------------------------------------------------------------//
// CosLssMessageHandler()                                                     //
// handle all incoming LSS messages                                           //
//----------------------------------------------------------------------------//
void  CosLssMessageHandler(void)
{
   uint8_t  ubLssCommandT;


   //----------------------------------------------------------------
   // get data from LSS receive buffer
   //
   CpCoreBufferGetData(&tsCanPortG, eCosBuf_LSS_RCV, &aubCosLssRcvDataS[0]);


   //----------------------------------------------------------------
   // check the command byte
   //
   ubLssCommandT = aubCosLssRcvDataS[0];
   switch(ubLssCommandT)
   {
      //--- Switch mode global ----------------------------
      case eLSS_CMD_MODE_GLOBAL:
         CosLssSwitchModeGlobal();
         break;

      //--- Configure node ID -----------------------------
      case eLSS_CMD_CONFIG_NODE_ID:
         if(ubCosLssModeS == eLSS_MODE_CONFIG)
         {
            CosLssConfigureNodeId();
         }
         break;

      //--- Configure bit-timing --------------------------
      case eLSS_CMD_CONFIG_BIT_TIMING:
         if(ubCosLssModeS == eLSS_MODE_CONFIG)
         {
            CosLssConfigureBitTiming();
         }
         break;

      //--- Store configuration ---------------------------
      case eLSS_CMD_CONFIG_STORE:
         if(ubCosLssModeS == eLSS_MODE_CONFIG)
         {
            CosLssStoreConfiguration();
         }
         break;

      //--- Switch mode selective -------------------------
      case eLSS_CMD_MODE_SEL_VENDOR_ID:
      case eLSS_CMD_MODE_SEL_PRODUCT_CODE:
      case eLSS_CMD_MODE_SEL_REVISION_NUM:
      case eLSS_CMD_MODE_SEL_SERIAL_NUM:
         CosLssSwitchModeSelective(ubLssCommandT);
         break;

      //--- Inquire service -------------------------------
      case eLSS_CMD_INQUIRE_VENDOR_ID:
      case eLSS_CMD_INQUIRE_PRODUCT_CODE:
      case eLSS_CMD_INQUIRE_REVISION_NUM:
      case eLSS_CMD_INQUIRE_SERIAL_NUM:
      case eLSS_CMD_INQUIRE_NODE_ID:
         if(ubCosLssModeS == eLSS_MODE_CONFIG)
         {
            CosLssInquiryService(ubLssCommandT);
         }
         break;

      //--- Unknown command -------------------------------
      default:

         break;
   }
}


//----------------------------------------------------------------------------//
// CosLssStoreConfiguration()                                                 //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLssStoreConfiguration(void)
{
   uint16_t  uwDataT;
   uint8_t  ubDataCntT;


   //----------------------------------------------------------------
   // store actual baudrate and node-id into non-volatile memory

   //--------------------------------------------------------
   // enable write operation
   //
   McNvmWriteEnable();


   //--------------------------------------------------------
   // store LSS parameter
   //
   McNvmWrite(eNVM_305_BAUDRATE_U08, &ubCosLssBaudrateS, 1);
   McNvmWrite(eNVM_305_NODE_ID_U08 , &ubCosLssNodeIdS  , 1);


   //----------------------------------------------------------------
   // build checksum and disable write operation
   //
   uwDataT = McNvmBuildChecksum(eNVM_CHECKSUM_START, eNVM_CHECKSUM_END);
   McNvmWrite(eNVM_CHECKSUM_U16, &uwDataT, 2);
   McNvmWriteDisable();


   //--------------------------------------------------------
   // send response message
   //
   aubCosLssTrmDataS[0] = eLSS_CMD_CONFIG_STORE;
   for(ubDataCntT = 1; ubDataCntT < 8; ubDataCntT++)
   {
      aubCosLssTrmDataS[ubDataCntT] = 0x00;
   }

   CpCoreBufferSetData(&tsCanPortG, eCosBuf_LSS_TRM, &aubCosLssTrmDataS[0]);
   CpCoreBufferSend(&tsCanPortG, eCosBuf_LSS_TRM);


}


//----------------------------------------------------------------------------//
// CosLssSwitchModeGlobal()                                                   //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLssSwitchModeGlobal(void)
{
   if( aubCosLssRcvDataS[1] == 0x00)
   {
      ubCosLssModeS = eLSS_MODE_WAIT;
      return;
   }

   if( aubCosLssRcvDataS[1] == 0x01)
   {
      ubCosLssModeS = eLSS_MODE_CONFIG;
      return;
   }

}


//----------------------------------------------------------------------------//
// CosLssSwitchModeSelective()                                                //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLssSwitchModeSelective(uint8_t ubServiceV)
{
   uint32_t  ulDataT;       // 32-bit value that is compared to object 1018h
   uint8_t  ubDataCntT;    // counter for response message

   //----------------------------------------------------------------
   // read value from message buffer
   //
   ulDataT = aubCosLssRcvDataS[4];
   ulDataT = ulDataT << 8;
   ulDataT = ulDataT | aubCosLssRcvDataS[3];
   ulDataT = ulDataT << 8;
   ulDataT = ulDataT | aubCosLssRcvDataS[2];
   ulDataT = ulDataT << 8;
   ulDataT = ulDataT | aubCosLssRcvDataS[1];


   //----------------------------------------------------------------
   // compare the value with object 1018h
   //
   switch(ubServiceV)
   {
      case eLSS_CMD_MODE_SEL_VENDOR_ID:
         if(ulDataT == ulIdx1018_VendorIdC)
         {
            ubCosLssModeS |= eLSS_MODE_SEL_VENDOR_ID;
         }
         break;

      case eLSS_CMD_MODE_SEL_PRODUCT_CODE:
         if(ulDataT == ulIdx1018_ProductCodeC)
         {
            ubCosLssModeS |= eLSS_MODE_SEL_PRODUCT_CODE;
         }
         break;

      case eLSS_CMD_MODE_SEL_REVISION_NUM:
         if(ulDataT == ulIdx1018_RevisionNumC)
         {
            ubCosLssModeS |= eLSS_MODE_SEL_REVISION_NUM;
         }
         break;

      case eLSS_CMD_MODE_SEL_SERIAL_NUM:
         if(ulDataT == CosMgrGetSerialNumber())
         {
            ubCosLssModeS |= eLSS_MODE_SEL_SERIAL_NUM;
         }
         break;

      default:
         break;
   }


   //----------------------------------------------------------------
   // if all four values are equal, we are in configuration mode
   //
   if(ubCosLssModeS == eLSS_MODE_CONFIG)
   {
      aubCosLssTrmDataS[0] = eLSS_CMD_MODE_SEL_RESPONSE;

      for(ubDataCntT = 1; ubDataCntT < 8; ubDataCntT++)
      {
         aubCosLssTrmDataS[ubDataCntT] = 0x00;
      }

      //--------------------------------------------------------
      // send response message
      //
      CpCoreBufferSetData(&tsCanPortG, eCosBuf_LSS_TRM, &aubCosLssTrmDataS[0]);
      CpCoreBufferSend(&tsCanPortG, eCosBuf_LSS_TRM);

   }
}

#endif   // COS_LSS_SUPPORT > 0

