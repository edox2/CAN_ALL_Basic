/*
 * Init.c
 *
 *  Created on: 12.03.2022
 *      Author: benja
 */


//-----------------------------------------------------------------------------
// Init.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains routines for initialization functions.
//
//-----------------------------------------------------------------------------

#include "../inc/compiler_defs.h"
#include "SI_C8051F550_Defs.h"
#include "../inc/Common.h"
#include "../inc/Init.h"
//#include "../inc/Pins.h"
//#include "../inc/Serial.h"

//-----------------------------------------------------------------------------
// Internal Constants
//-----------------------------------------------------------------------------

#define BAUDRATE (115200L)             // UART baud rate in bps

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

void WDT_Init (void);
void SYSCLK_Init (void);
void UART0_Init (void);
void PORT_Init (void);
//void PIN_Init(void);
void Timer0_Init (void);
void PCA0_Init (void);
void OSCILLATOR_Init (void);
void CAN_Init(int canNode);
void ADC0_Init (void);
void SetADC0Mux(int pin);

//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Internal prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Internal global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Initialization Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// WDT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine disables the WDT.
//
//-----------------------------------------------------------------------------
void WDT_Init (void)
{
   PCA0MD &= ~0x40;
}

//-----------------------------------------------------------------------------
// SYSCLK_Init 12 MHz intosc * 4 = 48 MHz
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This selects the clock multiplier as the system clock source.  The input
// to the multiplier is the 12 MHz internal oscillator.
// Also sets Flash read timing for 50 MHz operation.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
//    U16 i = 0;
//
//    FLSCL     = 0x90;                  // Set Flash read timing for 50 MHz
//
//    OSCICN    = 0x83;                  // Enable intosc for div 1 mode
//
//    CLKMUL    = 0x80;                  // Enable clkmul
//
//    for (i = 0; i < 20; i++);          // Wait at least 5us for init
//
//    CLKMUL    |= 0xC0;
//
//    while ((CLKMUL & 0x20) == 0);
//
//    CLKSEL    = 0x03;                  // enable CLKMUL as sysclk
  //internal osc @ 24MHz
  SFRPAGE   = CONFIG_PAGE;
  OSCICN    = 0xC7;
//  SFRPAGE   = ACTIVE_PAGE;
}


//-----------------------------------------------------------------------------
void PORT_Init (void)
{

  // P0.0  -  C-Port,  Open-Drain, Digital
  // P0.1  -  DIA_EN,  Push-Pull,  Digital
  // P0.2  -  SEL1,  Push-Pull,  Digital
  // P0.3  -  SEL2,  Push-Pull,  Digital
  // P0.4  -  UART_TX (UART0), Open-Drain, Digital
  // P0.5  -  UART_RX (UART0), Open-Drain, Digital
  // P0.6  -  CAN_TX (CAN0), Open-Drain, Digital
  // P0.7  -  CAN_RX (CAN0), Open-Drain, Digital

  // P1.0  -  SNS-A,  Open-Drain, Analog
  // P1.1  -  SNS-B,  Open-Drain, Analog
  // P1.2  -  EN-A,  Push-Pull,  Digital
  // P1.3  -  EN-B,  Push-Pull,  Digital
  // P1.4  -  LATCH,  Push-Pull,  Digital
  // P1.5  -  ST-A,  Open-Drain, Digital
  // P1.6  -  ST-B,  Open-Drain, Digital
  // P1.7  -  A-Port,  Open-Drain, Digital

  // P2.0  -  B-Port,  Open-Drain, Digital
  // P2.1  -  C2D,  Open-Drain, Digital

  uint8_t SFRPAGE_save = SFRPAGE;
  SFRPAGE = CONFIG_PAGE;
//  //todo debug config (ADC)
//  SFRPAGE   = CONFIG_PAGE;
//  P0MDIN    = 0xFE;
//  P1MDIN    = 0x7C;
//  P2MDIN    = 0xFE;
//  XBR0      = 0x03;
//  XBR2      = 0x40;

  //todo original config
  SFRPAGE   = CONFIG_PAGE;
  P1MDIN    = 0xFC;
  P0MDOUT   = 0x0E;
  P1MDOUT   = 0x1C;
  XBR0      = 0x03;
  XBR2      = 0x40;
  SFRPAGE   = SFRPAGE_save;
}
//-----------------------------------------------------------------------------
// Pin Declarations
//-----------------------------------------------------------------------------
//void PIN_Init (void)
//{
//  SI_SBIT(C_Port, SFR_P0, 0);
//  SI_SBIT(DIA_EN, SFR_P0, 1);
//  SI_SBIT(SEL1, SFR_P0, 2);
//  SI_SBIT(SEL2, SFR_P0, 3);
//  SI_SBIT(UART_TX, SFR_P0, 4);
//  SI_SBIT(UART_RX, SFR_P0, 5);
//  SI_SBIT(CAN_TX, SFR_P0, 6);
//  SI_SBIT(CAN_RX, SFR_P0, 7);
//  SI_SBIT(SNS_A, SFR_P1, 0);
//  SI_SBIT(SNS_B, SFR_P1, 1);
//  SI_SBIT(EN_A, SFR_P1, 2);
//  SI_SBIT(EN_B, SFR_P1, 3);
//  SI_SBIT(LATCH, SFR_P1, 4);
//  SI_SBIT(ST_A, SFR_P1, 5);
//  SI_SBIT(ST_B, SFR_P1, 6);
//  SI_SBIT(A_PORT, SFR_P1, 7);
//  SI_SBIT(B_PORT, SFR_P2, 0);
//  SI_SBIT(C2D, SFR_P2, 1);
//}
//-----------------------------------------------------------------------------
// UART_Init (defs)
//-----------------------------------------------------------------------------
//
// Configure the UART0 using Timer1 for <BAUDRATE> and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART0_Init(void)
{
    SCON0 = 0x10;                      // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
#if (SYSCLK/BAUDRATE/2/256 < 1)
    TH1 = -(SYSCLK/BAUDRATE/2);
    CKCON |=  0x08;                    // T1M = 1; SCA1:0 = xx
#elif (SYSCLK/BAUDRATE/2/256 < 4)
    TH1 = -(SYSCLK/BAUDRATE/2/4);
    CKCON &= ~0x0B;                    // T1M = 0; SCA1:0 = 01
    CKCON |=  0x01;
#elif (SYSCLK/BAUDRATE/2/256 < 12)
    TH1 = -(SYSCLK/BAUDRATE/2/12);
    CKCON &= ~0x0B;                    // T1M = 0; SCA1:0 = 00
#elif (SYSCLK/BAUDRATE/2/256 < 48)
    TH1 = -(SYSCLK/BAUDRATE/2/48);
    CKCON &= ~0x0B;                    // T1M = 0; SCA1:0 = 10
    CKCON |=  0x02;
#else
#error Unsupported SYSCLK/BAUDRATE combination.
#endif

    TL1 = TH1;                         // Initialize Timer1
    TMOD &= ~0xF0;                     // TMOD: Timer1 in 8-bit autoreload
    TMOD |=  0x20;
    TCON_TR1 = 1;                      // START Timer1 ben: used to be TR1 = 1
    SCON0_TI = 1;                           // Indicate TX0 ready ben: used to be TI0 = 1
}


