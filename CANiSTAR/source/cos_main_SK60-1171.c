//****************************************************************************//
// File:          cos_main_SK60-1171.c                                        //
// Description:   Program for the SK60-1171 PCB                               //
// Author:        Matthias Siegenthaler                                       //
// e-mail:        sigi@gengethoma.ch                                      	  //
//                                                                            //
//****************************************************************************//
//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define P_TARGET CP_CC_C8051F550
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_C8051F550_Register_Enums.h>// SFR declarations
//#include "c51f550_can.h"

//-----------------------------------------------------------------------------
// SiLabs_Startup() Routine
// ----------------------------------------------------------------------------
// This function is called immediately after reset, before the initialization
// code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
// useful place to disable the watchdog timer, which is enable by default
// and may trigger before main() in some instances.
//-----------------------------------------------------------------------------
void SiLabs_Startup (void)
{
  // Disable the watchdog here
	   PCA0MD &= ~0x40;                    // Disable Watchdog Timer
}
//****************************************************************************//
// File:          cos_main_SK60-1171.c                                        //
// Description:   Program for the SK60-1171 PCB                               //
// Author:        Matthias Siegenthaler                                       //
// e-mail:        sigi@gengethoma.ch                                      	  //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Lindlaustr. 2c                                                             //
// 53842 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11  													  //
// e-mail:        koppe@microcontrol.net 									  //
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





/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "cos_led.h"
#include "cos_mgr.h"
#include "cos_nmt.h"
#include "cos_pdo.h"
#include "cos_time.h"


#include "mc_i2c.h"
#include "mc_nvm.h"
#include "mc_tmr.h"
#include "mc_cpu.h"
#include "c51f550_hal.h"
#define TRANSMIT_MO        0x05

extern void Sk60AppInit(void);
extern void McCpuInit(void);
extern void McWdtInit(void);
extern void McWdtTrigger(void);
extern void AppEvent(void);

extern void AppSetAnalogValue(uint8_t ubChannelV, uint16_t uwAdcValueT);

uint8_t  ubTimerTriggerG;
static uint16_t  auwADC_TestS[1];


