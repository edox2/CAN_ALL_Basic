/*
 * Can.h
 *
 *  Created on: 17.04.2022
 *      Author: benja
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include <SI_C8051F550_Defs.h>
#include <SI_C8051F550_Register_Enums.h>
#include <STRING.h>
#include "../inc/Timing.h"

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK         (24000000)        // System clock speed in Hz

#define POLLRATE_HZ          (20)        // Timer Frequency in Hz.
                                         // Minimum value is 1 (battery needs min. 10Hz)
#define STARTUP_DELAY        (10)        // POLLRATE_HZ /s * STARTUP_DELAY

#define MESSAGE_OBJECTS      (33)        // Number of message objects to use
                                       // Range is 1-32 + 1 Dummy 0 + 32 is same

#define TX_COMPLETE_MASK  (0xFFFFFFFF)   // Set this to (2^MESSAGE_OBJECTS - 1)

#define NUM_OF_BATTERIES (0)            // Deduced by CAN replays, if set to 0.

// Reload value for Timer0 assuming a 1:48 scaling factor for Timer0 clock
#define TIMER0_RL_HIGH  (0xFFFF -((SYSCLK/48/POLLRATE_HZ >> 8) & 0x00FF))
#define TIMER0_RL_LOW   (0xFFFF -(SYSCLK/48/POLLRATE_HZ & 0x00FF))

#define SEND_HEARTBEAT (1)
#define SEND_DBG_COUNTER (0)

// NODE-Ids for Message Objects.


#define CAN_ID_BROADCAST   (0x000)
#define CAN_ID_HEARTBEAT   (0x700) // Test Message of this device
#define CAN_ALL_ID         (0x01B) // Test Message of this device

#define CAN_ID_TEMP   (0x400)

// Contents of message
#define BMS_BAT_ON          (0x01)
#define BMS_BAT_OFF         (0x00)
#define BMS_BAT_CH          (0x02)
#define BMS_STATE_VALID_VALUES (0x0004)
#define BMS_STATE_NODE_ACTIVE  (0x0001)

//-----------------------------------------------------------------------------
// CAN IDs
//-----------------------------------------------------------------------------

#define BAT_CH   (0x02)
#define BAT_ON   (0x01)
#define BAT_OFF  (0x00)

#define CAN_DIR_TX    0x01
#define CAN_DIR_RX    0x00

#define CAN_ID_INVERTER     (0x001) // CAN-ID of Inverter device
#define CAN_ID_BAT1         (0x006) // CAN-ID of Battery 1 BMS
#define CAN_ID_BAT2         (0x007) // CAN-ID of Battery 2 BMS
#define CAN_ID_BAT3         (0x008) // CAN-ID of Battery 3 BMS
#define CAN_ID_BAT4         (0x009) // CAN-ID of Battery 4 BMS
#define CAN_ID_IMD_REQUEST  (0x022) // CAN-ID of Isolation Monitor Request
#define CAN_ID_IMD_RESPONSE (0x023) // CAN-ID of Isolation Monitor Response
#define CAN_ID_IMD_INFO     (0x037) // CAN-ID of Isolation Monitor Info-Messag

#define TPDO1_INV     0x200 // Frame 1 of Invertor
#define TPDO2_INV     0x201 // Frame 2 of Invertor

#define TPDO1_BMS     0x1E0 // Frame 1 of Battery x BMS
#define TPDO2_BMS     0x2E0 // Frame 2 of Battery x BMS
#define TPDO3_BMS     0x3E0 // Frame 3 of Battery x BMS
#define TPDO4_BMS     0x4E0 // Frame 4 of Battery x BMS
#define RPDO1_BMS     0x200 // Frame 5 of Battery x BMS

//-----------------------------------------------------------------------------
// CAN OBJECTS
//-----------------------------------------------------------------------------

#define SET_BAT1_STATE (1)  // Message Object to transmit State Msg to BAT1
#define SET_BAT2_STATE (2)  // Message Object to transmit State Msg to BAT2
#define SET_BAT3_STATE (3)  // Message Object to transmit State Msg to BAT3
#define SET_BAT4_STATE (4)  // Message Object to transmit State Msg to BAT4

#define GET_BAT1_STATE (5)  // Message Object to receive State Msg from BAT1
#define GET_BAT2_STATE (6)  // Message Object to receive State Msg from BAT2
#define GET_BAT3_STATE (7)  // Message Object to receive State Msg from BAT3
#define GET_BAT4_STATE (8)  // Message Object to receive State Msg from BAT4

#define SET_BROADCAST  (9)  // Message Object to transmit Broadcast such as <Start All Nodes>
#define SET_HEARTBEAT (10)  // Message Object to transmit Heartbeat or Startup Message to the CAN-BUS
#define GET_DEBUG_IN  (11)  // Message Object to receive Commands to this Module
#define SET_DEBUG_OUT (12)  // Message Object to transmit Commands to this Module

#define XET_DUMMY_13  (13)  // Message Object to
#define XET_DUMMY_14  (14)  // Message Object to
#define XET_DUMMY_15  (15)  // Message Object to
#define XET_DUMMY_16  (16)  // Message Object to

#define GET_INV_STATE1 (17)  // Message Object to receive first State Msg from INVERTER
#define GET_INV_STATE2 (18)  // Message Object to receive second State Msg from INVERTER
#define GET_IMD_INFO   (19)  // Message Object to receive Isolation Monitor Device Information
#define XET_DUMMY_20   (20)  // Message Object to

#define GET_BAT1_CURRENT  (21)  // Message Object to TPDO3 of BMS from BAT1
#define GET_BAT2_CURRENT  (22)  // Message Object to TPDO3 of BMS from BAT2
#define GET_BAT3_CURRENT  (23)  // Message Object to TPDO3 of BMS from BAT3
#define GET_BAT4_CURRENT  (24)  // Message Object to TPDO3 of BMS from BAT4

#define XET_DUMMY_25  (25)  // Message Object to
#define XET_DUMMY_26  (26)  // Message Object to
#define XET_DUMMY_27  (27)  // Message Object to
#define XET_DUMMY_28  (28)  // Message Object to

#define XET_DUMMY_29  (29)  // Message Object to
#define XET_DUMMY_30  (30)  // Message Object to
#define XET_DUMMY_32  (31)  // Message Object to
#define GET_BROADCAST (32)  // Message Object to get all the stuff not covered by other Message objects


//-----------------------------------------------------------------------------
// Bit Definition Masks
//-----------------------------------------------------------------------------

// CAN0STAT
#define BOff  (0x80)                     // Busoff Status
#define EWarn (0x40)                     // Warning Status
#define EPass (0x20)                     // Error Passive
#define RxOk  (0x10)                     // Receive Message Successfully
#define TxOk  (0x08)                     // Transmitted Message Successfully
#define LEC   (0x07)                     // Last Error Code

typedef union SI_UU64
{
  uint32_t u32[2];
  uint16_t u16[4];
  uint8_t u8[8];
} SI_UU64_t;

//-----------------------------------------------------------------------------
// CAN STATICS
//-----------------------------------------------------------------------------

static uint8_t NumOfBat = 0;


static const uint16_t MessageBoxCanId[MESSAGE_OBJECTS] = // List of all CAN IDs associated to the CAN-objects
{
    CAN_ID_BROADCAST, // Dummy Line MsgObj0
    RPDO1_BMS + CAN_ID_BAT1, RPDO1_BMS + CAN_ID_BAT2, RPDO1_BMS + CAN_ID_BAT3, RPDO1_BMS + CAN_ID_BAT4, TPDO1_BMS + CAN_ID_BAT1, TPDO1_BMS + CAN_ID_BAT2, TPDO1_BMS + CAN_ID_BAT3, TPDO1_BMS + CAN_ID_BAT4,  //line 1-8
    CAN_ID_BROADCAST, CAN_ID_HEARTBEAT + CAN_ALL_ID, CAN_ALL_ID, CAN_ALL_ID, 0x013, 0x014, 0x015, 0x016,  //line 9-16
    0x017, 0x018, 0x019, 0x020, 0x021, 0x022, 0x023, 0x024,  //line 17-24
    0x025, 0x026, 0x027, 0x028, 0x029, 0x030, 0x031, CAN_ID_BROADCAST,  //line 25-32
};

static const uint8_t MessageBoxDirTx[MESSAGE_OBJECTS] = // List of all directions associated to the CAN-objects
{
    CAN_DIR_RX, // Dummy Line MsgObj0
    CAN_DIR_TX, CAN_DIR_TX, CAN_DIR_TX, CAN_DIR_TX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX,  //line 1-8
    CAN_DIR_TX, CAN_DIR_TX, CAN_DIR_RX, CAN_DIR_TX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX,  //line 9-16
    CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX,  //line 17-24
    CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX, CAN_DIR_RX,  //line 25-32
};

static const uint8_t MessageBoxSize[MESSAGE_OBJECTS] = // List of all size  of all active CAN-objects 0-7
{
    8,// Dummy Line MsgObj0 Copy of 32!
    8, 8, 8, 8, 8, 8, 8, 8,   //line 1-8
    2, 1, 1, 2, 0, 0, 0, 0,   //line 9-16
    0, 0, 0, 0, 0, 0, 0, 0,   //line 17-24
    0, 0, 0, 0, 0, 0, 0, 8,   //line 25-32
};

static const uint8_t MessageBoxInUse[MESSAGE_OBJECTS] = // List of all active CAN-objects
{
    1,  // Dummy Line MsgObj0 Copy of 32!
    1, 1, 1, 1, 1, 1, 1, 1,   //line 1-8
    1, 1, 1, 1, 0, 0, 0, 0,   //line 9-16
    0, 0, 0, 0, 0, 0, 0, 0,   //line 17-24
    0, 0, 0, 0, 0, 0, 0, 1,   //line 25-32
};

#endif /* INC_CAN_H_ */
