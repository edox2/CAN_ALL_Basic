/*
 * CAN.c
 * Based on Sigis CanObjectDispatcherV5
 *  Created on: 17.04.2022
 *      Author: benja
 */



//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <SI_C8051F550_Defs.h>
#include <SI_C8051F550_Register_Enums.h>
#include <STRING.h>
#include "../inc/Can.h"


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void CANsetupMessageObj(uint8_t objNum, uint16_t canId, uint8_t dirTx,  uint8_t len, SI_UU64_t payload);
void CANtransferMessageObj(uint8_t objNum, SI_UU64_t payload);
void CANtriggerMessageObj(uint8_t objNum);
SI_UU64_t CANgetMessageObjPayload(uint8_t objNum);
int16_t CANgetMessageInt(uint8_t objNum, uint8_t startIndex);

SI_INTERRUPT_PROTO(CAN0_ISR, INTERRUPT_CAN0);
SI_INTERRUPT_PROTO(TIMER0_ISR, INTERRUPT_TIMER0);


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

bit CAN_ERROR = 0;                     // 0 = No Errors during transmission
                                       // 1 = Some error(s) occurred

SI_UU32_t CAN_RXTX_COMPLETE;           // Bit status register that is updated
                                       // when a RX or TX is complete is received for
                                       // a specific message object.  Should be
                                       // equal to TX_COMPLETE_MASK when done

SI_UU64_t LastBroadcastMsg;                // TempData to define CAN-Messages


uint8_t DebugInput = 0;
uint16_t Battery1SoC;
uint16_t Battery1SoH;
uint16_t Battery2SoC;
uint16_t Battery2SoH;
uint16_t Battery3SoC;
uint16_t Battery3SoH;
uint16_t Battery4SoC;
uint16_t Battery4SoH;
uint16_t BatterySoC;
uint16_t BatterySoH;
int16_t Current1;
int16_t Current2;
int16_t Current3;
int16_t Current4;



//-----------------------------------------------------------------------------
// PUBLIC FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PRIVATE FUNCTIONS
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
void CANsetupMessageObj(uint8_t objNum, uint16_t canId, uint8_t dirTx,  uint8_t len, SI_UU64_t payload)
{
     uint8_t SFRPAGE_save = SFRPAGE;
     SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

     // Deactivate MessageObject before modification can take place
     CAN0IF1A2 = 0x0000;              // Set MsgVal to 0 to Ignore
     CAN0IF1CR = objNum;              // Start command request
     while (CAN0IF1CRH & 0x80);       // Poll on Busy bit

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


     if (dirTx)
       {
         // Message Control Registers
 //        CAN0IF1MC = 0x0880 | len;  // Enable Transmit Interrupt
         CAN0IF1MC = 0x0080 | len;  // Disable Transmit Interrupt
                                            // Message Object is a Single Message
                                             // Message Size set by variable: len


         CAN0IF1A2 = 0xA000 | (canId << 2);  // Set MsgVal to valid

         CAN0IF1DA1L = payload.u8[0]; // Initialize data registers based
         CAN0IF1DA1H = payload.u8[1]; // on message object used
         CAN0IF1DA2L = payload.u8[2];
         CAN0IF1DA2H = payload.u8[3];
         CAN0IF1DB1L = payload.u8[4];
         CAN0IF1DB1H = payload.u8[5];
         CAN0IF1DB2L = payload.u8[6];
         CAN0IF1DB2H = payload.u8[7];
       }
     else
       {

         // Message Control Registers
         CAN0IF1MC = 0x1480 | len;  // Enable Receive Interrupt
                                             // Message Object is a Single Message
                                             // Message Size set by variable: len
         // Arbitration Registers
         CAN0IF1A2 = 0x8000 | (canId << 2);  // Set MsgVal to valid
                                             // Set 11-bit Identifier to iter
       }
     CAN0IF1CR = objNum;                // Start command request

     while (CAN0IF1CRH & 0x80);       // Poll on Busy bit

     // Message Control Registers
     CAN0IF1MC = 0x0080 | len;  // Disable Transmit Interrupt
                                         // Message Object is a Single Message
                                         // Message Size set by #define
     // Arbitration Registers
     CAN0IF1A1 = 0x0000;                 // 11-bit ID, so lower 16-bits not used

     // Arbitration Registers
     CAN0IF1A2 = 0xA000 | (canId << 2);  // Set MsgVal to valid
                                            // Set Direction to write
                                            // Set 11-bit Identifier to iter

     CAN0IF1CR = objNum;            // Start command request

     while (CAN0IF1CRH & 0x80);        // Poll on Busy bit
     SFRPAGE = SFRPAGE_save;
}


