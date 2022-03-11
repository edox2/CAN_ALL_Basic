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
//#define  SMB_NUM_BYTES_RD               42 //32 //24 =  Number of bytes to read
                                       // Slave -> Master
#define  SMB_NUM_BYTES_RD               56 //32 //24 =  Number of bytes to read


#define SMB_NOTCH_POS 			0
#define SMB_BUTTON_POS 			1
#define SMB_DEFLECTION_POS 		2
#define SMB_ANGLE_POS 			3
#define SMB_SUPPLY_POS 			4
#define SMB_TEMPBASE_POS 		5
#define SMB_TEMPHANDLE_POS 		6
#define SMB_TG_VERSION_POS 		7
#define SMB_UID_01_POS 			8
#define SMB_UID_02_POS 			9
#define SMB_UID_03_POS 			10
#define SMB_UID_04_POS 			11
#define SMB_UID_05_POS 			12
#define SMB_UID_06_POS 			13
#define SMB_UID_07_POS 			14
#define SMB_UID_08_POS 			15
#define SMB_UID_09_POS 			16
#define SMB_UID_10_POS 			17
#define SMB_UID_11_POS 			18
#define SMB_UID_12_POS 			19
#define SMB_UID_13_POS 			20
#define SMB_UID_14_POS 			21
#define SMB_UID_15_POS 			22
#define SMB_UID_16_POS 			23
#define SMB_SW_VERSION_POS 		24
#define SMB_HW_VERSION_POS 		25
#define SMB_ROT_ENTRY_POS_H 	26
#define SMB_ROT_ENTRY_POS_L 	27
#define SMB_ROT_CENTER_POS_H 	28
#define SMB_ROT_CENTER_POS_L 	29
#define SMB_ROT__DEV_POS_H 		30
#define SMB_ROT__DEV_POS_L 		31
#define SMB_ROT_ABS_POS_H 		32
#define SMB_ROT_ABS_POS_L 		33
#define SMB_ROT_EXIT_POS_H 		34
#define SMB_ROT_EXIT_POS_L 		35
#define SMB_VOLTAGE_LED_R 		36
#define SMB_VOLTAGE_LED_G 		37
#define SMB_VOLTAGE_LED_B 		38
#define SMB_VOLTAGE_OUT 		39
#define SMB_VOLTAGE_SUP 		40
#define SMB_VOLTAGE_PBIN 		41
#define SMB_RAW_BAS_X_H 		42
#define SMB_RAW_BAS_X_L 		43
#define SMB_RAW_BAS_Y_H 		44
#define SMB_RAW_BAS_Y_L 		45
#define SMB_RAW_BAS_Z_H 		46
#define SMB_RAW_BAS_Z_L 		47
#define SMB_RAW_ROT_X_H 		48
#define SMB_RAW_ROT_X_L 		49
#define SMB_RAW_ROT_Y_H 		50
#define SMB_RAW_ROT_Y_L 		51
#define SMB_RAW_ROT_Z_H 		52
#define SMB_RAW_ROT_Z_L 		53
#define SMB_CRC16_H 			54
#define SMB_CRC16_L 			55


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

