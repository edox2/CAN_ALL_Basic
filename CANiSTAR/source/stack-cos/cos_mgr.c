//****************************************************************************//
// File:          cos_mgr.c                                                   //
// Description:   Init and control functions of the CANopen slave             //
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
// 16.05.2002  Added LSS support                                              //
// 09.09.2002  Added CAN Error Handler support / rename CosMgrMessageHandler  //
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


#include "cos_mgr.h"

#include "cos_dict.h"            // object dictionary
#include "cos_emcy.h"            // Emergency service
#include "cos_led.h"             // LED support
#include "cos_lss.h"             // LSS support
#include "cos_nmt.h"             // NMT service
#include "cos_nvm.h"             // Non-volatile memory access
#include "cos_pdo.h"             // PDO service
#include "cos_sdo.h"             // SDO services
#include "cos_sync.h"            // SYNC service
#include "cos_time.h"            // TIME service
#include "cos301.h"              // Variables from CiA 301

#if COS_LSS_SUPPORT > 1
#include "com_lss.h"             // LSS master
#endif

#if COS_SDO_CLIENT > 0
#include "com_sdo_cln.h"         // SDO client
#endif

#if COS_NMT_MASTER > 0
#include "com_mgr.h"
#endif

#if COS_DICT_MAN > 0
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

#if COS_DS402 > 0
#include "cos402.h"              // Objects from CiA 402, analogue output
#endif

#if COS_DS404_AI > 0
#include "cos404ai.h"            // Objects from CiA 404, analogue input
#endif

#if COS_DS404_AO > 0
#include "cos404ao.h"            // Objects from CiA 404, analogue output
#endif

#if COS_DS404_AL > 0
#include "cos404al.h"            // Objects from CiA 404, alarm block
#endif

#if COS_DS406 > 0
#include "cos406.h"              // Objects from CiA 406, encoder
#endif

/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

uint8_t        ubCosMgrNodeAddressG;      // node address
uint8_t        ubCosMgrBaudrateG;         // baudrate (required for LSS)
#if CP_SMALL_CODE == 0
CpPort_ts      tsCanPortG;                // CAN interface
#endif


#if COS_DICT_OBJ_1010 > 0
extern uint8_t  ubCos301ParmSaveG;        // requested save operation
extern uint8_t  ubCos301ParmLoadG;        // requested load operation
#endif


#if COS_MGR_INT == 0
extern uint32_t         ulCpRcvBufferFlagG;
extern uint32_t         ulCpTrmBufferFlagG;
extern CpCanMsg_ts      atsCanMsgG[];
#endif