//-----------------------------------------------------------------------------
// CANtransferMessageObj
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : uint8_t obj_num - message object number to send data
//                             range is 0x01 - 0x20
//              : SI_UU64_t payload - CAN message to send (8 Bytes)
//
// Send data using the message object and payload passed as the parameters.
//
//-----------------------------------------------------------------------------

void CANtransferMessageObj(uint8_t objNum, SI_UU64_t payload)
{

   uint8_t SFRPAGE_save = SFRPAGE;
   SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

   // Initialize all 8 data bytes even though they might not be sent
   // The number to send was configured earlier by setting Message Control

   CAN0IF1DA1L = payload.u8[0]; // Initialize data registers based
   CAN0IF1DA1H = payload.u8[1]; // on message object used
   CAN0IF1DA2L = payload.u8[2];
   CAN0IF1DA2H = payload.u8[3];
   CAN0IF1DB1L = payload.u8[4];
   CAN0IF1DB1H = payload.u8[5];
   CAN0IF1DB2L = payload.u8[6];
   CAN0IF1DB2H = payload.u8[7];

   CAN0IF1CM = 0x0087;                 // Set Direction to Write
                                       // Write TxRqst, all 8 data bytes
                                       // (chapter 4.4 Bosch C_CAN)

   CAN0IF1CR = objNum;                // Start command request

   while (CAN0IF1CRH & 0x80);          // Poll on Busy bit

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// CANtriggerMessageObj
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : uint8_t obj_num - message object number to send data
//                             range is 0x01 - 0x20
///
// Send data using the message object and payload passed while init phase.
//
//-----------------------------------------------------------------------------

void CANtriggerMessageObj(uint8_t objNum)
{

   uint8_t SFRPAGE_save = SFRPAGE;
   SFRPAGE  = CAN0_PAGE;               // All CAN register are on page 0x0C

   CAN0IF1CR = objNum;                // Start command request

   while (CAN0IF1CRH & 0x80);          // Poll on Busy bit

   SFRPAGE = SFRPAGE_save;
}


//-----------------------------------------------------------------------------
// CANgetMessageObjPayload
//-----------------------------------------------------------------------------
//
// Return Value : SI_UU64_t payload -Full CAN-Message
// Parameters   : uint8_t objNum - message object number to read data from
//                             range is 0x01 - 0x20
//
// Provides an atomic section for the consistent access to a specific payload
//
//-----------------------------------------------------------------------------

SI_UU64_t CANgetMessageObjPayload(uint8_t objNum)
{
  SI_UU64_t payload;
  uint8_t intStatSave = IE_EA;         // Preserve state of global Interrupts

  IE_EA = 0;                            // Disable global interrupts
//  EIE2 &= ~0x02;                      // Disable CAN interrupts
  payload = CanMsgObject[objNum];
//  EIE2 |= 0x02;                       // Enable CAN interrupts
  IE_EA = intStatSave;                 // restore global interrupts

  return payload;
}



//-----------------------------------------------------------------------------
// CANgetMessageInt
//-----------------------------------------------------------------------------
//
// Return Value : int16_t
// Parameters   : uint8_t objNum - message object number to read data from
//                             range is 0x01 - 0x20
//                uint8_t startIndex - index where to start conversion from
//
// Provides an atomic section for the consistent access to a specific signed word
//
//-----------------------------------------------------------------------------

int16_t CANgetMessageInt(uint8_t objNum, uint8_t startIndex)
{
  SI_UU64_t payload;
  SI_UU16_t result;
  uint8_t intStatSave = IE_EA;         // Preserve state of global Interrupts

  IE_EA = 0;                            // Disable global interrupts
//  EIE2 &= ~0x02;                      // Disable CAN interrupts
  payload = CanMsgObject[objNum];
//  EIE2 |= 0x02;                       // Enable CAN interrupts
  IE_EA = intStatSave;                 // restore global interrupts
  result.u8[0] = payload.u8[startIndex+1];
  result.u8[1] = payload.u8[startIndex];
  return result.s16;
}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TIMER0_ISR
//-----------------------------------------------------------------------------
//
// The ISR is triggered upon any CAN errors or upon a message reception
//
// If an error occurs, a global counter is updated
//
//-----------------------------------------------------------------------------

SI_INTERRUPT(TIMER0_ISR, TIMER0_IRQn)
{
   // Timer0 ISR pending flag is automatically cleared by vectoring to ISR

  SI_UU64_t DataBaguette;                // TempData to define CAN-Messages
  static uint16_t FirstRun = STARTUP_DELAY; //Skip at startup
  static uint8_t NumOfBat = 0;

  uint8_t TestBat1 = 0;
  uint8_t TestBat2 = 0;
  uint8_t TestBat3 = 0;
  uint8_t TestBat4 = 0;


#if (SEND_DBG_COUNTER == 1)
  static uint16_t DbgCounter = 0;

  DataBaguette.u16[0] = DbgCounter;
  DataBaguette.u8[0] ^= DataBaguette.u8[1];    // Swap Bytes by XORing Step1
  DataBaguette.u8[1] ^= DataBaguette.u8[0];    // Swap Bytes by XORing Step2
  DataBaguette.u8[0] ^= DataBaguette.u8[1];    // Swap Bytes by XORing Step3
  CANtransferMessageObj(SET_DEBUG_OUT, DataBaguette);
  CANtriggerMessageObj(SET_DEBUG_OUT);      // Send the prepared Message-Objects to the CAN-Bus
  DbgCounter++;
#endif


  if (FirstRun)
    {
      //comment if other client sends <reset all nodes>
      if (FirstRun == STARTUP_DELAY) // Do this only once before startup
        {
          DataBaguette.u8[0] = 0x00;    // Prepare CAN-Message <Reset all Nodes>
          DataBaguette.u8[1] = 0x81;
          CANtransferMessageObj(SET_BROADCAST, DataBaguette);
          CANtriggerMessageObj(SET_BROADCAST);      // Send the prepared Message-Objects to the CAN-Bus
      }


      if (FirstRun == (STARTUP_DELAY >> 2)) // Do this only once during startup
      {
          DataBaguette.u8[0] = 0x01;    // Prepare Message <Start All Nodes>
          DataBaguette.u8[1] = 0x00;
          CANtransferMessageObj(SET_BROADCAST, DataBaguette);
          CANtriggerMessageObj(SET_BROADCAST);      // Send the prepared Message-Objects to the CAN-Bus
      }


      if (FirstRun == 1) // Do this only once after startup
      {
          DataBaguette.u8[0] = 0x00;    // Prepare CAN-Message <Startup>
          CANtransferMessageObj(SET_HEARTBEAT, DataBaguette);
          CANtriggerMessageObj(SET_HEARTBEAT);      // Send the prepared Message-Objects to the CAN-Bus
      }

      FirstRun--;
    }
  else
    {
#if (SEND_HEARTBEAT == 1)
      DataBaguette.u8[0] = 0x05;    // Prepare CAN-Message <Heartbeat>
      CANtransferMessageObj(SET_HEARTBEAT, DataBaguette);
      CANtriggerMessageObj(SET_HEARTBEAT);      // Send the prepared Message-Objects to the CAN-Bus
#endif

    }

  // Count the number of batteries after <first-run> phase
  TestBat1 = CANgetMessageInt(GET_BAT1_STATE, 0) & 0x01;
  TestBat2 = CANgetMessageInt(GET_BAT2_STATE, 0) & 0x01;
  TestBat3 = CANgetMessageInt(GET_BAT3_STATE, 0) & 0x01;
  TestBat4 = CANgetMessageInt(GET_BAT4_STATE, 0) & 0x01;
  NumOfBat = TestBat1 + TestBat2 + TestBat3 + TestBat4;


  DataBaguette.u32[0] = 0x00000000;
  DataBaguette.u32[1] = 0x00000000;

  if (NUM_OF_BATTERIES != 0)
    NumOfBat = NUM_OF_BATTERIES;

  if ((CANgetMessageInt(GET_BAT1_STATE, 0) & BMS_STATE_VALID_VALUES) +
      (CANgetMessageInt(GET_BAT2_STATE, 0) & BMS_STATE_VALID_VALUES) +
      (CANgetMessageInt(GET_BAT3_STATE, 0) & BMS_STATE_VALID_VALUES) +
      (CANgetMessageInt(GET_BAT4_STATE, 0) & BMS_STATE_VALID_VALUES) ==
      (BMS_STATE_VALID_VALUES * NumOfBat)) // Dynamic Num of Bat
 //     (BMS_STATE_VALID_VALUES * NUM_OF_BATTERIES)) // Static num of bat
    {
      DataBaguette.u8[0] = BAT_ON;
    }
  else
    {
      DataBaguette.u8[0] = BAT_OFF;
    }

  if (TestBat1)
    {
      CANtransferMessageObj(SET_BAT1_STATE, DataBaguette);
      CANtriggerMessageObj(SET_BAT1_STATE);
    }

  if (TestBat2)
    {
      CANtransferMessageObj(SET_BAT2_STATE, DataBaguette);
      CANtriggerMessageObj(SET_BAT2_STATE);
    }

  if (TestBat3)
    {
      CANtransferMessageObj(SET_BAT3_STATE, DataBaguette);
      CANtriggerMessageObj(SET_BAT3_STATE);
    }

  if (TestBat4)
    {
      CANtransferMessageObj(SET_BAT4_STATE, DataBaguette);
      CANtriggerMessageObj(SET_BAT4_STATE);
    }
}


//-----------------------------------------------------------------------------
// CAN0_ISR
//-----------------------------------------------------------------------------
//
// The ISR is triggered upon any CAN errors or upon a complete transmission
//
// If an error occurs, a global counter is updated
//
//-----------------------------------------------------------------------------

SI_INTERRUPT(CAN0_ISR, CAN0_IRQn)
{
   // SFRPAGE is set to CAN0_Page automatically when ISR starts

   uint8_t status = CAN0STAT;          // Read status, which clears the Status
                                       // Interrupt bit pending in CAN0IID

   uint8_t Interrupt_ID = CAN0IID;     // Read which message object caused
                                       // the interrupt

   CAN0IF1CM = 0x007F;                 // Read all of message object to IF1
                                       // Clear IntPnd and newData


   CAN0IF1CR = Interrupt_ID;           // Start command request to actually
                                       // clear the interrupt

   // Message Object 0 reports as 0x20 in the CAN0IID register, so
   // convert it to 0x00
   if (Interrupt_ID == 0x20)
   {
      Interrupt_ID = 0x00;
   }


   while (CAN0IF1CRH & 0x80) {}        // Poll on Busy bit

   if (status & (TxOk | RxOk))                  // If transmit completed successfully
   {
      // Set variable to indicate this message object's transfer completed


      // Bit-shifting doesn't work with an operator greater than 15, so
      // account for it
      if (Interrupt_ID <= 15)
      {
        CAN_RXTX_COMPLETE.u32 |= (uint16_t) (0x01 << Interrupt_ID);
      }
      else if (Interrupt_ID <= 0x1F)
      {
         CAN_RXTX_COMPLETE.u16[MSB] |= (uint16_t) (0x01 << (Interrupt_ID - 16));
      }
   }

   //if ((status & RxOk) && (Interrupt_ID != 0))

   if (status & RxOk)                  // If transmit completed successfully
   {
       // Read all 8 data bytes to rxdata, even though they might not be valid

       CanMsgObject[Interrupt_ID].u8[0] = CAN0IF1DA1L;    // First pair of bytes
       CanMsgObject[Interrupt_ID].u8[1] = CAN0IF1DA1H;    // First pair of bytes
       CanMsgObject[Interrupt_ID].u8[2] = CAN0IF1DA2L;    // Second pair of bytes
       CanMsgObject[Interrupt_ID].u8[3] = CAN0IF1DA2H;    // Second pair of bytes
       CanMsgObject[Interrupt_ID].u8[4] = CAN0IF1DB1L;    // Third pair of bytes
       CanMsgObject[Interrupt_ID].u8[5] = CAN0IF1DB1H;    // Third pair of bytes
       CanMsgObject[Interrupt_ID].u8[6] = CAN0IF1DB2L;    // Fourth and last pair of bytes
       CanMsgObject[Interrupt_ID].u8[7] = CAN0IF1DB2H;    // Fourth and last pair of bytes
       CanMsgId[Interrupt_ID] = CAN0IF1A2;

   }




   // If an error occurred, simply update the global variable and continue
   if (status & LEC)
   {
       // The LEC bits identify the type of error, but those are grouped here
      if ((status & LEC) != 0x07)
      {
          CAN_ERROR = 1;
      }
   }

   if (status & BOff)
   {
      CAN_ERROR = 1;
   }

   if (status & EWarn)
   {
      CAN_ERROR = 1;
   }

   // Old SFRPAGE is popped off the SFR stack when ISR exits
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
