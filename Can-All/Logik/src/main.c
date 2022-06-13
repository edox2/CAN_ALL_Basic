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
#define VACUUM_MAX                600.0   // [mBar] enable vacuum pump at this level todo ben: defnie more accurate
#define INVERTER_TEMPERATURE_MAX  95.0    // [°C] Maximal inverter Temperature -> shut down todo ben: defnie more accurate
#define INVERTER_TEMPERATURE_WARN 65.0    // [°C] Temperature to start cooling todo ben: defnie more accurate
#define INVERTER_TEMPERATURE_MIN  45.0    // [°C] Temperature to stop cooling todo ben: defnie more accurate
#define MOTOR_TEMPERATURE_MAX     95.0      // [°C] Maximal motor Temperature -> shut down todo ben: defnie more accurate
#define MOTOR_TEMPERATURE_WARN    65.0      // [°C] Temperature to start cooling todo ben: defnie more accurate
#define MOTOR_TEMPERATURE_MIN     45.0    // [°C] Temperature to stop cooling todo ben: defnie more accurate
#define VREF                      2200.0  // [mV] ADC Voltage Reference
#define HSS_TEMPERATURE_FACTOR    0.012   // [°C/mV]
#define HSS_TEMPERATURE_OFFSET    -500.0  // [mV] 0°C @ 0.5V @ V_out
#define HSS_CURRENT_FACTOR        0.225   // [A/mV]
#define HSS_CURRENT_OFFSET        0.0     // [mV]
#define HSS_VOLTAGE_FACTOR        0.085   // [V/mV]
#define HSS_VOLTAGE_OFFSET        0.0     // [mV]
#define VACUUM_FACTOR             -0.25    // [mBar/mV]
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
long getHighSideSwitchData(HighSideSwitchData *Switch_A, HighSideSwitchData *Switch_B);
int getHighSideSwitchACurrent_mA(void);
int getHighSideSwitchBCurrent_mA(void);
void SetHSSDiagnostics(void);
long DoMainStartup(void);
long DoPumpStartup(void);
long DoRevHeatStartup(void);
long DoRunMain(void);
long DoRunPump(void);
long DoRunRevHeater(void);
uint16_t getAdcReading_single(uint8_t pin);
long getAdcReading_HighRes(int pin, float *adc_value);
long getPressureReading_mbar(int *value);
void DoMainError(void);
void DoPumpError(void);
void DoRevHeaterError(void);
long checkCan(void);
void enableInterrupts(void);
void disableInterrupts(void);
int isInvertorReady(void);

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
BoardType Board = MAIN; //MAIN, VACUUM_WATER_PUMP, REVERSE_HEATER
StateMachine CURRENT_STATE = ERROR;


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

//leads to problems with generic code (highsideSwitch init etc (works with EN_*)
//#ifndef Board == MAIN
//SI_SBIT(BenderError, SFR_P1, 7);  //A_Port
//SI_SBIT(InterlockSig, SFR_P0, 0); //B_Port
//SI_SBIT(OilPressureWarningLight, SFR_P1, 2);  //EN_A
//SI_SBIT(InterlockRelais, SFR_P1, 3);  //EN_B
//#endif
//
//#ifndef Board == VACUUM_WATER_PUMP
//SI_SBIT(Vacum_ADC, SFR_P1, 7);  //A_Port
//SI_SBIT(VacuumPump, SFR_P1, 2); //EN_A
//SI_SBIT(WatherPump, SFR_P1, 3); //EN_B
//#endif
//
//#ifndef Board == REVERSE_HEATER
//SI_SBIT(Rev_GearSelected, SFR_P1, 7);  //A_Port (Gearbox Sensor)
//SI_SBIT(HeaterMeltFuse, SFR_P2, 0); //B_Port (RevDirSelected)
//SI_SBIT(Rev_DirectionSelected, SFR_P0, 0); //C_Port
//SI_SBIT(HeaterRelais, SFR_P1, 2); //EN_A
//SI_SBIT(ReverseLight, SFR_P1, 3); //EN_B
//#endif

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{

   long error;
   error = NO_ERROR;
   SFRPAGE = LEGACY_PAGE;              // Set SFR Page for PCA0MD

   //disable interrupt's (and CAN)
   IE_EA = 0;                          // Disable global interrupts
   EIE2 &= ~0x02;                      // Disable CAN interrupt

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
   error += checkCan();

   if(error > 0)
   {
     // todo error: init failed, what now?
   }

   CURRENT_STATE = STARTUP;
   SEND_HEARTBEAT = 0;

   //enable interrupt's
   EIE2 |= 0x02;                       // Enable CAN interrupts
   //start CAN
   IE_EA = 1;                          // Enable global interrupts

   switch(Board)
   {
     case(MAIN):
//            SEND_HEARTBEAT = 1; //enable Battery's -> will be enabled by can-i-ball
            error += DoMainStartup();
         break;
     case(VACUUM_WATER_PUMP):
            error += DoPumpStartup();
         break;
     case(REVERSE_HEATER):
            error += DoRevHeatStartup();
         break;
     default:
       error = ERROR_LEVEL_FATAL;
       break;
   }

   if(error > 0)
   {
     // todo error: DoStartup failed, what now?
   }

   CURRENT_STATE = RUN;
   while (1 && CURRENT_STATE == RUN)
   {
     Wait_5ms(10);  //todo: define reasoneable cycling rate

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
     }
     if(error > ERROR_LEVEL_FATAL)
     {
       switch(Board)
       {
         case(MAIN):
               DoMainError();
             break;
         case(VACUUM_WATER_PUMP):
              DoPumpError();
             break;
         case(REVERSE_HEATER):
              DoRevHeaterError();
             break;
       }
       CURRENT_STATE = ERROR;
     }
     else if (error>0)
       {
         //todo error: define error handling
       }

   }                                   // end of while(1) todo: will run out if error state is reached -> define proceeding (cycle ignition?)
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

  if(ST_A == LOW)
    {
      error = BOARD_HSS_A_ERROR;
    }
  else if(ST_B == LOW)
    {
      error = BOARD_HSS_B_ERROR;
    }
  //todo can maybe check CAN
  //todo different stuff?

  return error;
}

