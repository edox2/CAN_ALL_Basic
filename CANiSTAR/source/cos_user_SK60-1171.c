//****************************************************************************//
// File:          cos_user_SK60-1171.c                                        //
// Description:   Program for the SK60-1171 PCB                               //
// Author:        Matthias Siegenthaler                                       //
// e-mail:        sigi@gengethoma.ch                                      					  //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Lindlaustr. 2c                                                             //
// 53842 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11  													  //
// e-mail:        koppe@microcontrol.net 									  //
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
// 06.10.2009  Initial version                                                //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date:2008-10-15 14:16:26 +0000 (Mi, 15 Okt 2008) $
// SVN  $Rev:1874 $ --- $Author:koppe $
//------------------------------------------------------------------------------



/*----------------------------------------------------------------------------*\
** IMPORTANT NOTE                                                             **
** This file provides a template for all functions that need to be adopted to **
** YOUR hardware. Copy this file to <myfile.c> and make your changes in the   **
** new file.                                                                  **
\*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


#include "cos_dict.h"            // Object dictionary
#include "cos_mgr.h"             // CANopen Slave management functions
#include "cos_nmt.h"             // NMT service
#include "cos_nvm.h"

#include "cos_emcy.h"

#include "cos_pdo.h"             // PDO service
#include "cos_sdo.h"             // SDO service
#include "cos_sync.h"            // SYNC service
#include "cos301.h"              // Objects from DS-301

#include "cos401di.h"
#include "cos401ai.h"


#if COS_DICT_MAN > 0
#include "cos_mobj.h"
#endif

//#include "mc_i2c.h"
#include "mc_cpu.h"


/*----------------------------------------------------------------------------*\
** Variables                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/

uint32_t   ulIdx1000_DeviceTypeC   = 0x000050191;



#if COS_DICT_OBJ_1008 == 1
uint8_t   ubIdx1008_DeviceNameC[] = "SK60-01174";
#endif


//-------------------------------------------------------------------
// this object holds the version number of the target hardware
// as visible string
//
#if COS_DICT_OBJ_1009 == 1
uint8_t   ubIdx1009_HwVersionC[]  = "1.00";
#endif


//-------------------------------------------------------------------
// this object holds the version number of the application
// firmware as visible string
//
#if COS_DICT_OBJ_100A == 1
uint8_t   ubIdx100A_SwVersionC[]  = "1.00";
#endif


uint8_t    ubIdx1018_SubNumberC    = 0x04;
uint32_t   ulIdx1018_VendorIdC     = 0x00000456;
uint32_t   ulIdx1018_ProductCodeC  = 0000275;
uint32_t   ulIdx1018_RevisionNumC  = COS_VERSION_MAJ | COS_VERSION_MIN | 0x0100;


//---------------------------------------------------------
// reserve some memory for data of Trm & Rcv PDO
//
#if COS_PDO_RCV_NUMBER > 0
   static uint8_t        aubRcvPdoDataT[8];
#endif

#if COS_PDO_TRM_NUMBER > 0
   static uint8_t        aubTrmPdoDataT[8];
#endif

//-------------------------------------------------------------------
// Devices with a fixed PDO mapping can have the mapping
// information in code space.
// The initialisation of the structure has to be made
// here. The initialisation code here is ONLY AN EXAMPLE!
// It has to be adopted to the device profile!
//

//-------------------------------------------------------------//
// Receive PDO mapping (fixed)                                 //
//                                                             //
//-------------------------------------------------------------//
#if (COS_PDO_RCV_NUMBER > 0) && (COS_PDO_MAPPING == 0)
CosPdoMap_ts atsRcvPdoMapG[] = {

   //--- Index 1600 -----------------------------
   { 0x6200, 0x01, 0x08 },    // sub-index 1
   { 0x0000, 0x00, 0x00 },

   //--- Index 1601 -----------------------------
   { 0x2003, 0x00, 0x10 },    // sub-index 1
   { 0x2003, 0x00, 0x10 },    // sub-index 2
   { 0x2003, 0x00, 0x10 },    // sub-index 3
   { 0x2003, 0x00, 0x10 },    // sub-index 4
   { 0x0000, 0x00, 0x00 },

   //--- Index 1602 -----------------------------
   { 0x2004, 0x00, 0x20 },    // sub-index 1
   { 0x0000, 0x00, 0x00 },

   //--- Index 1603 -----------------------------
   { 0x6200, 0x01, 0x08 },    // sub-index 1
   { 0x0000, 0x00, 0x00 }

};
#endif


//-------------------------------------------------------------//
// Transmit PDO mapping (fixed)                                //
//                                                             //
//-------------------------------------------------------------//
#if (COS_PDO_TRM_NUMBER > 0) && (COS_PDO_MAPPING == 0)
CosPdoMap_ts  atsTrmPdoMapG[] = {

   //--- Index 1A00 -----------------------------
   { 0x6000, 0x01, 0x08 },    // sub-index 1
   #if COS_DS401_DI > 1
   { 0x6000, 0x02, 0x08 },    // sub-index 2
   #endif
   { 0x0000, 0x00, 0x00 },

   //--- Index 1A01 -----------------------------
   { 0x6401, 0x01, 0x10 },    // sub-index 1
   #if COS_DS401_AI > 2
   { 0x6401, 0x03, 0x10 },    // sub-index 3
	 { 0x6401, 0x04, 0x10 },    // sub-index 4
   #endif
   { 0x0000, 0x00, 0x00 },

   //--- Index 1A02 -----------------------------
   { 0x6000, 0x01, 0x08 },    // sub-index 1
   { 0x0000, 0x00, 0x00 },

   //--- Index 1A03 -----------------------------
   { 0x6000, 0x01, 0x08 },    // sub-index 1
   { 0x0000, 0x00, 0x00 }

};
#endif


//-------------------------------------------------------------//
// Receive PDO mapping (dynamic)                               //
//                                                             //
//-------------------------------------------------------------//
#if (COS_PDO_RCV_NUMBER > 0) && (COS_PDO_MAPPING == 1)
const CosPdoDynMapList_ts  atsRcvPdoDynMapListG[] = {
   { 0x00000000, 0L                    },    // start (dummy) entry
   { 0x20020008, &ubCosMob_Var2002G    },    // index 1: 2002h
   { 0x20030010, &uwCosMob_Var2003G    },    // index 2: 2003h
   { 0x20040020, &ulCosMob_Var2004G    },    // index 3: 2004h
   #if MC_COMPILER > 1
   { 0x20050040, &uqCosMob_Var2005G    },    // index 4: 2005h
   #endif
   { 0xFFFFFFFF, 0L                    }     // final (dummy) entry
};


CosPdoDynMapParm_ts        atsRcvPdoDynMapParmG[] = {
   //----------------------------------------------------------------
   // mapping: 1 entry   - index 1           - DLC = 1
   { 0x01, {1, 0, 0, 0, 0, 0, 0, 0} , 1, {0x08, 0x00, 0x00, 0, 0, 0, 0, 0 } },
   //----------------------------------------------------------------
   // mapping: 2 entries - index 2 & 3       - DLC = 6
   { 0x02, {2, 3, 0, 0, 0, 0, 0, 0} , 6, {0x10, 0x20, 0x00, 0, 0, 0, 0, 0 } },
   //----------------------------------------------------------------
   // mapping: 3 entries - index 3 & 2 & 1   - DLC = 7
   { 0x03, {3, 2, 1, 0, 0, 0, 0, 0} , 7, {0x20, 0x10, 0x08, 0, 0, 0, 0, 0 } },

   #if MC_COMPILER > 1
   //----------------------------------------------------------------
   // mapping: 1 entry   - index 4           - DLC = 8
   { 0x01, {4, 0, 0, 0, 0, 0, 0, 0} , 8, {0x40, 0x00, 0x00, 0, 0, 0, 0, 0 } },
   #else
   //----------------------------------------------------------------
   // mapping: 1 entry   - index 3           - DLC = 4
   { 0x01, {3, 0, 0, 0, 0, 0, 0, 0} , 4, {0x20, 0x00, 0x00, 0, 0, 0, 0, 0 } },
   #endif
};
#endif

//-------------------------------------------------------------//
// Transmit PDO mapping (dynamic)                              //
//                                                             //
//-------------------------------------------------------------//
#if (COS_PDO_TRM_NUMBER > 0) && (COS_PDO_MAPPING == 1)
const CosPdoDynMapList_ts  atsTrmPdoDynMapListG[] = {
   { 0x00000000, 0L                    },    // first entry
   { 0x20020008, &ubCosMob_Var2002G    },
   { 0x20030010, &uwCosMob_Var2003G    },
   { 0x20040020, &ulCosMob_Var2004G    },
   #if MC_COMPILER > 1
   { 0x20050040, &uqCosMob_Var2005G    },
   #endif
   { 0xFFFFFFFF, 0L                    }     // last entry
};


CosPdoDynMapParm_ts        atsTrmPdoDynMapParmG[] = {
   //----------------------------------------------------------------
   // mapping: 1 entry   - index 1           - DLC = 1
   { 0x01, {1, 0, 0, 0, 0, 0, 0, 0} , 1, {0x08, 0x00, 0x00, 0, 0, 0, 0, 0 } },
   //----------------------------------------------------------------
   // mapping: 2 entries - index 2 & 3       - DLC = 6
   { 0x02, {2, 3, 0, 0, 0, 0, 0, 0} , 6, {0x10, 0x20, 0x00, 0, 0, 0, 0, 0 } },
   //----------------------------------------------------------------
   // mapping: 3 entries - index 3 & 2 & 1   - DLC = 7
   { 0x03, {3, 2, 1, 0, 0, 0, 0, 0} , 7, {0x20, 0x10, 0x08, 0, 0, 0, 0, 0 } },

   #if MC_COMPILER > 1
   //----------------------------------------------------------------
   // mapping: 1 entry   - index 4           - DLC = 8
   { 0x01, {4, 0, 0, 0, 0, 0, 0, 0} , 8, {0x40, 0x00, 0x00, 0, 0, 0, 0, 0 } },
   #else
   //----------------------------------------------------------------
   // mapping: 1 entry   - index 3           - DLC = 4
   { 0x01, {3, 0, 0, 0, 0, 0, 0, 0} , 4, {0x20, 0x00, 0x00, 0, 0, 0, 0, 0 } },
   #endif

};
#endif

/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/



//----------------------------------------------------------------------------//
// CosMgrGetBaudrate()                                                        //
// get module baudrate setting                                                //
//----------------------------------------------------------------------------//
uint8_t CosMgrGetBaudrate(void)
{
   uint8_t  ubBaudT;             // value for baudrate switch
   uint8_t	ubBitrateT;

	 uint8_t	aubI2C_DataT[4];
	
   //----------------------------------------------------------------
   // Example:
   // Read value of DIP-/HEX-switch here
   //
	 aubI2C_DataT[0] = 0xFF;
	 aubI2C_DataT[1] = 0xFF;

//   McI2C_DataRead(eI2C_NET_1, 0x40, 0x00,
//                         &aubI2C_DataT[0],
//                         2);
	
//	 ubBaudT = aubI2C_DataT[0] >> 7;
//	 ubBaudT = ubBaudT | (aubI2C_DataT[1] << 1);
//	 ubBaudT = ~ubBaudT;
//	 ubBaudT = ubBaudT & 0x07;
	
	 ubBaudT = 3;
	 switch(ubBaudT)
	 {
		 case 0:
			 ubBitrateT = CP_BAUD_1M;
		   break;
		 
		 case 1:
			 ubBitrateT = CP_BAUD_800K;
		   break;

		 case 2:
			 ubBitrateT = CP_BAUD_500K;
		   break;
		 
		 case 3:
			 ubBitrateT = CP_BAUD_250K;
		   break;
		 
		 case 4:
			 ubBitrateT = CP_BAUD_125K;
		   break;
		 
		 case 5:
			 ubBitrateT = CP_BAUD_50K;
		   break;
		
     default:
			 ubBitrateT = CP_BAUD_250K;
			 break;
	 }
	 


   return(ubBitrateT);
}


//----------------------------------------------------------------------------//
// CosMgrGetNodeAddress()                                                     //
// get module address setting                                                 //
//----------------------------------------------------------------------------//
uint8_t CosMgrGetNodeAddress(void)
{
   uint8_t  ubAddrT = 11;
   uint8_t	aubI2C_DataT[4];
	
   //----------------------------------------------------------------
   // Example:
   // Read value of DIP-/HEX-switch here
   //

//	 aubI2C_DataT[0] = 0xFF;
//	 aubI2C_DataT[1] = 0xFF;
//   McI2C_DataRead(eI2C_NET_1, 0x40, 0x00,
//                         &aubI2C_DataT[0],
//                         2);
//
//	 ubAddrT = ~aubI2C_DataT[0];
//	 ubAddrT = ubAddrT & 0x7F;
	
   if (McGetChannelId() == 1)
	 ubAddrT = 21;
   else
	 ubAddrT = 20;
	 return(ubAddrT);
}


//----------------------------------------------------------------------------//
// CosMgrGetSerialNumber()                                                    //
// get Serial Number of module                                                //
//----------------------------------------------------------------------------//
uint32_t  CosMgrGetSerialNumber(void)
{
   uint32_t  ulSerNumT;

   //----------------------------------------------------------------
   // read serial number from Non-volatile memory
   //
   ulSerNumT = 1;

   ulSerNumT = McGetSerialNumber();
   /*
   ulSerNumT = McNvmRead(eNVM_301_OBJ_1018_4_U32, &ulSerNumT, 4);
   */
   return(ulSerNumT);
}


