
#include <SI_C8051F550_Defs.h>
#include <SI_C8051F550_Register_Enums.h>
#include <STRING.h>


//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------


typedef union SI_UU64
{
  uint32_t u32[2];
  uint16_t u16[4];
  uint8_t u8[8];
} SI_UU64_t;


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init (void);
void TIMER0_Init(void);

void CANinit (void);
void CANsetupMessageObj(uint8_t objNum, uint16_t canId, uint8_t dirTx,  uint8_t len, SI_UU64_t payload);
void CANtransferMessageObj(uint8_t objNum, SI_UU64_t payload);
void CANtriggerMessageObj(uint8_t objNum);
SI_UU64_t CANgetMessageObjPayload(uint8_t objNum);
int16_t CANgetMessageInt(uint8_t objNum, uint8_t startIndex);
//SI_UU64_t CANgetMessageObj(uint8_t objNum, uint16_t *canId, uint8_t *dirTx,  uint8_t *len, SI_UU64_t payload);

SI_INTERRUPT_PROTO(CAN0_ISR, INTERRUPT_CAN0);
SI_INTERRUPT_PROTO(TIMER0_ISR, INTERRUPT_TIMER0);

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------


#define SYSCLK         (24000000)        // System clock speed in Hz

#define POLLRATE_HZ          (20)        // Timer Frequency in Hz.
                                         // Minimum value is 1 (battery needs min. 10Hz)
#define STARTUP_DELAY        (10)        // POLLRATE_HZ /s * STARTUP_DELAY
                                       
#define MESSAGE_OBJECTS      (33)        // Number of message objects to use
                                       // Range is 1-32 + 1 Dummy 0 + 32 is same

#define TX_COMPLETE_MASK  (0xFFFFFFFF)   // Set this to (2^MESSAGE_OBJECTS - 1)

#define NUM_OF_BATTERIES (0)            // Minimal amount of batteries in use

// Reload value for Timer0 assuming a 1:48 scaling factor for Timer0 clock
#define TIMER0_RL_HIGH  (0xFFFF -((SYSCLK/48/POLLRATE_HZ >> 8) & 0x00FF))
#define TIMER0_RL_LOW   (0xFFFF -(SYSCLK/48/POLLRATE_HZ & 0x00FF))

#define SEND_HEARTBEAT (1)
#define SEND_DBG_COUNTER (0)

// CAN-Ids for Message Objects.

#define CAN_ID_BROADCAST   (0x000)
#define CAN_ID_HEARTBEAT   (0x700) // Test Message of this device
#define CAN_ALL_ID         (0x01B) // Test Message of this device

#define CAN_ID_BAT1   (0x006)
#define CAN_ID_BAT2   (0x007)
#define CAN_ID_BAT3   (0x008)
#define CAN_ID_BAT4   (0x009)

// Contents of message
#define BMS_BAT_ON          (0x01)
#define BMS_BAT_OFF         (0x00)
#define BMS_BAT_CH          (0x02)
#define BMS_STATE_VALID_VALUES (0x0004)
#define BMS_STATE_NODE_ACTIVE  (0x0001)

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

//-----------------------------------------------------------------------------
// Pin Definitions
//-----------------------------------------------------------------------------

SI_SBIT(LED, SFR_P0, 0);                 // LED = 0 turns on the green LED

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
//uint16_t MotorRpm;
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


#define BAT_CH   (0x02)
#define BAT_ON   (0x01)
#define BAT_OFF  (0x00)

#define TPDO1_BMS     0x1E0
#define RPDO1_BMS     0x200

#define CAN_DIR_TX    0x01
#define CAN_DIR_RX    0x00


#define SET_BAT1_STATE (1)  // Message Object to transmit State Msg to BAT1
#define SET_BAT2_STATE (2)  // Message Object to transmit State Msg to BAT2
#define SET_BAT3_STATE (3)  // Message Object to transmit State Msg to BAT3
#define SET_BAT4_STATE (4)  // Message Object to transmit State Msg to BAT4

#define GET_BAT1_STATE (5)  // Message Object to receive State Msg from BAT1
#define GET_BAT2_STATE (6)  // Message Object to receive State Msg from BAT2
#define GET_BAT3_STATE (7)  // Message Object to receive State Msg from BAT3
#define GET_BAT4_STATE (8)  // Message Object to receive State Msg from BAT4

#define SET_BROADCAST (9)   // Message Object to transmit Broadcast such as <Start All Nodes>
#define SET_HEARTBEAT (10)  // Message Object to transmit Heartbeat or Startup Message to the CAN-BUS
#define GET_DEBUG_IN  (11)  // Message Object to receive Commands to this Module
#define SET_DEBUG_OUT (12)  // Message Object to transmit Commands to this Module