//----------------------------------------------------------------------------//
// main()                                                                     //
// This function initialises the microcontroller.                             //
//----------------------------------------------------------------------------//
void main(void)
{
   uint8_t  ubBaudSelT = CP_BAUD_250K;    // selected CANopen baudrate
   uint8_t  ubNodeIdT = 20;     // selected CANopen Node-ID
   uint16_t uwBaudWaitT;   // wait time for automatic bit-rate detection
   #if COS_TMR_INT == 0
   uint32_t ulTimerTickT = 0;    // timer tick value
   #endif

   //----------------------------------------------------------------
   // Initialise the target CPU
   //
   McCpuInit();

   if (McGetChannelId() == 1 )
   {
	   ubNodeIdT += 1;
   }

   //----------------------------------------------------------------
   // Initialise the CANopen slave stack
   //
   CosMgrInit(CP_CHANNEL_1, 0);


   //----------------------------------------------------------------
   // Initialise the timer resource on the target CPU
   //
   McTmrInit();
   //----------------------------------------------------------------
   // Initialise EEPROM
   //
//   McNvmInit();

   //----------------------------------------------------------------
   // Initialise the hardware / demo board
   //
   Sk60AppInit();

   //McI2C_Init(eI2C_NET_1);

   //----------------------------------------------------------------
   // read the actual baurate setting and test if it is valid
   //
   ubBaudSelT = CP_BAUD_250K;
   if(ubBaudSelT > CP_BAUD_MAX)
   {
      //---------------------------------------------------
      // baudrate value is out of limit:
      // - show failure status
      // - stop program here in endless loop
      //
      #if COS_LED_SUPPORT > 0
      CosLedNetworkError(eCosLedErr_BUS_OFF);
      #endif
      #if COS_LED_SUPPORT > 1
      CosLedModuleStatus(eCosLedMod_FAIL_BAUD);
      #endif
      while(1) { };
   }

   //----------------------------------------------------------------
   // test for auto-baudrate detection (this feature depends on the
   // used CAN controller)
   //
   if(ubBaudSelT == CP_BAUD_AUTO)
   {
      uwBaudWaitT = 1;
      while( CpCoreAutobaud(CP_CHANNEL_1, &ubBaudSelT, &uwBaudWaitT));
   }


   //----------------------------------------------------------------
   // read the actual node ID setting and test if it is valid
   //
   ubNodeIdT  = CosMgrGetNodeAddress();
   if((ubNodeIdT > 127) || (ubNodeIdT < 1))
   {
      //---------------------------------------------------
      // address value is out of limit:
      // - show failure status
      // - stop program here in endless loop
      //
      #if COS_LED_SUPPORT > 0
      CosLedNetworkError(eCosLedErr_BUS_OFF);
      #endif
      #if COS_LED_SUPPORT > 1
      CosLedModuleStatus(eCosLedMod_FAIL_ADDR);
      #endif
      while(1) { };
   }


   #if COS_LSS_SUPPORT == 0
   if(ubNodeIdT == 0)
   {
      //---------------------------------------------------
      // address value is out of limit:
      // - show failure status
      // - stop program here in endless loop
      //
      #if COS_LED_SUPPORT > 0
      CosLedNetworkError(eCosLedErr_BUS_OFF);
      #endif
      #if COS_LED_SUPPORT > 1
      CosLedModuleStatus(eCosLedMod_FAIL_ADDR);
      #endif
      while(1) { };
   }
   #endif


   //----------------------------------------------------------------
   // Initialise the watchdog timer on the target CPU
   //
  // McWdtInit();


   //----------------------------------------------------------------
   // start the CANopen slave stack
   //

   CosMgrStart(ubBaudSelT, ubNodeIdT);

//	ulCurrentTickT = McTmrTick();
	 
   //----------------------------------------------------------------
   // this is the main loop of the embedded application
   //


   //---------Initialize settings for Transmit Message Object

   while (1)
   {
/*
		  if( McTmrTick() > ulCurrentTickT + 10)
			{
				 AppEvent();
				 ulCurrentTickT = McTmrTick();
				
			}
*/
      //---------------------------------------------------
      // if the target system can not call the
      // CosTmrEvent() function inside the timer
      // interrupt, this is performed here inside
      // the main loop
      //
//      #if COS_TMR_INT == 0
//      if(McTmrTick() > ulTimerTickT)
//      {
//         CosTmrEvent();
//				 AppEvent();
//         ulTimerTickT = McTmrTick();
//      }
//      #endif

//	  if (!ADC0CN_ADBUSY)
//	  {

//			auwADC_ConvertedValueS[ADC0MX && MAXADCCHANNEL] = ADC0;
		  //auwADC_TestS[0] = ADC0;
		  AppSetAnalogValue(0, McAdcStartConversion(0x0D));
//			ADC0CN_ADINT = 0;                   // Clear ADC conversion complete


//		  McAdcStartConversion(0x0D);                      // Set ADC input to P1.5 and start conversion;
//	  }

//	  McAdcStartConversion(0x0D);                      // Set ADC input to P1.5 and start conversion;



	  #if COS_TMR_INT == 0
      if (ubTimerTriggerG)
      {
          CosTmrEvent();
          AppEvent();
    	  ubTimerTriggerG = 0;
    	  // CpCoreBufferSend(0,1);
      }
      #endif



      //---------------------------------------------------
      // check the result of the CANopen manager call
      //
      if(CosMgrProcess() != eCosErr_NODE_RESET)
      {
         //--------------------------------------
         // trigger the watch-dog timer each time
         // we run successful through the CANopen
         // manager
         //
         McWdtTrigger();
      }
      else
      {
         //--------------------------------------
         // release all CAN resources and wait
         // for the watchdog timer to restart
         // the application
         //
         CosMgrRelease();
      }
   }  // end while (1)

//   return(0); //comment to prevent compiler warning
}


