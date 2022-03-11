/*
 * SMBusSlave.h
 *
 *  Created on: 20.05.2018
 *********************************************
 *      (c)2018 SIGITRONIC SOFTWARE          *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef SMBUSSLAVE_H_
#define SMBUSSLAVE_H_


//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define  SMB_WRITE                0x00 // SMBus WRITE command
#define  SMB_READ                 0x01 // SMBus READ command

#define  SMB_SLAVE_ADDR           0xF0 // Device addresses (7 bits,
                                       // lsb is a don't care)

// Status vector - top 4 bits only
#define  SMB_SRADD                0x20 // (SR) slave address received
                                       //    (also could be a lost
                                       //    arbitration)
#define  SMB_SRSTO                0x10 // (SR) STOP detected while SR or ST,
                                       //    or lost arbitration
#define  SMB_SRDB                 0x00 // (SR) data byte received, or
                                       //    lost arbitration
#define  SMB_STDB                 0x40 // (ST) data byte transmitted
#define  SMB_STSTO                0x50 // (ST) STOP detected during a
                                       //    transaction; bus error
// End status vector definition

#define  SMB_NUM_BYTES_WR                4 //4 = Number of bytes to write
                                       // Slave <- Master
#define  SMB_NUM_BYTES_RD                24 //24 =  Number of bytes to read
                                       // Slave -> Master

#define SMB_NOTCH_POS 0
#define SMB_BUTTON_POS 1
#define SMB_DEFLECTION_POS 2
#define SMB_ANGLE_POS 3
#define SMB_SUPPLY_POS 4
#define SMB_TEMPBASE_POS 5
#define SMB_TEMPHANDLE_POS 6
#define SMB_VERSION_POS 7



//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
// Global holder for SMBus data.
// All receive data is written here
// NUM_BYTES_WR used because an SMBus write is Master->Slave
extern uint8_t SMB_DATA_IN[SMB_NUM_BYTES_WR];

// Global holder for SMBus data.
// All transmit data is read from here
// NUM_BYTES_RD used because an SMBus read is Slave->Master
extern uint8_t SMB_DATA_OUT[SMB_NUM_BYTES_RD];

extern bool SMB_DATA_READY;             // Set to '1' by the SMBus ISR
                                       // when a new data byte has been
                                       // received.
extern bool SMB_TRANSFER_ONGOING;             // Set to '1' by the SMBus ISR
                                       // when transmission is in progress

#endif // SMBUSSLAVE_H_

