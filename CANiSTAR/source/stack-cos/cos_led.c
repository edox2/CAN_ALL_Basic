//****************************************************************************//
// File:          cos_led.c                                                   //
// Description:   LED Manager for CANopen Slave                               //
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
// 17.03.2001  Initial version                                                //
// 11.02.2003  Changed blinkcodes                                             //
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
#include "cos_led.h"       // LED management
#include "cos_nmt.h"       // Network management


//------------------------------------------------------------------#
// test if LED support is enabled
#if COS_LED_SUPPORT > 0


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  NET_GRN_OFF          0x00000000
#define  NET_GRN_INIT         0x00550055
#define  NET_GRN_STOP         0x00030003
#define  NET_GRN_PREOP        0x0F0F0F0F
#define  NET_GRN_OPER         0xFFFFFFFF

#define  NET_RED_OFF          0x00000000
#define  NET_RED_BUS_WARN     0x0000F0F0     // bus warn
#define  NET_RED_BUS_PASV     0x00000550     // bus passive
#define  NET_RED_BUS_RES      0x000000F0     // reserved
#define  NET_RED_BUS_OFF      0xFFFFFFFF     // bus off



/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


static uint8_t  ubLedEventCounterS;    // counter for timer events

static uint8_t  ubLedNetStatusS;       // current network status
static uint8_t  ubLedNetErrorS;        // current network error
static uint8_t  ubLedNetCodeS = 0xFF;  // currend blinking of LEDs


static uint32_t  ulLedNetRedPatternS;    // pattern for red network LED
static uint32_t  ulLedNetGrnPatternS;    // pattern for green network LED

#if COS_LED_SUPPORT > 1
static uint8_t  ubLedModStatusS;        // current module status
static uint32_t  ulLedModRedPatternS;    // pattern for red module LED
static uint32_t  ulLedModGrnPatternS;    // pattern for green module LED
#endif


/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/




//----------------------------------------------------------------------------//
// CosLedInit()                                                               //
// initialize CANopen LED management                                          //
//----------------------------------------------------------------------------//
void  CosLedInit(void)
{
   //----------------------------------------------------------------
   // setup event counter for LED blinking
   //
   ubLedEventCounterS  = COS_LED_PERIOD / COS_TIMER_PERIOD;


   //----------------------------------------------------------------
   // initial network status: eCosLedNet_PREOPERATIONAL
   //
   ubLedNetStatusS = eCosLedNet_INIT;
   ubLedNetErrorS  = eCosLedErr_OK;
   ulLedNetRedPatternS = NET_RED_OFF;
   ulLedNetGrnPatternS = NET_GRN_INIT;


   //----------------------------------------------------------------
   // initial module status: CosLedMod_OK
   //
   #if COS_LED_SUPPORT > 1
   ubLedModStatusS = eCosLedMod_OK;
   ulLedModRedPatternS = 0x00000000;
   ulLedModGrnPatternS = 0xFFFFFFFF;
   #endif
}


//----------------------------------------------------------------------------//
// CosLedModuleStatus()                                                       //
// setup LED blink code for module status                                     //
//----------------------------------------------------------------------------//
#if COS_LED_SUPPORT > 1
void  CosLedModuleStatus(uint8_t ubStatusV)
{
   //--- compare to current status ----------------------------------
   if(ubStatusV == ubLedModStatusS) return;


   //--- save the new status ----------------------------------------
   ubLedModStatusS = ubStatusV;

   //--- display the new status -------------------------------------
   switch(ubStatusV)
   {
      case eCosLedMod_OK:
         ulLedModRedPatternS = 0x00000000;
         ulLedModGrnPatternS = 0xFFFFFFFF;
         break;

      case eCosLedMod_FAIL_BAUD:
         ulLedModRedPatternS = 0xFF030000;
         ulLedModGrnPatternS = 0x00000000;
         break;

      case eCosLedMod_FAIL_ADDR:
         ulLedModRedPatternS = 0xFF030300;
         ulLedModGrnPatternS = 0x00000000;
         break;

      case eCosLedMod_RECOVER:
         ulLedModRedPatternS = 0xF0F0F0F0;
         ulLedModGrnPatternS = 0x0F0F0F0F;
         break;

      case eCosLedMod_APP1:
         ulLedModRedPatternS = 0x00550055;
         ulLedModGrnPatternS = 0x00000000;
         break;

      case eCosLedMod_APP2:
         ulLedModRedPatternS = 0x7F557F55;
         ulLedModGrnPatternS = 0x00000000;
         break;

      default:
         ulLedModRedPatternS = 0xFFFFFFFF;
         ulLedModGrnPatternS = 0x00000000;
         break;
   }


}
#endif


//----------------------------------------------------------------------------//
// CosLedNetworkError()                                                       //
// setup LED blink code for network error                                     //
//----------------------------------------------------------------------------//
void  CosLedNetworkError(uint8_t ubErrorV)
{
   //--- save the new error -----------------------------------------
   ubLedNetErrorS = ubErrorV;
}