//----------------------------------------------------------------------------//
// CosMgrOnBusOff()                                                           //
// Handler for Bus-Off condition                                              //
//----------------------------------------------------------------------------//
void CosMgrOnBusOff(void)
{

}

//----------------------------------------------------------------------------//
// CosNmtServiceOnError()                                                     //
// Handler for NMT protocol error                                             //
//----------------------------------------------------------------------------//
void  CosNmtServiceOnError(void)
{

}


//----------------------------------------------------------------------------//
// CosNmtServiceOnGuardingEvent()                                             //
// Function handler for Life Guarding Event                                   //
//----------------------------------------------------------------------------//
void  CosNmtServiceOnGuardingEvent(void)
{
   CosEmcySend(EMCY_ERR_CAN_LIFEGUARD, 0L);
}


//----------------------------------------------------------------------------//
// CosNmtServiceOnGuardingEvent()                                             //
// Function handler for Life Guarding Event                                   //
//----------------------------------------------------------------------------//
void  CosNmtServiceOnHeartbeatEvent(void)
{
   CosEmcySend(EMCY_ERR_CAN_LIFEGUARD, 0L);
}


//----------------------------------------------------------------------------//
// CosNmtServiceOnStart()                                                     //
// Function handler for NMT Start                                             //
//----------------------------------------------------------------------------//
void  CosNmtServiceOnStart(void)
{
   //----------------------------------------------------------------
   // this is an example for NMT start
   // it has to be adopted to the device profile / application
   //
   #if COS_PDO_TRM_NUMBER > 0
   CosPdoSend(0);
   #endif

   #if COS_PDO_TRM_NUMBER > 1
   CosPdoSend(1);
   #endif

   #if COS_PDO_TRM_NUMBER > 2
   CosPdoSend(2);
   #endif

   #if COS_PDO_TRM_NUMBER > 3
   CosPdoSend(3);
   #endif

}