// check highside diagnostic's
long getHighSideSwitchData(HighSideSwitchData *Switch_A, HighSideSwitchData *Switch_B)
{
  int adcReading;

  Switch_A->current = 0;//I_Sens = 4.35 mA @ I_out = 20A; I_Sens = 1.74 mA @ I_out = 8A; I_Sens = 0.65 mA @ I_out = 3A; I_Sens = 0.217 mA @ I_out = 1A
  Switch_A->voltage = 0;//I_Sens = 2.25 mA @ T = 150°C; I_Sens = 1.52 mA @ T = 85°C; I_Sens = 0.85 mA @ T = 25°C
  Switch_A->temperature = 0;//I_Sens = 2.25 mA @ T = 150°C; I_Sens = 1.52 mA @ T = 85°C; I_Sens = 0.85 mA @ T = 25°C
  Switch_B->current = 0;
  Switch_B->voltage = 0;
  Switch_B->temperature = 0;

  DIA_EN = HIGH;
  //Check output Current
  SEL1 = LOW;
  SEL2 = LOW;

  if(EN_A)
  {
    //todo Calculate Value according to I_Sens
      //getAdcReading()
      Switch_A->current = (float)getAdcReading_single(P1_0)*HSS_CURRENT_FACTOR+HSS_CURRENT_OFFSET;
  }
  if(EN_B)
  {
      //todo Calculate Value according to I_Sens
      Switch_B->current = (float)getAdcReading_single(P1_1)*HSS_CURRENT_FACTOR+HSS_CURRENT_OFFSET;
  }

  //Check output Voltage
  SEL1 = HIGH;
  SEL2 = HIGH;

  //todo Calculate Value according to I_Sens
  Switch_A->voltage = (float)getAdcReading_single(P1_0)*HSS_VOLTAGE_FACTOR+HSS_VOLTAGE_OFFSET;
  Switch_B->voltage = (float)getAdcReading_single(P1_1)*HSS_VOLTAGE_FACTOR+HSS_VOLTAGE_OFFSET;

  //Check Temperature
  SEL1 = HIGH;
  SEL2 = LOW;

  //todo Calculate Value according to I_Sens
  Switch_A->temperature = (float)getAdcReading_single(P1_0)*HSS_TEMPERATURE_FACTOR+HSS_VOLTAGE_OFFSET;
  Switch_B->temperature = (float)getAdcReading_single(P1_1)*HSS_TEMPERATURE_FACTOR+HSS_VOLTAGE_OFFSET;

  return NO_ERROR;
}

int getHighSideSwitchACurrent_mA()
{
  int adcReading;
  long error = NO_ERROR;

  SetHSSDiagnostics();

  return getAdcReading_single(P1_0)*HSS_CURRENT_FACTOR+HSS_CURRENT_OFFSET;
}

int getHighSideSwitchBCurrent_mA()
{
  int adcReading;
  long error = NO_ERROR;

  SetHSSDiagnostics();

  return getAdcReading_single(P1_1)*HSS_CURRENT_FACTOR+HSS_CURRENT_OFFSET;
}

