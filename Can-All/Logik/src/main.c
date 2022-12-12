//-----------------------------------------------------------------------------
// F560_Ports_SwitchLED.c
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
//
// Program Description:
//
// This program demonstrates how to configure port pins as digital inputs
// and outputs.  The C8051F560 target board has one push-button switch
// connected to a port pin and one LED.  The program constantly checks the
// status of the switch and if it is pushed, it turns on the LED.
//
// Resources Used:
// * Internal Oscillator: MCU clock source
//
// How To Test:
//
// 1) Download code to a 'F560 target board
// 2) Ensure that the Switch and LED pins on the J19 header are
//    properly shorted
// 3) Push the button (P1.4) and see that the LED turns on
//
//
// Target:         C8051F56x (Side A of a C8051F560-TB)
// Tool chain:     Simplicity Studio / Keil C51 9.51
// Command Line:   None
//
// Release 1.2 / 8  MAY 2015 (DL)
//    -Updated to use with Simplicity Studio
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0 / 15 JAN 2009 (GP)
//    -Initial Revision


//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define VACUUM_MIN                400.0   // [mBar] disable vacuum pump at this level todo ben: defnie more accurate
#define VACUUM_MAX                700.0   // [mBar] enable vacuum pump at this level todo ben: defnie more accurate
#define INVERTER_TEMPERATURE_MAX  95.0    // [°C] Maximal inverter Temperature -> shut down todo ben: defnie more accurate
#define INVERTER_TEMPERATURE_WARN 60.0//65.0    // [°C] Temperature to start cooling todo ben: defnie more accurate
#define INVERTER_TEMPERATURE_MIN  45.0//45.0    // [°C] Temperature to stop cooling todo ben: defnie more accurate
#define MOTOR_TEMPERATURE_MAX     95.0    // [°C] Maximal motor Temperature -> shut down todo ben: defnie more accurate
#define MOTOR_TEMPERATURE_WARN    65.0    // [°C] Temperature to start cooling todo ben: defnie more accurate
#define MOTOR_TEMPERATURE_MIN     45.0    // [°C] Temperature to stop cooling todo ben: defnie more accurate
#define VREF                      2200.0  // [mV] ADC Voltage Reference
#define HSS_TEMPERATURE_FACTOR    0.012   // [°C/mV]
#define HSS_TEMPERATURE_OFFSET    -500.0  // [mV] 0°C @ 0.5V @ V_out
#define HSS_CURRENT_FACTOR        0.225   // [A/mV]
#define HSS_CURRENT_OFFSET        0.0     // [mV]
#define HSS_VOLTAGE_FACTOR        0.085   // [V/mV]
#define HSS_VOLTAGE_OFFSET        0.0     // [mV]
#define VACUUM_FACTOR             -0.25   // [mBar/mV]
#define VACUUM_OFFSET             -500.0  // [mV] 0hpa @ 0.5V V_out
#define ADC_TO_MV                 1.915   // [mV/DAC]
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <SI_C8051F550_Defs.h>
#include <SI_C8051F550_Register_Enums.h>
#include "stdint.h"
#include "../inc/Init.h"
#include "../inc/TypeDefs.h"
#include "../inc/compiler_defs.h"
#include "../inc/Common.h"
#include "../inc/Errors.h"
#include "../inc/Timing.h"

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
long doInit(void);
void highSide_Init(void);
long checkBoard(void);
long DoRunMain(void);
long DoRunPump(void);
long DoRunRevHeater(void);
long DoRunDummy(void);
uint16_t getAdcReading_single(uint8_t pin);
long getPressureReading_mbar(int *value);
long isCanOffline(void);
void enableInterrupts(void);
void disableInterrupts(void);
int isInvertorReady(void);
Gear getCurrentGear(void);
uint8_t isRegainActive();


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
BoardType Board = DUMMY; //MAIN, VACUUM_WATER_PUMP, REVERSE_HEATER, DUMMY
uint8_t DEGUG = 1;
StateMachine CURRENT_STATE = ERROR;
Gear CURRENT_GEAR = NEUTRAL;

