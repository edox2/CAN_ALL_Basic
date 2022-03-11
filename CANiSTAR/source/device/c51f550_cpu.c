//****************************************************************************//
// File:          c51f550_cpu.h                                               //
// Description:   DBasic CPU functions for Silabs C8051F55x-f56x-f57x         //
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



/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/


#include <SI_C8051F550_Register_Enums.h>// SFR declarations
#include "compiler.h"
#include "c51f550_hal.h"
#include "mc_cpu.h"



/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define CHANNEL_ID_A	    (-1)
#define CHANNEL_ID_B 	    (1)
#define CHANNEL_ID_INVALID 	(0)


int8_t sChannelId = CHANNEL_ID_INVALID;




/*----------------------------------------------------------------------------*\
** Functions                                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
// McGetChannelId()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//

int8_t McGetChannelId(void)
{
	return(sChannelId); // channel ID in case of redundant System
}

uint8_t McGetPushbuttonState(void)
{
	return(!PB_IN); // Pushbutton state
}


uint32_t McGetSerialNumber(void)
{

	uint32_t serialNumber = 0;
	serialNumber = SN0;
	serialNumber <<= 8;
	serialNumber |= SN1;
	serialNumber <<= 8;
	serialNumber |= SN2;
	serialNumber <<= 8;
	serialNumber |= SN3;
	return (serialNumber);

}


//----------------------------------------------------------------------------//
// McCpuInit()                                                                //
// initialize CPU                                                             //
//----------------------------------------------------------------------------//
void McCpuInit(void)
{
//   _U16  uwCntrT;
   
   //----------------------------------------------------------------
   // disable all interrupts 
   // Note: Any instruction that clears the EA bit should be 
   // immediately followed by an instruction that has two
   // or more opcode bytes. For example:
   IE_EA = 0;
   IE_EA = 0;
   
   //----------------------------------------------------------------
   // disable watchdog timer
   // 
   PCA0MD &= ~0x40;                    // Disable Watchdog Timer
//   WDTCN = 0xDE;
//   WDTCN = 0xAD;
   
     // Determine Chahnnel_ID
 	 sChannelId = (CH_ID) ? CHANNEL_ID_A : CHANNEL_ID_B;

 	 //ADC_Init

 	   SFRPAGE = LEGACY_PAGE;

 	   // Initialize the Gain to account for a 3V3 input and 2.25 VREF
 	   // Solve the equation provided in Section 9.3.1 of the Datasheet

 	   // The 3V3 input is scaled by a factor of 0.666748 so that the maximum input
 	   // voltage seen by the pin is 2.200269V

 	   // 0.666748 = (GAIN/4096) + GAINADD * (1/64)

 	   // Set GAIN to 0x6CA and GAINADD to 0
 	   // GAIN = is the 12-bit word formed by ADC0GNH[7:0] ADC0GNL[7:4]
 	   // GAINADD is bit ADC0GNA.0

 	   ADC0CF |= 0x01;                     // Set GAINEN = 1
 	   ADC0H   = 0x04;                     // Load the ADC0GNH address
 	   ADC0L   = 0xAA;                     // Load the upper byte of 0xAAB to
 	                                       // ADC0GNH
 	   ADC0H   = 0x07;                     // Load the ADC0GNL address
 	   ADC0L   = 0xB0;                     // Load the lower nibble of 0xAAB to
 	                                       // ADC0GNL
 	   ADC0H   = 0x08;                     // Load the ADC0GNA address
 	   ADC0L   = 0x00;                     // Set the GAINADD bit
 	   ADC0CF &= ~0x01;                    // Set GAINEN = 0

 	   ADC0CN = 0xC0;                      // ADC0 disabled, normal tracking,
 	                                       // conversion triggered on TMR2 overflow
 	                                       // Output is right-justified

 	   REF0CN = 0x33;                      // Enable on-chip VREF and buffer
 	                                       // Set voltage reference to 2.25V

 	   ADC0MX = 0x0D;                      // Set ADC input to P1.5

// 	   ADC0CF = ((SYSCLK / 3000000) - 1) << 3;   // Set SAR clock to 3MHz
 	   ADC0CF    = 0xFE;

 //	   EIE1 |= 0x04;                       // Enable ADC0 conversion complete int.

 	   ADC0CN_ADEN = 1;                    // Enable ADC0




// 	 ADC0MX    = 0x0D;
// 	 ADC0CF    = 0xFE;
// 	 ADC0CN    = 0xC0;

 	 //CAN_Init
     SFRPAGE   = CAN0_PAGE;
     CAN0CFG   = 0x00;
     SFRPAGE   = LEGACY_PAGE;

     //Voltage_Reference_Init
     REF0CN    = 0x13;


     //Port_IO_Init
     // P0.0  -  Skipped,     Open-Drain, Analog
     // P0.1  -  Skipped,     Open-Drain, Digital
     // P0.2  -  Unassigned,  Push-Pull,  Digital
     // P0.3  -  Unassigned,  Push-Pull,  Digital
     // P0.4  -  Unassigned,  Open-Drain, Digital
     // P0.5  -  Unassigned,  Open-Drain, Digital
     // P0.6  -  CAN_TX (CAN0), Push-Pull,  Digital
     // P0.7  -  CAN_RX (CAN0), Open-Drain, Digital

     // P1.0  -  Unassigned,  Open-Drain, Digital
     // P1.1  -  Unassigned,  Open-Drain, Digital
     // P1.2  -  Unassigned,  Open-Drain, Digital
     // P1.3  -  Unassigned,  Open-Drain, Digital
     // P1.4  -  Unassigned,  Open-Drain, Digital
     // P1.5  -  Skipped,     Open-Drain, Analog
     // P1.6  -  Unassigned,  Open-Drain, Digital
     // P1.7  -  Unassigned,  Open-Drain, Digital

     // P2.0  -  Unassigned,  Open-Drain, Digital
     // P2.1  -  Unassigned,  Open-Drain, Digital

     SFRPAGE   = CONFIG_PAGE;
     P0MDIN    = 0xFE;
     P1MDIN    = 0xDF;
     P0MDOUT   = 0x4C;
     P0SKIP    = 0x03;
     P1SKIP    = 0x20;
     XBR0      = 0x02;
     XBR2      = 0x40;


     //Oscillator_Init
     OSCICN    = 0xC7;	// Internal Oszillator 24MHz
     SFRPAGE   = LEGACY_PAGE;
     X_IN = 0;			   // Reference Potential for Pushbutton. Set to GND!!

     //Interrupts_Init
     EIE2      = 0x02;
     IE        = 0xA0;
 }
   


//----------------------------------------------------------------------------//
// McWdtInit()                                                               //
//                                                                            //
//----------------------------------------------------------------------------//
void  McWdtInit(void)
{
	   // Calculate Watchdog Timer Timeout
	   // Offset calculated in PCA clocks
	   // Offset = ( 256 x PCA0CPL5 ) + 256 - PCA0L
	   //        = ( 256 x 255(0xFF)) + 256 - 0
	   // Time   = Offset * (12/SYSCLK)
	   //        = 255 ms ( PCA uses SYSCLK/12 as its clock source)

	   PCA0MD  &= ~0x40;                   // Disable watchdog timer
	   PCA0L    = 0x00;                    // Set lower byte of PCA counter to 0
	   PCA0H    = 0x00;                    // Set higher byte of PCA counter to 0
	   PCA0CPL5 = 0xFF;                    // Write offset for the WDT
	   PCA0MD  |= 0x40;                    // Enable the WDT

	   IE_EA = 1;                          // Enable global interrupts
}


//----------------------------------------------------------------------------//
// McWdtTrigger()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
void  McWdtTrigger(void)
{
   // reset watchdog
	PCA0CPH5 = 0x00;             // Write a 'dummy' value to the PCA0CPH5
                                 // register to reset the watchdog timer
                                 // timeout. If a delay longer than the
                                 // watchdog timer delay occurs between
                                 // successive writes to this register,
                                 // the device will be reset by the watch
                                 // dog timer.
}




//----------------------------------------------------------------------------//
// McAdcTrigger()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
uint16_t  McAdcStartConversion(uint8_t channel)
{

    static uint16_t OldResult = 0;
	IE_EA = 0;                          // Enable global interrupts
	SFRPAGE   = LEGACY_PAGE;

    if (!ADC0CN_ADBUSY)
	{
		OldResult = 0xffff - ADC0;
		ADC0CN_ADINT = 0;                   // Clear ADC conversion complete
		// Start ADC-Conversion
		ADC0MX = channel;
		ADC0CN_ADBUSY = 1;
	 }
	 IE_EA = 1;                          // Enable global interrupts
	 return OldResult;
}

/*
INTERRUPT(ADC0EOC_IRQ, ADC0EOC_IRQn)
{
//	auwADC_ConvertedValueS[ADC0MX && MAXADCCHANNEL] = ADC0;
//	auwADC_ConvertedValueS[0] = ADC0;
//	ADC0CN_ADINT = 0;                   // Clear ADC conversion complete
}
*/















