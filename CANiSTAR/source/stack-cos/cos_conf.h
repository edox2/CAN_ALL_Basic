//****************************************************************************//
// File:          cos_conf.h                                                  //
// Description:   Configuration file for CANopen Slave Source Code            //
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
// 26.06.2000  Initial version                                                //
// 06.11.2000  Added Online-Documentation for Doxygen                         //
// 07.12.2000  Additional symbols                                             //
// 05.02.2002  Additional symbols, several symbols renamed                    //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-04-21 15:43:13 +0200 (Mo, 21. Apr 2014) $
// SVN  $Rev: 5829 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  COS_CONF_H_
#define  COS_CONF_H_



//-----------------------------------------------------------------------------
/*!
** \file    cos_conf.h
** \brief   CANopen slave configuration
**
** The file cos_conf.h holds all definitions for the configuration of the
** CANopen slave protocol stack. Please set the symbols to an appropriate
** value in order to achieve a specific CANopen slave behaviour.
*/

#include "SK60-1171_conf.h"
/*----------------------------------------------------------------------------*\
** Timing Configuration Options                                               **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
/*!
** \def     COS_TIMER_PERIOD
** \brief   Period of timer interrupt
** \todo    The value of the timer interrupt period has
**          to be adjusted to the target hardware.
**
** This symbol defines the period of the timer interrupt. The value
** is a multiple of 1 microsecond. It is used for timing services.
** Please set this value to the timer interrupt period of the target
** system.
**
** <b>Please note that the value must be at least 1000 [microseconds],
** because all CANopen services use a multiple of 1 millisecond.</b>
**
*/
#define  COS_TIMER_PERIOD        5000



/*----------------------------------------------------------------------------*\
** Device Type Configuration Options                                          **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
/*!
** \def     COS_DS401_DI
** \brief   Support DS-401, digital inputs
**
** This symbol defines if objects for digital inputs from
** the device profile for I/O modules (DS-401) are included
** in the dictionary.
**
** \li   0 : do not support DS-401, digital inputs
** \li   N : support DS-401, N is number of digital input blocks
*/
#ifndef  COS_DS401_DI
#define  COS_DS401_DI                  1
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS401_DO
** \brief   Support DS-401, digital outputs
**
** This symbol defines if objects for digital outputs from
** the device profile for I/O modules (DS-401) are included
** in the dictionary.
**
** \li   0 : do not support DS-401, digital outputs
** \li   N : support DS-401, N is number of digital output blocks
*/
#ifndef  COS_DS401_DO
#define  COS_DS401_DO                  0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS401_AI
** \brief   Support DS-401, analogue inputs
**
** This symbol defines if objects for analogue inputs from
** the device profile for I/O modules (DS-401) are included
** in the dictionary.
**
** \li   0 : do not support DS-401, analogue inputs
** \li   N : support DS-401, N is number of analogue input blocks
*/
#ifndef  COS_DS401_AI
#define  COS_DS401_AI                  1
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS401_AO
** \brief   Support DS-401, analogue outputs
**
** This symbol defines if objects for analogue outputs from
** the device profile for I/O modules (DS-401) are included
** in the dictionary.
**
** \li   0 : do not support DS-401, analogue outputs
** \li   1 : support DS-401, analogue outputs
*/
#ifndef  COS_DS401_AO
#define  COS_DS401_AO                  0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS402
** \brief   Support DS-402
**
** This symbol defines if objects for drives device profile (DS-402)
** are included in the dictionary.
**
** \li   0 : do not support DS-402
** \li   1 : support DS-402
*/
#ifndef  COS_DS402
#define  COS_DS402                     0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS404_AI
** \brief   Support DS-404, analogue inputs
**
** This symbol defines if objects for analogue inputs from
** the device profile for Sensors and Closed-loop Control
** (DS-404) are included in the dictionary.
**
** \li   0 : do not support DS-404, analogue inputs
** \li   N : support DS-404, N is number of analogue inputs
*/
#ifndef  COS_DS404_AI
#define  COS_DS404_AI                  0
#endif

#define  COS_DS404_AI_FILTER           1