uint8_t  ubCosMgrStatusG;                 // status of CANopen stack
uint16_t uwCosMgrConfigG;                 // configuration of CANopen stack

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------//
// CosMgrCanErrHandler()                                                      //
// do error handling                                                          //
//----------------------------------------------------------------------------//
uint8_t  CosMgrCanErrHandler(CpState_ts * ptsStateV)
{
   static uint8_t ubLastErrorT = CP_STATE_BUS_ACTIVE;

   #if COS_BUS_EMCY == 1
   uint8_t  aubEmcyAddCode[5];         // additional code for emergency message
   #endif

   //----------------------------------------------------------------
   // notify state changes via LED and/or Emergency messages
   //
   switch(ptsStateV->ubCanErrState)
   {

      //---------------------------------------------------
      // active state, no error
      //
      case CP_STATE_BUS_ACTIVE:

         //-------------------------------------------
         // clear error register: communication error
         //
         ubIdx1001_ErrorRegisterG &= ~eCoErrReg_COMMUNICATION;

         #if COS_LED_SUPPORT > 0
         CosLedNetworkError(eCosLedErr_OK);
         #endif

         #if COS_BUS_EMCY == 1
         if(ubLastErrorT != CP_STATE_BUS_ACTIVE)
         {
            if(ubLastErrorT == CP_STATE_BUS_OFF)
            {
               CosEmcySend(EMCY_ERR_CAN_BUSOFF_RECOVER, (uint8_t *) 0);
            }
            CosEmcySend(EMCY_ERR_NONE, (uint8_t *) 0);
         }
         #endif

         ubLastErrorT = CP_STATE_BUS_ACTIVE;
         break;


      //---------------------------------------------------
      // bus warning status
      //
      case CP_STATE_BUS_WARN:

         //-------------------------------------------
         // update error register: communication error
         //
         ubIdx1001_ErrorRegisterG |= eCoErrReg_COMMUNICATION;

         #if COS_LED_SUPPORT > 0
         CosLedNetworkError(eCosLedErr_BUS_WARN);
         #endif

         #if COS_BUS_EMCY == 1
         if(ubLastErrorT != CP_STATE_BUS_WARN)
         {
            aubEmcyAddCode[0] = ptsStateV->ubCanErrState;
            aubEmcyAddCode[1] = ptsStateV->ubCanErrType;
            aubEmcyAddCode[2] = ptsStateV->ubCanRcvErrCnt;
            aubEmcyAddCode[3] = ptsStateV->ubCanTrmErrCnt;
            aubEmcyAddCode[4] = 0x00;
            CosEmcySend(EMCY_ERR_COMMUNICATION, &aubEmcyAddCode[0]);
         }
         #endif


         ubLastErrorT = CP_STATE_BUS_WARN;
         break;


      //---------------------------------------------------
      // error passive
      //
      case CP_STATE_BUS_PASSIVE:

         //-------------------------------------------
         // update error register: communication error
         //
         ubIdx1001_ErrorRegisterG |= eCoErrReg_COMMUNICATION;

         #if COS_LED_SUPPORT > 0
         CosLedNetworkError(eCosLedErr_BUS_PASSIVE);
         #endif

         #if COS_BUS_EMCY == 1
         if(ubLastErrorT != CP_STATE_BUS_PASSIVE)
         {
            aubEmcyAddCode[0] = ptsStateV->ubCanErrState;
            aubEmcyAddCode[1] = ptsStateV->ubCanErrType;
            aubEmcyAddCode[2] = ptsStateV->ubCanRcvErrCnt;
            aubEmcyAddCode[3] = ptsStateV->ubCanTrmErrCnt;
            aubEmcyAddCode[4] = 0x00;
            CosEmcySend(EMCY_ERR_CAN_ERROR_PASSIVE, &aubEmcyAddCode[0]);
         }
         #endif


         ubLastErrorT = CP_STATE_BUS_PASSIVE;
         break;

      //---------------------------------------------------
      // bus off
      //
      case CP_STATE_BUS_OFF:

         #if COS_LED_SUPPORT > 0
         CosLedNetworkError(eCosLedErr_BUS_OFF);
         #endif
         if(ubLastErrorT != CP_STATE_BUS_OFF)
         {
            CosMgrOnBusOff();
            ubCosMgrStatusG = eCOS_MGR_BUS_OFF;
            ubLastErrorT = CP_STATE_BUS_OFF;
         }
         break;

      //---------------------------------------------------
      // Unknown state, handle like bus off
      // i.e. CP_STATE_STOPPED, CP_STATE_SLEEPING
      //
      default:
         #if COS_LED_SUPPORT > 0
         CosLedNetworkError(eCosLedErr_BUS_OFF);
         #endif
         CosMgrOnBusOff();
         ubCosMgrStatusG = eCOS_MGR_BUS_OFF;
         ubLastErrorT = CP_STATE_BUS_OFF;
         break;
   }

   return(CP_CALLBACK_PROCESSED);
}