//-----------------------------------------------------------------------------
// SiLabs_Startup() Routine
// ----------------------------------------------------------------------------
// This function is called immediately after reset, before the initialization
// code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
// useful place to disable the watchdog timer, which is enable by default
// and may trigger before main() in some instances.
//-----------------------------------------------------------------------------
// todo needed?
void SiLabs_Startup (void)
{
   PCA0MD &= ~0x40;                    // Disable the watchdog timer
}

//-----------------------------------------------------------------------------
// Pin Declarations
// todo: maybe move to Init.h
//-----------------------------------------------------------------------------
SI_SBIT(C_Port, SFR_P0, 0);
SI_SBIT(DIA_EN, SFR_P0, 1);
SI_SBIT(SEL1, SFR_P0, 2);
SI_SBIT(SEL2, SFR_P0, 3);
SI_SBIT(UART_TX, SFR_P0, 4);
SI_SBIT(UART_RX, SFR_P0, 5);
SI_SBIT(CAN_TX, SFR_P0, 6);
SI_SBIT(CAN_RX, SFR_P0, 7);
SI_SBIT(SNS_A, SFR_P1, 0);
SI_SBIT(SNS_B, SFR_P1, 1);
SI_SBIT(EN_A, SFR_P1, 2);
SI_SBIT(EN_B, SFR_P1, 3);
SI_SBIT(LATCH, SFR_P1, 4);
SI_SBIT(ST_A, SFR_P1, 5);
SI_SBIT(ST_B, SFR_P1, 6);
SI_SBIT(A_PORT, SFR_P1, 7);
SI_SBIT(B_PORT, SFR_P2, 0);
SI_SBIT(C2D, SFR_P2, 1);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   long error;
   int waitForBreak = 1;
   int test1, test2, test3;

   uint8_t SFRPAGE_save = SFRPAGE;

   error = NO_ERROR;
   SFRPAGE = LEGACY_PAGE;              // Set SFR Page for PCA0MD

   // Init board auxillary's
   CURRENT_STATE = INIT;
   error += doInit();

   SFRPAGE_save = SFRPAGE;
   SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

   //enable CAN and global interrupt's
   EIE2 |= 0x02;                      // enable CAN interrupt
   IE_EA = 1;                         // enable global interrupts

   SFRPAGE = SFRPAGE_save;

   error += checkBoard();

   if(DEBUG == 0)
     {
     while(isCanOffline() != 0 || isCharging())  //Wait untill all CAN are online
       {
         test1 = 0;
         Wait_5ms((U16) 10);
       };
     }

   if(Board == MAIN)  //wait for break press of driver
     {
       while(C_Port != HIGH)
         {
           Wait_5ms((U16) 1);
         }
     }

   CURRENT_GEAR = getCurrentGear();

   CURRENT_STATE = RUN;
   while (CURRENT_STATE == RUN)
   {
       error=0;
//       test1 = (((isCanOffline() == 0) && isInvertorReady()) || DEGUG);
       if(isCharging())
       {
           EN_B=LOW;
           EN_A=LOW;
       }
       else if(((isCanOffline() == 0) && isInvertorReady()) || DEGUG)
       {
         switch(Board)
         {
           case(MAIN):
               error = DoRunMain();
               break;
           case(VACUUM_WATER_PUMP):
               error = DoRunPump();
               break;
           case(REVERSE_HEATER):
                error = DoRunRevHeater();
               break;
           case(DUMMY):
               error = DoRunDummy();
         }
       }
       else
       {
         EN_A = LOW;
         EN_B = LOW;
       }
   }                                   // end of while(1) todo: will run out if error state is reached -> define proceeding (cycle ignition?)
   Wait_5ms((U16) 100);
   CURRENT_STATE = RUN;
}                                      // end of main()

//initialize generic Board Auxillery (ED, SYSCLK, ADC, etc.)
long doInit(void)
{
  WDT_Init ();
  SYSCLK_Init ();
  UART0_Init ();
  PORT_Init ();             // Initialize Port I/O
//  PIN_Init();
  Timer0_Init ();
  PCA0_Init ();
  OSCILLATOR_Init ();       // Initialize Oscillator
  CAN_Init();
  highSide_Init();
  ADC0_Init();

  XBR2      = 0x40;

  return NO_ERROR;
}

//init the High Side Switches
void highSide_Init(void)
{
  LATCH = LOW;  // restart switch if failure occures todo: change later, if error handling is OK
  DIA_EN = LOW;   // enable SNS output for both switches
}

