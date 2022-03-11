/* SK225_I2C_MASTER VirtualComPort.c
 *********************************************
 *      		   (c)2019                   *
 *    GENGE & THOMA AG Lengnau Switzerland   *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *            sigi@gengethoma.ch             *
 *********************************************
 */

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8UB3_Register_Enums.h>                // SI_SFR declarations
#include "VCPXpress.h"
#include "descriptor.h"
#include <stdint.h>
#include "I2cDispatcher.h"
#include "HalDef.h"
#include "MATH.H"
#include "STDIO.H"
#include "STRING.H"

/****************************************************************************
 *  The UDB-Device Section is based on the VCPXpress library from Silabs
 *****************************************************************************/

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define RX_PACKET_SIZE 64
#define TX_PACKET_SIZE 256

#define PB_ABSENT (0)
#define PB_PRESENT (1)
#define PB_NEW_EVENT (3)

uint8_t	LightMode = 0;
uint16_t xdata InCount;                   // Holds size of received packet
uint16_t xdata ToSendCount;                // Holds size of transmitted packet
uint16_t xdata SentCount;                  // Holds size of transmitted packet

uint8_t xdata RX_Packet[RX_PACKET_SIZE];     // Packet received from host
uint8_t xdata TX_Packet[TX_PACKET_SIZE];     // Packet to transmit to host

int8_t Notch = 0;
const uint8_t RedLine[20] = {36, 0, 0, 0, 0, 0, 0, 0, 36, 73, 109, 146, 182, 219, 255, 219, 182, 145, 109, 73};
const uint8_t GreenLine[20] = { 0, 36, 73, 109, 146, 182, 219, 255, 219, 182, 145, 109, 73, 36, 0, 0, 0, 0, 0, 0};
const uint8_t BlueLine[20] = { 219, 255, 219, 182, 145, 109, 73, 36, 0, 0, 0, 0, 0, 0, 0, 36, 73, 109, 146, 182,};
uint8_t DataArrayRead[24];
uint8_t DataArrayWrite[4];

int8_t LastNotch = 0;
uint8_t ButtonState = 0;
uint8_t SwitchDirection = 0;
int8_t SwitchDirectionX = 0;
int8_t SwitchDirectionY = 0;
uint8_t Segment = 0;
uint8_t Red = 255;
uint8_t Green = 0;
uint8_t Blue = 0;
uint8_t MixColor = 0;
uint8_t WorkColor = 0;
uint8_t Intensity = 0;
bool up = false;
bool down = false;
int8_t MaxNotch = 20;
int8_t DeltaNotch = 0;
int16_t Gain = 0;
bool RainbowMode = true;
volatile uint8_t Angle;
volatile uint8_t Supply;
volatile int8_t TempBase;
volatile int8_t TempHandle;
volatile uint8_t Version;
volatile uint8_t serialNo[16];


// Function Prototypes
//-----------------------------------------------------------------------------
void Delay (void);
void Sysclk_Init (void);
static void Port_Init (void);
void myAPICallback(void);

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
   WDTCN = 0xDE;
   WDTCN = 0xAD;
}


/**************************************************************************//**
 * @brief Main loop
 *
 * The main loop sets up the device and then waits forever. All active tasks
 * are ISR driven.
 *
 *****************************************************************************/

