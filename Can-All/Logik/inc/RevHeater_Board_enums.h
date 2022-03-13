/*
 * RevHeater_Board_enums.h
 *
 *  Created on: 13.03.2022
 *      Author: benja
 */

#ifndef INC_REVHEATER_BOARD_ENUMS_H_
#define INC_REVHEATER_BOARD_ENUMS_H_

//Reverse and Heater Board State Machine
typedef enum
{
   INIT,                  // init pins, ports, clk's, etc...
   RUN,                   // Heater ON as long as MeltFuse is LOW, controll Reverse Light (Rev_Gear xor RevSelected)
   ERROR,                 // check error, decide how to proceed

}RevHeaterStateMachine;

#endif /* INC_REVHEATER_BOARD_ENUMS_H_ */
