/*
 * Pump_Board_enums.h
 *
 *  Created on: 13.03.2022
 *      Author: benja
 */

#ifndef INC_PUMP_BOARD_ENUMS_H_
#define INC_PUMP_BOARD_ENUMS_H_

//Pump Board State Machine
typedef enum
{
   INIT,                  // init pins, ports, clk's, etc...
   STARTUP,               // check/test ADC, check inverter temperature
   RUN,                   // Monitor Vacuum (fill up if needed), Monitor Inverter Temp. (cool if needed)
   ERROR,                 // check error, decide how to proceed

}PumpBoardStateMachine;


#endif /* INC_PUMP_BOARD_ENUMS_H_ */
