//****************************************************************************//
// File:          cos301.h                                                    //
// Description:   Objects from CiA 301                                        //
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
// 29.06.2000  Initial version                                                //
// 10.11.2000  Added Online-Documentation for Doxygen                         //
// 29.01.2001  Moved several variables to cos_user.c                          //
// 30.10.2002  Changed callback structure to reduce code size                 //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2014-02-03 17:09:58 +0100 (Mo, 03. Feb 2014) $
// SVN  $Rev: 5697 $ --- $Author: koppe $
//------------------------------------------------------------------------------


#ifndef  COS_301_H_
#define  COS_301_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <cos_defs.h>      // CANopen Slave definition file


//-----------------------------------------------------------------------------
/*!
** \file    cos301.h
** \brief   Object Dictionary for CiA 301
**
** This module implements the variables and callback functions for
** the object dictionary from CiA 301 for various indices.
*/


//-------------------------------------------------------------------//
// take precautions if compiled with C++ compiler                    //
#ifdef __cplusplus                                                   //
extern "C" {                                                         //
#endif                                                               //
//-------------------------------------------------------------------//


/*----------------------------------------------------------------------------*\
** Variables of module for external use                                       **
**                                                                            **
\*----------------------------------------------------------------------------*/


/*!
** \var     ulIdx1000_DeviceTypeC
** \brief   Constant for device type (Index 1000h)
** \todo    Set this constant to the desired profile number (cos_user.c)
**
** The constant ulIdx1000_DeviceType holds the device profile number.
** If the CANopen slave does not support a standard profile, the value
** must be set to zero (0x00000000).
*/
extern uint32_t ulIdx1000_DeviceTypeC;


/*!
** \var     ubIdx1001_ErrorRegisterG
** \brief   Error register (Index 1001h)
**
** The variable ubIdx1001_ErrorRegisterG holds the error status of the
** device. It must be set by the application program.
*/
extern uint8_t ubIdx1001_ErrorRegisterG;


/*!
** \var     ulIdx1002_StatusRegisterG
** \brief   Manufacturer status register (Index 1002h)
**
** The variable ulIdx1002_StatusRegisterG holds a manufacturer
** specific value (32 bit). It must be set by the application program.
*/
extern uint32_t ulIdx1002_StatusRegisterG;


/*!
** \var     ubIdx1008_DeviceNameC
** \brief   Device name string (Index 1008h)
** \todo    Set this string to the desired device name (cos_user.c)
**
** The string ubIdx1008_DeviceNameC holds the name of the device.
*/
extern uint8_t  ubIdx1008_DeviceNameC[];

/*!
** \var     ubIdx1009_HwVersionC
** \brief   Device hardware version (Index 1009h)
** \todo    Set this string to the current device hardware version (cos_user.c)
**
** The string ubIdx1009_HwVersionC holds the hardware version of the device.
*/
extern uint8_t  ubIdx1009_HwVersionC[];


/*!
** \var     ubIdx100A_SwVersionC
** \brief   Device software version (Index 100Ah)
** \todo    Set this string to the current device software version (cos_user.c)
**
** The string ubIdx100A_SwVersionC holds the software version of the device.
*/
extern uint8_t  ubIdx100A_SwVersionC[];


/*!
** \var     ubIdx1018_SubNumberC
** \brief   Identity object (Index 1018h)
** \todo    Set this value to the number of supported sub-indices. It is
**          recommended to support all possible entries (cos_user.c).
**
** The value ubIdx1018_SubNumberC holds the number of supported sub-indices.
*/
extern uint8_t  ubIdx1018_SubNumberC;


/*!
** \var     ulIdx1018_VendorIdC
** \brief   Identity object - vendor ID (Index 1018h)
** \todo    Set this value to your CANopen vendor ID (cos_user.c).
**
** The value ulIdx1018_VendorIdC holds the CANopen vendor ID. The
** vendor ID is assigned by the CiA office.
*/
extern uint32_t  ulIdx1018_VendorIdC;


/*!
** \var     ulIdx1018_ProductCodeC
** \brief   Identity object - product code (Index 1018h)
** \todo    Set this value to your product code (cos_user.c).
**
** The value ulIdx1018_ProductCodeC identifies a specific device
** version.
*/
extern uint32_t  ulIdx1018_ProductCodeC;


/*!
** \var     ulIdx1018_RevisionNumC
** \brief   Identity object - revision number (Index 1018h)
** \todo    Set this value to your software revision number (cos_user.c).
**
** The value ulIdx1018_RevisionNumC consists of a major revision number
** and a minor revision number. The major revision number identifies a
** specific CANopen behaviour. If the CANopen functionality is expanded,
** the major revision has to be incremented. The minor revision number
** identifies different versions with the same CANopen behaviour.
*/
extern uint32_t  ulIdx1018_RevisionNumC;

extern uint8_t   ubIdx1022_EdsTypeC;


/*-------------------------------------------------------------------------*\
** Function prototypes                                                     **
**                                                                         **
\*-------------------------------------------------------------------------*/


void  Cos301_ClearVerifyConfiguration(void);

//-------------------------------------------------------------------
/*!
** \brief   Index 1010 - Store Parameters
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code (enumeration #CosSdo_e)
**
** This object supports the saving of parameters in non volatile memory.
** By read access the device provides information about its saving
** capabilities. Several parameter groups are distinguished:
** \li Sub-Index 0 contains the largest Sub-Index that is supported
** \li Sub-Index 1 refers to all parameters that can be stored on the device
** \li Sub-Index 2 refers to communication related parameters
**     (Index 1000h - 1FFFh manufacturer specific communication parameters)
** \li Sub-Index 3 refers to application related parameters
**     (Index 6000h - 9FFFh manufacturer specific application parameters)
** \li Sub-Index 4 - 127 are for manufacturer parameters
** \li Sub-Index 128 - 254 are reserved for future use
**
** In order to avoid storage of parameters by mistake, storage is only
** executed when a specific signature is written to the appropriate Sub-Index.
** The signature is <b>save</b>.
** <p>
** On reception of the correct signature in the appropriate sub-index
** the device stores the parameter and then confirms the SDO transmission
** (initiate download response).
** If a  wrong signature is written, the device refuses to store and responds
** with Abort SDO Transfer (abort code: 0800 002xh).
** <p>
** The function returns a SDO response code defined by the #CosSdo_e
** enumeration.
** The index is only available when the symbol #COS_DICT_OBJ_1010 is set.
*/
uint8_t  Cos301_Idx1010(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


//-------------------------------------------------------------------
/*!
** \brief   Index 1011 - Restore Default Parameters
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code (enumeration #CosSdo_e)
**
** With this object the default values of parameters according to the
** communication or device profile are restored. By read access the
** device provides information about its capabilities to restore these
** values. Several parameter groups are distinguished:
** \li Sub-Index 0 contains the largest Sub-Index that is supported
** \li Sub-Index 1 refers to all parameters that can be restored
** \li Sub-Index 2 refers to communication related parameters
**     (Index 1000h - 1FFFh manufacturer specific communication parameters)
** \li Sub-Index 3 refers to application related parameters
**     (Index 6000h - 9FFFh manufacturer specific application parameters)
** \li Sub-Index 4 - 127 are for manufacturer parameters
** \li Sub-Index 128 - 254 are reserved for future use
**
** In order to avoid the restoring of default parameters by mistake,
** restoring is only executed when a specific signature is written to
** the appropriate sub-index. The signature is <b>load</b>.
** <p>
** On reception of the correct signature in the appropriate sub-index
** the device restores the default parameters and then confirms the SDO
** transmission (initiate download response).
** If a wrong signature is written, the device refuses to restore the
** defaults and responds with an Abort SDO Transfer (abort code: 0800 002xh).
** The default values are set valid after the device is reset (reset node
** for sub-index 1h - 7Fh, reset communication for sub-index 2h) or power cycled.
** <p>
** The function returns a SDO response code defined by the #CosSdo_e
** enumeration.
** The index is only available when the symbol #COS_DICT_OBJ_1011 is set.
*/
uint8_t  Cos301_Idx1011(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


//-------------------------------------------------------------------
/*!
** \brief   Index 1018 - Identity Object
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code (enumeration #CosSdo_e)
**
** The object at index 1018h contains general information about the device.
** The Vendor ID (sub-index 1h) contains a unique value allocated to each
** manufacturer. The manufacturer-specific Product code (sub-index 2h)
** identifies a specific device version. The manufacturer-specific Revision
** number (sub-index 3h) consists of a major revision number and a minor
** revision number. The major revision number identifies a specific CANopen
** behaviour. If the CANopen functionality is expanded, the major revision
** has to be incremented. The minor revision number identifies different
** versions with the same CANopen behaviour. The manufacturer-specific
** Serial number (sub-index 4h) identifies a specific device.
** <p>
** The function returns a SDO response code defined by the #CosSdo_e
** enumeration.
*/
uint8_t  Cos301_Idx1018(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


//-------------------------------------------------------------------
/*!
** \brief   Index 1020 - Verify configuration
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code (enumeration #CosSdo_e)
**
** This object indicates the downloaded configuration date and time.
** If a CANopen device supports the saving of parameters in non-volatile
** memory, a network configuration tool or a CANopen manager uses this
** object to verify the configuration after a CANopen device reset and to
** check if a reconfiguration is necessary. The configuration tool stores
** the date and time in that object and stores the same values in the DCF.
** Now the configuration tool lets the CANopen device save its configuration
** by writing to index 1010h sub-index 01h the signature "save". After a
** reset the CANopen device shall restore the last configuration and the
** signature automatically or by request. If any other command changes
** boot-up configuration values, the CANopen device resets this object
** to 0. <p>
** The Configuration Manager compares signature and configuration with the
** value from the DCF and decides if a reconfiguration is necessary or not.
** <p>
** Note: The usage of this object allows a significant speed-up of the
** boot-up process. If it is used, the system integrator considers that
** an user changes a configuration value and afterwards activate the command
** store configuration 1010h without changing the value of 1020h. So the system
** integrator ensures a 100% consequent usage of this feature.
** <p>
** <b>Value definition</b>
** <ul>
** <li>Sub-index 01h (configuration date) shall contain the number of days
** since January 1, 1984.</li>
** <li>Sub-index 02h (configuration time) shall be the number of ms after
** midnight.</li>
** </ul>
** <p>
** The function returns a SDO response code defined by the #CosSdo_e
** enumeration.
*/
uint8_t  Cos301_Idx1020(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


//-------------------------------------------------------------------
/*!
** \brief   Index 1021 - Store EDS
** \param   ubSubIndexV    sub-index
** \param   ubReqCodeV     read / write access
** \return  SDO response code (enumeration #CosSdo_e)
**
** This object shall indicate the downloaded EDS. The storage of EDS
** files in the CANopen device has some advantages:
**
** \li The manufacturer has not does not have the problem of
**     distributing the EDS via disks
** \li Management of different EDS versions for different software
**     versions is less error prone, if they are stored together
** \li The complete network settings are stored in the network.
**     This makes the task of analyzing or reconfiguring a network
**     easier for tools and more transparent for the users.
**
** <p>
** The filename does not need to be stored since every EDS contains
** its own filename. This object is enabled by setting #COS_DICT_OBJ_1021
** to a value > 0.
**
*/
uint8_t  Cos301_Idx1021(uint8_t ubSubIndexV, uint8_t ubReqCodeV);


//-------------------------------------------------------------------
/*!
** \brief   Set Default Parameters
** \see     CosMgrParameterSave()
**
** This function sets the default values to parameters which can be stored
** in a CANopen slave. The values of the following parameters are set
** according to CiA 301:
**
** \li   Identifier Sync-Message
** \li   Identifier Emergency
** \li   Identifier of all PDOs
** \li   Producer Heartbeat Time
** \li   Nodeguarding
**
*/
void  Cos301_ParmInit(void);

uint8_t  Cos301_ParmLoad(void);

uint8_t  Cos301_ParmSave(void);


//-------------------------------------------------------------------//
#ifdef __cplusplus                                                   //
}                                                                    //
#endif                                                               //
// end of C++ compiler wrapper                                       //
//-------------------------------------------------------------------//


#endif   // COS_301_H_

