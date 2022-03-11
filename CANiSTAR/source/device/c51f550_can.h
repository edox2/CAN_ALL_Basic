//****************************************************************************//
// File:          c51f550_can.h                                               //
// Description:   Defines for CAN registers of C8051F55x-f56x-f57x            //
// Author:        Matthias Siegenthaler                                       //
// e-mail:        matthias@sigitronic.com                                     //
//                                                                            //
//----------------------------------------------------------------------------//
//                                                                            //
// Date        History                                                        //
// ----------  -------------------------------------------------------------- //
// 26.09.2017  Initial version derived from microcontrol can-pie              //
//                                                                            //
//****************************************************************************//


#ifndef _C51F550_CAN_H_
#define _C51F550_CAN_H_
#include <SI_C8051F550_Register_Enums.h>// SFR declarations

//--------------------------------------------------------------------
// CAN Protocol Register Index for CAN0ADR, 
// from TABLE 21.2 of the C8051F55x-F56x-F57x datasheet
// 
//#define CANCTRL     CAN0CN                     //Control               Register
//#define CANSTAT     CAN0STAT                   //Status                Register
//#define ERRCNT      CAN0ERR                    //Error Counter         Register
//#define BITREG      CAN0BT                     //Bit Timing            Register
//#define INTREG      CAN0IID                    //Interrupt Low Byte    Register
//#define CANTSTR     CAN0TST                    //Test                  Register
//#define BRPEXT      CAN0BRPE                   //BRP Extension         Register

//--------------------------------------------------------------------
// IF1 Interface Registers
// 
//#define IF1CMDRQST  CAN0IF1CR                  //IF1 Command Request   Register
//#define IF1CMDMSK   CAN0IF1CM                  //IF1 Command Mask      Register
//#define IF1MSK1     CAN0IF1M1                  //IF1 Mask1             Register
//#define IF1MSK2     CAN0IF1M2                  //IF1 Mask2             Register
//#define IF1ARB1     CAN0IF1A1                  //IF1 Arbitration 1     Register
//#define IF1ARB2     CAN0IF1A2                  //IF1 Arbitration 2     Register
//#define IF1MSGC     CAN0IF1MC                  //IF1 Message Control   Register
//#define IF1DATA1    CAN0IF1DA1                 //IF1 Data A1           Register
//#define IF1DATA2    CAN0IF1DA2                 //IF1 Data A2           Register
//#define IF1DATB1    CAN0IF1DB1                 //IF1 Data B1           Register
//#define IF1DATB2    CAN0IF1DB2                 //IF1 Data B2           Register

//--------------------------------------------------------------------
// IF2 Interface Registers
// 
//#define IF2CMDRQST  CAN0IF2CR                  //IF2 Command Request   Register
//#define IF2CMDMSK   CAN0IF2CM                  //IF2 Command Mask      Register
//#define IF2MSK1     CAN0IF2M1                  //IF2 Mask1             Register
//#define IF2MSK2     CAN0IF2M2                  //IF2 Mask2             Register
//#define IF2ARB1     CAN0IF2A1                  //IF2 Arbitration 1     Register
//#define IF2ARB2     CAN0IF2A2                  //IF2 Arbitration 2     Register
//#define IF2MSGC     CAN0IF2MC                  //IF2 Message Control   Register
//#define IF2DATA1    CAN0IF2DA1                 //IF2 Data A1           Register
//#define IF2DATA2    CAN0IF2DA2                 //IF2 Data A2           Register
//#define IF2DATB1    CAN0IF2DB1                 //IF2 Data B1           Register
//#define IF2DATB2    CAN0IF2DB2                 //IF2 Data B2           Register

//--------------------------------------------------------------------
// Message Handler Registers
//
#define TRANSREQ1   CAN0TR1                    //Transmission Request1 Register
#define TRANSREQ2   CAN0TR2                    //Transmission Request2 Register