void SetHSSDiagnostics()
{
  DIA_EN = HIGH;
  //Check output Current
  SEL1 = LOW;
  SEL2 = LOW;
}

//check, enable inverter, battery and bender
long DoMainStartup(void)
{
  long error = NO_ERROR;
  struct BenderIMC bender;
  struct Invertor Invertor;

  //Check battery dynamic battery
  if(NumOfBat == 0)
    {
      error += BATTERY_ERROR_GENERIC;
      return error;
    }

  //check if Bender is up and running
  UpdateBenderImcReadings(&bender);

  if((bender.VifcStatus.OverallImcSelbsttest
      && bender.VifcStatus.ParameterImcSelbsttest
      && bender.VifcStatus.IsolationMeasurementActivated)
      != 1)
      {
        //todo error
      }

  //check inverter state
  if((Invertor.SystemFlags.isPoweringReady != 1)
      && (Invertor.FaultCode == 0)) //todo live: check flags...
    {
      //todo error
    }

  EN_B = HIGH;  //enable Interlock Relais (close interlock loop)

  if(Invertor.SystemFlags.isTractionEnabled != 1)
    {
      //todo error
    }

  return error;
}

//check inverter tempereature, check vacuum sensor
long DoPumpStartup(void)
{
  long error = NO_ERROR;
  int vacuumLevel = 0;
  int vacuumLevel_off = 0;
  int tempCurrent = 0;

  //get Vacuum Level
  error += getPressureReading_mbar(&vacuumLevel_off);

//  enable Vacuum Pump
  EN_A = HIGH;

  Wait_5ms((U16) 10); //todo sigi: does not work (TFms flag gets never set -> timer/interrupt not initialized)

  if(getHighSideSwitchACurrent_mA() < 100)  //check if Pump is running
  {
    error = BOARD_VACUUM_PUMP_ERROR; //pump should be running, seams not ON
  }

  Wait_5ms((U16) 50);

  error += getPressureReading_mbar(&vacuumLevel);

  if(vacuumLevel+50 >= vacuumLevel_off) //check if Vacuum is building up
  {
    if(error == BOARD_VACUUM_PUMP_ERROR)  // pump is not running, no Vacuum is building up
      {
        error = BOARD_VACUUM_PUMP_ERROR;
      }
    else
      {
        error = BOARD_VACUUM_SYSTEM_ERROR;  // pump is running, but vacuum is not building up
      }
    }

  //disable Vacuum Pump
  EN_A = LOW;

  Wait_5ms((U16) 50);

  if(getHighSideSwitchACurrent_mA() > 100)  //check if Pump is OFF
  {
    error = BOARD_VACUUM_PUMP_ERROR; //pump should be running, seams not ON
  }

  //check wather pump as well
  tempCurrent= getHighSideSwitchBCurrent_mA();

  EN_B = HIGH;
  Wait_5ms((U16)10);
  if(getHighSideSwitchBCurrent_mA() <= (tempCurrent+100)) //Water pump is running now, so it should drain at least 100mA more than bevore
    {
      //todo error, water pump not running
    }

  if(error == NO_ERROR) //Board seams OK, signal accordingly
  {
    B_PORT = HIGH;
  }
  return error;
}

//check heater melting fuse
long DoRevHeatStartup(void)
{
  long error = NO_ERROR;

  //todo ben: set internal PullUp of B_Port (P2.0)

  if(B_PORT == HIGH)  //check Heater Melting Fuse
    {
      error = HEATER_FUSE_ERROR;
    }

  return error;
}

long DoRunMain(void)
{
  long error = NO_ERROR;
  struct Invertor invertor;
  struct Battery bat1, bat2, bat3;
  struct BenderIMC bender;

  //check inverter
  UpdateInvertorReadings(&invertor);

  if(invertor.FaultLevel.Blocking)
    {
      error += ERROR_LEVEL_FATAL + BOARD_MAIN_CONTROLLER + INVERTER_ERROR_FAULTLEVEL_BLOCKING;
    }

  if(invertor.Inverter_Temp > INVERTER_TEMPERATURE_MAX)
    {
      error += INVERTER_ERROR_TEMPERATURE;
    }
  if(invertor.Motor_Temp > MOTOR_TEMPERATURE_MAX)
    {
      error += INVERTER_ERROR_TEMPERATURE;
    }
  //todo what more to check?

  //check battery state
  UpdateBatteryReadings(&bat1, 1);
  UpdateBatteryReadings(&bat2, 2);
  UpdateBatteryReadings(&bat3, 3);

  if(bat1.Status.Error || bat2.Status.Error ||bat3.Status.Error )
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

 return error;
}

