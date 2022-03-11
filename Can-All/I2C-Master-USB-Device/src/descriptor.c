/**************************************************************************//**
 * @file
 * @brief   Hardware initialization functions.
 * @author  Silicon Laboratories
 * @version 1.0.0 (DM: July 14, 2014)
 *
 *******************************************************************************
 * @section License
 * (C) Copyright 2014 Silicon Labs Inc,
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *******************************************************************************
 *
 *
 * Descriptor information to pass to USB_Init()
 * 
 *****************************************************************************/

#include "descriptor.h"

/*** [BEGIN] USB Descriptor Information [BEGIN] ***/

#define STRING1_LEN sizeof ("SIGITRONIC") * 2
const uint8_t code USB_MfrStr[] = // Manufacturer String
{
   STRING1_LEN, 0x03,
   'S', 0,
   'I', 0,
   'G', 0,
   'I', 0,
   'T', 0,
   'R', 0,
   'O', 0,
   'N', 0,
   'I', 0,
   'C', 0,
};

#define STRING2_LEN sizeof("RCC Moose") * 2
const uint8_t code USB_ProductStr[] = // Product Desc. String
{
   STRING2_LEN, 0x03,
   'R', 0,
   'C', 0,
   'C', 0,
   ' ', 0,
   'M', 0,
   'o', 0,
   'o', 0,
   's', 0,
   'e', 0
};

#define STRING3_LEN sizeof("2705") * 2
const uint8_t code USB_SerialStr[] = // Serial Number String
{
   STRING3_LEN, 0x03,
   '2', 0,
   '7', 0,
   '0', 0,
   '5', 0
};


const VCPXpress_Init_TypeDef InitStruct =
{
   0x10C4,                 // Vendor ID
   0xEA63,                 // Product ID
   USB_MfrStr,             // Pointer to Manufacturer String
   USB_ProductStr,         // Pointer to Product String
   USB_SerialStr,          // Pointer to Serial String
   0xff,                   // Max Power / 2
   0xA0,                   // Power Attribute: Set Bit5 to prevent USB form entering SUSPEND
   0x0100,                 // Device Release # (BCD format)
   false                    // Use USB FIFO space true
};
