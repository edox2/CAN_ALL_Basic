/*
 * Init.h
 *
 *  Created on: 12.03.2022
 *      Author: benja
 */

//-----------------------------------------------------------------------------
// Init.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for initialization functions.
//
//-----------------------------------------------------------------------------

#ifndef INIT_H
#define INIT_H


// Board Types
typedef enum
{
   MAIN = 0,              //Main CAN communicator (Battery/inverter),
                          //IN: BenderError, interlock Signal, Pump OK
                          //OUT: OilPressure(Dash Light for Bender error), interlock Relais (open/close interlock loop)

   VACUUM_WATER_PUMP = 1, //water and Vacuum Pump controller, CAN inverter temperature
                          //IN: Vacuum Sensor (ADC)
                          //OUT: water Pump (depending on inverter Temp. (CAN)), VakuumPump (depending on Vacuum(Schmitt-trigger), Pump OK

   REVERSE_HEATER = 2,    //Heater, Reverse Light controller
                          //IN: Reverse Gear Selector (FNR), Reverse Gear Sensor (gearbox), Heater Melting Fuse (internal pull UP)
                          //OUT: Heater Relais, Reverse Light
}BoardType;

//Main State Machine
typedef enum
{
   INIT,                  // init pins, ports, clk's, etc...
   STARTUP,               // Test Auxillary's (CAN,
   RUN,                   // check inverter, close inverter Relais, check interlock loop, start battery's, check inverter, check Bender
                          // all is good (Interlock closed, battery's ON, Inverter OK), go to error state if something happens
   ERROR,                 // check error, decide how to proceed

}StateMachine;

//Can Nodes
//todo figure out nodes
typedef enum
{
  CAN_NO_BATTERY_1 = 84, //todo figure out
  CAN_NO_BATTERY_2 = 85, //todo figure out
  CAN_NO_BATTERY_3 = 86, //todo figure out
  CAN_NO_BATTERY_4 = 87, //todo figure out
  CAN_NO_BATTERY_5 = 88, //todo figure out
  CAN_NO_INVERER = 1, //todo figure out
  CAN_NO_CHARGER = 2, //todo figure out
  CAN_NO_CAN_ALL_MAIN = 110,  //todo define
  CAN_NO_CAN_ALL_PUMP = 111,  //todo define
  CAN_NO_CAN_ALL_REV_HEATER = 112,  //todo define

}CanNumber;

//pin declaration to select ADC0 Mux (ADC0MX register)
typedef enum
{
  P0_0 = 0x00,
  P0_1 = 0x01,
  P0_2 = 0x02,
  P0_3 = 0x03,
  P0_4 = 0x04,
  P0_5 = 0x05,
  P0_6 = 0x06,
  P0_7 = 0x07,
  P1_0 = 0x08,
  P1_1 = 0x09,
  P1_2 = 0x0A,
  P1_3 = 0x0B,
  P1_4 = 0x0C,
  P1_5 = 0x0D,
  P1_6 = 0x0E,
  P1_7 = 0x0F,
  P2_0 = 0x10,
  P2_1 = 0x11
}ADC_PIN;

//typedef enum
//{
//  NO_ERROR = 0,
//  MAIN_BOARD_ERROR = 100,   // Startup/HW error (board is not working properly)
//  BENDER_ERROR = 101,     // error code by CAN? (warning, error, todo figure out codes)
//  INVERTOR_ERROR = 102,   // error code by CAN? (Voltage, temperature todo figure out codes)
//  INTERLOCK_ERROR = 103,  // InterlockSig high (loop is not closed) -> disable battery's
//  PUMP_ERROR = 104,       // Something with the Pump's is wrong
//  PUMP_BOARD_ERROR = 200,   // Startup/HW error (board is not working properly)
//  INVERTER_COM = 201,           // Inverter Temperature cannot be read
//  VACUUM_ERROR = 202,           // Vacuum cannot be build up
//  REV_HEATER_BOARD_ERROR = 300,   // Startup/HW error (board is not working properly)
//  HEATER_ERROR = 301,     // Melt Fuse is busted, Heater OFF
//
//}CanALLErrors;

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

extern void WDT_Init (void);
extern void SYSCLK_Init (void);
extern void UART0_Init (void);
extern void PORT_Init (void);
extern void Timer0_Init (void);
extern void PCA0_Init (void);
extern void OSCILLATOR_Init (void);
extern void CAN_Init(int canNode);
extern void ADC0_Init (void);
extern void SetADC0Mux(int pin);

#endif // INIT_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------


