//****************************************************************************//
// File:          cos_app_SK60-1171.c                                         //
// Description:   Program for the SK60-1171 PCB                               //
// Author:        Matthias Siegenthaler / Uwe Koppe                           //
// e-mail:                                              	        		  //
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

#include "c51f550_hal.h"
#include "cos_emcy.h"
#include "cos_mgr.h"
#include "cos_pdo.h"             // PDO service

#if COS_DS401_DI > 0
#include "cos401di.h"
#endif

#if COS_DS401_AI > 0
#include "cos401ai.h"
#endif

#include "mc_tmr.h"
#include "mc_cpu.h"

#include <stdio.h>
#include <stdbool.h>



/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
// The following value is used inside a PDO in case the ADC value
// for the joystick is out of bounds.
//
#define  JOYSTICK_ERROR_VALUE    ((uint16_t) 0x0000)
#define  JOYSTICK_ANALOG_MAX		1

/*----------------------------------------------------------------------------*\
** Variables                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/


static uint16_t auwADC_ConvertedValueS[JOYSTICK_ANALOG_MAX];

/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/



void AppSetAnalogValue(uint8_t ubChannelV, uint16_t uwAdcValueT)
{
  if(ubChannelV < JOYSTICK_ANALOG_MAX)
  {
	  auwADC_ConvertedValueS[ubChannelV] = uwAdcValueT;
  }
}





//----------------------------------------------------------------------------//
// AppEvent()                                                                 //
// timer event for application                                                //
//----------------------------------------------------------------------------//
void AppEvent(void)
{
   volatile uint8_t  ubDigitalInputValueT = 0;
   volatile uint16_t uwAdcWertT = 0;
   
   //----------------------------------------------------------------
   // read digital inputs
   //
   ubDigitalInputValueT  = McGetPushbuttonState(); 		    // D0
   Cos401_DI_Set(0, ubDigitalInputValueT);
    
 
   //-----------------------------------------------------
   // Check if there was a change of the digital inputs
   // trigger an PDO application event if there was a
   // change
   //    
   if(Cos401_DI_PdoSend(0))
   {
     CosPdoAppEvent(0);
   }
    
   //----------------------------------------------------------------
   // read analogue inputs
   //
   uwAdcWertT = auwADC_ConvertedValueS[0];
   Cos401_AI_SetAdcValue(0, uwAdcWertT);
    
   //----------------------------------------------------------------
   // check if there was a change (similar to digital inputs),
   // this code section is commented out because it makes no
   // sense for analogue inputa, analogue PDO values are send
   // timer triggered
   //
   #if 0
   if(Cos401_AI_PdoSend(0) || Cos401_AI_PdoSend(1))
   {
      CosPdoAppEvent(1);
   }
   #endif
}




//----------------------------------------------------------------------------//
// CosLedNetGreen()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLedNetGreen(_BIT btSwitchOnV)
{
	nLED_GREEN = !btSwitchOnV; // green NET LED according state of parameter
}


//----------------------------------------------------------------------------//
// CosLedNetRed()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLedNetRed(_BIT btSwitchOnV)
{
	nLED_RED = !btSwitchOnV; // red NET LED according state of parameter
}


//----------------------------------------------------------------------------//
// CosLedNetGreen()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLedModGreen(_BIT btSwitchOnV)
{
	nOUT_GREEN = !btSwitchOnV; // green NET LED according state of parameter
}


//----------------------------------------------------------------------------//
// CosLedNetRed()                                                             //
//                                                                            //
//----------------------------------------------------------------------------//
void  CosLedModRed(_BIT btSwitchOnV)
{
	nOUT_RED = !btSwitchOnV; // red NET LED according state of parameter
}




//----------------------------------------------------------------------------//
// DemoAppInit()                                                              //
// Initialise the application / demo board                                    //
//----------------------------------------------------------------------------//
void  Sk60AppInit(void)
{


}
