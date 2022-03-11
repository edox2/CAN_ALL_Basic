//============================================================================//
// File:          mc_tmr.h                                                    //
// Description:   Timer functions                                             //
// Author:        Uwe Koppe                                                   //
// e-mail:        koppe@microcontrol.net                                      //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Junkersring 23                                                             //
// 53844 Troisdorf                                                            //
// Germany                                                                    //
// Tel: +49-2241-25659-0                                                      //
// Fax: +49-2241-25659-11                                                     //
//                                                                            //
// The copyright to the computer program(s) herein is the property of         //
// MicroControl GmbH & Co. KG, Germany. The program(s) may be used            //
// and/or copied only with the written permission of MicroControl GmbH &      //
// Co. KG or in accordance with the terms and conditions stipulated in        //
// the agreement/contract under which the program(s) have been supplied.      //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 04.02.2004  Initial version                                                //
//                                                                            //
//============================================================================//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-04-13 22:41:43 +0200 (So, 13. Apr 2014) $
// SVN  $Rev: 5807 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  MC_TMR_H_
#define  MC_TMR_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include "compiler.h"


//-----------------------------------------------------------------------------
/*!
** \file    mc_tmr.h
** \brief   MCL - timer control
**
** This header file includes macros and definitions to access the timer
** resource of a microcontroller. The general timer resource is initialised
** with the function McTmrInit().
** One timer period is equal to one <b>timer tick</b>.<p>
** Functions which rely on the general timer resource always take timer ticks
** as function parameter. In order to convert a time period into timer ticks
** the function McTmrTimeToTicks() is provided.
**
*/

/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/
extern uint8_t  ubTimerTriggerG;


/*----------------------------------------------------------------------------*/
/*!
** \def  MC_TMR_FUNCTION
** \brief   Number of function timers
**
** This symbol defines the maximum number of function timers. The default
** value is set to <b>4</b>.
*/
#ifndef MC_TMR_FUNCTION
#define MC_TMR_FUNCTION                4
#endif

/*----------------------------------------------------------------------------*/
/*!
** \def  MC_TMR_MAX
** \brief   Number of software timers
**
** This symbol defines the maximum number of software timers. The default
** value is set to <b>4</b>.
*/
#ifndef MC_TMR_MAX
#define MC_TMR_MAX                     4
#endif


/*----------------------------------------------------------------------------*/
/*!
** \def  MC_TMR_PERIOD
** \brief   Period of timer interrupt
**
** This symbol defines the period of the timer interrupt. The value
** is a multiple of 1 microsecond. It is used for timing services.
** Please set this value to the timer interrupt period of the target
** system.
*/
#ifndef  MC_TMR_PERIOD
#define  MC_TMR_PERIOD                 1000
#endif


/*----------------------------------------------------------------------------*\
** Enumerations                                                               **
**                                                                            **
\*----------------------------------------------------------------------------*/

//--------------------------------------------------------------------
/*!
** \enum    TMR_ERR_e
**
** The error codes of the timer services have the prefix
** <b>TMR_ERR_e</b>.
**
*/
enum TMR_ERR_e {
   /*! no error                                             */
   eTMR_ERR_OK = 0,

   /*! timer is running                                     */
   eTMR_ERR_RUN,

   /*! timer is NOT started/initialised                     */
   eTMR_ERR_START,

   /*! timer parameter is not valid or out of range         */
   eTMR_ERR_PARM_INVALID,

   /*! timer resource is not valid                          */
   eTMR_ERR_RES_INVALID,

   /*! timer resource is full                               */
   eTMR_ERR_RES_FULL
};


//--------------------------------------------------------------------
/*!
** \enum    TMR_CTRL_e
**
** The mode of a timer can be controlled with the values taken
** from the <b>TMR_CTRL_e</b> enumeration.
**
*/
enum TMR_CTRL_e {

   /*! stop the timer                                                */
   eTMR_CTRL_STOP    = 0,

   /*! start the timer                                               */
   eTMR_CTRL_START   = 1,

