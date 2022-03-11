//****************************************************************************//
// File:          c51f550_tmr.c                                               //
// Description:   Basic timer functions for C8051F5xx Silabs controller       //
// Author:        Matthias Siegenthaler                                       //
// e-mail:        matthias@sigitronic.com                                     //
//                                                                            //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 26.09.2017  Initial version derived from microcontrol can-pie              //
//                                                                            //
//****************************************************************************//


#include "cos_conf.h"
#include "SI_C8051F550_Register_Enums.h" // SFR declarations
#include "SI_C8051F550_Defs.h" // SFR declarations
#include "mc_tmr.h"
#include "stdint.h"
#include "c51f550_hal.h"


/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
// this pointer stores the callback handler for a timer function
//
//void  (* pfnTmrHandler) (void);

/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/



//----------------------------------------------------------------------------//
// McTmrFunction()                                                            //
// initialize Timer                                                           //
//----------------------------------------------------------------------------//
/*
void  McTmrFunction( void (*pfnTmrFunction)(void) )
{
   //----------------------------------------------------------------
   // save callback for timer function
   //
   pfnTmrHandler = pfnTmrFunction;
}
*/


//----------------------------------------------------------------------------//
// McTimerInit()                                                              //
// initialize Timer                                                           //
//----------------------------------------------------------------------------//
void McTmrInit(void)
{
  
   //----------------------------------------------------------------
   // callback for timer function set to 0L
   //
   //pfnTmrHandler = 0L;


//	_U08 XRAM BackupPage;
  ubTimerTriggerG = 0;
/*
   // ----------------------------------------------------------------
   // SYSCLOCK ist configured to internal clock of 24MHz
   // condifgure CKCON: Clock Control register
   // to set a prescale value for Timer0
   
   // config SFRPAGE to access Timer0 registers
   SFRPAGE = CONFIG_PAGE;

   // Disable Timer 2  
   TMR2CN = 0x00;
   
   // Timer 2 uses internal clock divided by 12
   // resulting clock is 24MHz / 12 = 2 MHz
   CKCON  &= ~0x60;                    // Timer2 clocked based on T2XCLK;
   
   //----------------------------------------------------------------
   // Initialize timer 0 to generate an interrupt every 1ms     
   // One timer count is 500ns, for 1 ms it is 2000 counts
   // 65536 - 2000 = 63536 (0xF830)  underflow after 1ms
   // 65536 - 2000*5 = 55536 (0xCA63)  underflow after 5ms
   //
   // @11.0592MHz system clock:
   // One timer count is 723ns, for 1 ms it is 1382,4 counts
   // 65536 - 1382 = 64154 (0xFA9A)
   //
   
   #if COS_TIMER_PERIOD == 5000
   TMR2RL = 0xD8F0;  // direct access to RCAP2L and RCAP2H is possible here
   #else
   TMR2RL = 0xF830;    // @24MHz system clock
   #endif
   TMR2 = 0xFFFF;   // direct access to TMR2L and TMR2H is possible here

   // enable Timer 2 counter 
   TR2 = 1;

   // enable interrupt for Timer2
   ET2 = 1;
 */

//  BackupPage = SFRPAGE;
  SFRPAGE = LEGACY_PAGE;

   TMR2CN  = 0x00;                     // Stop Timer2; Clear TF2;
                                       // use SYSCLK/12 as timebase
   CKCON  &= ~0x60;                    // Timer2 clocked based on T2XCLK;

#if COS_TIMER_PERIOD == 5000
   TMR2RL  = -10000;                  // Init reload values (direct access to RCAP2L and RCAP2H is possible here)
//TMR2RL = 0xD8F0;  // direct access to RCAP2L and RCAP2H is possible here
#else
   TMR2RL  = -2000;                  // Init reload values (direct access to RCAP2L and RCAP2H is possible here)
//TMR2RL = 0xF830;    // @24MHz system clock
#endif

   TMR2    = 0xFFFF;                   // Set to reload immediately
   IE_ET2  = 1;                        // Enable Timer2 interrupts
   TMR2CN_TR2 = 1;                     // Start Timer2

//   SFRPAGE = BackupPage;



}



//----------------------------------------------------------------------------//
// McTmrIntHandler()                                                          //
// Interrupt Handler for Timer                                                //
//----------------------------------------------------------------------------//
INTERRUPT(TIMER2_ISR, TIMER2_IRQn)
{
   // NOTE: The SFRPAGE register will automatically be switched to the Timer 0 Page
   // When an interrupt occurs.  
   // SFRPAGE will return to its previous setting on exit
   // from this routine.   
 
   static bool_t blink = true;
	TMR2CN_TF2H = 0;                    // Clear Timer2 interrupt flag
   blink = !blink;
   nOUT_RED = blink;
   ubTimerTriggerG = 1;

   /*
   //----------------------------------------------------------------
   // if there is a function, call it
   //
   if(pfnTmrHandler)
   {
      (*pfnTmrHandler)();
   }
   */
}