//check board auxillary functins (switches, can, todo: what else?)
long checkBoard(void)
{
  long error = 0;

  EN_A = LOW;
  EN_B = LOW;

//  if(ST_A == LOW)
//    {
//      error = BOARD_HSS_A_ERROR;
//    }
//  else if(ST_B == LOW)
//    {
//      error = BOARD_HSS_B_ERROR;
//    }
  //todo can maybe check CAN
  //todo different stuff?

  return error;
}

long DoRunMain(void)
{
  long error = NO_ERROR;
  Gear tempGear = getCurrentGear();

  //DISABLE INVERTOR IF MOTOR DIRECTION (GEAR) WAS CHANGED
  if(CURRENT_GEAR != tempGear)
    {
      EN_B = LOW;
      if(tempGear == NEUTRAL)
        {
          CURRENT_GEAR = tempGear;
          EN_B = HIGH;
        }
      else if(C_Port == HIGH)  //Enable Invertor if Break is pressed (after gear change)
        {
          CURRENT_GEAR = tempGear;
          EN_B = HIGH;
        }
      else
        {
          return 0;
        }
    }

  EN_B = HIGH;

  //enable break light if regain is happening
//  if(isRegainActive())
//    {
//      EN_A = HIGH;
//    }
//  else
//    {
//      EN_A = LOW;
//    }
//        EN_A = LOW;
//  if(B_PORT == HIGH)
//    {
//      EN_A = HIGH;
//    }
//  else
//    {
//      EN_A = LOW;
//    }


  if(A_PORT == LOW ) //DC-DC FAILURE
    {
    Wait_5ms((U16) 10); //give the relays some time to switch
    if(A_PORT == LOW)
      {
//        EN_B = LOW;
        EN_A = LOW;
        return 1;
      }
    }

  /*
  //check battery state
  UpdateBatteryReadings(&bat1, 1);
  UpdateBatteryReadings(&bat2, 2);
  UpdateBatteryReadings(&bat3, 3);

  if(bat1.Status.Error || bat2.Status.Error ||bat3.Status.Error )
  {
    //todo how to handle if one battery has error
  }
  if(!(bat1.Status.HvRelaisEnable && bat2.Status.HvRelaisEnable && bat3.Status.HvRelaisEnable))
  {
    //todo how to handle if one battery has error
  }
  //todo what more to check?

  //check bender state
  UpdateBenderImcReadings(&bender);
  if(bender.ImcStatus.SystemError)
    {
      //todo error -> bender not working
    }
  if(bender.ImcStatus.IsolationError)
    {
      //todo error
    }
  if(bender.ImcStatus.IsolationWarning)
    {
      //todo error
    }
  //todo what more to check?

  //pump's Signal
  if(B_PORT != HIGH)
    {
      error += BOARD_PUMPS_EROR;
    }

  */
 return 0;
}

long DoRunPump(void)
{
  long error = NO_ERROR;
  int vacuumLevel;
  struct Invertor inv = {0};

  error += getPressureReading_mbar(&vacuumLevel);

  if(vacuumLevel > VACUUM_MAX)
    {
      EN_A = HIGH;
    }

  if (vacuumLevel < VACUUM_MIN)
    {
      EN_A = LOW;
    }

  UpdateInvertorReadings(&inv);

  if(inv.Inverter_Temp > INVERTER_TEMPERATURE_WARN )
    {
      EN_B = HIGH;
    }

  if (inv.Inverter_Temp <= INVERTER_TEMPERATURE_MIN)
    {
      EN_B = LOW;
    }

  return error;
}

long DoRunRevHeater(void)
{
  long error = NO_ERROR;
  uint8_t rev, aport, con1, con2;

  rev = isRevSelected();
  aport = A_PORT;

  con1 = rev && (A_PORT == LOW);
  con2 = (rev == LOW) && aport;

  //do reverse light logic
  if((isRevSelected() && (A_PORT == LOW))
      || ((isRevSelected() == LOW) && A_PORT))
    {
      EN_B = HIGH;
    }
  else
    {
      EN_B = LOW;
    }

  // check melting fuse
  if(B_PORT == HIGH)
    {
//      error = HEATER_FUSE_ERROR;
      EN_A = LOW;
    }
  else
    {
      EN_A = HIGH;
    }

  return error;
}