//----------------------------------------------------------------------------//
// CosNmtServiceOnStop()                                                      //
// Function handler for NMT Stop                                              //
//----------------------------------------------------------------------------//
void  CosNmtServiceOnStop(void)
{

}


//----------------------------------------------------------------------------//
// CosNmtServiceOnPreOperational()                                            //
// Function handler for NMT Pre-Operational                                   //
//----------------------------------------------------------------------------//
void  CosNmtServiceOnPreOperational(void)
{

}


//----------------------------------------------------------------------------//
// CosPdoComSetup()                                                           //
// update receive PDO data                                                    //
//----------------------------------------------------------------------------//
void CosPdoComSetup(void)
{
   //----------------------------------------------------------------
   // This is an example for changing the default values of a PDO
   // communication parameter record. In this example the timer of
   // the transmit PDO is activated and the receive PDO transmission
   // type is set to 1 (SYNC)
   //

   #if COS_PDO_TRM_NUMBER > 0
   atsTrmPdoComG[0].ubTransType   = 0xFF;
   atsTrmPdoComG[0].uwEventTime   = 0000;
   #endif

   #if COS_PDO_TRM_NUMBER > 1
   atsTrmPdoComG[1].ubTransType   = 0xFF;
   atsTrmPdoComG[1].uwEventTime   = 10;
	 atsTrmPdoComG[1].uwEventTick   = 10;
   #endif

}