//----------------------------------------------------------------------------//
// CosMgrCanRcvHandler()                                                      //
// do receive message handling                                                //
//----------------------------------------------------------------------------//
/*!
** \callgraph
*/
uint8_t CosMgrCanRcvHandler(CpCanMsg_ts * ptsCanMsgV, uint8_t ubBufferIdxV)
{

   //----------------------------------------------------------------
   // test for buffer overrun
   //
   if(CpMsgIsOverrun(ptsCanMsgV))
   {
      CosEmcySend(EMCY_ERR_CAN_OVERRUN, 0L);
   }

   //----------------------------------------------------------------
   // proceed according to the buffer index, i.e. a specific
   // CANopen service
   //
   switch(ubBufferIdxV)
   {

      //---------------------------------------------------
      // NMT messages
      //
      case eCosBuf_NMT:
         CosNmtMessageHandler();
         break;


      //---------------------------------------------------
      // SYNC messages
      // handled only in Operational mode
      //
      #if COS_SYNC_SUPPORT > 0
      case eCosBuf_SYNC:
         if( CosNmtGetNodeState() == NODE_STATE_OPERATIONAL)
         {
            CosSyncMessageHandler();
         }
         break;
      #endif

      //---------------------------------------------------
      // SDO messages
      //
      case eCosBuf_SDO_RCV:
         if( CosNmtGetNodeState() != NODE_STATE_STOPPED)
         {
            CosSdoMessageHandler();
         }
         break;


      //---------------------------------------------------
      // NMT error messages (Node Guarding)
      // the request must be a remote frame
      //
      #if COS_DICT_OBJ_100C == 1
      case eCosBuf_NMT_ERR:
         if(CpMsgIsRemote(ptsCanMsgV))
         {
            CosNmtGuardingHandler();
         }
         break;
      #endif

      //---------------------------------------------------
      // NMT heartbeat consumer messages
      // it is limited to 4 heartbeat consumers here
      //
      #if COS_DICT_OBJ_1016 > 0
      case (eCosBuf_NMT_HBC + 0):
         CosNmtHBConsHandler(0);
         break;
      #endif

      #if COS_DICT_OBJ_1016 > 1
      case (eCosBuf_NMT_HBC + 1):
         CosNmtHBConsHandler(1);
         break;
      #endif

      #if COS_DICT_OBJ_1016 > 2
      case (eCosBuf_NMT_HBC + 2):
         CosNmtHBConsHandler(2);
         break;
      #endif

      #if COS_DICT_OBJ_1016 > 3
      case (eCosBuf_NMT_HBC + 3):
         CosNmtHBConsHandler(3);
         break;
      #endif

      //---------------------------------------------------
      // TIME consumer messages
      //
      #if COS_DICT_OBJ_1012 > 0
      case eCosBuf_TIME:
         CosTmrTimeStampHandler();
         break;
      #endif

      //---------------------------------------------------
      // LSS messages (slave only)
      //
      #if COS_LSS_SUPPORT == 1
      case eCosBuf_LSS_RCV:
         if( CosNmtGetNodeState() != NODE_STATE_OPERATIONAL)
         {
            CosLssMessageHandler();
         }
         break;
      #endif

      //---------------------------------------------------
      // LSS messages (slave and master)
      //
      #if COS_LSS_SUPPORT == 2
      case eCosBuf_LSS_RCV:
         if(uwCosMgrConfigG & CO_CONF_MASTER)
         {
            ComLssMessageHandler();
         }
         else
         {
            if( CosNmtGetNodeState() != NODE_STATE_OPERATIONAL)
            {
               CosLssMessageHandler();
            }
         }
         break;
      #endif

      //---------------------------------------------------
      // SDO client messages
      //
      #if COS_SDO_CLIENT > 0
      case eCosBuf_SDOC_RCV:
         ComSdoMessageHandler();
         break;
      #endif

      //---------------------------------------------------
      // Receive PDO messages
      //
      #if COS_PDO_RCV_NUMBER > 0
      case (eCosBuf_PDO1_RCV + 0):
         if( CosNmtGetNodeState() != NODE_STATE_OPERATIONAL) break;
          #if COS_PDO_MAPPING == 0
         CosPdoRcvDataUpdate(0);
         #else
         CosPdoReceive(0);
         #endif
         break;
      #endif

      #if COS_PDO_RCV_NUMBER > 1
      case (eCosBuf_PDO1_RCV + 1):
         if( CosNmtGetNodeState() != NODE_STATE_OPERATIONAL) break;
         #if COS_PDO_MAPPING == 0
         CosPdoRcvDataUpdate(1);
         #else
         CosPdoReceive(1);
         #endif
         break;
      #endif

      #if COS_PDO_RCV_NUMBER > 2
      case (eCosBuf_PDO1_RCV + 2):
         if( CosNmtGetNodeState() != NODE_STATE_OPERATIONAL) break;
         #if COS_PDO_MAPPING == 0
         CosPdoRcvDataUpdate(2);
         #else
         CosPdoReceive(2);
         #endif
         break;
      #endif

      #if COS_PDO_RCV_NUMBER > 3
      case (eCosBuf_PDO1_RCV + 3):
         if( CosNmtGetNodeState() != NODE_STATE_OPERATIONAL) break;
         #if COS_PDO_MAPPING == 0
         CosPdoRcvDataUpdate(3);
         #else
         CosPdoReceive(3);
         #endif
         break;
      #endif

      //---------------------------------------------------
      // if global reception is enabled message are
      // handled via the ComMgrCanRcvHandler() function
      //
      #if CP_GLOBAL_RCV_ENABLE > 0
      case CP_GLOBAL_RCV_BUFFER:
         ComMgrCanRcvHandler(&tsCanPortG, ptsCanMsgV);
         break;
      #endif

      //---------------------------------------------------
      // all other messages may come from a transmit
      // buffer, so send an emergency here
      //
      default:
         CosEmcySend(EMCY_ERR_CAN_TX_COB_COLLISION, 0L);
         break;
   }

   //--- do not put message into FIFO -------------------------------
   return(CP_CALLBACK_PROCESSED);
}


//----------------------------------------------------------------------------//
// CosMgrCanTransmitHandler()                                                 //
// do transmit message handling                                               //
//----------------------------------------------------------------------------//
uint8_t CosMgrCanTrmHandler(CpCanMsg_ts * CPP_PARM_UNUSED(ptsCanMsgV),
                            uint8_t ubBufferIdxV)
{

   //----------------------------------------------------------------
   // set the state to eCOS_MGR_RUN after the boot-up message
   // has been sent
   //
   if(ubCosMgrStatusG == eCOS_MGR_INIT)
   {
      switch(ubBufferIdxV)
      {
         case eCosBuf_NMT_ERR:
            ubCosMgrStatusG = eCOS_MGR_RUN;

            #if COS_DICT_OBJ_100C == 1
            if(uwCosNmtHeartTimeG == 0) CosNmtErrorSetup(1);
            #endif
            CosNmtSetHeartbeatProd(uwCosNmtHeartTimeG);

            //--------------------------------------------------
            // set network LED according to NMT state
            //
            #if COS_LED_SUPPORT > 0
            if(CosNmtGetNodeState() == NODE_STATE_OPERATIONAL)
            {
               CosLedNetworkStatus(eCosLedNet_OPERATIONAL);
            }
            else
            {
               CosLedNetworkStatus(eCosLedNet_PREOPERATIONAL);
            }
            #endif

            break;

         default:

            break;
      }

      return(CP_CALLBACK_PROCESSED);
   }


   //----------------------------------------------------------------
   // when the node is up and running (state = eCOS_MGR_RUN), then
   // we modify the buffer direction for nodeguarding here
   //
   switch(ubBufferIdxV)
   {
      case eCosBuf_NMT_ERR:
         #if COS_DICT_OBJ_100C == 1
         if(uwCosNmtHeartTimeG == 0) CosNmtErrorSetup(1);
         #endif
         break;

      default:

         break;
   }

   //--- do not put message into FIFO -------------------------------
   return(CP_CALLBACK_PROCESSED);
}


