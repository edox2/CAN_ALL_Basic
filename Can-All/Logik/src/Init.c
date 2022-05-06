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
#include "../inc/Can.h"
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
//  SFRPAGE   = CONFIG_PAGE;
//  OSCICN    = 0xC7;
//  SFRPAGE   = ACTIVE_PAGE;
}


//-----------------------------------------------------------------------------
void PORT_Init (void)
{
/*
  // P0.0  -  C-Port,  Open-Drain, Digital
  // P0.1  -  DIA_EN,  Push-Pull,  Digital
  // P0.2  -  SEL1,  Push-Pull,  Digital
  // P0.3  -  SEL2,  Push-Pull,  Digital
  // P0.4  -  UART_TX (UART0), Open-Drain, Digital
  // P0.5  -  UART_RX (UART0), Open-Drain, Digital
  // P0.6  -  CAN_TX (CAN0), push-pull, Digital
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
*/
  uint8_t SFRPAGE_save = SFRPAGE;
  SFRPAGE = CONFIG_PAGE;

  SFRPAGE   = CONFIG_PAGE;
  P1MDIN    = 0xFC;
  P0MDOUT   = 0x4E; // PP for 0.2, 0.3, 0.6,
  P1MDOUT   = 0x1C; // PP for 1.2, 1.3, 1.4
  XBR0      = 0x02; // Enable CAN0 on Crossbar todo merge: might also be 0x05 (used to be 0x03 | 0x02 for CAN)
  XBR2      = 0x40; // Enable Crossbar and weak pull-ups
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
// Timer0_Init
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
  // No need to set SFRPAGE as all registers accessed in this function
  // are available on all pages

   TCON &= ~0x30;                      // Stop timer and clear flags
   //todo merge: setting for ADC, has to be changed for CAN ->check if ADC still works
   /*ADC config
   //   TMOD &= ~0x0F;                      // set mode to 8-bit autoreload
   //   TMOD |=  0x02;


//   TH0 = (U8) -(SYSCLK / 1000000);     // set to reload at 1 us interval
//   TL0 = TH0;
 */
   //CAN config
   TH0 = TIMER0_RL_HIGH;               // Init Timer0 High register
   TL0 = TIMER0_RL_LOW;                // Init Timer0 Low register

   TMOD  = 0x01;                       // Timer0 in 16-bit mode
   CKCON = 0x02;                       // Timer0 uses a 1:48 prescaler
   CKCON |= 0x04;                      // Timer counts SYSCLKs
   IE_ET0 = 1;                         // Timer0 interrupt enabled
   TCON  = 0x10;                       // Timer0 ON

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

   OSCICN = 0x87;                      // Set internal oscillator divider to 1

   SFRPAGE = LEGACY_PAGE;

   RSTSRC = 0x04;                      // Enable missing clock detector

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// CAN_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the CAN peripheral and message objects
//
// CAN Bit Clock : 500kbps
// Auto Retransmit : Automatic Retransmission is enabled
// MsgVal        : Set to Valid based on the #define MESSAGE_OBJECTS
// Filtering     : Enabled for all valid message objects
// Message Identifier : 11-bit standard; Each message object is only used by
//                      one message ID
// Direction     : One buffer each is configured for transmit and receive
// End of Buffer : All message objects are treated as separate buffers
//
// The following interrupts are enabled and are handled by CAN0_ISR
//
// Error Interrupts
// Status Change Interrupt
// Receive Interrupt
// Transmit Interrupt
//-----------------------------------------------------------------------------
void CANinit (void)
{
   uint8_t i;

   uint8_t SFRPAGE_save = SFRPAGE;
   SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

   CAN0CN |= 0x01;                     // Start Initialization mode

   //---------Initialize general CAN peripheral settings

   CAN0CN |= 0x4E;                     // Enable Error and Module Interrupts
                                       // Enable access to bit timing register

   // See the CAN Bit Timing Spreadsheet for how to calculate this value
   CAN0BT = 0x1C02;                    // Based on 24 MHz CAN clock, set the
                                       // CAN bit rate to 500kbps


   //---------Initialize settings common to all message objects

   // Command Mask Register
   CAN0IF1CM = 0x00F0;                 // Write Operation
                                       // Transfer ID Mask, MDir, MXtd
                                       // Transfer ID, Dir, Xtd, MsgVal
                                       // Transfer Control Bits
                                       // Don't set TxRqst or transfer data

   // Mask Registers
   CAN0IF1M1 = 0x0000;                 // Mask Bits 15-0 not used for filtering
   CAN0IF1M2 = 0x5FFC;                 // Ignore Extended Identifier for
                                       // filtering
                                       // Used Direction bit for filtering
                                       // Use ID bits 28-18 for filtering

   // Arbitration Registers
   CAN0IF1A1 = 0x0000;                 // 11-bit ID, so lower 16-bits not used


   //---------Initialize settings for each valid message object

   for (i = 0; i < MESSAGE_OBJECTS; i++) // 0 denotes MsgObject 32!
   {

       if (MessageBoxInUse[i])
         {
           if (MessageBoxDirTx[i])
            {
              // Message Control Registers
    //          CAN0IF1MC = 0x0880 | MessageBoxSize[i];  // Enable Transmit Interrupt
              CAN0IF1MC = 0x0080 | MessageBoxSize[i];  // Disable Transmit Interrupt
                                                  // Message Object is a Single Message
                                                  // Message Size set by #define

              CAN0IF1A2 = 0xA000 | (MessageBoxCanId[i] << 2);  // Set MsgVal to valid
                                               // Set Direction to write
                                               // Set 11-bit Identifier
            }
          else
            {
              CAN0IF1MC = 0x1480 | MessageBoxSize[i];  // Enable Receive Interrupt
                                                  // Message Object is a Single Message
                                                  // Message Size set by #define

              // Arbitration Registers
              CAN0IF1A2 = 0x8000 | (MessageBoxCanId[i] << 2);  // Set MsgVal to valid
                                                     // Set Object Direction to read
                                                     // Set 11-bit Identifier
            }
         }
       else
         {
           // Set remaining message objects to be Ignored
           CAN0IF1A2 = 0x0000;              // Set MsgVal to 0 to Ignore
           CAN0IF1CR = i;                // Start command request

         }
      CAN0IF1CR = i;                // Start command request
      while (CAN0IF1CRH & 0x80);       // Poll on Busy bit
   }
   //--------- CAN initialization is complete

   CAN0CN &= ~0x41;                    // Return to Normal Mode and disable
                                       // access to bit timing register

   EIE2 |= 0x02;                       // Enable CAN interrupts

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// Supporting Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CANsetupMessageObj
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : uint8_t obj_num - message object number to send data
//                             range is 0x01 - 0x20
//              : SI_UU64_t payload - CAN message to send (8 Bytes)
//
//
// Send data using the message object and payload passed as the parameters.
//
//-----------------------------------------------------------------------------

void CAN_Init (void)
{
   uint8_t i;

   uint8_t SFRPAGE_save = SFRPAGE;
   SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

   CAN0CN |= 0x01;                     // Start Initialization mode

   //---------Initialize general CAN peripheral settings

   CAN0CN |= 0x4E;                     // Enable Error and Module Interrupts
                                       // Enable access to bit timing register

   // See the CAN Bit Timing Spreadsheet for how to calculate this value
   CAN0BT = 0x1C02;                    // Based on 24 MHz CAN clock, set the
                                       // CAN bit rate to 500kbps


   //---------Initialize settings common to all message objects

   // Command Mask Register
   CAN0IF1CM = 0x00F0;                 // Write Operation
                                       // Transfer ID Mask, MDir, MXtd
                                       // Transfer ID, Dir, Xtd, MsgVal
                                       // Transfer Control Bits
                                       // Don't set TxRqst or transfer data

   // Mask Registers
   CAN0IF1M1 = 0x0000;                 // Mask Bits 15-0 not used for filtering
   CAN0IF1M2 = 0x5FFC;                 // Ignore Extended Identifier for
                                       // filtering
                                       // Used Direction bit for filtering
                                       // Use ID bits 28-18 for filtering

   // Arbitration Registers
   CAN0IF1A1 = 0x0000;                 // 11-bit ID, so lower 16-bits not used


   //---------Initialize settings for each valid message object

   for (i = 0; i < MESSAGE_OBJECTS; i++) // 0 denotes MsgObject 32!
   {

       if (MessageBoxInUse[i])
         {
           if (MessageBoxDirTx[i])
            {
              // Message Control Registers
    //          CAN0IF1MC = 0x0880 | MessageBoxSize[i];  // Enable Transmit Interrupt
              CAN0IF1MC = 0x0080 | MessageBoxSize[i];  // Disable Transmit Interrupt
                                                  // Message Object is a Single Message
                                                  // Message Size set by #define

              CAN0IF1A2 = 0xA000 | (MessageBoxCanId[i] << 2);  // Set MsgVal to valid
                                               // Set Direction to write
                                               // Set 11-bit Identifier
            }
          else
            {
              CAN0IF1MC = 0x1480 | MessageBoxSize[i];  // Enable Receive Interrupt
                                                  // Message Object is a Single Message
                                                  // Message Size set by #define

              // Arbitration Registers
              CAN0IF1A2 = 0x8000 | (MessageBoxCanId[i] << 2);  // Set MsgVal to valid
                                                     // Set Object Direction to read
                                                     // Set 11-bit Identifier
            }
         }
       else
         {
           // Set remaining message objects to be Ignored
           CAN0IF1A2 = 0x0000;              // Set MsgVal to 0 to Ignore
           CAN0IF1CR = i;                // Start command request

         }
      CAN0IF1CR = i;                // Start command request
      while (CAN0IF1CRH & 0x80);       // Poll on Busy bit
   }
   //--------- CAN initialization is complete

   CAN0CN &= ~0x41;                    // Return to Normal Mode and disable
                                       // access to bit timing register

   EIE2 |= 0x02;                       // Enable CAN interrupts

   SFRPAGE = SFRPAGE_save;
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

