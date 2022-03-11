//****************************************************************************//
// File:          mc_i2c.h                                                    //
// Description:   Functions for accessing I2C                                 //
// Author:        Johann Tiderko                                              //
// e-mail:        johan.tiderko@microcontrol.net                              //
//                                                                            //
// Copyright (C) MicroControl GmbH & Co. KG                                   //
// Lindlaustr. 2c                                                             //
// 53842 Troisdorf                                                            //
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
// 08.04.2011  Initial version                                                //
//                                                                            //
//****************************************************************************//


//------------------------------------------------------------------------------
// SVN  $Date: 2012-07-10 15:46:20 +0200 (Di, 10 Jul 2012) $
// SVN  $Rev: 4676 $ --- $Author: tiderko $
//------------------------------------------------------------------------------


#ifndef  _MC_I2C_H_
#define  _MC_I2C_H_


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/




//-----------------------------------------------------------------------------
/*!
** \file    mc_I2C.h
** \brief   Access to I2C devices
**
** For access of data from I2C devices a set of versatile
** functions is provided.
**
**
*/


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/


//-------------------------------------------------------------------
/*!
** \enum    I2C_ERR_e
** \brief   Error code for I2C
**
** The following error codes are used by the I2C driver. The
** status can be checked via the function McI2CGetStatus().
*/
enum I2C_ERR_e {
   /*! I2C is operational, success of latest operation      */
   eI2C_ERR_OK          = 0x00,

   /*! Hardware or initialisation failure of I2C            */
   eI2C_ERR_INIT        = 0x01,

   /*! Address out of range                                    */
   eI2C_ERR_ADDRESS     = 0x02,

   /*! Collision during read / write operation                 */
   eI2C_ERR_COLLISION   = 0x40,

   /*! Timeout at waiting for a request                        */
   eI2C_ERR_TIMEOUT     = 0x80
};

//-------------------------------------------------------------------
/*!
** \enum    I2C_NET_e
** \brief   I2C networks
**
** Support for I2C Networks
*/
enum I2C_NET_e {
   eI2C_NET_0 = 0x00,
   eI2C_NET_1,
   eI2C_NET_MAX
};

/*----------------------------------------------------------------------------*\
** Function prototypes                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/



//--------------------------------------------------------------------
/*!
** \brief
** \param   ubNetV - Number of I2C interface on a device
** \return  status of initialisation
*/
Status_tv McI2C_Init(uint8_t ubNetV);

//--------------------------------------------------------------------
/*!
** \param ubNetV - Number of I2C interface on a device
** \param ubDevV - Address of a I2C device
** \param ubAddrV - Address of Register the data should be read from
** \param pubDataV - pointer to RAM the data should be written to
** \param uwDataLenV - length of read data can be more that 256 bytes at continuous read
** \return status of read operation
*/
Status_tv McI2C_DataRead ( uint8_t  ubNetV,
                           uint8_t  ubDevV,
                           uint8_t  ubAddrV,
                           uint8_t *pubDataV,
                           uint16_t uwDataLenV);

//-------------------------------------------------------------------
/*!
** \param ubNetV - Number of I2C interface on a device
** \param ubDevV - Address of a I2C device
** \param ubAddrV - Address of Register the data should be written to
** \param pubDataV - pointer to RAM the data should be read from
** \param ubDataLenV - length of write data
** \return status of write operation
*/
Status_tv McI2C_DataWrite( uint8_t  ubNetV,
                           uint8_t  ubDevV,
                           uint8_t  ubAddrV,
                           uint8_t *pubDataV,
                           uint8_t  ubDataLenV);

uint32_t McI2C_GetTimeOutCntr(uint8_t ubNetV);

Status_tv McI2C_Release(uint8_t ubNetV);

#endif   // _MC_NVM_H_1