//----------------------------------------------------------------------------//
// CosMgrIdCheck()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CosMgrIdCheck(uint32_t ulIdentifierV)
{
   //----------------------------------------------------------------
   // mask the upper 3 bits (which have a special meaning in CANopen)
   //
   ulIdentifierV = ulIdentifierV & 0x1FFFFFFF;

   //----------------------------------------------------------------
   // test for NMT service identifier
   //
   if(ulIdentifierV == 0x00000000) return(0);

   //----------------------------------------------------------------
   // test for identifier 001h
   //
   if(ulIdentifierV == 0x00000001) return(0);

   //----------------------------------------------------------------
   // test for identifier 071h .. 07Fh
   //
   if( (ulIdentifierV >= 0x00000071) &&
       (ulIdentifierV <= 0x0000007F)         ) return(0);

   //----------------------------------------------------------------
   // test for SDO Tx identifier
   //
   if( (ulIdentifierV >= 0x00000581) &&
       (ulIdentifierV <= 0x000005FF)         ) return(0);

   //----------------------------------------------------------------
   // test for SDO Rx identifier
   //
   if( (ulIdentifierV >= 0x00000601) &&
       (ulIdentifierV <= 0x0000067F)         ) return(0);


   //----------------------------------------------------------------
   // test for NMT error control identifier
   //
   if( (ulIdentifierV >= 0x00000701) &&
       (ulIdentifierV <= 0x0000077F)         ) return(0);

   //----------------------------------------------------------------
   // test for reserved identifier
   //
   if( (ulIdentifierV >= 0x00000780) &&
       (ulIdentifierV <= 0x000007FF)         ) return(0);

   //----------------------------------------------------------------
   // identifier does not match, it is accepted
   //
   return(1);
}


//----------------------------------------------------------------------------//
// CosMgrInit()                                                               //
// Initialise the CANopen Slave                                               //
//----------------------------------------------------------------------------//
uint8_t CosMgrInit(uint8_t ubCanIfV, uint16_t uwConfigV)
{
   uint8_t           ubInitFailT;   // failure in init routine


   //----------------------------------------------------------------
   // setup the CAN interface
   //
   #if CP_SMALL_CODE == 1
   CpCoreDriverInit(CP_CHANNEL_1, &tsCanPortG);
   ubCanIfV = 0;     // dummy operation to avoid compiler warning
   #else
   CpCoreDriverInit(ubCanIfV, &tsCanPortG);
   #endif

   //----------------------------------------------------------------
   // we are in initialisation mode
   //
   ubCosMgrStatusG = eCOS_MGR_INIT;

   //----------------------------------------------------------------
   // store configuration option
   //
   uwCosMgrConfigG = uwConfigV;


   //----------------------------------------------------------------
   // clear initialisation failure
   // bit 0: EEPROM read failure
   //
   ubInitFailT = 0;


   //----------------------------------------------------------------
   // initialise CANopen LED management
   //
   #if COS_LED_SUPPORT > 0
   CosLedInit();
   #endif


   return(ubInitFailT);
}