long DoRunDummy()
{
  if(DEBUG == 1)
    {
      EN_A = HIGH;
      EN_B = HIGH;
      Wait_5ms(20);
      EN_A = LOW;
      EN_B = LOW;
      Wait_5ms(20);
    }
  return 0;
}

uint16_t getAdcReading_single(uint8_t pin)
{
  uint16_t currval;                        // Current value of ADC0

  uint8_t SFRPAGE_save = SFRPAGE;
  SFRPAGE = LEGACY_PAGE;

  ADC0MX = pin;

  ADC0CN_ADINT = 0;                         // Clear end-of-conversion indicator
  ADC0CN_ADBUSY = 1;                        // Initiate conversion

  while (!ADC0CN_ADINT);                 // Wait for conversion to complete
  ADC0CN_ADINT = 0;                      // Clear end-of-conversion indicator

  currval = ADC0;                  // Store latest ADC conversion
  ADC0CN_ADBUSY = 1;                     // Initiate conversion

  SFRPAGE = SFRPAGE_save;
  return (uint16_t)((double)(currval*ADC_TO_MV));
}

// calculate and return the pressure measured by the pressure sensor
long getPressureReading_mbar(int *value)
{
 long error = NO_ERROR;
 double vac = 0;
  //Vacuum Sensor 0kPa @ 0.5V, -100kPa @ 4.5V
  //calculate pressure [mBar], based on 101kPa (athmosperic pressure @ 0müm)
 *value = (int)(1100+((getAdcReading_single(P1_7)+VACUUM_OFFSET)*VACUUM_FACTOR));

  return error;
}

long isCanOffline()
{
  struct BenderIMC bender = {0};
  struct Invertor invertor = {0};
  struct Battery bat1 = {0};
  struct Battery bat2 = {0};
  struct Battery bat3 = {0};
  uint8_t error = 0;

  UpdateBatteryReadings(&bat1, 1);
  UpdateBatteryReadings(&bat2, 2);
  UpdateBatteryReadings(&bat3, 3);

  if((bat1.Status.NodeActive + bat2.Status.NodeActive + bat3.Status.NodeActive) != NumOfBat)
    {
      error = 1;
      //todo error, not all batterys are online
    }

  UpdateInvertorReadings(&invertor);

  if(invertor.SystemFlags.isPoweringEnabled <=0)
    {
      error += 20;
      //todo error, Invertor is not online
    }
    UpdateBenderImcReadings(&bender);

//    if(bender.VifcStatus.ImcAliveStaterecognition == 0)
//    {
//      error += 30;
//    }

    return error;
}

void enableInterrupts()
{
  uint8_t SFRPAGE_save = SFRPAGE;
  SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

  //enable CAN and global interrupt's
  IE_EA = 1;                         // enable global interrupts
  EIE2 |= 0x02;                      // enable CAN interrupt
  SFRPAGE = SFRPAGE_save;
}
void disableInterrupts()
{
  uint8_t SFRPAGE_save = SFRPAGE;
  SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

  //disable CAN and global interrupt's
  IE_EA = 0;                          // Disable global interrupts
  EIE2 &= ~0x02;                      // Disable CAN interrupt
  SFRPAGE = SFRPAGE_save;

}

int isInvertorReady(void)
{
  struct Invertor inv = {0};
  UpdateInvertorReadings(&inv);

  if(inv.FaultLevel == 0 || inv.FaultLevel == 3 || inv.FaultLevel == 4)
    // 0=Ready, 1=Blocking, 2=Stopping, 3=Limiting, 4=Warning
      {
        return 1;
      }
    else
      {
        return 0;
      }
}

Gear getCurrentGear(void)
{
  struct Invertor inv = {0};
  UpdateInvertorReadings(&inv);

  if(inv.SystemFlags.isForwardActive)
    {
      return FORWARD;
    }
  if(inv.SystemFlags.isReverseActive)
    {
      return REVERSE;
    }
  return NEUTRAL;
}

uint8_t isRegainActive()
{
  struct Invertor inv = {0};
  int test;
  UpdateInvertorReadings(&inv);

  test = inv.DC_Bus_Current.s16;

  if(test<0)
    {
      return 1;
    }
  return 0;
}
// Initialization Subroutines
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