//----------------------------------------------------------------------------//
// CosPdoRcvDataUpdate()                                                      //
// update receive PDO data                                                    //
//----------------------------------------------------------------------------//
#if COS_PDO_RCV_NUMBER > 0
void CosPdoRcvDataUpdate(uint8_t ubPdoNumberV)
{
   //----------------------------------------------------------------
   // example for PDO receive update
   //
   CpCoreBufferGetData( &tsCanPortG,
                        eCosBuf_PDO1_RCV + ubPdoNumberV,
                        &aubRcvPdoDataT[0]);

}
#endif


//----------------------------------------------------------------------------//
// CosPdoTrmDataUpdate()                                                      //
// update transmit PDO data                                                   //
//----------------------------------------------------------------------------//
#if COS_PDO_TRM_NUMBER > 0
void CosPdoTrmDataUpdate(uint8_t ubPdoNumberV)
{
   //----------------------------------------------------------------
   // setup TPDO1 (digital inputs = buttons)
   //
	 if(ubPdoNumberV == 0)
	 {
      aubTrmPdoDataT[0] = Cos401_DI_Read(0);
 
	 }		 
	 else
	 {
		  aubTrmPdoDataT[0] = (uint8_t) Cos401_AI_GetAdcValue(0);
		  aubTrmPdoDataT[1] = (uint8_t) (Cos401_AI_GetAdcValue(0) >> 8);
	 }
   
   CpCoreBufferSetData( &tsCanPortG,
                        eCosBuf_PDO1_TRM + ubPdoNumberV,
                        &aubTrmPdoDataT[0]);

}
#endif


//----------------------------------------------------------------------------//
// CosSdoBlkUpObjectSize                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
#if COS_SDO_BLOCK > 0
uint32_t  CosSdoBlkUpObjectSize(uint16_t uwIndexV, uint8_t ubSubIndexV)
{
   uint32_t  ulObjSizeT = 0;



   return(ulObjSizeT);
}
#endif


//----------------------------------------------------------------------------//
// CosSdoSegFinal()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
uint8_t  CosSdoSegFinal(uint8_t ubReqCodeV, uint16_t uwIndexV,
                        uint8_t ubSubIndexV)
{
   //----------------------------------------------------------------
   // test parameters in order to avoid compiler warnings
   //
   if(ubReqCodeV == eSDO_WRITE_REQ_SEG)
   {
      if((uwIndexV == 0) && (ubSubIndexV == 0))
      {

      }
   }

   return(eCosSdo_WRITE_OK);
}


//----------------------------------------------------------------------------//
// CosSyncEventRcvPdo()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosSyncEventRcvPdo(void)
{

}