//-------------------------------------------------------------------
/*!
** \def     COS_DS404_AL
** \brief   Support DS-404, alarm block
**
** This symbol defines if objects for alarms from
** the device profile for Sensors and Closed-loop Control
** (DS-404) are included in the dictionary.
**
** \li   0 : do not support DS-404, alarms
** \li   1 : support DS-404, alarms
*/
#ifndef  COS_DS404_AL
#define  COS_DS404_AL                  0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS404_AO
** \brief   Support DS-404, analogue outputs
**
** This symbol defines if objects for analogue outputs from
** the device profile for Sensors and Closed-loop Control
** (DS-404) are included in the dictionary.
**
** \li   0 : do not support DS-404, analogue outputs
** \li   N : support DS-404, analogue outputs
*/
#ifndef  COS_DS404_AO
#define  COS_DS404_AO                  0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS404_DT_INT16
** \brief   DS-404 data type
**
** This symbol defines if the data type INT16 is supported for
** the device profile for Sensors and Closed-loop Control
** (DS-404)
**
** \li   0 : not supported
** \li   1 : support data type Integer16
*/
#define  COS_DS404_DT_INT16            1


//-------------------------------------------------------------------
/*!
** \def     COS_DS404_DT_INT32
** \brief   DS-404 data type
**
** This symbol defines if the data type INT32 is supported for
** the device profile for Sensors and Closed-loop Control
** (DS-404)
**
** \li   0 : not supported
** \li   1 : support data type Integer32
*/
#define  COS_DS404_DT_INT32            0


//-------------------------------------------------------------------
/*!
** \def     COS_DS404_DT_FLOAT
** \brief   DS-404 data type
**
** This symbol defines if the data type FLOAT is supported for
** the device profile for Sensors and Closed-loop Control
** (DS-404)
**
** \li   0 : not supported
** \li   1 : support data type FLOAT
*/
#define  COS_DS404_DT_FLOAT            0


//-------------------------------------------------------------------
/*!
** \def     COS_DS406
** \brief   Support DS-406
**
** This symbol defines if objects for the device profile DS-406
** (Encoder) are included in the dictionary.
**
** \li   0 : do not support DS-406
** \li   1 : support DS-406, Single turn encoder
** \li   2 : support DS-406, Multi turn encoder
*/
#ifndef  COS_DS406
#define  COS_DS406                     0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS410
** \brief   Support DS-410
**
** This symbol defines if objects for the device profile DS-410
** Inclinometer are included in the dictionary.
**
** \li   0 : do not support DS-410
** \li   1 : support DS-410
*/
#ifndef  COS_DS410
#define  COS_DS410                     0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS417
** \brief   Support DS-417
**
** This symbol defines if objects for the application profile CiA 417
** (Lift) are included in the dictionary.
**
** \li   0 : do not support CiA 417
** \li   1 : support CiA 417, Lift
*/
#ifndef  COS_DS417
#define  COS_DS417                     0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS418
** \brief   Support DS-418
**
** This symbol defines if objects for the device profile CiA 418
** (battery modules) are included in the dictionary.
**
** \li   0 : do not support CiA 418
** \li   1 : support CiA 418, Battery modules
*/
#ifndef  COS_DS418
#define  COS_DS418                     0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS437_TS
** \brief   Support CiA 437, temperature sensor
**
** This symbol defines if the virtual device <i>temperature sensor</i>
** is included in the dictionary.
**
** \li   0 : do not support virtual device temperature sensor
** \li   N : support virtual device temperature sensor, N is the
**           number of analogue inputs
*/
#ifndef  COS_DS437_TS
#define  COS_DS437_TS                  0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS437_DI
** \brief   Support CiA 437, digital input module
**
** This symbol defines if the virtual device <i>digital input module</i>
** is included in the dictionary.
**
** \li   0 : do not support virtual device digital input module
** \li   N : support virtual device digital input module, N is the
**           number of digital input module blocks
*/
#ifndef  COS_DS437_DI
#define  COS_DS437_DI                  0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS437_DO
** \brief   Support CiA 437, digital output module
**
** This symbol defines if the virtual device <i>digital output module</i>
** is included in the dictionary.
**
** \li   0 : do not support virtual device digital output module
** \li   N : support virtual device digital output module, N is the
**           number of digital output module blocks
*/
#ifndef  COS_DS437_DO
#define  COS_DS437_DO                  0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS437_AI
** \brief   Support CiA 437, analogue input module
**
** This symbol defines if the virtual device <i>analogue input module</i>
** is included in the dictionary.
**
** \li   0 : do not support virtual device analogue input module
** \li   N : support virtual device analogue input module, N is the
**           number of analogue inputs
*/
#ifndef  COS_DS437_AI
#define  COS_DS437_AI                  0
#endif


