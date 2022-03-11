/*
 * HalDef.c
 *
 *  Created on: 12.07.2016
 *********************************************
 *    (c)2016-2018 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
#include "HalDef.h"
#include "si_toolchain.h"

#define FRAM_SIZE (0x2000)
#ifndef BASE_ADDR_FRAM
#define BASE_ADDR_FRAM (0xA0)
#endif // BASE_ADDR_FRAM

volatile uint32_t xdata x;


SI_LOCATED_VARIABLE_NO_INIT(flash_write_array[512], uint8_t, SI_SEG_CODE, START_ADDRESS_FLASH);

void CountDelay(uint32_t count) {
	for (x = 0; x < count; x)
		x++;
}

uint8_t UpdateButton2State(uint8_t *ButtonState) {
	*ButtonState =
			(*ButtonState == PB_ABSENT) ? (C_BUS) ? PB_ABSENT : PB_NEW_EVENT
			: (C_BUS) ? PB_ABSENT : PB_PRESENT;
	return (*ButtonState);
}

uint8_t UpdateButton1State(uint8_t *ButtonState) {
	*ButtonState =
			(*ButtonState == PB_ABSENT) ? (PB_IN) ? PB_ABSENT : PB_NEW_EVENT
			: (PB_IN) ? PB_ABSENT : PB_PRESENT;
	return (*ButtonState);
}

uint8_t UpdateKeyState()
{
	  char c;
	  if (SCON1_RI)
	  {
		  c = SBUF1;
		  SCON1_RI = 0;
		  if ('\n' == c)
		  {
			  return (PB_NEW_EVENT);
		  }
		  else
		  {

			  return (PB_NEW_EVENT);
		  }
	  }
	  return (PB_ABSENT);
}

uint8_t WriteByte (uint8_t Data)
{
  uint8_t savedPage;
  do{
	  savedPage = SFRPAGE;
      SFRPAGE = (0x20);
  } while(0);

  while (!SCON1_TI);
  SCON1_TI = 0;
  SBUF1 = Data;

  do{
	  SFRPAGE = savedPage;
  } while(0);
  return (Data);
}

uint8_t getCharModal(bool NonBlocking, uint8_t defaultChar)
{
  uint8_t c;
  if (NonBlocking)
  {
	  if (SCON1_RI)
	  {
		  c = SBUF1;
		  SCON1_RI = 0;
		  return (c);
	  }
	  else
	  {
		  return (defaultChar);
	  }
  }
  while (!SCON1_RI);
  c = SBUF1;
  SCON1_RI = 0;
  return (c);
}

uint8_t UpdateButtonState(uint8_t *ButtonState) {
	*ButtonState =
			(*ButtonState == PB_ABSENT) ? (PB_IN) ? PB_ABSENT : PB_NEW_EVENT
			: (PB_IN) ? PB_ABSENT : PB_PRESENT;
	return (*ButtonState);
}

void LoopDelayShort(void)
{
	for(x = 0;x < 15 ;x) // Delay
	{
		x++;
	}
}

void LoopDelay3ms(void)
{
	for(x = 0;x < RESET_DELAY_3ms ;x) // Delay
	{
		x++;
	}
}

void LoopDelay150ms(void)
{
	uint8_t j;
	for (j = 0; j < 50; j++) // Stabilize Power supply
	{
		LoopDelay3ms();
	}
}



void WatchdogRefresh()
{
	uint8_t SFRPAGE_save = SFRPAGE;
#ifdef SK202
	AN_IN ^= 1;
#endif
#ifndef NO_WATCHDOG
	if (TMR3CN0 & (0x80))
	{
		SFRPAGE = 0x00;
		// Only refresh if TF3H is set
		WDTCN = 0xA5;
		TMR3CN0 &= ~(0x80);
		SFRPAGE = SFRPAGE_save;
	}
#endif
}

void WatchdogDisable()
{
uint8_t SFRPAGE_save = SFRPAGE;
	SFRPAGE = 0x00;
	WDTCN = 0xDE; //First key
	WDTCN = 0xAD; //Second key
	SFRPAGE = SFRPAGE_save;
}

void WatchdogEnable()
{
#ifndef NO_WATCHDOG
	bit EA_SAVE = IE_EA;                // Preserve IE_EA
	uint8_t SFRPAGE_save = SFRPAGE;
	IE_EA = 0;                          // Disable interrupts
	SFRPAGE = 0x00;
	WDTCN = 0xff; // lock out watchdog disable option;
	WatchdogRefresh();
	SFRPAGE = SFRPAGE_save;
	IE_EA = EA_SAVE;                    // Restore interrupts
#endif
}