//-----------------------------------------------------------------------------
// Timer0_Init   8-bit autoreload at 1 us
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure Timer0 for 8-bit autoreload at 1us, no interrupts enabled.
//
//-----------------------------------------------------------------------------
void Timer0_Init (void)
{
   TCON &= ~0x30;                      // Stop timer and clear flags
   TMOD &= ~0x0F;                      // set mode to 8-bit autoreload
   TMOD |=  0x02;

   CKCON |= 0x04;                      // Timer counts SYSCLKs

   TH0 = (U8) -(SYSCLK / 1000000);     // set to reload at 1 us interval
   TL0 = TH0;

   IE_ET0 = 0;                            // interrupts disabled ben: used to be ET0= = 1

   TCON_TR0 = 1;                            // Start timer ben: used to be TR0 = 1
}

//-----------------------------------------------------------------------------
// PCA0_Init   Timer0 is clock source
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure PCA0 as follows:
//  * Main time base clocked from Timer0 overflows
//  * Main counter interrupt enabled to provide 32-bits of freerunning us
//    count
//  * Module 3 set in Software Timer mode, interrupts enabled here,
//    set to interrupt at 1 ms intervals (ms counter, ms Timeout)
//  * Module 4 set in Software Timer mode, interrupts enabled elsewhere,
//     set to interrupt on match (us Timeout) or CC4 for used to poll for
//     match
//
//-----------------------------------------------------------------------------
void PCA0_Init (void)
{
   UU16 temp;                          // temporary math register

   PCA0CN = 0x00;                      // Stop PCA counter; clear all flags
   PCA0MD = 0x05;                      // PCA counts in IDLE mode; Timer0
                                       // is clock source; PCA counter
                                       // interrupts are enabled
   PCA0L = 0x00;                       // initialize main counter
   PCA0H = 0x00;

   PCA0CPM3 = 0x49;                    // Module 3 in Software Timer mode
                                       //  with interrupts enabled
   // configure Module 3 to interrupt in 1000 us from now
   // PCA0CP3 = PCA0 + 1000
   temp.U8[LSB] = PCA0L;               // read Low byte first
   temp.U8[MSB] = PCA0H;
   temp.U16 = temp.U16 + 1000;         // add 1000
   PCA0CPL3 = temp.U8[LSB];            // write Low byte first
   PCA0CPH3 = temp.U8[MSB];            // followed by High byte

   PCA0CPM4 = 0x48;                    // Module 4 in Software Timer mode
                                       //  with interrupts disabled

   PCA0CN_CR = 1;                             // Start PCA0 counter ben: used to be CR = 1

   EIE1 |= 0x10;                       // Enable PCA0 interrupts

}

void OSCILLATOR_Init (void)
{
   uint8_t SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   OSCICN = 0xC4;                      // Configure internal oscillator for
                                       // 24 MHz / 8

   SFRPAGE = LEGACY_PAGE;

   RSTSRC = 0x04;                      // Enable missing clock detector

   SFRPAGE = SFRPAGE_save;
}


void CAN_Init(int canNode)
{
  //todo
}

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure ADC0 to use ADBUSY as conversion source.
// Disables ADC end of conversion interrupt. Leaves ADC
// disabled.
//
//-----------------------------------------------------------------------------

void ADC0_Init (void)
{
   uint8_t SFRPAGE_save = SFRPAGE;
   SFRPAGE = LEGACY_PAGE;

   ADC0CN = 0x00;                      // ADC0 disabled; Burst mode disabled
                                       // Data is right-justified
                                       // ADC0 conversions are initiated
                                       // on a write to AD0Busy

   ADC0CF = (SYSCLK/3000000) << 3;     // ADC conversion clock <= 3MHz
                                       // Repeat Count = 1

   REF0CN = 0x17;                      // Enable on-chip VREF and buffer
                                       // Set voltage reference to 2.20V setting
                                       // Enable temperature sensor

   EIE1 &= ~0x04;                      // Disable ADC0 conversion interrupt

   ADC0CN_ADEN = 1;                          // Enable ADC0 (ADC0CN)

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//Select the Input for ADC0
void SetADC0Mux(int pin)
{
  ADC0MX = pin;                      // ADC MUX to P1.7
}
//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