//-------------------------------------------------------------------
/*!
** \def     COS_DS437_AO
** \brief   Support CiA 437, analogue output module
**
** This symbol defines if the virtual device <i>analogue output module</i>
** is included in the dictionary.
**
** \li   0 : do not support virtual device analogue output module
** \li   N : support virtual device analogue output module, N is the
**           number of analogue outputs
*/
#ifndef  COS_DS437_AO
#define  COS_DS437_AO                  0
#endif


/*----------------------------------------------------------------------------*\
** Dictionary Configuration Options                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
/*!
** \def     COS_DICT_MAN
** \brief   Include manufacturer objects
**
** This symbol defines if manufacturer specific objects are included
** in the dictionary. A value of 0 means the manufacturer objects are
** not included. A value of 1 means they are included.
**
*/
#define  COS_DICT_MAN                  0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_SEARCH_FAST
** \brief   Use fast search algorithm
**
** This symbol defines if a fast search algorithm is used for the
** dictionary. The fast search algorithm increases the code size.
** A value of 0 means a linear search method is used. A value of 1
** means the fast search algorithm is used.
**
*/
#define  COS_DICT_SEARCH_FAST          1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1003
** \brief   Implementation of index 1003h
**
** This symbol defines the maximum number of errors that
** can be stored by the device. Please note that the errors
** are kept in RAM and require 4 byte for each entry. A value
** of 0 means that this object is not supported. The maximum
** size of the list is 254.
**
*/
#define  COS_DICT_OBJ_1003             1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1005
** \brief   Implementation of index 1005h
**
** This symbol defines if the object 1005h (SYNC identifier)
** is supported by the device.
**
** \li   0 : do not support index 1005h
** \li   1 : support index 1005h
**
*/
#define  COS_DICT_OBJ_1005             1