   /*! timer is in one-shot mode                                     */
   eTMR_CTRL_ONESHOT = 2
};


//--------------------------------------------------------------------
/*!
** \enum    TMR_HRES_TICK_e
**
** The timer tick period of the optional high-resolution timer
** (McTmrHResInit())is defined by this enumeration.
**
*/
enum TMR_HRES_TICK_e {

   /*! 50 ns                                                */
   eTMR_HRES_TICK_50ns = 1,

   /*! 100 ns                                               */
   eTMR_HRES_TICK_100ns,

   /*! 200 ns                                               */
   eTMR_HRES_TICK_200ns,

   /*! 500 ns                                               */
   eTMR_HRES_TICK_500ns,

   /*! 1 us                                                 */
   eTMR_HRES_TICK_1us,

   /*! 2 us                                                 */
   eTMR_HRES_TICK_2us,

   /*! 5 us                                                 */
   eTMR_HRES_TICK_5us,

   /*! 10 us                                                */
   eTMR_HRES_TICK_10us,

   /*! 20 us                                                */
   eTMR_HRES_TICK_20us,

   /*! 50 us                                                */
   eTMR_HRES_TICK_50us

};

/*!
** \typedef    TmrHandler_fn
** \brief      Timer callback function
**
** Functions can be called periodically by the timer or in a
** one-shot mode. The functions do not carry any parameters.
** They do not return any value. The declaration of a timer
** function is:
** \code
** void MyTimerFunction(void);
** \endcode
*/
typedef void (*TmrHandler_fn)(void);


/*----------------------------------------------------------------------------*\
** Structures                                                                 **
**                                                                            **
\*----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
/*!
** \struct  McTmrFunc_s mc_tmr.h
** \brief   timer functions
**
**
*/
struct McTmrFunc_s {

   uint32_t       ulControl;

   /*!
   **    Current tick value, the tick value is decremented to zero.
   **    A value of zero calls the function \a fnTmrCall.
   */
   uint32_t       ulTickCurrent;

   /*!
   **    period value, the value holds the number of ticks that are
   **    used to reload the counter (ulTickCurrent). A value of 0
   **    sets the counter in one-shot mode
   */
   uint32_t       ulTickPeriod;

   /*!
   **    Function pointer, stores pointer of the function which
   **    is called.
   */
   TmrHandler_fn  fnTmrCall;

};

/*!
** \typedef    McTmrFunc_ts
** \brief      timer functions
*/
typedef struct McTmrFunc_s    McTmrFunc_ts;


//------------------------------------------------------------------------------
/*!
** \struct  McTmrSoft_s mc_tmr.h
** \brief   software timer
**
**
*/
struct McTmrSoft_s {

   /*!
   **    Software timer state
   **    0: timer is free for usage, 1: timer is locked
   */
   uint8_t       ubState;

   /*!
   **    Current tick value, the tick value is decremented to zero.
   **    A value of zero expires the software timer.
   */
   uint32_t       ulTick;
};

/*!
** \typedef    McTmrSoft_s
** \brief      software timer
*/
typedef struct McTmrSoft_s    McTmrSoft_ts;

/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
/*!
** \brief   Blocking delay function
** \param   ulTicksV - ticks for delay
** \return  #eTMR_ERR_OK when delay has been performed or negative value on error.
**
** \attention This function can only be called from the main loop!
*/
Status_tv McTmrDelayTicks(uint32_t ulTicksV);


