/*
 * Main_Board_enums.h
 *
 *  Created on: 13.03.2022
 *      Author: benja
 */

#ifndef INC_MAIN_BOARD_ENUMS_H_
#define INC_MAIN_BOARD_ENUMS_H_

//Main State Machine
typedef enum
{
   INIT,                  // init pins, ports, clk's, etc...
   STARTUP,               // check inverter, close inverter Relais, check interlock loop, start battery's, check inverter, check Bender
   RUN,                   // all is good (Interlock closed, battery's ON, Inverter OK), go to error state if something happens
   ERROR,                 // check error, decide how to proceed

}MainStateMachine;


#endif /* INC_MAIN_BOARD_ENUMS_H_ */