//----------------------------------------------------------------------------//
// CosLedNetworkPattern()                                                     //
// setup LED blink code for network status / error                            //
//----------------------------------------------------------------------------//
void  CosLedNetworkPattern(void)
{
   uint8_t  ubLedNetCodeT;  // for actual LedNetCode

   //----------------------------------------------------------------
   // create compare status byte
   //
   ubLedNetCodeT = ubLedNetErrorS;
   ubLedNetCodeT = ubLedNetCodeT << 4;
   ubLedNetCodeT |= ubLedNetStatusS;

   //--- compare to current status ----------------------------------
   if(ubLedNetCodeT == ubLedNetCodeS) return;


   // Define new Pattern for the LED's
   switch (ubLedNetStatusS)
   {
      case eCosLedNet_STOPPED:
         ulLedNetGrnPatternS = NET_GRN_STOP;
         break;

      case eCosLedNet_PREOPERATIONAL:
         ulLedNetGrnPatternS = NET_GRN_PREOP;
         break;

      case eCosLedNet_OPERATIONAL:
         ulLedNetGrnPatternS = NET_GRN_OPER;
         break;

      case eCosLedNet_INIT:
      default:
         ulLedNetGrnPatternS = NET_GRN_INIT;
         break;
   }

   switch (ubLedNetErrorS)
   {
      case eCosLedErr_OK:
         ulLedNetRedPatternS = NET_RED_OFF;
         break;

      case eCosLedErr_BUS_WARN:
         ulLedNetRedPatternS = NET_RED_BUS_WARN;
         ulLedNetGrnPatternS &= ~ulLedNetRedPatternS;
         break;

      case eCosLedErr_BUS_PASSIVE:
         ulLedNetRedPatternS = NET_RED_BUS_PASV;
         ulLedNetGrnPatternS &= ~ulLedNetRedPatternS;
         break;

      case eCosLedErr_BUS_OFF:
         ulLedNetRedPatternS = NET_RED_BUS_OFF;
         ulLedNetGrnPatternS = 0x0;
         break;

      case eCosLedErr_RESERVED:
      default:
         ulLedNetRedPatternS = NET_RED_BUS_RES;
         ulLedNetGrnPatternS &= ~ulLedNetRedPatternS;
         break;
   }

   //--- save the new status ----------------------------------------
   ubLedNetCodeS = ubLedNetCodeT;

}


//----------------------------------------------------------------------------//
// CosLedNetworkStatus()                                                      //
// setup LED blink code for network status                                    //
//----------------------------------------------------------------------------//
void  CosLedNetworkStatus(uint8_t ubStatusV)
{
   //--- save the new status ----------------------------------------
   ubLedNetStatusS = ubStatusV;
}


//----------------------------------------------------------------------------//
// CosLedEvent()                                                              //
// CANopen LED event, is called by CosTmrEvent()                              //
//----------------------------------------------------------------------------//
void  CosLedTmrEvent(void)
{
   bool_t  btLedT;     // LED status: ON or OFF

   //----------------------------------------------------------------
   // check event counter status
   //
   if(ubLedEventCounterS)
   {
      ubLedEventCounterS--;
      return;
   }

   //----------------------------------------------------------------
   // Define new Pattern for the NET-Status LED's
   //
   CosLedNetworkPattern();

   //----------------------------------------------------------------
   // Network Status, red light
   //
   btLedT = ulLedNetRedPatternS & 0x00000001;
   ulLedNetRedPatternS = ulLedNetRedPatternS >> 1;
   if(btLedT)
   {
      CosLedNetRed(1);
      ulLedNetRedPatternS = ulLedNetRedPatternS | 0x80000000;
   }
   else
   {
      CosLedNetRed(0);
   }


   //----------------------------------------------------------------
   // Network Status, green light
   //
   btLedT = ulLedNetGrnPatternS & 0x00000001;
   ulLedNetGrnPatternS = ulLedNetGrnPatternS >> 1;
   if(btLedT)
   {
      CosLedNetGreen(1);
      ulLedNetGrnPatternS = ulLedNetGrnPatternS | 0x80000000;
   }
   else
   {
      CosLedNetGreen(0);
   }

   //----------------------------------------------------------------
   // Module Status, red light
   //
   #if COS_LED_SUPPORT > 1
   btLedT = ulLedModRedPatternS & 0x00000001;
   ulLedModRedPatternS = ulLedModRedPatternS >> 1;
   if(btLedT)
   {
      CosLedModRed(1);
      ulLedModRedPatternS = ulLedModRedPatternS | 0x80000000;
   }
   else
   {
      CosLedModRed(0);
   }

   //----------------------------------------------------------------
   // Module Status, green light
   //
   btLedT = ulLedModGrnPatternS & 0x00000001;
   ulLedModGrnPatternS = ulLedModGrnPatternS >> 1;
   if(btLedT)
   {
      CosLedModGreen(1);
      ulLedModGrnPatternS = ulLedModGrnPatternS | 0x80000000;
   }
   else
   {
      CosLedModGreen(0);
   }
   #endif

   //----------------------------------------------------------------
   // reload event counter for LED blinking
   //
   ubLedEventCounterS  = COS_LED_PERIOD / COS_TIMER_PERIOD;
}


#endif  //COS_LED_SUPPORT == 1
// test if LED support is enabled (end)
//------------------------------------------------------------------#