void main (void)
{
	uint8_t ButtonState = PB_ABSENT;
	uint8_t DataArray[4];

	//Disable WDT
   SiLabs_Startup();

   VDM0CN = VDM0CN_VDMEN__ENABLED;            // Enable VDD Monitor
   Delay ();                                  // Wait for VDD Monitor to stabilize
   RSTSRC = RSTSRC_PORSF__SET;                // Enable VDD Monitor as a reset source

   Sysclk_Init ();                            // Initialize system clock
   Port_Init ();                              // Initialize crossbar and GPIO
//   Timer0_Init ();                            // Initialize Timer2

   //VCPXpress Initialization
   USB_Init(&InitStruct);

   // Enable VCPXpress API interrupts
   API_Callback_Enable(myAPICallback);

   IE_EA = 1;       // Enable global interrupts

	while (1)// Spin forever
	{
		ButtonState = (ButtonState == PB_ABSENT) ? (PB_IN) ? PB_ABSENT : PB_NEW_EVENT : (PB_IN) ? PB_ABSENT : PB_PRESENT;
		CountDelay(12000);
		if (ButtonState == PB_NEW_EVENT)
		{

			IE_EA = 0;       // Enable global interrupts

			LightMode = (LightMode > 2) ? 0 : LightMode + 1;
			DataArray[0] = 0x00;
			DataArray[1] = (LightMode == 1 || LightMode == 0) ? 0xff : 0x00;
			DataArray[2] = (LightMode == 2 || LightMode == 0) ? 0xff : 0x00;
			DataArray[3] = (LightMode == 3 || LightMode == 0) ? 0xff : 0x00;
			AccessI2C(I2C_Channel_RCC, 240, 4, &DataArray, 0, &DataArray, MULTIPLE_START);
			IE_EA = 1;       // Enable global interrupts

		}
	}
}

// Interrupt Service Routines
//-----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Timer0_ISR
 *
 *
 *****************************************************************************/
/*SI_INTERRUPT(Timer0_ISR, TIMER0_IRQn)
{
  TH0 = TIMER_RELOAD_HIGH;            // Reload Timer0 High register
  TL0 = TIMER_RELOAD_LOW;             // Reload Timer0 Low register
  
}
*/

/**************************************************************************//**
 * @brief VCPXpress callback
 *
 * This function is called by VCPXpress. In this example any received data
 * is immediately transmitted to the UART. On completion of each write the
 * next read is primed.
 *
 *****************************************************************************/
VCPXpress_API_CALLBACK(myAPICallback)
{
   uint32_t INTVAL = Get_Callback_Source();
   uint16_t i;
   static uint8_t DataArray[24];
   static uint8_t xdata OutStr[TX_PACKET_SIZE];     // Packet to transmit to host



   if (INTVAL & DEVICE_OPEN)
   {
      ToSendCount = 0;                  // Holds size of transmitted packet
	  SentCount = 0;                  // Holds size of transmitted packet
      Block_Read(RX_Packet, RX_PACKET_SIZE, &InCount);   // Start first USB Read
   }

   if (INTVAL & RX_COMPLETE)                          // USB Read complete
   {
		   DataArray[0] = 0x00;
		   DataArray[1] = (RX_Packet[0] == 'R' || RX_Packet[0] == 'r' || RX_Packet[0] == 'W' || RX_Packet[0] == 'w') ? 0xff : 0x00;
		   DataArray[2] = (RX_Packet[0] == 'G' || RX_Packet[0] == 'g' || RX_Packet[0] == 'W' || RX_Packet[0] == 'w') ? 0xff : 0x00;
		   DataArray[3] = (RX_Packet[0] == 'B' || RX_Packet[0] == 'b' || RX_Packet[0] == 'W' || RX_Packet[0] == 'w') ? 0xff : 0x00;
		   AccessI2C(I2C_Channel_RCC, 240, 4, &DataArray, 24, &DataArray, MULTIPLE_START);
			i= 0;
		    i += sprintf(&OutStr[i], "\n\rNOTCH:%02BX", DataArray[0]);
		    i += sprintf(&OutStr[i], " SWITCH:%BX", DataArray[1]);
		    i += sprintf(&OutStr[i], " X-AXIS:%+01Bd", ((int8_t)(DataArray[2] & 0xF0)) / 16);
		    i += sprintf(&OutStr[i], " Y-AXIS:%+01Bd", ((int8_t)((DataArray[2] & 0x0F)<<4)) / 16);
		    i += sprintf(&OutStr[i], " ANGLE:%02BX", DataArray[3]);
		    i += sprintf(&OutStr[i], " SUPPLY:%03Bu", DataArray[4]);
		    i += sprintf(&OutStr[i], " TEMP-BASE:%+02Bd", DataArray[5]);
		    i += sprintf(&OutStr[i], " TEMP-HANDLE:%+02Bd", DataArray[6]);
		    i += sprintf(&OutStr[i], " VERSION:%02BX", DataArray[7]);
		    i += sprintf(&OutStr[i], " UNIQUE-ID:%02BX", DataArray[8]);
		    i += sprintf(&OutStr[i], "%02BX", DataArray[9]);
		    i += sprintf(&OutStr[i], "-%02BX", DataArray[10]);
		    i += sprintf(&OutStr[i], "%02BX", DataArray[11]);
		    i += sprintf(&OutStr[i], "-%02BX", DataArray[12]);
		    i += sprintf(&OutStr[i], "%02BX", DataArray[13]);
		    i += sprintf(&OutStr[i], "-%02BX", DataArray[14]);
		    i += sprintf(&OutStr[i], "%02BX", DataArray[15]);
		    i += sprintf(&OutStr[i], "-%02BX", DataArray[16]);
		    i += sprintf(&OutStr[i], "%02BX", DataArray[17]);
		    i += sprintf(&OutStr[i], "-%02BX", DataArray[18]);
		    i += sprintf(&OutStr[i], "%02BX", DataArray[19]);
		    i += sprintf(&OutStr[i], "-%02BX", DataArray[20]);
		    i += sprintf(&OutStr[i], "%02BX", DataArray[21]);
		    i += sprintf(&OutStr[i], "-%02BX", DataArray[22]);
		    i += sprintf(&OutStr[i], "%02BX", DataArray[23]);
		    ToSendCount = i;
		    SentCount = 0;
	        Block_Write(OutStr, ToSendCount, &SentCount);
   }

   if (INTVAL & TX_COMPLETE)                          // USB Write complete
   {
	   if (ToSendCount)
	   {
		   ToSendCount -= SentCount;
	   }
	   if (!ToSendCount)
	   {
		   Block_Read(RX_Packet, RX_PACKET_SIZE, &InCount);   // Start next USB Read
	   }
   }
}