//------------------------------------------------------------------------------
/*!
** \brief   Initialise function timer
** \param	fnHandlerV - pointer to function callback
** \param	ulTicksV   - number of timer ticks
** \param	ubControlV - configuration parameter
** \return	Negative value in case of an error, a positive value denotes
**          the associated timer number
** \sa		McTmrFunctionRelease()
**
** The function \a fnHandlerV is registered to the timer. It is called
** after a period of \a ulTmrTicksV. The time period of one timer tick
** depends on the implementation and hardware limitations. The default
** value shall be 1 millisecond. The function McTmrTicksToTime() can be
** used to evaluate the time period of one timer tick.
** <p>
** The parameter \a ubControlV is defined by the enumeration #TMR_CTRL_e.
** The timer associated to the function can initially be in stopped state
** (#eTMR_CTRL_STOP) or active state (#eTMR_CTRL_START). In addition it is
** possible to call the function only one time (#eTMR_CTRL_ONESHOT). In this
** case \a ulTicksV defines a delay.
** <p>
** The function returns a positive timer resource number
** that can be used to release the function \a fnHandlerV with
** McTmrFunctionRelease().
** \code
** void MyPeriodicFunction(void)
** {
**    // ... do some periodic stuff
** }
**
** void MyOneShotFunction(void)
** {
**    // ... do something only once
** }
**
** void ApplicationTimerInit(void)
** {
**    Status_tv  tvTmrResultT;
**
**    //-------------------------------------------------------------
**    // call periodic function every 10 timer ticks
**    //
**    McTmrFunctionInit(MyPeriodicFunction, 10, eTMR_CTRL_START);
**
**    //-------------------------------------------------------------
**    // call one-shot function after 20 timer ticks
**    // It will be started later with McTmrFunctionStart()
**    //
**    tvTmrResultT = McTmrFunctionInit(MyOneShotFunction, 20,
**                                     eTMR_CTRL_STOP | eTMR_CTRL_ONESHOT);
**
**    // ....
**
**    if(tvTmrResultT > 0)
**    {
**       //------------------------------------------------
**       // start one-shot function timer now
**       //
**       McTmrFunctionStart(tvTmrResultT);
**    }
**
** }
** \endcode
*/
Status_tv McTmrFunctionInit(TmrHandler_fn fnHandlerV,
                            uint32_t ulTicksV, uint8_t ubControlV);


//------------------------------------------------------------------------------
/*!
** \brief   Release function timer
** \param	ubTmrNumV - function timer number
** \return	#eTMR_ERR_OK or negative value taken from TMR_ERR_e
** \sa		McTmrFunctionInit()
**
** This function releases the timer function with number \a ubTmrNumV.
** initialises a timer resource of the microcontroller. The function
** returns #eTMR_ERR_RES_INVALID if the parameter \a ubTmrNumV is not valid.
*/
Status_tv McTmrFunctionRelease(uint8_t ubTmrNumV);


//------------------------------------------------------------------------------
/*!
** \brief   Start function timer
** \param   ubTmrNumV - function timer number
** \return  #eTMR_ERR_OK or negative value taken from TMR_ERR_e
** \sa      McTmrFunctionInit()
**
** This function starts the timer function with number \a ubTmrNumV.
** The function returns #eTMR_ERR_RES_INVALID if the parameter
** \a ubTmrNumV is not valid.
*/
Status_tv McTmrFunctionStart(uint8_t ubTmrNumV);


//------------------------------------------------------------------------------
/*!
** \brief   Stop function timer
** \param   ubTmrNumV - function timer number
** \return  #eTMR_ERR_OK or negative value taken from TMR_ERR_e
** \sa      McTmrFunctionInit()
**
** This function stops the timer function with number \a ubTmrNumV.
** The function returns #eTMR_ERR_RES_INVALID if the parameter
** \a ubTmrNumV is not valid.
*/
Status_tv McTmrFunctionStop(uint8_t ubTmrNumV);

//------------------------------------------------------------------------------
/*!
** \brief   Initialise High-Resolution timer
** \param   ubTickPeriodV  - Tick period taken from TMR_HRES_TICK_e
** \return  #eTMR_ERR_OK or negative error value taken from TMR_ERR_e
**
** The function initialises the optional high-resolution timer with the
** tick period given by \a ubTickPeriodV. If the requested tick period
** is not supported the function returns -#eTMR_ERR_RES_INVALID. The
** high-resolution timer is started by calling McTmrHResStart().
**
*/
Status_tv McTmrHResInit(uint8_t ubTickPeriodV);


