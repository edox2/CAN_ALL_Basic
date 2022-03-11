//****************************************************************************//
// File:          c51f550_can.c                                               //
// Description:   CANpie Core functions for C8051F05xx Silabs controller      //
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


/*----------------------------------------------------------------------------*\
** Include files                                                              **
**                                                                            **
\*----------------------------------------------------------------------------*/

#include <SI_C8051F550_Register_Enums.h>
#include "cp_core.h"
#include "cp_msg.h"
#include "c51f550_can.h"



/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/




//-----------------------------------------------------------------------------
// the following two macros enable / disable the global CAN interrupt
//
#define  CAN_IRQ_ENABLE()     EIE2 |= 0x02;
#define  CAN_IRQ_DISABLE()    EIE2 &= ~0x02;

/*! 
** \def     MC_CAN0_INT interrupt vector for CAN 0
** \brief   Defines number of CAN Interrupt Vector for CAN 0 ISR
** 
*/
#define  MC_CAN0_INT    16

#define  MSG_DIR_RCV    0x00
#define  MSG_DIR_TRM    0x01

/*----------------------------------------------------------------------------*\
** Variables of module                                                        **
**                                                                            **
\*----------------------------------------------------------------------------*/


static CpCanMsg_ts      tsCanMsgS;        // global buffer for CAN message (IRQ)

#if CP_AUTOBAUD > 0
static uint8_t          ubAutobaudSelectS;
#endif

#if CP_STATISTIC > 0
static uint32_t         ulTrmCountS;
static uint32_t         ulRcvCountS;
static uint32_t         ulErrCountS;
#endif

static uint16_t         uwCanStatusOldS;

static uint16_t         uwCanDebugS;

//-------------------------------------------------------------------
// this CAN implementation is a little weird concerning RTR, so we
// keep the information about message direction at this place.
//
static uint8_t          aubMsgDirectionS[CP_BUFFER_MAX];


//-------------------------------------------------------------------
// these pointers store the callback handlers
//
uint8_t           (* pfnRcvIntHandler) (CpCanMsg_ts *, uint8_t);
uint8_t           (* pfnTrmIntHandler) (CpCanMsg_ts *, uint8_t);
uint8_t           (* pfnErrIntHandler) (uint8_t);


//-------------------------------------------------------------------
// declaration of internal functions
//
static uint8_t CAN_GetFreeIF(void);
static void    CAN_WaitIF(uint8_t);

uint16_t  uwCANDebugRegVG;

/*------------------------------------------------------------------------
** Bit timing values
**
** This table has the baudrates for a C8051F550 with a CAN clock frequency
** (With 24MHz,10KBit is out of reach)
*/

static uint16_t atsBitTimingTableS[] = {
   0x7EF1,   //  10 KBit/s	: BRP = 50, SJW = 4, 24 Tq
   0x7ED8,   //  20 KBit/s	: BRP = 25, SJW = 4, 24 Tq
   0x1C1D,   //  50 KBit/s	: BRP = 30, SJW = 1, 16 Tq // angepasst auf 87.5%
   0x1C0E,   // 100 KBit/s  : BRP = 15, SJW = 1, 16 Tq // angepasst auf 87.5%
   0x1C0B,   // 125 KBit/s  : BRP = 12, SJW = 1, 16 Tq // angepasst auf 87.5%
   0x1C05,   // 250 KBit/s  : BRP =  6, SJW = 1, 16 Tq // angepasst auf 87.5%
   0x1C02,   // 500 KBit/s  : BRP =  3, SJW = 1, 16 Tq // angepasst auf 87.5%
   0x1602,   // 800 KBit/s  : BRP =  3, SJW = 1, 10 Tq // angepasst auf 80%
   0x1801    //   1 MBit/s  : BRP =  1, SJW = 1, 12 Tq // angepasst auf 83.3%
};

/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
**                                                                            **
\*----------------------------------------------------------------------------*/




//----------------------------------------------------------------------------//
// CpCoreAutobaud()                                                           //
// run automatic baudrate detection                                           //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreAutobaud(CpPort_ts * ptsPortV, uint8_t * pubBaudSelV, 
                           uint16_t * puwWaitV)
{
   //----------------------------------------------------------------
   // check if automatic baudrate detection is enabled
   //
	#if CP_AUTOBAUD == 0
   return(CpErr_NOT_SUPPORTED);
   #else

   //----------------------------------------------------------------
   // ToDo: Implement here AutoBaudrate detection 
   //

   return(CpErr_BAUDRATE);
   #endif

}


