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
   HIGH_SIDE_SWITCH_A_ERROR = 100,  //Switch A indicates error
   HIGH_SIDE_SWITCH_B_ERROR = 101, //Switch B indicates error
   MAIN_BOARD_ERROR = 200,   // Startup/HW error (board is not working properly)
   BENDER_ERROR = 201,     // error code by CAN? (warning, error, todo figure out codes)
   INVERTOR_ERROR = 202,   // error code by CAN? (Voltage, temperature todo figure out codes)
   INTERLOCK_ERROR = 203,  // InterlockSig high (loop is not closed) -> disable battery's
   PUMP_ERROR = 204,       // Something with the Pump's is wrong
   PUMP_BOARD_ERROR = 300,   // Startup/HW error (board is not working properly)
   INVERTER_COM = 301,           // Inverter Temperature cannot be read
   VACUUM_ERROR = 302,           // Vacuum cannot be build up
   VACUUM_PUMP_ERROR = 303,   //Vacuum pump not responding
   VACUUM_SENSOR_ERROR = 304,   //vacuum Sensor reading wrong
   REV_HEATER_BOARD_ERROR = 400,   // Startup/HW error (board is not working properly)
   HEATER_ERROR = 401,     // Melt Fuse is busted, Heater OFF
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

