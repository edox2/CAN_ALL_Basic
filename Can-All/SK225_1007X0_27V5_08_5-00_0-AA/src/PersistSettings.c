/*
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
*/
#include "FlashPrimitives.h"
#include "PersistSettings.h"
#include "CRC.h"

bool ClearAllSettingsFLASH(FLADDR baseAddr,  uint16_t numbytes)
{
	uint16_t pageOffset;
	if ((baseAddr == START_ADDRESS_CALIB_FLASH) && (numbytes <= SIZE_CALIB_FLASH)) // to make sure we use it as intended
	{
		for (pageOffset = 0; START_ADDRESS_CALIB_FLASH + (pageOffset * PAGE_SIZE_FLASH) < START_ADDRESS_CALIB_FLASH + SIZE_CALIB_FLASH; pageOffset++)
		{
			FLASH_PageErase(baseAddr + (PAGE_SIZE_FLASH * pageOffset));
		}

		return (true);
	}
	return (false);
}

uint16_t BiStoreSettingFLASH(FLADDR flash, uint8_t *ram, uint16_t numbytes, bool StoreToFlash, uint16_t *CRC16)
{
	uint16_t index;

   if (StoreToFlash)
   {
	   for (index = flash; index < flash + numbytes; index++)
	   {
	      FLASH_ByteWrite (index, *ram++);
	   }
   }
   else
   {
	   for (index = 0; index < numbytes; index++)
	   {
	      *ram++ = FLASH_ByteRead (flash+index);
	   }
   }
   * CRC16 = calcCRC16Array(numbytes, ram, * CRC16);
   return(flash + numbytes); // return starting point of next address in Settings
}


void GetSerial(FLADDR flash, uint8_t *ram, uint16_t numbytes)
{
	uint16_t index;
	for (index = 0; index < numbytes; index++)
	{
	   *ram++ = FLASH_ByteRead (flash+index);
	}
}