//----------------------------------------------------------------------------//
// CosMgrNvmOperation()                                                       //
// run non-volatile memory operation                                          //
//----------------------------------------------------------------------------//
void  CosMgrNvmOperation(void)
{
   #if COS_DICT_OBJ_1010 > 0
   uint16_t  uwParmSelT;          // the setup for load/restore
   uint16_t  uwNvmChecksumT;      // checksum for restore operation


   //----------------------------------------------------------------
   // check for a save operation, issued by writing on index 1010h
   //
   if(ubCos301ParmSaveG > 0)
   {
      //--------------------------------------------------------
      // Set the highest bit in the 'ubCos301ParmSaveG'
      // variable. This is a flag for a save operation
      // in progress. The function Cos301_Idx1010() checks
      // for this flag.
      //
      ubCos301ParmSaveG |= 0x80;

      //--------------------------------------------------------
      // save parameters to EEPROM
      // only the last three bits are valid (values 0 .. 4)
      // send deferred SDO response
      //
      if(CosMgrParmSave(ubCos301ParmSaveG & 0x07) == eCosErr_OK)
      {
         CosSdoResponse(eCosSdo_WRITE_OK);
      }
      else
      {
         CosSdoResponse(eCosSdo_ERR_HARDWARE);
      }

      ubCos301ParmSaveG = 0;
   }


   //----------------------------------------------------------------
   // check for a restore default operation, issued by writing on
   // index 1011h
   //
   if(ubCos301ParmLoadG > 0)
   {
      //--------------------------------------------------------
      // Set the highest bit in the 'ubCos301ParmLoadG'
      // variable. This is a flag for a restore default
      // operation in progress.
      // The function Cos301_Idx1011() checks for this flag.
      //
      ubCos301ParmLoadG |= 0x80;

      //--------------------------------------------------------
      // read current setting from EEPROM:
      // the parameter eNVM_PARM_SEL has a magic code in the 12 MSB
      // (0x5ACx) to see if the EEPROM was configured before
      // the lower nibble tells us which parameters shall be loaded:
      // 0xC0: no parameter
      // 0xC1: communication profile
      // 0xC2: device profile
      // 0xC4: manufacturer profile
      //
      // Combinations are possible, e.g.  0xC7 = all parameters
      //
      // | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
      // -----------------------------------------------------------------
      // |   1   |   1   |   0   |   0   |   x   |Manuf. |DS-4xx |DS-301 |
      //
      McNvmRead(eNVM_PARM_SEL_U16, &uwParmSelT, 2);
      if( (uwParmSelT & 0xFFF0) != 0x5AC0)
      {
         //------------------------------------------------
         // data has not been stored in EEPROM
         // set restore default to all parameters
         //
         uwParmSelT = 0x5AC0;
         ubCos301ParmLoadG = eCOS_PARM_ALL | 0x80;
      }

      //--------------------------------------------------------
      // now set the lower nibble of 'uwParmSelT' according to
      // the choice
      //
      switch(ubCos301ParmLoadG & 0x07)
      {
         case eCOS_PARM_ALL:
            uwParmSelT = uwParmSelT & 0xFFF0;
            break;

         case eCOS_PARM_COM:
            uwParmSelT = uwParmSelT & 0xFFFE;
            break;

         case eCOS_PARM_APP:
            uwParmSelT = uwParmSelT & 0xFFFD;
            break;

         case eCOS_PARM_MAN:
            uwParmSelT = uwParmSelT & 0xFFFB;
            break;

         default:
            uwParmSelT = uwParmSelT & 0xFFF0;
            break;
      }


      //--------------------------------------------------------
      // enable write operation
      //
      McNvmWriteEnable();


      //--------------------------------------------------------
      // store new setup
      //
      McNvmWrite(eNVM_PARM_SEL_U16, &uwParmSelT, 2);


      //--------------------------------------------------------
      // build checksum and disable write operation
      //
      uwNvmChecksumT = McNvmBuildChecksum(eNVM_CHECKSUM_START,
                                          eNVM_CHECKSUM_END);
      McNvmWrite(eNVM_CHECKSUM_U16, &uwNvmChecksumT, 2);
      McNvmWriteDisable();

      //--------------------------------------------------------
      // send deferred SDO response
      //
      CosSdoResponse(eCosSdo_WRITE_OK);
      ubCos301ParmLoadG = 0;
   }

   #endif
}


//----------------------------------------------------------------------------//
// CosMgrParmInit()                                                           //
// Initialise objects of communication and device profile                     //
//----------------------------------------------------------------------------//
void CosMgrParmInit(void)
{
   //----------------------------------------------------------------
   // Setup default parameter for the communication profile
   //
   Cos301_ParmInit();


   //----------------------------------------------------------------
   // Setup default parameter for the different device profiles:
   // The following routines perform the default setup for the
   // different device profiles, which is equivalent to a "restore
   // default parameter" command. If there is no non-volatile memory
   // or there are no stored parameters, then this is the default.
   //
   #if COS_DS401_DI > 0
   Cos401_DI_ParmInit();      // CiA 401, init digital input
   #endif

   #if COS_DS401_DO > 0
   Cos401_DO_ParmInit();      // CiA 401, init digital output
   #endif

   #if COS_DS401_AI > 0
   Cos401_AI_ParmInit();      // CiA 401, init analogue input
   #endif

   #if COS_DS401_AO > 0
   Cos401_AO_ParmInit();      // CiA 401, init analogue output
   #endif

   #if COS_DS402 > 0
   Cos402_ParmInit();         // CiA 402, init parameters
   #endif

   #if COS_DS404_AI > 0
   Cos404_AI_ParmInit();      // CiA 404, init analogue input
   #endif

   #if COS_DS404_AO > 0
   Cos404_AO_ParmInit();      // CiA 404, init analogue output
   #endif

   #if COS_DS404_AL > 0
   Cos404_AL_ParmInit();      // CiA 404, init alarm functions
   #endif

   #if COS_DS406 > 0
   Cos406_ParmInit();         // CiA 404, init parameters
   #endif

   #if COS_DS410 > 0
   Cos410_ParmInit();         // CiA 410, init parameters
   #endif

   #if COS_DS417 > 0
   Cos417_ParmInit();         // CiA 417, init parameters
   #endif

   #if COS_DS437 > 0
   Cos437_ParmInit();         // CiA 437, init parameters
   #endif


   //----------------------------------------------------------------
   // Setup default parameter for manufacturer objects
   //
   #if COS_DICT_MAN > 0
   CosMob_ParmInit();
   #endif
}