//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1006
** \brief   Implementation of index 1006h
**
** This symbol defines if the object 1006h (communication cycle
** period) is supported by the device. If the object is supported,
** the device can also be used as SYNC producer.
**
** \li   0 : do not support index 1006h
** \li   1 : support index 1006h
**
*/
#define  COS_DICT_OBJ_1006             1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1008
** \brief   Implementation of index 1008h
**
** This symbol defines if the object 1008 is supported by the device.
**
** \li   0 : do not support index 1008h
** \li   1 : support index 1008h
*/
#define  COS_DICT_OBJ_1008             1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1009
** \brief   Implementation of index 1009h
**
** This symbol defines if the object 1009 is supported by the device.
**
** \li   0 : do not support index 1009h
** \li   1 : support index 1009h
*/
#define  COS_DICT_OBJ_1009             1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_100A
** \brief   Implementation of index 100Ah
**
** This symbol defines if the object 100A is supported by the device.
**
** \li   0 : do not support index 100Ah
** \li   1 : support index 100Ah
*/
#define  COS_DICT_OBJ_100A             1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_100C
** \brief   Implementation of index 100Ch and 100Dh
**
** Every CANopen slave must support the Heartbeat protocol. It is
** optional possible to use Node Guarding. It is recommended not to
** support Node Guarding for new implementations because of its
** technical drawbacks.
**
** \li   0 : do not support Node Guarding
** \li   1 : support node guarding
*/
#define  COS_DICT_OBJ_100C             1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1010
** \brief   Store Parameters
**
** The symbol defines if the object 1010h is supported and how
** many sub-indices are used. A value of 0 means the object is
** not supported, i.e. the device can not store parameters.
** A value greater 0 denotes the highest supported sub-index.
** The value must be in the range from 0 to 4.
**
*/
#define  COS_DICT_OBJ_1010             0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1011
** \brief   Restore Parameters
**
** The symbol defines if the object 1011h is supported and how
** many sub-indices are used. A value of 0 means the object is
** not supported, i.e. the device can not restore parameters.
** A value greater 0 denotes the highest supported sub-index.
** The value must be in the range from 0 to 4.
**
*/
#define  COS_DICT_OBJ_1011             0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1012
** \brief   COB-ID time stamp object
**
** The symbol defines if the object 1012h is supported. A value of 0
** means the object is not supported, i.e. the device has no Real-
** Time-Clock (RTC). A value of 1 denotes the device supports the
** TIME object.
**
*/
#define  COS_DICT_OBJ_1012             0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1013
** \brief   High resolution time stamp
**
** This object shall indicate the configured high resolution time
** stamp. It may be mapped into a PDO in order to exchange a high
** resolution time stamp message. Further application specific use
** is encouraged. The value of index 1013h is given in multiples
** of 1 microsecond.
**
** \li   0 : do not support index 1013h
** \li   1 : support index 1013h
**
*/
#define  COS_DICT_OBJ_1013             0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1014
** \brief   Emergency message
**
** The symbol defines if the object 1014h is supported and how
** many emergency messages can be queued.
**
*/
#define  COS_DICT_OBJ_1014             1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1015
** \brief   Emergency inhibit time
**
** This symbol defines if the object 1015 is supported by the device.
**
** \li   0 : do not support index 1015h
** \li   1 : support index 1015h
**
*/
#define  COS_DICT_OBJ_1015             0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1016
** \brief   Consumer heartbeat
**
** The symbol defines if the object 1016h is supported and how
** many sub-indices are used. A value of 0 means the object is
** not supported, i.e. the device has no consumer heartbeat.
** A value greater 0 denotes the highest supported sub-index.
** The maximum number of entries is limited to 4.
**
*/
#define  COS_DICT_OBJ_1016             0

//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1019
** \brief   Implementation of index 1019h
**
** This symbol defines if the object 1019h (SYNC counter) is
** supported by the device.
**
** \li   0 : do not support index 1019h
** \li   1 : support index 1019h
**
*/
#define  COS_DICT_OBJ_1019             0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1020
** \brief   Verify Configuration
**
** The symbol defines if the object 1020h is supported. If this
** object is supported, the device must be capable to store
** configuration data, i.e. object 1010 must also be supported.
**
** \li   0 : do not support index 1020h
** \li   1 : support index 1020h
*/
#define  COS_DICT_OBJ_1020             0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1021
** \brief   Support EDS store
**
** The symbol defines if the objects 1021h (EDS store) and 1022h
** (Store format) are supported. The storage format of the EDS
** may be compliant to ISO10646 (not compressed) or any manufacturer-
** specific format.
**
** \li   0 : do not support index 1021h and 1022h
** \li   1 : EDS is not compressed, 1022h:00h = 00h
** \li   2 : EDS has manufacturer specific format, 1022h:00h = 80h
**
*/
#define  COS_DICT_OBJ_1021             0


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1029
** \brief   Error behaviour object
**
** The symbol defines if the object 1029h is supported.
** A value greater 0 denotes the highest supported sub-index.
** The maximum number of entries is limited to 4.
**
** \li   0 : do not support index 1029h
** \li   N : support index 1029h, sub-indices 1 - N
*/
#define  COS_DICT_OBJ_1029             1


//-------------------------------------------------------------------
/*!
** \def     COS_DICT_OBJ_1F80
** \brief   NMT Startup
**
** The symbol defines if the object 1F80h is supported.
**
** \li   0 : do not support index 1F80h
** \li   1 : support index 1F80h
** \li   2 : support index 1F80h, backward compatibility mode
**
** The backwards compatibility mode allows additional (obsolete)
** values for setting the startup behaviour. For new CANopen devices,
** use the value '1' for support of index 1F80h.
*/
#define  COS_DICT_OBJ_1F80             0