#define GET_BROADCAST (32)  // Message Object to transmit Broadcast such as <Start All Nodes>

static uint8_t NumOfBat = 0;
volatile SI_UU64_t CanMsgObject[MESSAGE_OBJECTS];
volatile uint16_t CanMsgId[MESSAGE_OBJECTS];

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
   PCA0MD &= ~0x40;                    // Disable Watchdog Timer
}
 
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void)
{
  SFRPAGE = LEGACY_PAGE;              // Set for PCA0MD
  memset(CanMsgObject, 0, sizeof(CanMsgObject));

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init ();                       // Initialize crossbar and GPIO
   TIMER0_Init ();                     // Initialize Timer 0
   CANinit();                       // Start CAN peripheral

   CAN_RXTX_COMPLETE.u32 = 0x0000;       // Initialize as no messages transmitted

   EIE2 |= 0x02;                       // Enable CAN interrupts
   IE_EA = 1;                          // Enable global interrupts

   // The CAN0 ISR will handle any received messages

   // The Timer0 ISR will periodically poll and transmit a message

   // Example Application

   // StartAllNodes

   while (1)
     {
     //MotorRpm = CANgetMessageInt(GET_INV_RMP, x);
       IE_EA = 0;                          // Disable global interrupts
       DebugInput = CANgetMessageInt(GET_DEBUG_IN, 0);
       Battery1SoC  =  CANgetMessageInt(GET_BAT1_STATE, 4);
       Battery2SoC  =  CANgetMessageInt(GET_BAT2_STATE, 4);
       Battery3SoC  =  CANgetMessageInt(GET_BAT3_STATE, 4);
       Battery4SoC  =  CANgetMessageInt(GET_BAT4_STATE, 4);
       Battery1SoH  =  CANgetMessageInt(GET_BAT1_STATE, 6);
       Battery2SoH  =  CANgetMessageInt(GET_BAT2_STATE, 6);
       Battery3SoH  =  CANgetMessageInt(GET_BAT3_STATE, 6);
       Battery4SoH  =  CANgetMessageInt(GET_BAT4_STATE, 6);
       LastBroadcastMsg = CANgetMessageObjPayload(GET_BROADCAST);
       IE_EA = 1;                          // Enable global interrupts
       BatterySoC  =   (Battery1SoC + Battery2SoC + Battery3SoC + Battery4SoC)/(NumOfBat) ? NumOfBat : 1;
       BatterySoH  =   (Battery1SoH + Battery2SoH + Battery3SoH + Battery4SoH)/(NumOfBat) ? NumOfBat : 1;
       LED = DebugInput;
     }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Initialize the internal oscillator to 24 MHz
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   uint8_t SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

   OSCICN = 0x87;                      // Set internal oscillator divider to 1

   SFRPAGE = SFRPAGE_save;
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// P0.6   digital  push-pull        CAN TX
// P0.7   digital  open-drain       CAN RX
//
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   uint8_t SFRPAGE_save = SFRPAGE;
   SFRPAGE  = CONFIG_PAGE;             // Port SFR's on Configuration page

   P0MDOUT  |= 0x40;                   // P0.6 (CAN0 TX) is push-pull

   XBR0     = 0x02;                    // Enable CAN0 on Crossbar
   XBR2     = 0x40;                    // Enable Crossbar and weak pull-ups

   SFRPAGE = SFRPAGE_save;
}


//-----------------------------------------------------------------------------
// TIMER0_Init
//-----------------------------------------------------------------------------
//
// Return Value:  None
// Parameters:    None
//
// Configure Timer0 to 16-bit Timer mode and generate an interrupt
// every TIMER0_RL Timer0 cycles using SYSCLK/48 as the Timer0 time base.
//
//-----------------------------------------------------------------------------
void TIMER0_Init(void)
{
   // No need to set SFRPAGE as all registers accessed in this function
   // are available on all pages

   TH0 = TIMER0_RL_HIGH;               // Init Timer0 High register
   TL0 = TIMER0_RL_LOW;                // Init Timer0 Low register

   TMOD  = 0x01;                       // Timer0 in 16-bit mode
   CKCON = 0x02;                       // Timer0 uses a 1:48 prescaler
   IE_ET0 = 1;                         // Timer0 interrupt enabled
   TCON  = 0x10;                       // Timer0 ON
}

//-----------------------------------------------------------------------------
// CANinit
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
      LED = 0; //LED ON and Batteries ON
      DataBaguette.u8[0] = BAT_ON;
    }
  else
    {
      LED = 1; //LED OFF and all Batteries OFF
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

