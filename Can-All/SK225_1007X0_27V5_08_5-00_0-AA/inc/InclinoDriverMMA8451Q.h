/*
 * InclinoDriverMMA8451Q.h
 *
 *  Created on: 11.07.2016
 *********************************************
 *    (c)2016-2019 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 *  NXP (freescale) MMA8451Q Accelerometer related stuff
 */

#ifndef TILT_H_
#define TILT_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <si_toolchain.h>
//#include <SI_EFM8UB1_Register_Enums.h>

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

#define	BASE_ADDR_TILT		0x3A
#define	WRITE_ADDR_TILT		0x3A
#define	READ_ADDR_TILT		0x3B

#define TiltSTATUS_F_STATUS     0x00
#define TiltOUT_X_MSB			0x01
#define TiltOUT_X_LSB			0x02
#define TiltOUT_Y_MSB			0x03
#define TiltOUT_Y_LSB			0x04
#define TiltOUT_Z_MSB			0x05
#define TiltOUT_Z_LSB			0x06
#define TiltF_SETUP			    0x09
#define TiltTRIG_CFG			0x0A
#define TiltSYSMOD			    0x0B
#define TiltINT_SOURCE			0x0C
#define TiltWHO_AM_I			0x0D
#define TiltXYZ_DATA_CFG		0x0E
#define TiltHP_FILTER_CUTOFF	0x0F
#define TiltPL_STATUS			0x10
#define TiltPL_CFG			    0x11
#define TiltPL_COUNT			0x12
#define TiltPL_BF_ZCOMP		    0x13
#define TiltP_L_THS_REG		    0x14
#define TiltFF_MT_CFG			0x15
#define TiltFF_MT_SRC			0x16
#define TiltFF_MT_THS			0x17
#define TiltFF_MT_COUNT		    0x18
#define TiltTRANSIENT_CFG		0x1D
#define TiltTRANSIENT_SCR		0x1E
#define TiltTRANSIENT_THS		0x1F
#define TiltTRANSIENT_COUNT		0x20
#define TiltPULSE_CFG			0x21
#define TiltPULSE_SRC			0x22
#define TiltPULSE_THSX			0x23
#define TiltPULSE_THSY			0x24
#define TiltPULSE_THSZ			0x25
#define TiltPULSE_TMLT			0x26
#define TiltPULSE_LTCY			0x27
#define TiltPULSE_WIND			0x28
#define TiltASLP_COUNT			0x29
#define TiltCTRL_REG1			0x2A
#define TiltCTRL_REG2			0x2B
#define TiltCTRL_REG3			0x2C
#define TiltCTRL_REG4			0x2D
#define TiltCTRL_REG5			0x2E
#define TiltOFF_X				0x2F
#define TiltOFF_Y				0x30
#define TiltOFF_Z				0x31

uint8_t TiltGetStatus(uint8_t Channel, uint8_t BaseAddress,	SI_UU16_t *xAxis, SI_UU16_t *yAxis, SI_UU16_t *zAxis);

#endif /* TILT_H_ */
