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

bool ClearAllSettingsFLASH(FLADDR baseAddr)
{
	if (baseAddr == START_ADDRESS_FLASH) // to make sure we use it as intended
	{
		FLASH_PageErase(baseAddr);
		return (true);
	}
	return (false);
}

uint16_t BiStoreSettingFLASH(FLADDR flash, uint8_t *ram, uint16_t numbytes, bool StoreToFlash)
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
   return(flash + numbytes); // return startingpoint of next address in Settings
}

void GetSerial(FLADDR flash, uint8_t *ram, uint16_t numbytes)
{
	uint16_t index;
	for (index = 0; index < numbytes; index++)
	{
	   *ram++ = FLASH_ByteRead (flash+index);
	}
}