//------------------------------------------------------------------------------
/*!
** \brief   Start High-Resolution time measurement
**
** This function starts the high-resolution time measurement, i.e. it starts
** the tick counter with an initial value of 0.
** The actual tick value can be evaluated with McTmrHResTick().
**
*/
void McTmrHResStart(void);


//------------------------------------------------------------------------------
/*!
** \brief   Get High-Resolution tick value
** \return  High-Resolution tick value
**
** This function returns the high-resolution tick value.
**
**
*/
uint32_t McTmrHResTick(void);


//-----------------------------------------------------------------------------
/*!
** \brief   Initialise Timer
**
** This function initialises the general timer resource of the microcontroller.
** <p>
** The timer can be stopped by calling McTmrStop().
*/
void McTmrInit(void);


//------------------------------------------------------------------------------
/*!
** \brief   Release a software timer
** \param   ubTmrNumV - Timer number
** \return  #eTMR_ERR_OK or negative error value taken from TMR_ERR_e
**
** This function releases a software timer.
** A software timer that is in use will be stopped and released
** for further usage.
**
*/
Status_tv McTmrPeriodRelease(uint8_t ubTmrNumV);

//------------------------------------------------------------------------------
/*!
** \brief   Check if software timer expired
** \param   ubTmrNumV - Timer number
** \return  #eTMR_ERR_OK or negative error value taken from TMR_ERR_e
**
** This function checks for the expiration of a time period which has been
** started by calling McTmrPeriodStart(). The function returns #eTMR_ERR_OK
** if the time period has elapsed or -#eTMR_ERR_RUN if the timer is still
** active.
** When timer expired and returned #eTMR_ERR_OK value, so this timer
** will be released for further usage.
*/
Status_tv McTmrPeriodExpired(uint8_t ubTmrNumV);


//------------------------------------------------------------------------------
/*!
** \brief   Start a timer period
** \param   ulTicksV - ticks before timer elapse
** \return  Timer number or negative error value taken from TMR_ERR_e
**
** This function configures a software timer that expires after a total
** count of \a ulTicksV. The function returns a negative value taken
** from the enumeration #TMR_ERR_e in case of an error. A positive
** return value denotes the associated software timer number, which is
** necessary for testing with McTmrPeriodExpired(). The number of software
** timers is limited to #MC_TMR_MAX.
**
** \attention When timer expired, it will be NOT released by default.
*/
Status_tv McTmrPeriodStart(uint32_t ulTicksV);


//------------------------------------------------------------------------------
/*!
** \brief   Start Timer
** \sa      McTmrStop()
**
** This function starts the timer. The timer continues to count
** from where it has been stopped the last time.
**
*/
void McTmrStart(void);


//------------------------------------------------------------------------------
/*!
** \brief   Stop Timer
** \sa      McTmrStart()
**
** This function stops the timer. The contents of the internal timer register
** remains unchanged.
**
*/
void McTmrStop(void);


//------------------------------------------------------------------------------
/*!
** \brief   Get timer tick
** \return  Current timer tick value
**
** The function returns the current timer tick value. It can be used to
** check in the program main loop if the event "timer tick" occurred.
**
*/
uint32_t McTmrTick(void);


//------------------------------------------------------------------------------
/*!
** \brief   Convert tick value to time
** \param   ulTickV - Tick value
** \return  Equivalent time in microseconds
** \sa      McTmrTimeToTicks()
**
** The function converts the parameter \a ulTickV into the equivalent time
** value. The returned time value is given in microseconds.
**
*/
uint32_t McTmrTicksToTime(uint32_t ulTickV);


//------------------------------------------------------------------------------
/*!
** \brief   Convert time value to ticks
** \param   ulTimeV - Time value in microseconds
** \return  Equivalent number of timer ticks
** \sa      McTmrTicksToTime()
**
** The function converts the parameter \a ulTimeV, which is a multiple of
** 1 microsecond, into the equivalent number of timer ticks. If the time value
** is not a multiple of one timer tick, the return value is rounded to the
** next lower possible time value.
**
*/
uint32_t McTmrTimeToTicks(uint32_t ulTimeV);



#endif   // #define  MC_TMR_H_