//----------------------------------------------------------------------------//
// CosMgrParmLoad()                                                           //
// load all objects of communication and device profile                       //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1010 > 0
uint8_t CosMgrParmLoad(void)
{
   uint16_t  uwParmSelT;          // the setup
   uint16_t  uwNvmChecksumT;      // parameter checksum value


   //----------------------------------------------------------------
   // read current setting from EEPROM:
   // the parameter eNVM_PARM_SEL has a magic code in the 12 MSB
   // (0x5ACx) to see if the EEPROM was configured before
   // the lower nibble tells us which parameters shall be loaded:
   // 0xC0: no parameter
   // 0xC1: communication profile
   // 0xC2: device profile
   // 0xC4: manufacturer profile
   //
   // Combinations are possible, e.g.  0xC7 = all parameters
   //
   // | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
   // -----------------------------------------------------------------
   // |   1   |   1   |   0   |   0   |   x   |Manuf. |DS-4xx |DS-301 |
   //
   McNvmRead(eNVM_PARM_SEL_U16, &uwParmSelT, 2);
   if( (uwParmSelT & 0xFFF0) != 0x5AC0)
   {
      return(eCosErr_PARM_LOAD);
   }


   //----------------------------------------------------------------
   // build checksum and compare with stored value
   // in case of failure:
   // - return error code
   //
   McNvmRead(eNVM_CHECKSUM_U16, &uwNvmChecksumT, 2);
   if(uwNvmChecksumT != McNvmBuildChecksum(eNVM_CHECKSUM_START, eNVM_CHECKSUM_END))
   {
      return(eCosErr_PARM_LOAD);
   }


   //----------------------------------------------------------------
   // load parameters of CiA 301
   //
   if(uwParmSelT & 0x0001)
   {
      Cos301_ParmLoad();
   }


   //----------------------------------------------------------------
   // load parameters of CiA 4xx
   //
   if(uwParmSelT & 0x0002)
   {
      #if COS_DS401_DI > 0
      Cos401_DI_ParmLoad();      // CiA 401, load digital input
      #endif

      #if COS_DS401_DO > 0
      Cos401_DO_ParmLoad();      // CiA 401, load digital output
      #endif

      #if COS_DS401_AI > 0
      Cos401_AI_ParmLoad();      // CiA 401, load analogue input
      #endif

      #if COS_DS401_AO > 0
      Cos401_AO_ParmLoad();      // CiA 401, load analogue output
      #endif

      #if COS_DS402 > 0
      Cos402_ParmLoad();         // CiA 402, drive
      #endif

      #if COS_DS404_AI > 0
      Cos404_AI_ParmLoad();      // CiA 404, load analogue input
      #endif

      #if COS_DS404_AO > 0
      Cos404_AO_ParmLoad();      // CiA 404, load analogue output
      #endif

      #if COS_DS404_AL > 0
      Cos404_AL_ParmLoad();      // CiA 404, load alarm functions
      #endif

      #if COS_DS406 > 0
      Cos406_ParmLoad();         // CiA 406, encoder
      #endif

      #if COS_DS410 > 0
      Cos406_ParmLoad();         // CiA 410, inclinometer
      #endif

      #if COS_DS437 > 0
      Cos437_ParmLoad();         // CiA 437, photovoltaic
      #endif

   }


   //----------------------------------------------------------------
   // load manufacturer parameters
   //
   if(uwParmSelT & 0x0004)
   {
      #if COS_DICT_MAN > 0
      CosMob_ParmLoad();         // manufacturer objects
      #endif
   }

   return(eCosErr_OK);
}
#endif