#define NEWDAT1     CAN0ND1                    //New Data 1            Register
#define NEWDAT2     CAN0ND2                    //New Data 2            Register

#define INTPEND1    CAN0IP1                    //Interrupt Pending 1   Register
#define INTPEND2    CAN0IP2                    //Interrupt Pending 2   Register

#define MSGVAL1     CAN0MV1                    //Message Valid 1       Register
#define MSGVAL2     CAN0MV2                    //Message Valid 2       Register


/*!
** \def     CAN_IF2_INDICATOR
** \brief   Defines offset value, that should be added to IF1 register to mark
**          to access IF2 register set (in c51f5xx implementations, there is no
**          constant offset between the addresses of the tow IFn any longer)
*/
#define CAN_IF2_INDICATOR     0x30


/*!
** \def     CAN_CMDRQST_BUSY
** \brief   Defines Busy Flag of IFn Command Request Registers     
*/ 
#define CAN_CMDRQST_BUSY   0x8000


/*!
** \defgroup   CAN_CMDMSK
** \brief      Defines Flags of IFn Command Mask register     
*/ 
//@{
#define CAN_CMDMSK_WRRD       0x0080
#define CAN_CMDMSK_MASK       0x0040
#define CAN_CMDMSK_ARB        0x0020
#define CAN_CMDMSK_CONTROL    0x0010
#define CAN_CMDMSK_CLRINTPND  0x0008
#define CAN_CMDMSK_TXRQST     0x0004
#define CAN_CMDMSK_DATAA      0x0002
#define CAN_CMDMSK_DATAB      0x0001
//@}


/*!
** \defgroup   CAN_ARB2
** \brief      Defines Flags of IFn Arbitration 2 register     
*/ 
//@{ 
#define CAN_ARB2_MSGVAL  0x8000
#define CAN_ARB2_XTD     0x4000
#define CAN_ARB2_DIR     0x2000
//@}


/*!
** \defgroup   CAN_MSGC
** \brief      Defines Flags of IFn  Message Control register     
*/ 
//@{
#define CAN_MSGC_NEWDAT  0x8000
#define CAN_MSGC_MSGLST  0x4000
#define CAN_MSGC_INTPND  0x2000
#define CAN_MSGC_UMASK   0x1000
#define CAN_MSGC_TXIE    0x0800
#define CAN_MSGC_RXIE    0x0400
#define CAN_MSGC_RMTEN   0x0200
#define CAN_MSGC_TXRQST  0x0100
#define CAN_MSGC_EOB     0x0080
//@}


/*!
** \defgroup   CAN_MSK2
** \brief      Defines Flags of IFn  Mask 2 register     
*/ 
//@{
#define CAN_MSK2_MXTD    0x8000
#define CAN_MSK2_MDIR    0x4000
//@}


/*!
** \defgroup   CAN_STAT
** \brief      Defines Flags of CAN Status register     
*/ 
//@{
#define CAN_STAT_LEC      0x0007
#define CAN_STAT_TXOK     0x0008
#define CAN_STAT_RXOK     0x0010
#define CAN_STAT_EPASS    0x0020
#define CAN_STAT_EWARN    0x0040
#define CAN_STAT_BOFF     0x0080
//@}


/*!
** \defgroup   CAN_STAT
** \brief      Defines Flags of CAN Status register     
*/
//@{
#define CAN_CR_TEST     0x0080
#define CAN_CR_CCE      0x0040
#define CAN_CR_DAR      0x0020
#define CAN_CR_EIE      0x0008
#define CAN_CR_SIE      0x0004
#define CAN_CR_IE       0x0002
#define CAN_CR_INIT     0x0001
//@}


/*! 
** \brief 16Bit access to CAN0DAT Register
** 
** The CAN0DAT Registers are used to read/write register values 
** and data to and from the CAN Registers pointed to 
** with the index number in the CAN0ADR Register.
*/
//sfr16 CAN0DAT = 0xD8;


#endif /* _C51F550_CAN_H_ */