long DoRunPump(void)
{
  long error = NO_ERROR;
  int vacuumLevel;
  struct Invertor inverter;

  error += getPressureReading_mbar(&vacuumLevel);

  if(error != NO_ERROR)
    {
      //todo error: refine error handling (maybe pump should turn on)
      return error;
    }

  if(vacuumLevel > VACUUM_MAX)
    {
      EN_A = HIGH;
    }

  if (vacuumLevel < VACUUM_MIN)
    {
      EN_A = LOW;
    }


  if(inverter.Inverter_Temp > INVERTER_TEMPERATURE_WARN || inverter.Motor_Temp > MOTOR_TEMPERATURE_WARN)
    {
      EN_A = HIGH;
    }

  if (inverter.Inverter_Temp <= INVERTER_TEMPERATURE_MIN && inverter.Motor_Temp <= MOTOR_TEMPERATURE_MIN)
    {
      EN_A = LOW;
    }


  return error;
}

long DoRunRevHeater(void)
{
  long error = NO_ERROR;

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
      error = HEATER_FUSE_ERROR;
      EN_A = LOW;
    }
  else
    {
      EN_A = HIGH;
    }

  return error;
}

void DoMainError(void)
{
  //todo CAN: shutdown battery's
  }
void DoPumpError(void)
{
  }
void DoRevHeaterError(void)
{
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


long getAdcReading_HighRes(int pin, float *value) //todo: make work
{
  int adcReading = 0;

  //---------------------------
  uint16_t i;                              // Sample counter
  uint32_t accumulator = 0L;               // Where the ADC samples are integrated
  uint16_t currval;                        // Current value of ADC0

  uint8_t SFRPAGE_save = SFRPAGE;
  SFRPAGE = LEGACY_PAGE;

  ADC0MX = pin;

  ADC0CN_ADINT = 0;                         // Clear end-of-conversion indicator
  ADC0CN_ADBUSY = 1;                        // Initiate conversion

  // Accumulate 4096 samples and average to get a 16-bit result
  // 4096 samples = 12 bits; 12 extra bits + 12 samples per bit = 24 bits
  // Shift by 8 bits to calculate a 16-bit result.

  i = 0;
  do
  {
     while (!ADC0CN_ADINT);                 // Wait for conversion to complete
     ADC0CN_ADINT = 0;                      // Clear end-of-conversion indicator

     currval = ADC0;                  // Store latest ADC conversion
     ADC0CN_ADBUSY = 1;                     // Initiate conversion
     accumulator += currval;          // Accumulate
     i++;                             // Update counter
  } while (i != 4096);

  accumulator = accumulator >> 8;     // 8-bit shift to go from 24 to 16 bits

  *value = (uint16_t) (((double) accumulator * (double) VREF) / (double) 65536);

  // Note that the numbers were rounded when equation was solved.
  // This rounding does introduce error to the calculation, but it is
  // negligible when looking for an estimated temperature.

  SFRPAGE = SFRPAGE_save;

  return NO_ERROR;

}
//-----------------------------------------------------------------------------

// calculate and return the pressure measured by the pressure sensor
long getPressureReading_mbar(int *value)
{
 long error = NO_ERROR;
 double vac = 0;
  //Vacuum Sensor 0kPa @ 0.5V, -100kPa @ 4.5V
  //calculate pressure [mBar], based on 101kPa (athmosperic pressure @ 0müm)
//  *value = 101 - (int)(vacuumSensorReading*VacuumFactor+VacuumOffset);
// vac=getAdcReading_single(P1_7);
// vac=(vac+VACUUM_OFFSET)*VACUUM_FACTOR;
// *value=(int)vac;
 *value = (int)(1100+((getAdcReading_single(P1_7)+VACUUM_OFFSET)*VACUUM_FACTOR));

  return error;
}

long checkCan()
{
//  struct BenderIMC bender;

//  struct Battery bat1, bat2, bat3;
//
//  UpdateBatteryReadings(&bat1, 1);
//  UpdateBatteryReadings(&bat2, 2);
//  UpdateBatteryReadings(&bat3, 3);

//  UpdateBenderImcReadings(&bender);

  if (NumOfBat == 0)
    {
      return ERROR_LEVEL_FATAL + CAN_NOT_AVALIABLE; //todo can be checked a better way
    }
  else
    {
      return NO_ERROR;
    }
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
  struct Invertor inv;
  UpdateInvertorReadings(&inv);

  if(inv.FaultLevel == 0)
      {
        return 1;
      }
    else
      {
        return 0;
      }
}
// Initialization Subroutines
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