//----------------------------------------------------------------------------//
// CosMgrParmSave()                                                           //
// save all objects of communication and device profile                       //
//----------------------------------------------------------------------------//
#if COS_DICT_OBJ_1010 > 0
uint8_t CosMgrParmSave(uint8_t ubGroupV)
{
   uint16_t  uwNvmChecksumT;              // checksum of non-volatile memory
   uint16_t  uwParmSelT;                  // the current setup
   uint8_t   ubNvmFailT = 0;              // status of NVM operation


   //----------------------------------------------------------------
   // read current default setting from EEPROM
   //
   if(McNvmRead(eNVM_PARM_SEL_U16, &uwParmSelT, 2) > eNVM_ERR_OK)
   {
      ubNvmFailT++;
   }
   if( (uwParmSelT & 0xFFF0) != 0x5AC0)
   {
      uwParmSelT = 0x5AC0;    // set signature
   }


   //----------------------------------------------------------------
   // enable write operation
   //
   if(McNvmWriteEnable() > eNVM_ERR_OK)
   {
      ubNvmFailT++;
   }


   //----------------------------------------------------------------
   // save setting for CiA 301
   //
   if( (ubGroupV == eCOS_PARM_ALL) || (ubGroupV == eCOS_PARM_COM) )
   {
      if(Cos301_ParmSave() > 0)
      {
         ubNvmFailT++;
      }
      uwParmSelT = uwParmSelT | 0x0001;
   }

   //----------------------------------------------------------------
   // save setting for CiA 4xx
   //
   if( (ubGroupV == eCOS_PARM_ALL) || (ubGroupV == eCOS_PARM_APP) )
   {
      #if COS_DS401_DI > 0
      Cos401_DI_ParmSave();      // CiA 401, save digital input
      #endif

      #if COS_DS401_DO > 0
      Cos401_DO_ParmSave();      // CiA 401, save digital output
      #endif

      #if COS_DS401_AI > 0
      Cos401_AI_ParmSave();      // CiA 401, save analogue input
      #endif

      #if COS_DS401_AO > 0
      Cos401_AO_ParmSave();      // CiA 401, save analogue output
      #endif

      #if COS_DS402 > 0
      Cos402_ParmSave();         // CiA 402, drive
      #endif

      #if COS_DS404_AI > 0
      Cos404_AI_ParmSave();      // CiA 404, save analogue input
      #endif

      #if COS_DS404_AO > 0
      Cos404_AO_ParmSave();      // CiA 404, save analogue output
      #endif

      #if COS_DS404_AL > 0
      Cos404_AL_ParmSave();      // CiA 404, save alarm functions
      #endif

      #if COS_DS406 > 0
      Cos406_ParmSave();         // CiA 406, save paramaters
      #endif

      #if COS_DS410 > 0
      Cos406_ParmSave();         // CiA 410, inclinometer
      #endif

      #if COS_DS437 > 0
      Cos437_ParmSave();         // CiA 437, photovoltaic
      #endif

      uwParmSelT = uwParmSelT | 0x0002;

   }

   //----------------------------------------------------------------
   // save setting for manufacturer specific objects
   //
   if( (ubGroupV == eCOS_PARM_ALL) || (ubGroupV == eCOS_PARM_MAN) )
   {
      #if COS_DICT_MAN > 0
      CosMob_ParmSave();
      #endif
      uwParmSelT = uwParmSelT | 0x0004;
   }


   //----------------------------------------------------------------
   // store new setup
   //
   if(McNvmWrite(eNVM_PARM_SEL_U16, &uwParmSelT, 2) > eNVM_ERR_OK)
   {
      ubNvmFailT++;
   }


   //----------------------------------------------------------------
   // build checksum and disable write operation
   //
   uwNvmChecksumT = McNvmBuildChecksum(eNVM_CHECKSUM_START, eNVM_CHECKSUM_END);
   McNvmWrite(eNVM_CHECKSUM_U16, &uwNvmChecksumT, 2);
   McNvmWriteDisable();


   return(ubNvmFailT);
}
#endif


//----------------------------------------------------------------------------//
// CosMgrProcess()                                                            //
// process all events for the CANopen stack                                   //
//----------------------------------------------------------------------------//
uint8_t CosMgrProcess(void)
{

   //----------------------------------------------------------------
   // process messages if no CAN interrupt handler is available
   //
   #if COS_MGR_INT == 0
   CosMgrProcessMsg();
   #endif


   //----------------------------------------------------------------
   // the operations are only performed when the node left the
   // init state
   //
   if(ubCosMgrStatusG == eCOS_MGR_RUN)
   {

      //--------------------------------------------------------
      // test for NMT reset requests:
      // - reset node
      // - reset communication
      //
      if(CosNmtCheckNodeReset() != NMT_RESET_OFF)
      {
         return(eCosErr_NODE_RESET);
      }

      //--------------------------------------------------------
      // perform data load / restore operations
      //
      CosMgrNvmOperation();


      return(eCosErr_OK);
   }

   //----------------------------------------------------------------
   // the node is in reset state
   //
   if(ubCosMgrStatusG == eCOS_MGR_STOP)
   {
      return(eCosErr_NODE_RESET);
   }

   //----------------------------------------------------------------
   // the node is not initialised
   //
   return(eCosErr_NODE_INIT);
}


