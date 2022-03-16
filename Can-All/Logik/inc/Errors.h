//-----------------------------------------------------------------------------
// Errors.h
//-----------------------------------------------------------------------------
// Copyright 2015 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// This file contains public definitions for error return codes.
//
//-----------------------------------------------------------------------------

#ifndef ERRORS_H
#define ERRORS_H

#include "../inc/compiler_defs.h"
#include <SI_C8051F550_Defs.h>


//-----------------------------------------------------------------------------
// Exported structures, enumerations, and type definitions
//-----------------------------------------------------------------------------

// error codes
typedef enum
{
   NO_ERROR = 0,                       // 0
   INVALID_COMMAND,                    // 1
   COMMAND_FAILED,                     // 2
   INVALID_PARAMS,                     // 3
   C2DR_WRITE_TIMEOUT,                 // 4
   C2DR_READ_TIMEOUT,                  // 5
   C2_POLL_INBUSY_TIMEOUT,             // 6
   C2_POLL_OUTREADY_TIMEOUT,           // 7
   C2_POLL_OTPBUSY_TIMEOUT,            // 8
   DEVICE_READ_PROTECTED,              // 9
   DEVICE_NOT_BLANK,                   // 10
   NOT_A_HEX_RECORD,                   // 11
   UNSUPPORTED_HEX_RECORD,             // 12
   COMMAND_OK,                         // 13 (0x0d)
   BAD_CHECKSUM,                       // 14
   FAMILY_NOT_SUPPORTED,               // 15
   BAD_DEBUG_COMMAND,                  // 16
   DERIVATIVE_NOT_SUPPORTED,           // 17
   READ_ERROR,                         // 18
   OTP_READ_TIMEOUT,                   // 19
   OTP_WRITE_TIMEOUT,                  // 20
   WRITE_ERROR,                        // 21
   SFR_WRITE_TIMEOUT,                  // 22
   SFR_READ_TIMEOUT,                   // 23
   ADDRESS_OUT_OF_RANGE,               // 24
   PAGE_ERASE_TIMEOUT,                 // 25
   DEVICE_ERASE_TIMEOUT,               // 26
   DEVICE_ERASE_FAILURE,               // 27
   DEVICE_IS_BLANK,                    // 28
   IMAGE_OUT_OF_RANGE,                 // 29
   EOF_HEX_RECORD,                     // 30
   VERIFY_FAILURE,                     // 31
   IMAGE_NOT_FORMATTED,                // 32
   JTAG_POLLBUSY_TIMEOUT,              // 33
   JTAG_IREAD_TIMEOUT,                 // 34
   JTAG_IWRITE_TIMEOUT,                // 35
   JTAG_WRITE_COMMAND_TIMEOUT,         // 36
   JTAG_READ_COMMAND_TIMEOUT,          // 37
   //CAN-ALL errors
   // Xxxxxx Error Level 1-8: Level->decide reaction, 9: all Stop
   // xXxxxx Board, 1: Main Controller,  2: Pumps, 3: RevHeater
   // xxXXxx Type,
   // xxxxXX, Number
   ERROR_LEVEL_1 = 100000, // Error Level 1 ->
   ERROR_LEVEL_2 = 200000, // Error Level 2 ->
   ERROR_LEVEL_3 = 300000, // Error Level 3 ->
   ERROR_LEVEL_4 = 400000, // Error Level 4 ->
   ERROR_LEVEL_5 = 500000, // Error Level 5 ->
   ERROR_LEVEL_6 = 600000, // Error Level 6 ->
   ERROR_LEVEL_7 = 700000, // Error Level 7 ->
   ERROR_LEVEL_8 = 800000, // Error Level 8 ->
   ERROR_LEVEL_FATAL = 900000, // Error Level 9 -> Fatal Error, all stop

   //Board Definition
   BOARD_MAIN_CONTROLLER = 100000,
   BOARD_PUMP = 200000,
   BOARD_REV_HEATER = 300000,

   //Type Definition
   TYPE_GENERIC = 100,
   TYPE_BOARD = 200,
   TYPE_INVERTER = 300,
   TYPE_BATTERY = 400,
   TYPE_BENDER = 500,
   TYPE_COMM = 600,
   TYPE_GPIO = 700,
   TYPE_ADC = 800,
   TYPE_HSS = 900,
   TYPE_HW = 1000,



   //Board HW errors
   BOARD_ERROR_GENERIC = TYPE_BOARD + 0, // undefined board error
   BOARD_HSS_A_ERROR = TYPE_BOARD + TYPE_HSS + 1,  // Switch A indicates error
   BOARD_HSS_B_ERROR = TYPE_BOARD + TYPE_HSS + 2 , // Switch B indicates error

   //Inverter errors
   INVERTOR_ERROR_GENERIC = TYPE_INVERTER + 0,   // undefined inverter error
   INVERTOR_ERROR_CAN = TYPE_INVERTER + 1,  //
   INVERTER_ERROR_TEMPERATURE = TYPE_INVERTER + 2, //
   INVERTER_ERROR_BATTERY_LEVEL = TYPE_INVERTER + 3, //
   INVERTER_ERROR_PARAMETER = TYPE_INVERTER + 4, //
   //INVERTER_ERROR_ = 000105, //
   //INVERTER_ERROR_ = 000106, //
   //INVERTER_ERROR_ = 000107, //
   //INVERTER_ERROR_ = 000108, //
   //INVERTER_ERROR_ = 000109, //
   //todo figure out codes/errors

   //Battery Errors
   BATTERY_ERROR_GENERIC = TYPE_BATTERY + 0, //
   BATTERY_ERROR_UNDER_VOLTAGE = TYPE_BATTERY + 1, //
   BATTERY_ERROR_TEMPERATURE = TYPE_BATTERY + 2, //
   //BATTERY_ERROR_ = TYPE_BATTERY + , //
   //BATTERY_ERROR_ = TYPE_BATTERY + , //
   //BATTERY_ERROR_ = TYPE_BATTERY + , //
   //BATTERY_ERROR_ = TYPE_BATTERY + , //
   //BATTERY_ERROR_ = TYPE_BATTERY + , //
   //BATTERY_ERROR_ = TYPE_BATTERY + , //

   //Bender Errors
   BENDER_ERROR_GENERIC = TYPE_BENDER + 0,
   BENDER_ERROR_WARNING = TYPE_BENDER + 1, //  Insulation Warning
   BENDER_ERROR_ERROR = TYPE_BENDER + 2, // Insulation Error
   //BENDER_ERROR_ = 000303, //
   //BENDER_ERROR_ = 000304, //
   //BENDER_ERROR_ = 000305, //
   //BENDER_ERROR_ = 000306, //
   //BENDER_ERROR_ = 000307, //
   //BENDER_ERROR_ = 000308, //
   //BENDER_ERROR_ = 000309, //
   //todo figure out codes/errors

   //Main Controller Board
   BOARD_INTERLOCK_ERROR = BOARD_MAIN_CONTROLLER + TYPE_GPIO + 1,  // InterlockSig high (loop is not closed)
   BOARD_PUMPS_EROR = BOARD_MAIN_CONTROLLER + TYPE_HW + 2,  // one of the pumps is not wirking (signal from Pump-Board)
   //Pump Board
   BOARD_INVERTER_COM =  BOARD_PUMP + TYPE_COMM + 1, // Inverter Temperature cannot be read
   BOARD_VACUUM_PUMP_ERROR = BOARD_PUMP + TYPE_HSS + 2, // Something with the Pump's is wrong
   BOARD_VACUUM_SYSTEM_ERROR = BOARD_PUMP + TYPE_HW + 3 , // Vacuum cannot be build up
   BOARD_VACUUM_SENSOR_ERROR = BOARD_PUMP + TYPE_ADC + 2,   //vacuum Sensor reading wrong

   //Reverse Light, Heater Melting fuse BOARD
   HEATER_FUSE_ERROR = BOARD_REV_HEATER + TYPE_HW + 1,     // Melt Fuse is busted, Heater OFF
}ERROR_RECORD;

//typedef struct
//{
//   unsigned Error_number;
//   U8* Error_string;
//} ERROR_RECORD;

//-----------------------------------------------------------------------------
// Exported prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported global variables
//-----------------------------------------------------------------------------

extern SEGMENT_VARIABLE (ERROR_List[], ERROR_RECORD, SEG_CODE);

#endif // ERRORS_H

//-----------------------------------------------------------------------------
// End of File
//-----------------------------------------------------------------------------