//----------------------------------------------------------------------------//
// CpCoreBaudrate()                                                           //
// Setup baudrate of CAN controller                                           //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBaudrate(CpPort_ts * ptsPortV, uint8_t ubBaudSelV)
{
   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;
      
   //----------------------------------------------------------------
   // test if it is possible to change the bit timing register
   //
   if(CAN0CN_CCE == 0)
   {
      return(CpErr_INIT_FAIL);
   }


   //----------------------------------------------------------------
   // setup the values for btr0 and btr1
   //
   if(ubBaudSelV <= CP_BAUD_1M)
   {
      //---------------------------------------------------
      // take values from table
      //
      CAN0BT = atsBitTimingTableS[ubBaudSelV];
     
      return (CpErr_OK);
   }

   return (CpErr_BAUDRATE);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDataV)
{
   uint8_t  ubMsgIfRegT;   // message interface register set


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   //----------------------------------------------------------------
   // disable global CAN interrupt to avoid conflict between
   // application and CAN IRQ handler
   //
   CAN_IRQ_DISABLE();

   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;
   
   //----------------------------------------------------------------
   // get a free message interface register
   //
   while( (ubMsgIfRegT = CAN_GetFreeIF() ) > CAN_IF2_INDICATOR) { };

   if (ubMsgIfRegT)  // registerSet2
   {
	   //----------------------------------------------------------------
	   // read access to data register A and B
	   //

	   // Point to IFn Command Mask Register
	   CAN0IF2CM = (CAN_CMDMSK_DATAA | CAN_CMDMSK_DATAB);

	   // Point to IFn Command Request Register
	   CAN0IF2CR = ubBufferIdxV;

	   // wait until the Busy Flag  of IFn is cleared
	   while((CAN0IF2CR & CAN_CMDRQST_BUSY) > 0) { };

	   //----------------------------------------------------------------
	   // read data from selected buffer
	   //
	   *pubDataV = CAN0IF2DA1L;
	   pubDataV++;

	   *pubDataV = CAN0IF2DA1H;
	   pubDataV++;

	   *pubDataV = CAN0IF2DA2L;
	   pubDataV++;

	   *pubDataV = CAN0IF2DA2H;
	   pubDataV++;

	   *pubDataV = CAN0IF2DB1L;
	   pubDataV++;

	   *pubDataV = CAN0IF2DB1H;
	   pubDataV++;

	   *pubDataV = CAN0IF2DB2L;
	   pubDataV++;

	   *pubDataV = CAN0IF2DB2H;
	   pubDataV++;
   }
   else // registerSet1
   {
	   //----------------------------------------------------------------
	   // read access to data register A and B
	   //

	   // Point to IFn Command Mask Register
	   CAN0IF1CM = (CAN_CMDMSK_DATAA | CAN_CMDMSK_DATAB);

	   // Point to IFn Command Request Register
	   CAN0IF1CR = ubBufferIdxV;

	   // wait until the Busy Flag  of IFn is cleared
	   while((CAN0IF1CR & CAN_CMDRQST_BUSY) > 0) { };

	   //----------------------------------------------------------------
	   // read data from selected buffer
	   //
	   *pubDataV = CAN0IF1DA1L;
	   pubDataV++;

	   *pubDataV = CAN0IF1DA1H;
	   pubDataV++;

	   *pubDataV = CAN0IF1DA2L;
	   pubDataV++;

	   *pubDataV = CAN0IF1DA2H;
	   pubDataV++;

	   *pubDataV = CAN0IF1DB1L;
	   pubDataV++;

	   *pubDataV = CAN0IF1DB1H;
	   pubDataV++;

	   *pubDataV = CAN0IF1DB2L;
	   pubDataV++;

	   *pubDataV = CAN0IF1DB2H;
	   pubDataV++;
   }

   //----------------------------------------------------------------
   // enable global CAN interrupt
   //
   CAN_IRQ_ENABLE();

   return (CpErr_OK);

   }


