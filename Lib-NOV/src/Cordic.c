/*
 * Cordic.c
 *
 *  Created on: 12.07.2016
 *********************************************
 *    (c)2016-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#include "Cordic.h"

static const uint16_t code table[12] = {8192, 4836, 2555, 1297, 651, 325, 163, 81,41,20,10,5};
uint8_t cordic_steps = 8;
uint8_t cordic_abs;
uint16_t atan2_cordic(int16_t x, int16_t y)
{
	uint8_t i;
	int16_t temp = x;
	uint16_t angle = 32768;

	if (y < 0)
	{
		x = -y;
		y = temp;
		angle += 16384;
	}
	else
	{
		x = y;
		y = -temp;
		angle -= 16384;
	}

	for (i = 0; i < cordic_steps; i++)
	{
		temp = x;
		if (y < 0)
		{
			x -= y >> i;
			y += temp >> i;
			angle -= table[i];
		}
		else
		{
			x += y >> i;
			y -= temp >> i;
			angle += table[i];
		}

	}
	cordic_abs = x;
	return (uint16_t)angle;
}