/*----------------------------------------------------------------------------*\
** Manufacturer specific options                                              **
** these options require the files 'mc_co_mobj.*'                             **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
/*!
** \internal
** \def     COS_MOB_MC
** \brief   Manufacturer objects (MicroControl specific)
**
** With this symbol additional manufacturer objects are added.
** This options requires the mc_co_mobj.h file and corresponding
** implementation.
**
** \li   0 : No MicroControl specific objects (default)
** \li   1 : Use MicroControl specific objects
*/
#ifndef  COS_MOB_MC
#define  COS_MOB_MC                    0
#endif


//-------------------------------------------------------------------
/*!
** \internal
** \def     COS_MOB_MC_DISABLE_BOOTUP
** \brief   Bootup behavior
**
** With this symbol an additional object 2E10h is defined that
** allows to modify the bootup behaviour. This is not compliant
** to the CANopen specification.
**
** \li   0 : do not support object 2E10h
** \li   1 : support object 2E10h
*/
#define  COS_MOB_MC_DISABLE_BOOTUP     0


//-------------------------------------------------------------------
/*!
** \internal
** \def     COS_MOB_MC_STORE_COB_ID
** \brief   COB-ID storage behaviour
**
** With this symbol it is possible to enable the COB-ID storage
** behaviour. This allows to calculate the COB-ID for PDO and
** EMCY messages in different ways when parameters are stored in
** the device.
**
** \li   0 : do not use COB-ID storage behaviour
** \li   1 : use COB-ID storage behaviour
*/
#define  COS_MOB_MC_STORE_COB_ID       0


//-------------------------------------------------------------------
/*!
** \internal
** \def     COS_MOB_MC_PRODUCT_CODE
** \brief   Device Product code setup
**
** With this symbol it is possible to set the product code via
** the object dictionary.
**
** \li   0 : do not use Device Product code setup
** \li   1 : use Device Product code setup
*/
#define  COS_MOB_MC_PRODUCT_CODE       0

/*----------------------------------------------------------------------------*\
** SDO Configuration Options                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
/*!
** \def     COS_SDO_SEGMENTED
** \brief   Segmented SDO Transfer
**
** The symbol defines if the segmented SDO transfer is supported.
** If segmented SDOs are not supported, the code size can be
** reduced. However, segmented SDOs are required if the data type
** STRING must be supported with string length greater than 4.
**
** \li   0 : do not support segmented SDOs
** \li   1 : support segmented SDOs
*/
#define  COS_SDO_SEGMENTED             1


//-------------------------------------------------------------------
/*!
** \def     COS_SDO_BLOCK
** \brief   SDO Block Transfer
**
** The symbol defines if the SDO Block transfer is supported.
** A value of 0 denotes that SDO Block transfer is not supported.
** A value greater 0 denotes the maximum number of blocks that
** can be transfered between master and slave. The maximum value
** is 127 blocks.
**
*/
#define  COS_SDO_BLOCK                 0


//-------------------------------------------------------------------
/*!
** \def     COS_SDO_CLIENT
** \brief   SDO client functionality
**
** The symbol defines if SDO client functionality is supported.
** A value of 0 denotes that SDO client functionality is not
** supported.
**
*/
#define  COS_SDO_CLIENT                0


/*----------------------------------------------------------------------------*\
** PDO Configuration Options                                                  **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
/*!
** \def     COS_PDO_RCV_NUMBER
** \brief   Number of Receive PDOs
**
** This symbol defines the number of receive PDOs used by the
** CANopen slave. The number may vary between 0 (no receive PDO)
** and 4.
*/
#define  COS_PDO_RCV_NUMBER            0

//-------------------------------------------------------------------
/*!
** \def     COS_PDO_RCV_ID_CONST
** \brief   Receive PDO identifier is constant
**
** Depending on the device profile or application it may be
** required that the identifier of the receive PDO is constant.
** This symbol defines if index 140xh, sub 1 is read-only:
**
** \li   0 : PDO identifier is read-write
** \li   1 : PDO identifier is read-only
*/
#define  COS_PDO_RCV_ID_CONST    		0

//-------------------------------------------------------------------
/*!
** \def     COS_PDO_TRM_NUMBER
** \brief   Number of Transmit PDOs
**
** This symbol defines the number of transmit PDOs used by the
** CANopen slave. The number may vary between 0 (no transmit PDO)
** and 4.
*/
#define  COS_PDO_TRM_NUMBER            2