//----------------------------------------------------------------------------//
// CpCoreBufferGetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDlcV)
{
   uint8_t  ubMsgIfRegT;   // message interface register set
   uint16_t  uwDlcT;        // DLC value


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   //----------------------------------------------------------------
   // disable global CAN interrupt to avoid conflict between
   // application and CAN IRQ handler
   //
   CAN_IRQ_DISABLE();

   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;
   
   //----------------------------------------------------------------
   // get a free message interface register
   //
   while( (ubMsgIfRegT = CAN_GetFreeIF() ) > CAN_IF2_INDICATOR) { };
   
   if (ubMsgIfRegT) // IF2
   {
	   //----------------------------------------------------------------
	   // read access to message control register
	   //
	   // Point to IFn Command Mask Register
	   CAN0IF2CM= CAN_CMDMSK_CONTROL;

	   // Point to IFn Command Request Register
	   CAN0IF2CR = ubBufferIdxV;

	   // wait until the Busy Flag is cleared
	   while((CAN0IF2CR & CAN_CMDRQST_BUSY) > 0) { };

	   // get DLC
	   uwDlcT = CAN0IF2MC;

   }
   else	// IF2
   {
	   //----------------------------------------------------------------
	   // read access to message control register
	   //
	   // Point to IFn Command Mask Register
	   CAN0IF1CM= CAN_CMDMSK_CONTROL;

	   // Point to IFn Command Request Register
	   CAN0IF1CR = ubBufferIdxV;

	   // wait until the Busy Flag is cleared
	   while((CAN0IF1CR & CAN_CMDRQST_BUSY) > 0) { };

	   // get DLC
	   uwDlcT = CAN0IF1MC;
   }
   
   //----------------------------------------------------------------
   // enable global CAN interrupt
   //
   CAN_IRQ_ENABLE();

   //----------------------------------------------------------------
   // remove bit 4 .. 16, cast to byte value
   //
   uwDlcT = uwDlcT & 0x000F;
   *pubDlcV = (uint8_t) uwDlcT;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferInit()                                                         //
// initialize CAN message buffer                                              //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferInit( CpPort_ts * ptsPortV, CpCanMsg_ts * ptsCanMsgV,
                              uint8_t ubBufferIdxV, uint8_t ubDirectionV)
{
   uint8_t ubMsgIfRegT;    // message interface register set


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;
   
   //----------------------------------------------------------------
   // get a free message interface register
   //
   while( (ubMsgIfRegT = CAN_GetFreeIF() ) > CAN_IF2_INDICATOR) { };

   if (ubMsgIfRegT)
   {     // Interface 2
	   //----------------------------------------------------------------
	   // write to the command mask register, access to all register
	   //
	   // Point to IFn Command Mask Register
	   CAN0IF2CM = (CAN_CMDMSK_WRRD    |
	                CAN_CMDMSK_MASK    |
	                CAN_CMDMSK_ARB     |
	                CAN_CMDMSK_CONTROL |
	                CAN_CMDMSK_DATAA   |
	                CAN_CMDMSK_DATAB);


	   //----------------------------------------------------------------
	   // setup the buffer for Transmit / Receive operation
	   //
	   if(ubDirectionV == CP_BUFFER_DIR_TX)
	   {
	      //-------------------------------------------------------//
	      // direction is transmit                                 //
	      //                                                       //
	      //-------------------------------------------------------//

	      //--------------------------------------------------------
	      // set message direction to 'transmit'
	      //
	      aubMsgDirectionS[ubBufferIdxV - 1] = MSG_DIR_TRM;


	      if( CpMsgIsExtended(ptsCanMsgV) )
	      {
	         // config Arbitration registers
	         CAN0IF2CM  = (uint16_t) (CpMsgGetExtId(ptsCanMsgV));
	         CAN0IF2A2  = ((uint16_t) (CpMsgGetExtId(ptsCanMsgV) >> 16) |
	                     CAN_ARB2_XTD                             |
	                     CAN_ARB2_DIR                             |
	                     CAN_ARB2_MSGVAL);
	      }
	      else
	      {
	         // config Mask registers
	         CAN0IF2M1  = 0x0000;
	         CAN0IF2M2  = (0x07FF << 2);

	         // config Arbitration registers
	         CAN0IF2A1  = 0x0000;
	         CAN0IF2A2  = ((CpMsgGetStdId(ptsCanMsgV) << 2) |
	                     CAN_ARB2_DIR                     |
	                     CAN_ARB2_MSGVAL);
	      }

	      // config Message Control register
	      CAN0IF2MC  = (CpMsgGetDlc(ptsCanMsgV) |
	                  CAN_MSGC_EOB            |
	                  CAN_MSGC_TXIE           |
	                  CAN_MSGC_UMASK          |
	                  CAN_MSGC_RXIE);
	   }
	   else
	   {
	      //-------------------------------------------------------//
	      // direction is receive                                  //
	      //                                                       //
	      //-------------------------------------------------------//

	      //--------------------------------------------------------
	      // set message direction to 'receive'
	      //
	      aubMsgDirectionS[ubBufferIdxV - 1] = MSG_DIR_RCV;

	      if( CpMsgIsExtended(ptsCanMsgV) ) // Extended Identifier
	      {
	         // config Mask registers
	         CAN0IF2M1  = 0xFFFF;
	         CAN0IF2M2  = (0x01FF | CAN_MSK2_MXTD);

	         // config Arbitration registers
	         CAN0IF2A1  = (uint16_t) (CpMsgGetExtId(ptsCanMsgV));
	         CAN0IF2A2  = ((uint16_t) (CpMsgGetExtId(ptsCanMsgV) >> 16) |
	                     CAN_ARB2_XTD                             |
	                     CAN_ARB2_MSGVAL);
	      }
	      else // Standard Identifier
	      {
	         // config Mask registers
	         CAN0IF2A2  = 0x0000;
	         CAN0IF2M2  = (0x07FF << 2);

	         // config Arbitration registers
	         CAN0IF2A1  = 0x0000;
	         CAN0IF2A2  = ((CpMsgGetStdId(ptsCanMsgV) << 2) |
	                     CAN_ARB2_MSGVAL);
	      }

	      // config Message Control register
	      CAN0IF2MC  = (CAN_MSGC_EOB  |
	                  CAN_MSGC_RXIE |
	                  CAN_MSGC_UMASK);
	   }


	   //----------------------------------------------------------------
	   // clear the data registers
	   //
	   CAN0IF2DA1  = 0x0000;
	   CAN0IF2DA2  = 0x0000;
	   CAN0IF2DB1  = 0x0000;
	   CAN0IF2DB2  = 0x0000;

	   //----------------------------------------------------------------
	   // write data to selected buffer
	   //
	   CAN0IF2CR = ubBufferIdxV;            // Start command request

	   while (CAN0IF2CRH & 0x80) {}        // Poll on Busy bit


   }
   else  // Interface 1
   {
	   //----------------------------------------------------------------
	   // write to the command mask register, access to all register
	   //
	   // Point to IFn Command Mask Register
	   CAN0IF1CM = (CAN_CMDMSK_WRRD    |
	                CAN_CMDMSK_MASK    |
	                CAN_CMDMSK_ARB     |
	                CAN_CMDMSK_CONTROL |
	                CAN_CMDMSK_DATAA   |
	                CAN_CMDMSK_DATAB);


	   //----------------------------------------------------------------
	   // setup the buffer for Transmit / Receive operation
	   //
	   if(ubDirectionV == CP_BUFFER_DIR_TX)
	   {
	      //-------------------------------------------------------//
	      // direction is transmit                                 //
	      //                                                       //
	      //-------------------------------------------------------//

	      //--------------------------------------------------------
	      // set message direction to 'transmit'
	      //
	      aubMsgDirectionS[ubBufferIdxV - 1] = MSG_DIR_TRM;


	      if( CpMsgIsExtended(ptsCanMsgV) )
	      {
	         // config Arbitration registers
	         CAN0IF1CM  = (uint16_t) (CpMsgGetExtId(ptsCanMsgV));
	         CAN0IF1A2  = ((uint16_t) (CpMsgGetExtId(ptsCanMsgV) >> 16) |
	                     CAN_ARB2_XTD                             |
	                     CAN_ARB2_DIR                             |
	                     CAN_ARB2_MSGVAL);
	      }
	      else
	      {
	         // config Mask registers
	         CAN0IF1M1  = 0x0000;
	         CAN0IF1M2  = (0x07FF << 2);

	         // config Arbitration registers
	         CAN0IF1A1  = 0x0000;
	         CAN0IF1A2  = ((CpMsgGetStdId(ptsCanMsgV) << 2) |
	                     CAN_ARB2_DIR                     |
	                     CAN_ARB2_MSGVAL);
	      }

	      // config Message Control register
	      CAN0IF1MC  = (CpMsgGetDlc(ptsCanMsgV) |
	                  CAN_MSGC_EOB            |
	                  CAN_MSGC_TXIE           |
	                  CAN_MSGC_UMASK          |
	                  CAN_MSGC_RXIE);
	   }
	   else
	   {
	      //-------------------------------------------------------//
	      // direction is receive                                  //
	      //                                                       //
	      //-------------------------------------------------------//

	      //--------------------------------------------------------
	      // set message direction to 'receive'
	      //
	      aubMsgDirectionS[ubBufferIdxV - 1] = MSG_DIR_RCV;

	      if( CpMsgIsExtended(ptsCanMsgV) ) // Extended Identifier
	      {
	         // config Mask registers
	         CAN0IF1M1  = 0xFFFF;
	         CAN0IF1M2  = (0x01FF | CAN_MSK2_MXTD);

	         // config Arbitration registers
	         CAN0IF1A1  = (uint16_t) (CpMsgGetExtId(ptsCanMsgV));
	         CAN0IF1A2  = ((uint16_t) (CpMsgGetExtId(ptsCanMsgV) >> 16) |
	                     CAN_ARB2_XTD                             |
	                     CAN_ARB2_MSGVAL);
	      }
	      else // Standard Identifier
	      {
	         // config Mask registers
	         CAN0IF1A2  = 0x0000;
	         CAN0IF1M2  = (0x07FF << 2);

	         // config Arbitration registers
	         CAN0IF1A1  = 0x0000;
	         CAN0IF1A2  = ((CpMsgGetStdId(ptsCanMsgV) << 2) |
	                     CAN_ARB2_MSGVAL);
	      }

	      // config Message Control register
	      CAN0IF1MC  = (CAN_MSGC_EOB  |
	                  CAN_MSGC_RXIE |
	                  CAN_MSGC_UMASK);
	   }


	   //----------------------------------------------------------------
	   // clear the data registers
	   //
	   CAN0IF1DA1  = 0x0000;
	   CAN0IF1DA2  = 0x0000;
	   CAN0IF1DB1  = 0x0000;
	   CAN0IF1DB2  = 0x0000;

	   //----------------------------------------------------------------
	   // write data to selected buffer
	   //
	   CAN0IF1CR = ubBufferIdxV;            // Start command request

	   while (CAN0IF1CRH & 0x80) {}        // Poll on Busy bit

   }

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   uint8_t ubMsgIfRegT;    // message interface register set


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;
   
   
   //----------------------------------------------------------------
   // get a free message interface register
   //
   while( (ubMsgIfRegT = CAN_GetFreeIF() ) > CAN_IF2_INDICATOR) { };
   
   if (ubMsgIfRegT)
   {	// IF2

	   //----------------------------------------------------------------
	   // write to the command mask register, access to control register
	   //
	   CAN0IF2CM =  CAN_CMDMSK_WRRD | CAN_CMDMSK_CONTROL;


	   //----------------------------------------------------------------
	   // disable message object
	   //
	   // clear message controle register
	   CAN0IF2MC = 0x0000;

	   CAN0IF2CR = ubBufferIdxV;
	   while((CAN0IF2CR & CAN_CMDRQST_BUSY) > 0) { };

   }
   else
   {	// IF1

	   //----------------------------------------------------------------
	   // write to the command mask register, access to control register
	   //
	   CAN0IF1CM =  CAN_CMDMSK_WRRD | CAN_CMDMSK_CONTROL;


	   //----------------------------------------------------------------
	   // disable message object
	   //
	   // clear message controle register
	   CAN0IF1MC = 0x0000;

	   CAN0IF1CR = ubBufferIdxV;
	   while((CAN0IF1CR & CAN_CMDRQST_BUSY) > 0) { };

   }
   
   //----------------------------------------------------------------
   // set message direction to read (default)
   //
   aubMsgDirectionS[ubBufferIdxV - 1] = MSG_DIR_RCV;


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetData( CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t * pubDataV)
{
   uint8_t    ubMsgIfRegT;   // message interface register set


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   //----------------------------------------------------------------
   // disable global CAN interrupt to avoid conflict between
   // application and CAN IRQ handler
   //
   CAN_IRQ_DISABLE();

   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;   
   
   
   //----------------------------------------------------------------
   // get a free message interface register
   //
   while( (ubMsgIfRegT = CAN_GetFreeIF() ) > CAN_IF2_INDICATOR) { };

   if (ubMsgIfRegT)
   {	// IF2

	   //----------------------------------------------------------------
	   // write access to data register A and B
	   //
	   CAN0IF2CM= (CAN_CMDMSK_WRRD  |
	              CAN_CMDMSK_DATAA |
	              CAN_CMDMSK_DATAB);


	   //----------------------------------------------------------------
	   // write data to selected buffer
	   //
	   CAN0IF2DA1L = (*pubDataV);
	   pubDataV++;

	   CAN0IF2DA1H = (*pubDataV);
	   pubDataV++;

	   CAN0IF2DA2L = (*pubDataV);
	   pubDataV++;

	   CAN0IF2DA2H = (*pubDataV);
	   pubDataV++;

	   CAN0IF2DB1L = (*pubDataV);
	   pubDataV++;

	   CAN0IF2DB1H = (*pubDataV);
	   pubDataV++;

	   CAN0IF2DB2L= (*pubDataV);
	   pubDataV++;

	   CAN0IF2DB2H= (*pubDataV);


	   //----------------------------------------------------------------
	   // transfer data
	   //
	   CAN0IF2CRL = ubBufferIdxV;

   }
   else
   {	// IF1
	   //----------------------------------------------------------------
	   // write access to data register A and B
	   //
	   CAN0IF1CM= (CAN_CMDMSK_WRRD  |
	              CAN_CMDMSK_DATAA |
	              CAN_CMDMSK_DATAB);


	   //----------------------------------------------------------------
	   // write data to selected buffer
	   //
	   CAN0IF1DA1L = (*pubDataV);
	   pubDataV++;

	   CAN0IF1DA1H = (*pubDataV);
	   pubDataV++;

	   CAN0IF1DA2L = (*pubDataV);
	   pubDataV++;

	   CAN0IF1DA2H = (*pubDataV);
	   pubDataV++;

	   CAN0IF1DB1L = (*pubDataV);
	   pubDataV++;

	   CAN0IF1DB1H = (*pubDataV);
	   pubDataV++;

	   CAN0IF1DB2L= (*pubDataV);
	   pubDataV++;

	   CAN0IF1DB2H= (*pubDataV);


	   //----------------------------------------------------------------
	   // transfer data
	   //
	   CAN0IF1CRL = ubBufferIdxV;


   }

   //----------------------------------------------------------------
   // enable global CAN interrupt
   //
   CAN_IRQ_ENABLE();

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts * ptsPortV, uint8_t ubBufferIdxV)
{
   uint8_t ubMsgIfRegT;       // message interface register set


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);


   //----------------------------------------------------------------
   // disable global CAN interrupt to avoid conflict between
   // application and CAN IRQ handler
   //
   CAN_IRQ_DISABLE();

   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;   

   //----------------------------------------------------------------
   // get a free message interface register
   //
   while( (ubMsgIfRegT = CAN_GetFreeIF() ) > CAN_IF2_INDICATOR) { };

   if (ubMsgIfRegT)
   {	// IF2
	   //----------------------------------------------------------------
	   // write to the command mask register, set TxRqst bit
	   //
	   CAN0IF2CML =  CAN_CMDMSK_WRRD | CAN_CMDMSK_TXRQST;


	   //----------------------------------------------------------------
	   // write data to selected buffer
	   //
	   CAN0IF2CR = ubBufferIdxV;
	   while((CAN0IF2CR & CAN_CMDRQST_BUSY) > 0) { };

   }
   else
   {    // IF1

	   //----------------------------------------------------------------
	   // write to the command mask register, set TxRqst bit
	   //
	   CAN0IF1CML =  CAN_CMDMSK_WRRD | CAN_CMDMSK_TXRQST;


	   //----------------------------------------------------------------
	   // write data to selected buffer
	   //
	   CAN0IF1CR = ubBufferIdxV;
	   while((CAN0IF1CR & CAN_CMDRQST_BUSY) > 0) { };
   }


   //----------------------------------------------------------------
   // enable global CAN interrupt
   //
   CAN_IRQ_ENABLE();


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetDlc(  CpPort_ts * ptsPortV, uint8_t ubBufferIdxV,
                                 uint8_t ubDlcV)
{
   uint8_t ubMsgIfRegT;       // message interface register set
   uint16_t uwCanMsgCtrlT;     // message control register (MCR)


   //----------------------------------------------------------------
   // check for valid buffer number
   //
   if(ubBufferIdxV < CP_BUFFER_1  ) return(CpErr_BUFFER);
   if(ubBufferIdxV > CP_BUFFER_MAX) return(CpErr_BUFFER);

   //----------------------------------------------------------------
   // limit DLC value to prevent some undefined behaviour
   //
   if(ubDlcV > 8)
   {
      ubDlcV = 8;
   }

   //----------------------------------------------------------------
   // disable global CAN interrupt to avoid conflict between
   // application and CAN IRQ handler
   //
   CAN_IRQ_DISABLE();

   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;


   //----------------------------------------------------------------
   // get a free message interface register
   //
   while( (ubMsgIfRegT = CAN_GetFreeIF() ) > CAN_IF2_INDICATOR) { };

   if (ubMsgIfRegT)
   {
	   //----------------------------------------------------------------
	   // Modification of the DLC is performed via a Read-Modify-Write
	   // cycle, because the DLC is located inside the message control
	   // register (MCR).
	   //
	   // First, do a read access to message control register:
	   //
	   CAN0IF2CM = CAN_CMDMSK_CONTROL;
	   // wait until busy flag is cleared
	   while((CAN0IF2CR & CAN_CMDRQST_BUSY) > 0) { };


	   //----------------------------------------------------------------
	   // Now read value from CAN_IFn_MCR register and modify it.
	   //
	   uwCanMsgCtrlT  = CAN0IF2MC;
	   uwCanMsgCtrlT &= 0xFF80;               // clear old DLC
	   uwCanMsgCtrlT |= (uint16_t)(ubDlcV);   // set new DLC

	   //----------------------------------------------------------------
	   // setup the command mask register
	   //
	   CAN0IF2CM = (CAN_CMDMSK_WRRD |
	               CAN_CMDMSK_CONTROL);

	   //----------------------------------------------------------------
	   // Finally write the new value into the CAN message memory.
	   //
	   CAN0IF2MC = (uint8_t) uwCanMsgCtrlT;

	   //----------------------------------------------------------------
	   // write data to selected buffer
	   //
	   CAN0IF2CR = ubBufferIdxV;

	   // wait until busy flag is cleared
	   while((CAN0IF2CR & CAN_CMDRQST_BUSY) > 0) { };
   }
   else
   {
	   //----------------------------------------------------------------
	   // Modification of the DLC is performed via a Read-Modify-Write
	   // cycle, because the DLC is located inside the message control
	   // register (MCR).
	   //
	   // First, do a read access to message control register:
	   //
	   CAN0IF1CM = CAN_CMDMSK_CONTROL;
	   // wait until busy flag is cleared
	   while((CAN0IF1CR & CAN_CMDRQST_BUSY) > 0) { };


	   //----------------------------------------------------------------
	   // Now read value from CAN_IFn_MCR register and modify it.
	   //
	   uwCanMsgCtrlT  = CAN0IF1MC;
	   uwCanMsgCtrlT &= 0xFF80;               // clear old DLC
	   uwCanMsgCtrlT |= (uint16_t)(ubDlcV);   // set new DLC

	   //----------------------------------------------------------------
	   // setup the command mask register
	   //
	   CAN0IF1CM = (CAN_CMDMSK_WRRD |
	               CAN_CMDMSK_CONTROL);

	   //----------------------------------------------------------------
	   // Finally write the new value into the CAN message memory.
	   //
	   CAN0IF1MC = (uint8_t) uwCanMsgCtrlT;

	   //----------------------------------------------------------------
	   // write data to selected buffer
	   //
	   CAN0IF1CR = ubBufferIdxV;

	   // wait until busy flag is cleared
	   while((CAN0IF1CR & CAN_CMDRQST_BUSY) > 0) { };
   }
   
   //----------------------------------------------------------------
   // enable global CAN interrupt
   //
   CAN_IRQ_ENABLE();


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts * ptsPortV, uint8_t ubModeV)
{
   uint8_t  ubStatusT;
   
   //----------------------------------------------------------------
   // switch CAN controller into mode "ubModeV"
   //
   switch(ubModeV)
   {
      //--------------------------------------------------------
      // Stop the CAN controller (passive on the bus)
      // Disable CAN interrupt
      //
      case CP_MODE_STOP:
         //------------------------------------------------
         // config SFRPAGE to access CAN0 registers
         //
         SFRPAGE = CAN0_PAGE;
         
         CAN0CN = (CAN_CR_INIT | CAN_CR_CCE);
         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Start the CAN controller (active on the bus)
      // Enable CAN interrupt
      //
      case CP_MODE_START:
         //------------------------------------------------
         // config SFRPAGE to access CAN0 registers
         //
         SFRPAGE = CAN0_PAGE;
         
         CAN0CN |=  (CAN_CR_IE | CAN_CR_SIE | CAN_CR_EIE);
         CAN0CN &= ~(CAN_CR_INIT | CAN_CR_CCE);
         
         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Start the CAN controller (Listen-Only)
      // CAN General Control register:
      //    ENA = 1     : enable CAN
      //    AUTOBAUD = 1: Listen-Only mode
      // Enable CAN interrupt
      //
      case CP_MODE_LISTEN_ONLY:

         ubStatusT = CpErr_OK;
         break;

      //--------------------------------------------------------
      // Other modes are not supported
      //
      default:
         ubStatusT = CpErr_NOT_SUPPORTED;
         break;
   }

   return(ubStatusT);
}



//----------------------------------------------------------------------------//
// CpCoreCanState()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanState(CpPort_ts * ptsPortV, CpState_ts * ptsStateV)
{
   uint16_t  uwStatusRegT;        // status register

   
   //----------------------------------------------------------------
   // config SFRPAGE to access CAN0 registers
   //
   SFRPAGE = CAN0_PAGE;
   
   //----------------------------------------------------------------
   // read CAN status register
   //
   uwStatusRegT = CAN0STAT;


   //----------------------------------------------------------------
   // CAN is active by default
   //
   ptsStateV->ubCanErrState = CP_STATE_BUS_ACTIVE;

   //----------------------------------------------------------------
   // now test the various error conditions
   //

   if(uwStatusRegT & CAN_STAT_EWARN)
   {
      ptsStateV->ubCanErrState = CP_STATE_BUS_WARN;
   }

   if(uwStatusRegT & CAN_STAT_EPASS)
   {
      ptsStateV->ubCanErrState = CP_STATE_BUS_PASSIVE;
   }

   if(uwStatusRegT & CAN_STAT_BOFF)
   {
      ptsStateV->ubCanErrState = CP_STATE_BUS_OFF;
   }

   //----------------------------------------------------------------
   // test the LEC value
   //
   uwStatusRegT = uwStatusRegT & 0x0007;
   switch(uwStatusRegT)
   {
      case 1:
         ptsStateV->ubCanErrType = CP_ERR_TYPE_STUFF;
         break;

      case 2:
         ptsStateV->ubCanErrType = CP_ERR_TYPE_FORM;
         break;

      case 3:
         ptsStateV->ubCanErrType = CP_ERR_TYPE_ACK;
         break;

      case 4:
         ptsStateV->ubCanErrType = CP_ERR_TYPE_BIT0;
         break;

      case 5:
         ptsStateV->ubCanErrType = CP_ERR_TYPE_BIT1;
         break;

      case 6:
         ptsStateV->ubCanErrType = CP_ERR_TYPE_CRC;
         break;

      default:
         ptsStateV->ubCanErrType = CP_ERR_TYPE_NONE;
         break;
   }
   

   return(CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreDriverInit()                                                         //
// init CAN controller                                                        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverInit(uint8_t ubLogIfV, CpPort_ts * ptsPortV)
{
   uint8_t ubBufferT;         // message buffer
   //uint16_t uwMaskT;           // mask for GPIO configuration

   
   //----------------------------------------------------------------
   // reset CAN
   //
   // config SFRPAGE to access CAN0 registers
   SFRPAGE = CAN0_PAGE;

   // set "CAN Initialization bit" and "CAN Configuration Change Enable bit" 
   CAN0CN = (CAN_CR_INIT | CAN_CR_CCE);
   // reset status register
   CAN0STAT = 0x00;
   // reset test register
   CAN0TST = 0x00;
   
//   SFRPAGE  = CAN0_PAGE;
//   CAN0CN  |= 0x41;       // Configuration Change Enable CCE and INIT
//   CAN0BT  = 0x2640;     // Bit Timing register
//   CpCoreBaudrate(CP_CHANNEL_1, CP_BAUD_1M);
   
//   CAN0IF1CM  = 0x0087;     // Config for TX : WRITE to CAN RAM, write data bytes, (in Command Mask 1)
//                          // set TXrqst/NewDat, clr IntPnd
//
//   // RX-IF2 operation may interrupt TX-IF1 operation
//   CAN0IF2CM = 0x1F;       // Config for RX : READ CAN RAM, read data bytes (in Command Mask 2)
                          // clr NewDat and IntPnd
//   CAN0CN  |= 0x06;       // Global Int. Enable IE and SIE
//   CAN0CN  &= ~0x41;      // Clear CCE and INIT bits, starts CAN state machine
   
   
   //----------------------------------------------------------------
   // clear all message buffers
   //
   for(ubBufferT = CP_BUFFER_1; ubBufferT <= CP_BUFFER_MAX; ubBufferT++)
   {
      CpCoreBufferRelease(CP_CHANNEL_1, ubBufferT);
   }

   //----------------------------------------------------------------
   // enable status change interrupt
   //
   // set "CAN Status change Interrupt Enable Bit" 
   CAN0CN |= CAN_CR_SIE;

   
   //----------------------------------------------------------------
   // clear statistic counter
   //
   #if CP_STATISTIC > 0
   ulTrmCountS = 0;
   ulRcvCountS = 0;
   ulErrCountS = 0;
   #endif


   //----------------------------------------------------------------
   // The variable 'uwCanStatusOldS' holds the (previous) value
   // of the CAN status register (CAN_SR). It is updated inside the
   // CAN interrupt handler.
   //
   uwCanStatusOldS = 0;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts * ptsPortV)
{
   CpCoreCanMode(ptsPortV, CP_MODE_STOP);
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreIntFunctions()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreIntFunctions(uint8_t ubChannelV,
                        uint8_t (* pfnRcvHandler)(CpCanMsg_ts *, uint8_t),
                        uint8_t (* pfnTrmHandler)(CpCanMsg_ts *, uint8_t),
                        uint8_t (* pfnErrHandler)(CpState_ts *) )
{
   //----------------------------------------------------------------
   // test the channel number
   //
   #if   CP_SMALL_CODE == 0
   if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
   #endif


   //----------------------------------------------------------------
   // store the new callbacks
   //
   pfnRcvIntHandler = pfnRcvHandler;
   pfnTrmIntHandler = pfnTrmHandler;
   pfnErrIntHandler = pfnErrHandler;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreStatistic()                                                          //
// return statistical information                                             //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreStatistic(CpPort_ts * ptsPortV, CpStatistic_ts * ptsStatsV)
{
   #if CP_STATISTIC > 0
   ptsStatsV->ulRcvMsgCount = ulRcvCountS;
   ptsStatsV->ulTrmMsgCount = ulTrmCountS;
   ptsStatsV->ulErrMsgCount = ulErrCountS;
   return(CpErr_OK);
   #else
   return(CpErr_NOT_SUPPORTED);
   #endif
}


//----------------------------------------------------------------------------//
// CAN_GetFreeIF()                                                            //
// check which CAN IFx Message Interface Register Set is free                 //
//----------------------------------------------------------------------------//
static uint8_t CAN_GetFreeIF(void)
{

   //----------------------------------------------------------------
   // test if IF1 Message Interface Register Set if free
   //
   if((CAN0IF1CR & CAN_CMDRQST_BUSY) == 0)
   {
      return(0x00);
   }

   //----------------------------------------------------------------
   // test if IF2 Message Interface Register Set if free
   //
   if((CAN0IF2CR & CAN_CMDRQST_BUSY) == 0)
   {
      // return offset to access the second IF Register set
      return(CAN_IF2_INDICATOR);
   }

   //----------------------------------------------------------------
   // nothing is free
   //
   return(0xFF);
}


//----------------------------------------------------------------------------//
// CAN_WaitIF()                                                               //
// wait until access to CAN IFx Message Interface Register Set is free        //
//----------------------------------------------------------------------------//
static void CAN_WaitIF(uint8_t ubMsgIfRegV)
{
   //----------------------------------------------------------------
   // test valid IF number
   //
   if((ubMsgIfRegV != 0) | (ubMsgIfRegV != CAN_IF2_INDICATOR)) return;


   //----------------------------------------------------------------
   // wait until IFx Message Interface Register Set if free
   //
   if (ubMsgIfRegV)
   {
	   while((CAN0IF2CR & CAN_CMDRQST_BUSY) > 0) { };
   }
   else
   {
	   while((CAN0IF1CR & CAN_CMDRQST_BUSY) > 0) { };
   }
}


//----------------------------------------------------------------------------//
// CAN_IRQHandler()                                                           //
// interrupt handler                                                          //
//----------------------------------------------------------------------------//

INTERRUPT(CAN0_IRQ, CAN0_IRQn)
{
   uint16_t  uwIntIdRegT;
   uint16_t  uwCanStatusT;        // value of CAN status register
   uint8_t  ubMsgIfRegT;         // message interface register set
   uint16_t  uwCanMsgCtrlT;       // message control register
   uint16_t  uwCanMsgArbT;        // message arbitration register


   //---------------------------------------------------------------------
   // Retrieve the source of the interrupt. Valid values are:
   // 0x0000        : No Interrupt is Pending
   // 0x0001-0x0020 : Number of Message Object which caused the interrupt
   // 0x0021-0x7FFF : unused
   // 0x8000        : Status Interrupt
   // 0x8001-0xFFFF : unused
   //
   uwIntIdRegT = CAN0IID;

   while(uwIntIdRegT)
   {
      //-------------------------------------------------------------
      // handle status change interrupt, this has the
      // highest priority
      //
      if(uwIntIdRegT == 0x8000)
      {


         
         //-----------------------------------------------------
         // read CAN status register to clear interrupt
         //
         uwCanStatusT = CAN0STAT;
         
         //-----------------------------------------------------
         // we test only the bits BOff, EWarn, and EPass
         //
         uwCanStatusT = uwCanStatusT & 0x00E0;
         if(uwCanStatusOldS != uwCanStatusT)
         {
            uwCanStatusOldS = uwCanStatusT;
            #if CP_STATISTIC > 0
            ulErrCountS++;
            #endif
         }
      }

   
      
      //-------------------------------------------------------------
      // handle message buffer interrupt, the number identifies
      // the message buffer
      //
      if(uwIntIdRegT < 0x0021)
      {
         //-----------------------------------------------------
         // get a free message interface register
         //
         while( (ubMsgIfRegT = CAN_GetFreeIF() ) > CAN_IF2_INDICATOR) { };

         if (ubMsgIfRegT)
         {	// IF2
             //-----------------------------------------------------
             // Transfer data from the Message Object addressed
             // by the Command Request Register into the selected
             // Message Buffer Registers
             //

             // select registers
             CAN0IF2CM = (CAN_CMDMSK_CONTROL | // ok
                         CAN_CMDMSK_ARB     | // ok
                         CAN_CMDMSK_TXRQST  | // ok NewDat
                         CAN_CMDMSK_CLRINTPND); // clear interrupt pending bit
             // transfer data
             CAN0IF2CR = (uint8_t) uwIntIdRegT;


             //-----------------------------------------------------
             // copy the following information to the
             // tsCanRcvMsgS structure:
             // - DLC
             // - Remote Frame
             // - Buffer Overrun
             //
             CpMsgClear(&tsCanMsgS);

             //-----------------------------------------------------
             // read CAN message control register ..
             //
             uwCanMsgCtrlT = CAN0IF2MC;

             //-----------------------------------------------------
             // .. and copy DLC value to message structure
             //
             CpMsgSetDlc(&tsCanMsgS, (uint8_t) (uwCanMsgCtrlT & 0x000F));


             //-----------------------------------------------------
             // read CAN message arbitration register
             //
             uwCanMsgArbT = CAN0IF2A2;


             //-----------------------------------------------------
             // was this a Tx message buffer?
             //
             //-----------------------------------------------------
             if(aubMsgDirectionS[uwIntIdRegT - 1] == MSG_DIR_TRM)
             {
                //---------------------------------------------
                // Check if direction bit in arbitration
                // register is set to 0 (message reception
                // in transmit buffer).
                // If so, set it to 1 again!
                //
                if( (uwCanMsgArbT & CAN_ARB2_DIR) == 0)
                {
                   uwCanMsgArbT = uwCanMsgArbT | CAN_ARB2_DIR;

                   //--------------------------------
                   // write to the command mask
                   // register, access to message
                   // arbitration
                   //
                   CAN0IF2CM = (CAN_CMDMSK_WRRD | CAN_CMDMSK_ARB);

                   CAN0IF2A2  = uwCanMsgArbT;

                   CAN0IF2CR = (uint8_t) uwIntIdRegT;

                   if(pfnRcvIntHandler)
                   {
                      (* pfnRcvIntHandler)(&tsCanMsgS, (uint8_t) (uwIntIdRegT));
                   }
                }

                //---------------------------------------------
                // test for transmit callback handler
                //
                else
                {
                   if(pfnTrmIntHandler)
                   {
                      (* pfnTrmIntHandler)(&tsCanMsgS, (uint8_t) (uwIntIdRegT));
                   }
                }

                #if CP_STATISTIC > 0
                ulTrmCountS++;
                #endif

             }
             else
             //------------------------------------------------
             // it was a Rx message buffer
             //
             //------------------------------------------------
             {
                //----------------------------------------
                // newdat = 1 && DIR == 1 -> RTR
                //
                if( (uwCanMsgCtrlT & CAN_MSGC_NEWDAT) &&
                    (uwCanMsgArbT & CAN_ARB2_DIR) )
                {
                   CpMsgSetRemote(&tsCanMsgS);
                }


                //----------------------------------------
                // test for receive callback handler
                //

                if(pfnRcvIntHandler)
                {
                   (* pfnRcvIntHandler)(&tsCanMsgS, (uint8_t) (uwIntIdRegT));
                }

                #if CP_STATISTIC > 0
                ulRcvCountS++;
                #endif
             }
          }
         else
         {  // IF1
             //-----------------------------------------------------
             // Transfer data from the Message Object addressed
             // by the Command Request Register into the selected
             // Message Buffer Registers
             //

             // select registers
             CAN0IF1CM = (CAN_CMDMSK_CONTROL | // ok
                         CAN_CMDMSK_ARB     | // ok
                         CAN_CMDMSK_TXRQST  | // ok NewDat
                         CAN_CMDMSK_CLRINTPND); // clear interrupt pending bit
             // transfer data
             CAN0IF1CR = (uint8_t) uwIntIdRegT;


             //-----------------------------------------------------
             // copy the following information to the
             // tsCanRcvMsgS structure:
             // - DLC
             // - Remote Frame
             // - Buffer Overrun
             //
             CpMsgClear(&tsCanMsgS);

             //-----------------------------------------------------
             // read CAN message control register ..
             //
             uwCanMsgCtrlT = CAN0IF1MC;

             //-----------------------------------------------------
             // .. and copy DLC value to message structure
             //
             CpMsgSetDlc(&tsCanMsgS, (uint8_t) (uwCanMsgCtrlT & 0x000F));


             //-----------------------------------------------------
             // read CAN message arbitration register
             //
             uwCanMsgArbT = CAN0IF1A2;


             //-----------------------------------------------------
             // was this a Tx message buffer?
             //
             //-----------------------------------------------------
             if(aubMsgDirectionS[uwIntIdRegT - 1] == MSG_DIR_TRM)
             {
                //---------------------------------------------
                // Check if direction bit in arbitration
                // register is set to 0 (message reception
                // in transmit buffer).
                // If so, set it to 1 again!
                //
                if( (uwCanMsgArbT & CAN_ARB2_DIR) == 0)
                {
                   uwCanMsgArbT = uwCanMsgArbT | CAN_ARB2_DIR;

                   //--------------------------------
                   // write to the command mask
                   // register, access to message
                   // arbitration
                   //
                   CAN0IF1CM = (CAN_CMDMSK_WRRD | CAN_CMDMSK_ARB);

                   CAN0IF1A2  = uwCanMsgArbT;

                   CAN0IF1CR = (uint8_t) uwIntIdRegT;

                   if(pfnRcvIntHandler)
                   {
                      (* pfnRcvIntHandler)(&tsCanMsgS, (uint8_t) (uwIntIdRegT));
                   }
                }

                //---------------------------------------------
                // test for transmit callback handler
                //
                else
                {
                   if(pfnTrmIntHandler)
                   {
                      (* pfnTrmIntHandler)(&tsCanMsgS, (uint8_t) (uwIntIdRegT));
                   }
                }

                #if CP_STATISTIC > 0
                ulTrmCountS++;
                #endif

             }
             else
             //------------------------------------------------
             // it was a Rx message buffer
             //
             //------------------------------------------------
             {
                //----------------------------------------
                // newdat = 1 && DIR == 1 -> RTR
                //
                if( (uwCanMsgCtrlT & CAN_MSGC_NEWDAT) &&
                    (uwCanMsgArbT & CAN_ARB2_DIR) )
                {
                   CpMsgSetRemote(&tsCanMsgS);
                }


                //----------------------------------------
                // test for receive callback handler
                //

                if(pfnRcvIntHandler)
                {
                   (* pfnRcvIntHandler)(&tsCanMsgS, (uint8_t) (uwIntIdRegT));
                }

                #if CP_STATISTIC > 0
                ulRcvCountS++;
                #endif
             }
          }
      }
      
      //----------------------------------------------------------------
      // config SFRPAGE to access CAN0 registers
      //
      SFRPAGE = CAN0_PAGE;   
      
      // get pending interrupts...
      uwIntIdRegT = CAN0IID;
   }
   // end of while(uwIntIdRegT)
   //----------------------------------------------------------------
   CAN0STAT &= ~0x0018;  // clear RX and TX flag
}


