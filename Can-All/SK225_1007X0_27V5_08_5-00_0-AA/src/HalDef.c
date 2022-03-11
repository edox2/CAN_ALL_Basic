/*
 * HalDef.c
 *
 *  Created on: 12.07.2016
 *********************************************
 *      (c)2016 SIGITRONIC SOFTWARE          *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */
#include "HalDef.h"
#include <SI_EFM8UB3_Defs.h>
#include <SI_EFM8UB3_Register_Enums.h>

//#include <reg51.h>

volatile uint32_t x;
volatile bool VERS_100750 = true;
volatile bool VERS_100750V8 = false;




void CountDelay(uint32_t count) {
	for (x = 0; x < count; x)
		x++;
}

void LoopDelay3ms(void)
{
	for(x = 0;x < RESET_DELAY_3ms ;x) // Delay
	{
		x++;
	}
}



uint8_t UpdateButtonState(uint8_t *ButtonState) {
	*ButtonState = (*ButtonState == PB_ABSENT) ? (PB_IN) ? PB_ABSENT : PB_NEW_EVENT	: (PB_IN) ? PB_ABSENT : PB_PRESENT;
	return (*ButtonState);
}

/*
uint8_t UpdateComState() {
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
}*/

/*
uint8_t UpdateSnapdomeState() {
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
*/