//-------------------------------------------------------------------
/*!
** \def     COS_PDO_TRM_ID_CONST
** \brief   Transmit PDO identifier is constant
**
** Depending on the device profile or application it may be
** required that the identifier of the transmit PDO is constant.
** This symbol defines if index 180xh, sub 1 is read-only:
**
** \li   0 : PDO identifier is read-write
** \li   1 : PDO identifier is read-only
*/
#define  COS_PDO_TRM_ID_CONST    		0

//-------------------------------------------------------------------
/*!
** \def     COS_PDO_INHIBIT
** \brief   Support Inhibit time for transmit PDOs
**
** This symbol defines if an inhibit time is supported for
** transmit PDOs.
*/
#define  COS_PDO_INHIBIT               0


//-------------------------------------------------------------------
/*!
** \def     COS_PDO_SYNC_START
** \brief   Support SYNC start value for transmit PDOs
**
** This symbol defines if a SYNC start value is supported for
** transmit PDOs.
*/
#define  COS_PDO_SYNC_START            0


//-------------------------------------------------------------------
/*!
** \def     COS_PDO_MAPPING
** \brief   Implementation of PDO mapping
**
** This symbol defines how the PDO mapping is implemented.
** The fixed PDO mapping consumes less memory space and
** computation time. The variable PDO mapping offers the
** most flexibility for the PDO service.
**
** \li   0 : use fixed PDO mapping
** \li   1 : use variable PDO mapping
**
*/
#define  COS_PDO_MAPPING               0


/*----------------------------------------------------------------------------*\
** Other options                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

//-------------------------------------------------------------------
/*!
** \def     COS_NMT_MASTER
** \brief   CANopen Slave can also be NMT master
**
*/
#define  COS_NMT_MASTER                0


//-------------------------------------------------------------------
/*!
** \def     COS_LSS_SUPPORT
** \brief   Implementation of Layer Setting Services and Protocol
**
** LSS offers the possibility to inquire and change the settings of
** certain parameters of the local layers on a CANopen module with
** LSS Slave capabilities. This switch is used to enable/disable the
** LSS capability. If enabled, the file cos_lss.c must be included
** to the project.
**
** \li   0 : do not support LSS
** \li   1 : support LSS slave
** \li   2 : support LSS master
*/
#define  COS_LSS_SUPPORT               0


//-------------------------------------------------------------------
/*!
** \def     COS_LED_SUPPORT
** \brief   Support of LEDs
**
** With this symbol the LED support can be enabled. There are two
** LEDs supported: a Network LED and a Module LED. The Network LED
** denotes the state of the CANopen status machine (Operational,
** Stopped, Bus-Off, etc.). The optional Module LED denotes the
** physical state of the device (running, hardware failure, etc).
**
** \li   0 : do not support LEDs
** \li   1 : support Network LED
** \li   2 : support Network and Module LEDs
*/
#define  COS_LED_SUPPORT               1


//-------------------------------------------------------------------
/*!
** \def     COS_BUS_EMCY
** \brief   Emergency messages on bus errors
**
** With this symbol it is possible to enable the transmission of
** emergency messages upon bus errors. The functionality is handles
** inside the function CosMgrCanErrorHandler().
**
** \li   0 : do not send emergency messages on bus errors
** \li   1 : send emergency messages on bus errors
*/
#define  COS_BUS_EMCY                  1




//-------------------------------------------------------------------
/*!
** \def     COS_MGR_INT
** \brief   Run CAN message handler in Poll/IRQ mode
**
** With this symbol it is possible to switch the CAN message handler
** (message reception) between Polling- and IRQ-mode. In Polling mode
** the messages are read from the buffer during the main loop. The
** default mode is the IRQ-mode: received CAN messages are processed
** inside the CAN IRQ-handler.
**
** \li   0 : Run CAN message handler in Poll-Mode
** \li   1 : Run CAN message handler in IRQ-Mode
*/
#define  COS_MGR_INT                   1