//----------------------------------------------------------------------------//
// CosMgrProcessMsg()                                                         //
// process incoming messages, i.e. no CAN receive callback handler used       //
//----------------------------------------------------------------------------//
#if COS_MGR_INT == 0
void CosMgrProcessMsg(void)
{
   register uint8_t     ubBufferNumT = 0;
   register uint32_t    ulBufferMaskT = 1;
   static   CpState_ts  tsCanStateS;

   //----------------------------------------------------------------
   // Run through all possible buffers and check if there is
   // a message that need processing. We do this first for the
   // Rx buffers and then for the Tx buffers.
   //
   for(ubBufferNumT = 1; ubBufferNumT <= CP_BUFFER_MAX; ubBufferNumT++)
   {

      //---------------------------------------------------
      // test the Rx buffers
      //
      if(ulCpRcvBufferFlagG & ulBufferMaskT)
      {
         //--------------------------------------
         // process this buffer
         //
         CosMgrCanRcvHandler(&(atsCanMsgG[ubBufferNumT - 1]), ubBufferNumT);


         //--------------------------------------
         // clear flag for this buffer
         //
         ulCpRcvBufferFlagG &= ~ulBufferMaskT;
      }

      
      //---------------------------------------------------
      // test the Tx buffers
      //
      if(ulCpTrmBufferFlagG & ulBufferMaskT)
      {
         //--------------------------------------
         // process this buffer
         //
         CosMgrCanTrmHandler(&atsCanMsgG[ubBufferNumT - 1], ubBufferNumT);

         //--------------------------------------
         // clear flag for this buffer
         //
         ulCpTrmBufferFlagG &= ~ulBufferMaskT;

         //--------------------------------------
         // A CAN message was sent. Check for
         // pending transmit messages by calling
         // CpCoreBufferSend() with buffer index
         // 0 (invalid buffer). This is for
         // strange CAN controllers (e.g. PIC)
         // only.
         //
         CpCoreBufferSend(&tsCanPortG , 0);
      }

      ulBufferMaskT = ulBufferMaskT << 1;
   }

   
   //----------------------------------------------------------------
   // process error / status information
   //
   CpCoreCanState(&tsCanPortG, &tsCanStateS);
   CosMgrCanErrHandler(&tsCanStateS);

}
#endif


//----------------------------------------------------------------------------//
// CosMgrRelease()                                                            //
// release CANopen stack                                                      //
//----------------------------------------------------------------------------//
uint8_t CosMgrRelease(void)
{
   if(ubCosMgrStatusG != eCOS_MGR_STOP)
   {
      CpCoreCanMode(&tsCanPortG, CP_MODE_STOP);
      CpCoreIntFunctions(&tsCanPortG, 0L, 0L, 0L);
      CpCoreDriverRelease(&tsCanPortG);

      #if COS_LED_SUPPORT > 0
         CosLedNetworkError(eCosLedErr_BUS_OFF);
      #endif

      ubCosMgrStatusG = eCOS_MGR_STOP;
   }

   return(eCosErr_OK);
}


//----------------------------------------------------------------------------//
// CosMgrStart()                                                              //
// start CANopen stack                                                        //
//----------------------------------------------------------------------------//
uint8_t CosMgrStart(uint8_t ubBaudSelV, uint8_t ubNodeIdV)
{
   uint8_t           ubInitFailT;   // failure in init routine


   //----------------------------------------------------------------
   // clear init failure
   //
   ubInitFailT = 0;


   //----------------------------------------------------------------
   // setup the CAN interface
   //
   CpCoreBaudrate(&tsCanPortG, ubBaudSelV);


   //----------------------------------------------------------------
   // store baudrate (for LSS) and module address (for NMT)
   //
   ubCosMgrNodeAddressG = ubNodeIdV;
   ubCosMgrBaudrateG    = ubBaudSelV;

   //----------------------------------------------------------------
   // setup the parameters of the device
   //
   CosMgrParmInit();
   #if COS_DICT_OBJ_1010 > 0
   ubCos301ParmSaveG = eCOS_PARM_NONE;
   ubCos301ParmLoadG = eCOS_PARM_NONE;
   if( CosMgrParmLoad() == eCosErr_PARM_LOAD)
   {
      ubInitFailT |= 0x01;
   }
   #endif


   //----------------------------------------------------------------
   // setup the message buffers
   //


   //-----------------------------------------------------#
   // SYNC service
   //
   #if COS_SYNC_SUPPORT == 1
   CosSyncInit();
   #endif

   //-----------------------------------------------------#
   // EMCY service
   //
   CosEmcyInit();

   //-----------------------------------------------------#
   // SDO service
   //
   CosSdoInit(ubCosMgrNodeAddressG);


   //-----------------------------------------------------#
   // PDO service
   //
   #if COS_PDO_SUPPORT == 1
   CosPdoInit();
   #endif

   //-----------------------------------------------------#
   // TIME service
   //
   #if COS_DICT_OBJ_1012 > 0
   CosTmrTimeStampInit(ulCosTimeIdentifierG);
   #endif

   //-----------------------------------------------------#
   // LSS service
   //
   #if COS_LSS_SUPPORT == 2
   if(uwCosMgrConfigG & CO_CONF_MASTER)
   {
      ComLssInit();
   }
   else
   {
      CosLssInit();
   }
   #endif

   #if COS_LSS_SUPPORT == 1
   CosLssInit();
   #endif

   //-----------------------------------------------------#
   // SDO client service
   //
   #if COS_SDO_CLIENT > 0
   ComSdoInit();
   #endif

   //-----------------------------------------------------#
   // setup the callback function for message reception
   //
   #if COS_MGR_INT == 1
   CpCoreIntFunctions(&tsCanPortG,  CosMgrCanRcvHandler,
                                    CosMgrCanTrmHandler,
                                    CosMgrCanErrHandler);
   #endif



   CpCoreCanMode(&tsCanPortG, CP_MODE_START);

   //-----------------------------------------------------#
   // init NMT service and send Bootup message
   //
   CosNmtInit();


   //----------------------------------------------------------------
   // return value is 'ubInitFailT'
   //
   return(ubInitFailT);

}