/**************************************************************************//**
 * @brief Clock initialization
 *
 *****************************************************************************/
void Sysclk_Init (void)
{
  SFRPAGE = 0x10;
  HFOCN  = HFOCN_HFO1EN__ENABLED;      // Enable 48Mhz osc
  // need to set SYSCLK to > 24 MHz before switching to HFOSC1
  CLKSEL = CLKSEL_CLKDIV__SYSCLK_DIV_1 | CLKSEL_CLKSL__HFOSC0;
  // wait for clock divider ready
  while ((CLKSEL & CLKSEL_DIVRDY__BMASK) == CLKSEL_DIVRDY__NOT_READY)
  {}
  SFRPAGE = 0x0;
}

/**************************************************************************//**
 * @brief Port initialization
 *
  *****************************************************************************/
static void Port_Init (void)
{
   XBR2 = XBR2_XBARE__ENABLED;      // Enable crossbar
}

/**************************************************************************//**
 * @brief Timer initialization
 *
 * Configure Timer0 to 16-bit auto-reload and generate an interrupt at
 * interval specified by <counts> using SYSCLK/48 as its time base.
 *
 *****************************************************************************/
/*void Timer0_Init()
{
   TH0 = TIMER_RELOAD_HIGH;            // Init Timer0 High register
   TL0 = TIMER_RELOAD_LOW;             // Init Timer0 Low register
   TMOD = TMOD_T0M__MODE1;             // Timer0 in 16-bit mode
   CKCON0 = CKCON0_SCA__SYSCLK_DIV_48; // Timer0 uses a 1:48 prescaler
   IE_ET0 = 1;                         // Timer0 interrupt enabled
   TCON = TCON_TR0__RUN;               // Timer0 ON
}
*/
/**************************************************************************//**
 * @brief delay for approximately 1ms
 *
 *****************************************************************************/
void Delay (void)
{
   int16_t x;

   for (x = 0; x < 500; x)
   {
      x++;
   }
}