//-------------------------------------------------------------------
/*!
** \def     COS_TMR_INT
** \brief   Run timer events in Poll/IRQ mode
**
** With this symbol it is possible to switch the timer function
** (CosTmrEvent) between Polling- and IRQ-mode. In Polling mode
** the timer value is checked within the main loop. The
** default mode is the IRQ-mode: the function CosTmrEvent() is called
** within the timer interrupt.
**
** \li   0 : Run timer events in Poll-Mode
** \li   1 : Run timer events in IRQ-Mode
*/
#define  COS_TMR_INT                   0


/*----------------------------------------------------------------------------*\
** Generation of additional symbols                                           **
**                                                                            **
\*----------------------------------------------------------------------------*/

#if (COS_PDO_RCV_NUMBER == 0) && (COS_PDO_TRM_NUMBER == 0)
#define  COS_PDO_SUPPORT   0
#else
#define  COS_PDO_SUPPORT   1
#endif

#if (COS_DICT_OBJ_1005 == 0) && (COS_DICT_OBJ_1006 == 0)
#define  COS_SYNC_SUPPORT  0
#else
#define  COS_SYNC_SUPPORT  1
#endif

//-------------------------------------------------------------------
// setup the symbol COS_PROFILE_NUMBER
//
#if (COS_DS401_DI > 0) || (COS_DS401_DO > 0) || (COS_DS401_AI > 0) || (COS_DS401_AO > 0)
#define  COS_DS401            1
#define  COS_PROFILE_NUMBER   401
#else
#define  COS_DS401            0
#endif

#if COS_DS402 == 1
#define  COS_PROFILE_NUMBER   402
#endif

#if (COS_DS404_AI > 0) || (COS_DS404_AL > 0) || (COS_DS404_AO > 0)
#define  COS_DS404            1
#define  COS_PROFILE_NUMBER   404
#else
#define  COS_DS404            0
#endif

#if COS_DS406 == 1
#define  COS_PROFILE_NUMBER   406
#endif

#if COS_DS410 == 1
#define  COS_PROFILE_NUMBER   410
#endif

#if COS_DS418 == 1
#define  COS_PROFILE_NUMBER   418
#endif

#if (COS_DS437_TS > 0) || (COS_DS437_DI > 0) || (COS_DS437_DO > 0) || (COS_DS437_AI > 0) || (COS_DS437_AO > 0)
#define  COS_DS437            1
#define  COS_PROFILE_NUMBER   437
#else
#define  COS_DS437            0
#endif

#ifndef  COS_PROFILE_NUMBER
#define  COS_PROFILE_NUMBER   0
#endif



/*----------------------------------------------------------------------------*\
** Test for configuration errors                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#if COS_DICT_OBJ_1010 != COS_DICT_OBJ_1011
#error Value for symbol COS_DICT_OBJ_1011 does not match
#endif

#if COS_DICT_OBJ_1003 > 254
#error Value for symbol COS_DICT_OBJ_1003 out of range
#endif

#if COS_PDO_RCV_NUMBER > 4
#error Value for symbol COS_PDO_RCV_NUMBER out of range
#endif

#if COS_PDO_TRM_NUMBER > 4
#error Value for symbol COS_PDO_TRM_NUMBER out of range
#endif

#if COS_DICT_OBJ_1010 > 4
#error Value for symbol COS_DICT_OBJ_1010 out of range
#endif

#if COS_DICT_OBJ_1012 > 1
#error Value for symbol COS_DICT_OBJ_1012 out of range
#endif

#if COS_DICT_OBJ_1016 > 4
#error Value for symbol COS_DICT_OBJ_1016 out of range
#endif

#if COS_DICT_OBJ_1029 > 4
#error Value for symbol COS_DICT_OBJ_1029 out of range
#endif

#if COS_DICT_OBJ_1019 == 1 && COS_DICT_OBJ_1006 == 0
#error COS_DICT_OBJ_1006 must be set to 1
#endif

#if COS_DICT_OBJ_1019 == 0 && COS_PDO_SYNC_START > 0
#error COS_PDO_SYNC_START must be set to 0 or enable COS_DICT_OBJ_1019
#endif

#if COS_LSS_SUPPORT > 0 && COS_DICT_OBJ_1010 == 0
#error LSS support requires support of object 1010h (COS_DICT_OBJ_1010 > 0)
#endif

#endif   //  COS_CONF_H_

