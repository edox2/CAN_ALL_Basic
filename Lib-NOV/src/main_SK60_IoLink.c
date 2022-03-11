/*main_SK60_IoLink.c
 ***********************************************
 *               (c)2015-2021                  *
 *               GENGE & THOMA                 *
 *              Erlenstrasse 32                *
 *            CH-2555 Bruegg b.Biel             *
 *                                             *
 *      Author: Matthias Siegenthaler          *
 *                                             *
 *     LIBRARIES & CONCEPT (c) 2009-2022       *
 *           SIGITRONIC SOFTWARE               *
 *          Matthias Siegenthaler              *
 *              Kirchgaessli 3                  *
 *          CH-2502 Biel-Bienne                *
 *              Switzerland                    *
 *                                             *
 *         matthias@sigitronic.com             *
 ***********************************************
 */

#define CALIB_COMPATIBILITY_VERSION (0x42)  // select a magic number in the range of 01x to 0xfe

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////
#pragma optimize (11, speed)

#include <si_toolchain.h>
#include <SI_EFM8UB3_Register_Enums.h>                  // SFR declarations
#include <math.h>
#include <LIMITS.H>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "InitDevice_IOLINK.h"
#include "HalDef.h"
#include "InclinoDriverMMA8451Q.h"
#include "VibraDriverDRV2605L.h"
#include "LedDriverLP55231.h"
#include "AdcDriverADS7924.h"
#include "HallDriverTLE493D_W2B6.h"
#include "MagnetNodeMLX90395.h"
#include "FramDriverMB85RC64TA.h"
#include "Cordic.h"
#include "PersistSettings.h"
#include "I2cDispatcher.h"
#include "IoLinkPhy.h"
#include "Filter.h"

/****************************************************************************
 **
 **     Includes
 */
/* Generic Includes */

#include "Config.h"                 /* Definition of Configuration */
#include "DTypes.h"                 /* Definition of Plattform specific Macros */
/* Includes of other Modules */
#include "DeviceStack.h"            /* Definitions for the FrameHandler Module */
#include "string.h"
/* Includes of stack extensions */
#include "ParameterManager.h"		    /* files for use of stack extensions */
#include "DataStorage.h"
#include "DeviceAccess.h"
#include "EventDispatcher.h"
/* IO-Link-Profile */
#include "Profile_common.h"
#include "SmartSensor.h"
#include "HMT7742.h"

#include "FrameHandler.h"
#include "ParameterHandler.h"
#include "PDHandler.h"
#include "StateHandler.h"

/*     End Includes
 **
 ****************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////////////////////////

//#define HW_TEST (1)			 // Can't store calibration to FRAM as version is only used for hardware test purposes during production
#define Z_RATIO_EVAL (0)     // During development, the ratio between amplitudes from sine and cosine of z-Axis2 has to be evaluated for calculation of stretch-factor
#define Z_RATIO_FOR_Y_AGAINST_Z	(2.66F)
#define DIAGNOSTIC (0)       // Test the MODE of Operation after start up of device
#define INIT_SIGI (1)		 // Prevents compiler warnings coming form race conditions of not-reentrant code in TMG IO-Link stack (GT-Alternative code used instead)
#define WATCHDOG_ACTIVE (1)	 // This protects firmware from unintended hanging routines
#define ERROR_TRESHOLD (9)   // number of continuous warnings before a Error is populated to IO-Link
#define WARNING_TRESHOLD (5) // number of continuous warnings before a internal warning is populated to IO-Link
#define COMPARE_TRESHOLD (27) // accepted colinearity-error between X1/X2 or Y1/Y2 signal

#define FIRMWARE_VER ('1')    // Denotes Firmware Generation
#define FIRMWARE_REV (CALIB_COMPATIBILITY_VERSION)   // Increment for all productive builds

#ifdef HW_TEST
	#define	SETTING_VALID (0xBEEF) // all Settings using this number should be compatible, increment it otherwise. (do not exceed valid Range form 1...65534)
#else
	#define	SETTING_VALID (0x5161) // all Settings using this number should be compatible, increment it otherwise. (do not exceed valid Range form 1...65534)
#endif

#define	SETTING_INVALID (0xffff)
#define	STORE_SETTINGS_TO_FLASH (true)
#define	READ_SETTINGS_FROM_FLASH (false)
#define	STORE_SETTINGS_TO_FRAM (true)
#define	READ_SETTINGS_FROM_FRAM (false)

#define	LIMIT_POS (32767)
#define	LIMIT_NEG (-32767)

#define SEGMENT_LED_SECTOR_1		(0x01)
#define SEGMENT_LED_SECTOR_2		(0x02)
#define SEGMENT_LED_SECTOR_3		(0x04)
#define SEGMENT_LED_SECTOR_MERGE	(0x08)
#define SEGMENT_VIBRA_HIGH_BANK 	(0x10)
#define SEGMENT_VIBRA_REPLAY 		(0x20)
#define SEGMENT_HOLDOFF 			(0x40)
#define SEGMENT_TESTMODE_RESTART 	(0x80)
#define SEGMENT_LED_CMD (SEGMENT_LED_SECTOR_1 | SEGMENT_LED_SECTOR_2 | SEGMENT_LED_SECTOR_3 | SEGMENT_LED_SECTOR_MERGE)


//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define LFOCLK_HZ 10000UL         // LFOSC default frequency, in Hz
#define WDT_TICKS (LFOCLK_HZ*1)   // ~1 second worth of WDT ticks

// Default system clock for UB3 is 24.5 MHz HFOSC divided by 8
#define SYSTEM_CLOCK_FREQUENCY  (48000000)

// WDT delay cycles as defined by Erratum WDT_E102 is 2 clock cycles
#define WDT_DELAY_CYCLES        2

/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

uint16_t CalibrationSettingsFRAM(bool DirectionToFlash);

void WDT0_delay(uint32_t systemClkFreq, uint32_t wdtClkFreq, uint32_t wdtClkCycles);


/////////////////////////////////////////////////////////////////////////////
// type definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

static adcBaseStruct  AdcData1;
static adcBaseStruct  AdcData2;
static MLX90395_BaseStruct  TestMeterXY;
static MLX90395_BaseStruct  TestMeterZ;

static TLE493D_BaseStruct TestSensorXY;
static TLE493D_BaseStruct TestSensorZ;

uint8_t AllowReCalibrationTag = ~CALIB_COMPATIBILITY_VERSION;
uint16_t KeyDataL;
uint16_t KeyDataR;
uint16_t KeyDataT;
uint16_t KeyDataB;
uint8_t  KeyL_Reverse = 0xAA;
uint8_t  KeyR_Reverse = 0xAA;
uint8_t  KeyT_Reverse = 0xAA;
uint8_t  KeyB_Reverse = 0xAA;

uint16_t AdcMaxLeft = 0;
uint16_t AdcMinLeft = UINT_MAX;
uint16_t AdcMaxRight = 0;
uint16_t AdcMinRight = UINT_MAX;
uint16_t AdcMaxTop = 0;
uint16_t AdcMinTop = UINT_MAX;
uint16_t AdcMaxBottom = 0;
uint16_t AdcMinBottom = UINT_MAX;
uint16_t AdcMidBottom;
uint16_t AdcMidTop;
uint16_t AdcMidLeft;
uint16_t AdcMidRight;

uint16_t AngleMinSensor = INT_MAX;
uint16_t AngleMidSensor = INT_MAX;
uint16_t AngleMaxSensor = INT_MAX;
uint16_t AngleAbsSensor = INT_MAX;
uint16_t AngleRelSensor = INT_MAX;
int16_t AngleZPercentSensor = 0;
int16_t PosZMeter = 0;
uint32_t zRadiusMeter  = INT_MAX;
uint32_t zRadiusMinMeter = INT_MAX;
uint32_t zRadiusMaxMeter = 0;

bool ea_Val;
bool WdtReset = false;
bool ButtonLeft = false;
bool ButtonRight = false;
bool ButtonFront = false;
bool ButtonFront1 = false;
bool ButtonFront2 = false;
bool BootButtonAtStartup = true;
bool AllButtonsAtStartup = false;
bool InternalFault = false;
bool RestartFirmware = false;
bool TestMode = false;
bool Holdoff = false;
bool CommonInitError = false;

bool SensorXYError = false;
bool SensorZError = false;
bool ButtonLeftError = false;
bool ButtonRightError = false;
bool ButtonFrontError = false;
bool CrcError = false;
bool ButtonError = false;
bool AdcFrontError = false;
bool AdcTopError = false;
bool FramError = false;
bool VibraError = false;
bool LedError = false;
bool MeterXYError = false;
bool AngleZError = false;
bool MeterZError = false;
bool AxisXError = false;
bool AxisYError = false;
bool VibraOverCurrent = false;
bool VibraOverTemp = false;
bool VibraCalibTimeout = false;

int8_t CompareX1, CompareX2, CompareY1, CompareY2;
uint16_t LedStateOpen = 0;
uint16_t LedStateShort = 0;
uint8_t WarningToErrorConverter = 0;
bool SetVibraHighBank = false;
bool LastVibraReplay = false;
bool VibraReplay = false;
uint8_t RightButtonState = PB_ABSENT;
uint8_t LeftButtonState = PB_ABSENT;
uint8_t FrontButtonState = PB_ABSENT;
uint8_t FrontButton1State = PB_ABSENT;
uint8_t FrontButton2State = PB_ABSENT;
uint8_t LastVibra = 0;
uint8_t LastLib = 0;
SI_UU16_t Converter16;
uint16_t AngleMin = INT_MAX;
uint16_t AngleMid = INT_MAX;
uint16_t AngleMax = INT_MAX;
uint16_t AngleAbs = INT_MAX;
int16_t  AngleDeg = 0;
uint16_t AngleRel = INT_MAX;
uint16_t AngleMinLive = INT_MAX;
uint16_t AngleMaxLive = INT_MAX;
int16_t AngleZ = 0;
uint8_t SerialNr[15] = "XXXX-XXXX-XXXX";
uint8_t V_InfoList[8];
RollingAverageSignedFilterStruct FilterCartridgeOne;
RollingAverageSignedFilterStruct FilterCartridgeTwo;
RollingAverageUnsignedFilterStruct FilterCartridgeKeyL;
RollingAverageUnsignedFilterStruct FilterCartridgeKeyR;
RollingAverageUnsignedFilterStruct FilterCartridgeKeyT;
RollingAverageUnsignedFilterStruct FilterCartridgeKeyB;

#if (Z_RATIO_EVAL == 1)
int16_t zIminX  = INT_MAX;
int16_t zImaxX  = INT_MIN;
int16_t zIminY  = INT_MAX;
int16_t zImaxY  = INT_MIN;
int16_t zIminZ  = INT_MAX;
int16_t zImaxZ  = INT_MIN;
float zFactorStretchXtoY;
float zFactorStretchYtoZ;
#endif
/****************************************************************************
 **
 **     Definition of global variables
 */

PARAM_Declare(Params11, ms2CycleTime(2),USE_CONST_CAPABILITIES,PD_IN,PD_OUT,USE_CONST_VENDORID1,USE_CONST_VENDORID2,USE_CONST_DEVICEID1,USE_CONST_DEVICEID2,USE_CONST_DEVICEID3,0x11);
#if (USE_CHANGEABLE_REVISIONID == 1)
PARAM_Declare(Params10, ms2CycleTime(2),USE_CONST_CAPABILITIES_10,PD_IN,PD_OUT,USE_CONST_VENDORID1,USE_CONST_VENDORID2,USE_CONST_DEVICEID1,USE_CONST_DEVICEID2,USE_CONST_DEVICEID3,0x10);
#endif

/* Declaration of variables handled by the parameter manager */
TParameterSet ParSet; /* parset which shall be stored in data storage */

/* dynamic variables handled by application */
TUnsigned8 EABufferOut[USE_PDOUT_BYTES]; /* Buffer for the output process data, max. Size */
TUnsigned8 EABufferIn[USE_PDIN_BYTES]; /* Buffer for the output process data, max. Size */
uint8_t VibraBuffer[8]; // Vibra Buffer

/* read only variables */
SI_UU16_t X_Axis1 = {0};
SI_UU16_t X_Axis2 = {0};
SI_UU16_t Y_Axis1 = {0};
SI_UU16_t Y_Axis2 = {0};
SI_UU16_t Z_Axis1 = {0};
SI_UU16_t Z_Axis2 = {0};
SI_UU16_t SwitchTopL1 = {0};
SI_UU16_t SwitchTopL2 = {0};
SI_UU16_t SwitchTopR1 = {0};
SI_UU16_t SwitchTopR2 = {0};
SI_UU16_t SwitchFrontTop1 = {0};
SI_UU16_t SwitchFrontTop2 = {0};
SI_UU16_t SwitchFrontBottom1 = {0};
SI_UU16_t SwitchFrontBottom2 = {0};
int8_t  TempTop1 = 0;
int8_t  TempTop2 = 0;
int8_t  TempBase1 = 0;
int8_t  TempBase2 = 0;
int8_t  LRA_State = 0;

/* definition of used system commands */
PM_SYSTEMCOMMAND_DECLARE() = {
	PM_SYSTEMCOMMAND_BLOCKPARAM
	PM_SYSTEMCOMMAND_FACTORYRESET
};

/* definition of used events */
const TEvent EventCollection[] = {
	ED_STACK_EVENTS	//Event ID for ED_EventAppear and ED_EventDisappear
	/* the ID of the first user event is ED_ID_USER_START */
	EVENT_ERROR(6200) /* first test event used for protocol testing */
	EVENT_ERROR(6201) /* second test event used for protocol testing */
	/* application events */
	EVENT_MESSAGE(6202) /* Switch 1 */
	EVENT_MESSAGE(6203) /* Switch 2 */
	EVENT_MESSAGE(6204) /* Switch 3 */
	EVENT_MESSAGE(6205) /* Vibra finished */
};

/*-----------------------------------------------------------------------------*/
/* definitions used for IO-Link Common Profile */

PROFILEIDS = {
	/* Use this order: 0x0nnn, 0x4nnn, 0x8nnn */
	SMART_PROFILE_ID       /* 0x0001 = Smart Sensor Profile, Generic Profiled Sensor */
	/* no function IDs for Smart Sensor, because no switching channel and no teach-in is used */

	/* IO-Link Common Profile implicitly includes function IDs FCID_IDENT, FCID_PDV, FCID_DIAG */
	/*    and FCID_EXTIDENT. They shall not be listed separately. */
	COMPROF_PROFILE_ID          /* 0x4000 = Common Profile */
};

COMPROF_PDINDESC =
{
	COMPROF_PD_SETOFBOOL(16, 0)  /* 16 bit, offset 0, subindex 7-22: SwitchStateC, SwitchStateB, SwitchStateA, etc */
	COMPROF_PD_INT(8, 16)         /* 8 bit, offset 16, subindex 6: ValueAxisZ2 */
	COMPROF_PD_INT(8, 24)         /* 8 bit, offset 24, subindex 5: ValueAxisY2 */
	COMPROF_PD_INT(8, 32)         /* 8 bit, offset 32, subindex 4: ValueAxisX2 */
	COMPROF_PD_INT(8, 40)         /* 8 bit, offset 40, subindex 3: ValueAxisZ1 */
	COMPROF_PD_INT(16, 48)        /* 16 bit, offset 48, subindex 2: ValueAxisY1 */
	COMPROF_PD_INT(16, 64)        /* 16 bit, offset 64, subindex 1: ValueAxisX1 */
};

COMPROF_PDOUTDESC =
{
	COMPROF_PD_UINT(8, 0)         /* 8 bit, offset 32, subindex 8: Vibration 4 or Vibration 8 */
	COMPROF_PD_UINT(8, 8)         /* 8 bit, offset 40, subindex 7: Vibration 3 or Vibration 7 */
	COMPROF_PD_UINT(8, 16)        /* 8 bit, offset 48, subindex 6: Vibration 2 or Vibration 6 */
	COMPROF_PD_UINT(8, 24)        /* 8 bit, offset 56, subindex 5: Vibration 1 or Vibration 5 */
	COMPROF_PD_UINT(8, 32)        /* 8 bit, offset 64, subindex 4: LEDSegment */
	COMPROF_PD_UINT(8, 40)        /* 8 bit, offset 72, subindex 3: LEDStateBL */
	COMPROF_PD_UINT(8, 48)        /* 8 bit, offset 80, subindex 2: LEDStateGN */
	COMPROF_PD_UINT(8, 56)        /* 8 bit, offset 88, subindex 1: LEDStateRD */
};

/*-----------------------------------------------------------------------------*/

/* List of all variables which are given to the parameter manager */
// VORSICHT! : Die maximale ISDU-Länge wurde in Config.h auf 50 Bytes beschränkt. Falls mehr benötigt werden -> anpassen
const TVariable code VariableCollection[] = {
	/* Parameter Manager Parameter */
	PM_SYSTEMCOMMAND
	PM_DEVICEACCESS
    /* Smart Sensor Profile */
    PM_PROFILE                             /* (13) */
    PM_PDINDESC                            /* (14) */
    PM_PDOUTDESC                           /* (15) */

	/* Standard Parameter */
	PM_VENDORNAME ("Genge Thoma AG")
	PM_VENDORTEXT ("http://www.gengethoma.ch")
	PM_PRODUCTNAME ("SK60 IO-Link")
	PM_PRODUCTID ("SK60")
	PM_PRODUCTTEXT ("SK60 Joystick with IO-Link Interface")
	//PM_SERIALNUMBER ("S0001") /* (21) */
	{21,   PM_OSTRING       (PM_RO, STATIC(SerialNr), sizeof(SerialNr))},
	//{	0x15, PM_OSTRING(PM_RO, STATIC(SerialNumber), sizeof(SerialNumber))}, /* serial number */
	PM_HARDWAREREV ("1.2")
#if(WATCHDOG_ACTIVE == 1)

	PM_FIRMWAREREV ("1.0hWD-RTM") // RTM = Release To Market, RCx = Release Candidate x, TSTx = Test Release x
#else
	PM_FIRMWAREREV ("1.0h-RTM") // RTM = Release To Market, RCx = Release Candidate x, TSTx = Test Release x
#endif // Watchdog not active

	//  PM_APPSPECIFIC        (AppSpecificTag)    <- funktioniert auf dem 8051 so nicht, deshalb:
	//	{0x18, {sizeof(((TParameterSet*)0)->AppSpecificTag), (0x08 | 0x04 | 0x40), {/*(void*)*/ (size_t)&(((TParameterSet  *)0)->AppSpecificTag)}},},
	/* Common Profile, Extended Identification (FC 0x8100) */
	//	PM_FUNCTIONTAG        (FunctionTag)    <- funktioniert auf dem 8051 so nicht, deshalb:
	//	{0x19, {sizeof(((TParameterSet*)0)->FunctionTag), (0x08 | 0x04 | 0x40), {/*(void*)*/ (size_t)&(((TParameterSet  *)0)->FunctionTag)}},},    /* index 25, 32 octets */
	//  PM_LOCATIONTAG        (LocationTag)    <- funktioniert auf dem 8051 so nicht, deshalb:
	//	{0x1A, {sizeof(((TParameterSet*)0)->LocationTag), (0x08 | 0x04 | 0x40), {/*(void*)*/ (size_t)&(((TParameterSet  *)0)->LocationTag)}},},    /* index 26, 32 octets */

	{0x18, {sizeof(((TParameterSet*)0)->AppSpecificTag), (0x08 | 0x04 | 0x40), {&ParSet.AppSpecificTag - &ParSet}},},
	{0x19, {sizeof(((TParameterSet*)0)->FunctionTag), (0x08 | 0x04 | 0x40), {&ParSet.FunctionTag - &ParSet}},},
	{0x1A, {sizeof(((TParameterSet*)0)->LocationTag), (0x08 | 0x04 | 0x40), {&ParSet.LocationTag - &ParSet}},},

	PM_ERRORCOUNT
	PM_DEVICESTATUS
	PM_PD_IN (EABufferIn, USE_PDIN_BYTES)     /* (40) */
	PM_PD_OUT (EABufferOut, USE_PDOUT_BYTES)  /* (41) */
	PM_DETAILEDSTATUS


	/* User Parameter */

	{64,   PM_UINT16       (PM_RO, STATIC(&X_Axis1))},
	{65,   PM_UINT16       (PM_RO, STATIC(&X_Axis2))},
	{66,   PM_UINT16       (PM_RO, STATIC(&Y_Axis1))},
	{67,   PM_UINT16       (PM_RO, STATIC(&Y_Axis2))},
	{68,   PM_UINT16       (PM_RO, STATIC(&Z_Axis1))},
	{69,   PM_UINT16       (PM_RO, STATIC(&Z_Axis2))},
	{70,   PM_UINT16       (PM_RO, STATIC(&SwitchTopL1))},
	{71,   PM_UINT16       (PM_RO, STATIC(&SwitchTopL2))},
	{72,   PM_UINT16       (PM_RO, STATIC(&SwitchTopR1))},
	{73,   PM_UINT16       (PM_RO, STATIC(&SwitchTopR2))},
	{74,   PM_UINT16       (PM_RO, STATIC(&SwitchFrontTop1))},
	{75,   PM_UINT16       (PM_RO, STATIC(&SwitchFrontTop2))},
	{76,   PM_UINT16       (PM_RO, STATIC(&SwitchFrontBottom1))},
	{77,   PM_UINT16       (PM_RO, STATIC(&SwitchFrontBottom2))},
	{78,   PM_UINT8        (PM_RO, STATIC(&TempTop1))},
	{79,   PM_UINT8        (PM_RO, STATIC(&TempTop2))},
	{80,   PM_UINT8        (PM_RO, STATIC(&TempBase1))},
	{81,   PM_UINT8        (PM_RO, STATIC(&TempBase2))},
	{82,   PM_UINT8        (PM_RO, STATIC(&LRA_State))},
	{90, {sizeof(V_InfoList), 0x00, {(void*)V_InfoList}},},

	/* data storage and block parameters */
	PM_DSINDEX
	/* Test indexes which are used for certification */
	PM_CONFIG1
	PM_CONFIG2
	PM_CONFIG7

	/* bulk access must be the last entry because it contains zeros as index */
	DS_BULKACCESS /* start index for bulk access */

	/* end of list */
	PM_ENDMARKER
};



/*     End Definition of global variables
 **
 ****************************************************************************/

/****************************************************************************
 **
 **     Function Implementation
 */

/* this function is called at the end of the io-link communication */
void AL_DoProcessing(void) {
#if 0
	if(RESULT_SUCCESS == AL_SetInputReq(PD, 1))
	PD[0]++;
	else
	PD[1]++;
#endif
}

void AL_AbortInd(void) {
}

#if (USE_CHANGEABLE_REVISIONID == 1)
TUserParam* AL_CheckDeviceIdent(TUnsigned8 RevisionID)
{
	if (RevisionID == 0x10) {
		return &Params10;
	}

	return &Params11;
}
#endif

/*! @} */
/*     End Function Implementation
 **
 ****************************************************************************/
void IolInitSigi(void) {  // This routine substitutes the original IolInit from TMG as it produces no compiler warnings and avoids race conditions
	STACK_InitSigi (&Params11); // initialize stack //initRunSigi
	PM_Init(VariableCollection, &ParSet); // initialize parameter manager //initRunSigi
	STACK_StartSigi(COM2); // start stack and device  //initRunSigi
//	IE |= IE_EX0__ENABLED; ///this bas to be done later, after all initialization tasks are finished
}

void IolInit(void) {
	STACK_Init (&Params11); // initialize stack
	PM_Init(VariableCollection, &ParSet); // initialize parameter manager
	STACK_Start(COM2); // start stack and device
}

void IolRun(void) {
	TUnsigned8 Status = STACK_Run(); /* call stack cyclically, obtaining status */

	switch (Status) {
	case STACK_STATUS_SIO:
		break;
	case STACK_STATUS_PREOPERATE:
		break;
	case STACK_STATUS_OPERATE:
		break;
	case STACK_STATUS_STARTUP:
		break;
	default:
		break;
	}

	/* event generation */
#if 0 // just for explanation
	ED_EventAppear(ED_ID_USER_START);
	ED_EventDisappear(ED_ID_USER_START);
#endif

}

/* this functions is called on system command factory reset */
void AL_FactoryReset(TParameterSet *pParSet) {
//	/* set the default values for the parameter set */
//  memset(pParSet, 0, sizeof(TParameterSet));  /* clear parameter set and tag */
//  memset(pParSet->AppSpecificTag, ' ', sizeof(pParSet->AppSpecificTag));
//  pParSet->Temp.Upper = 64; /* initialize the upper and lower limits */
//  pParSet->Temp.Lower = 58;
//	#if USE_ASYNCHRONOUS_STORE == 1
//  bFactoryReset = TRUE;
//	#endif

	memset(pParSet, 0x00, sizeof(TParameterSet));

}

/* this function is called when the parameter manager has received new data and
 * is asking for the consistency of the different parameters. The application must
 * check the dependencies of the parameters
 */
TUnsigned16 AL_ConfirmConsistency(TParameterSet *pParSet, TUnsigned16 Index) {
//	   Index == 0 means that the complete parset has been written
//		 Index > 0 means a single parameter */
	if (Index)
	{
		if ((pParSet->AppSpecificTag))
		{
			return RESULT_OK;
		}
	}
	return RESULT_OK;
}

void AL_StoreParSet(TUnsigned16 Length, TUnsigned8 *pData) {
	/* if there is memory to store, store the parameter set */
	FramError = FramWrite(I2C_Channel_Satellite, BASE_ADDR_FRAM, Length, START_IOLINK_FRAM, pData);
}

void AL_RestoreParSet(TUnsigned16 Length, TUnsigned8 *pData) {
	/* read the memory from the eeprom. it can be done synchronous, because this is only
	 called at startup	*/
	FramError = FramRead(I2C_Channel_Satellite, BASE_ADDR_FRAM, Length, START_IOLINK_FRAM, pData);  //initRunSigi

}

void AL_NewOutputInd(TBoolean PDValid)
{
	  if (PDValid){}; // to prevent compiler warning
	AL_GetOutputReq(EABufferOut); // hier werden die LED-Werte und der Wert für das haptische Feedback in EABufferOut kopiert
	// hier die Daten aus EABufferOut weiterverabeiten/wegkopieren


	// Testmode Section has highest priority
	if (RestartFirmware)
	{
		if (!(EABufferOut[3] & SEGMENT_TESTMODE_RESTART))
		{
			//if restart flag was set before and is disappearing now, then we Restart the firmware
			// by entering an never ending loop that activates the watchdog
			if (RestartFirmware)
			{
				#if(WATCHDOG_ACTIVE == 1)
				while(1); // Test Watchdog
				#endif
			}
		}
	}
	else
	{
		if (EABufferOut[3] & SEGMENT_TESTMODE_RESTART)
		{
			if (EABufferOut[3] & SEGMENT_LED_CMD)
			{
				if (LastLib != EABufferOut[3] & (SEGMENT_LED_SECTOR_1 | SEGMENT_LED_SECTOR_2 | SEGMENT_LED_SECTOR_3))
				{
					LastLib = EABufferOut[3] & 0x07;
			  		DataToWrite[0] = VibraREG03;
			        DataToWrite[1] = LastLib; // Select the LRA-Library
			        VibraError = AccessI2C(I2C_Channel_Satellite, BASE_ADDR_VIBRA, 2, DataToWrite, 0, DataToRead, I2C_MODE_MULTIPLE_START);
				}
			}
			else
			{
				InternalFault = true; // Indicate armed Restart-Function to exterior
				RestartFirmware = true; //Prepare for Restart of Firmware
				TestMode = true;
			}
		}
	}

	// process Vibra-Patterns[0..7] from Bank stored in EABufferOut[4] to EABufferOut[7]
	SetVibraHighBank = (EABufferOut[3] & SEGMENT_VIBRA_HIGH_BANK);
	if (SetVibraHighBank)	// Engine provides a buffer for 8 patterns but we can access only four at once from IO-Link protocol
	{
		VibraBuffer[4] = EABufferOut[4];
		VibraBuffer[5] = EABufferOut[5];
		VibraBuffer[6] = EABufferOut[6];
		VibraBuffer[7] = EABufferOut[7];
	}
	else
	{
		VibraBuffer[0] = EABufferOut[4];
		VibraBuffer[1] = EABufferOut[5];
		VibraBuffer[2] = EABufferOut[6];
		VibraBuffer[3] = EABufferOut[7];
	}

	// Haptic Vibra Pattern Control Section
	// Replay is implemented as toggle bit
	VibraReplay = (EABufferOut[3] & SEGMENT_VIBRA_REPLAY);
	if ((LastVibraReplay != VibraReplay) || (LastVibra != VibraBuffer[0]))
	{
		LastVibraReplay = VibraReplay; //every transition of the toggle-bit executes the vibra-patterns again
		LastVibra =  VibraBuffer[0];
		VibraError = LraVibraSetPattern(I2C_Channel_Satellite, BASE_ADDR_VIBRA, VibraBuffer);
	}

	// Visual LED Command Section
	Holdoff = (EABufferOut[3] & SEGMENT_HOLDOFF);
	if (!Holdoff)
	{
		LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, EABufferOut[0], EABufferOut[1], EABufferOut[2], (EABufferOut[3] & SEGMENT_LED_CMD));
	}
}

void ByteToStr(uint8_t Value, uint8_t *HiNibbleChar, uint8_t *LoNibbleChar)
{
	uint8_t Nibble;
	Nibble = Value & 0x0f;
	*LoNibbleChar = (Nibble > 9) ? Nibble + 0x37 : Nibble + 0x30;
	Nibble = Value>>4;
	*HiNibbleChar = (Nibble > 9) ? Nibble + 0x37 : Nibble + 0x30;
}

void SprintForSerial(void)
{
	ByteToStr(TestMeterXY.ID3.s8[0], &SerialNr[0], &SerialNr[1]);
	ByteToStr(TestMeterXY.ID3.s8[1], &SerialNr[2], &SerialNr[3]);
	SerialNr[4] = '-';
	ByteToStr(TestMeterXY.ID2.s8[0], &SerialNr[5], &SerialNr[6]);
	ByteToStr(TestMeterXY.ID2.s8[1], &SerialNr[7], &SerialNr[8]);
	SerialNr[9] = '-';
	ByteToStr(TestMeterXY.ID1.s8[0], &SerialNr[10], &SerialNr[11]);
	ByteToStr(TestMeterXY.ID1.s8[1], &SerialNr[12], &SerialNr[13]);
	SerialNr[14] = 0x00;
}



uint16_t CalibrationSettingsFRAM(bool DirectionToPersistentMemory) {
	uint8_t FirmwareCompare;
	uint16_t SettingRelease;
	uint16_t CurrentSetting;
	uint16_t Crc16 = 0;
	uint16_t CrcCompare = 0;
	uint16_t CheckValue = 0xffff;
	FirmwareCompare = FIRMWARE_REV;

	SettingRelease = SETTING_VALID;

	CurrentSetting = START_SETTINGS_FRAM;


	CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting, (uint8_t*) &SettingRelease,	sizeof(SettingRelease), DirectionToPersistentMemory, &Crc16);
	if (SettingRelease == SETTING_VALID) {
		CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,	(uint8_t*) &FirmwareCompare, sizeof(FirmwareCompare), DirectionToPersistentMemory, &Crc16);

		if (FirmwareCompare == FIRMWARE_REV) {
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) SerialNr, sizeof(SerialNr),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &TestMeterXY.xAxis, sizeof(TestMeterXY.xAxis),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &TestMeterXY.yAxis, sizeof(TestMeterXY.yAxis),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AngleMin, sizeof(AngleMin),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AngleMax, sizeof(AngleMax),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AngleMid, sizeof(AngleMid),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &TempBase1, sizeof(TempBase1),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &TempTop1, sizeof(TempTop1),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &TestSensorXY.xAxis, sizeof(TestSensorXY.xAxis),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &TestSensorXY.yAxis, sizeof(TestSensorXY.yAxis),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AngleMinSensor, sizeof(AngleMinSensor),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AngleMaxSensor, sizeof(AngleMaxSensor),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AngleMidSensor, sizeof(AngleMidSensor),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &TempBase2, sizeof(TempBase2),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &TempTop2, sizeof(TempTop2),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AdcMaxBottom, sizeof(AdcMaxBottom),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AdcMinBottom, sizeof(AdcMinBottom),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AdcMaxTop, sizeof(AdcMaxTop),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AdcMinTop, sizeof(AdcMinTop),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AdcMaxLeft, sizeof(AdcMaxLeft),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AdcMinLeft, sizeof(AdcMinLeft),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AdcMaxRight, sizeof(AdcMaxRight),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &AdcMinRight, sizeof(AdcMinRight),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &KeyL_Reverse, sizeof(KeyL_Reverse),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &KeyR_Reverse, sizeof(KeyR_Reverse),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &KeyT_Reverse, sizeof(KeyT_Reverse),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &KeyB_Reverse, sizeof(KeyB_Reverse),DirectionToPersistentMemory, &Crc16);
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &Crc16, sizeof(Crc16),DirectionToPersistentMemory, &Crc16);
			if (DirectionToPersistentMemory)
				CrcCompare = Crc16;
			else
				CheckValue = Crc16;
			CurrentSetting = BiStoreSettingFRAM(I2C_Channel_Satellite, BASE_ADDR_FRAM, CurrentSetting,(uint8_t*) &CrcCompare, sizeof(CrcCompare),DirectionToPersistentMemory, &Crc16);
			if (Crc16 == 0 )
				CrcError = !(CrcCompare == CheckValue);
			else
				CrcError = true;
			if (CurrentSetting == START_SETTINGS_FRAM)
				return (SETTING_INVALID);
			if (CurrentSetting >=  (START_SETTINGS_FRAM + SIZE_APP_FRAM))
				return (SETTING_INVALID);
			return (SETTING_VALID);
		}
	} else
		SettingRelease = SETTING_VALID;	// Restore the Value overwritten by accident
	return (SETTING_INVALID);
}

void SampleButtons(bool CalibPhase)
{
#define SWITCH_HYSTERESIS (14000)
#define SWITCH_LEVEL_ (35313)
#define SWITCH_LEVEL_TOWARD_CLOSED (21504)
#define SWITCH_LEVEL_TOWARD_OPEN (51883)
#define ERROR_LO (9930)
#define ERROR_HI (58151)
	if (AdcConvert(I2C_Channel_Satellite, BASE_ADDR_ADC1, &AdcData1) == I2C_PRESENT)
	{
		ButtonLeftError = false;
		ButtonRightError = false;
		if (AdcData1.Channel2.u16 < ERROR_LO)	ButtonLeftError = true;
		if (AdcData1.Channel2.u16 > ERROR_HI)	ButtonLeftError = true;
		if (AdcData1.Channel3.u16 < ERROR_LO)	ButtonRightError = true;
		if (AdcData1.Channel3.u16 > ERROR_HI)	ButtonRightError = true;
		if (!(ButtonLeftError || ButtonRightError))
		{
			ButtonLeft = (ButtonLeft && (AdcData1.Channel2.u16 > SWITCH_LEVEL_TOWARD_OPEN))? false : (!ButtonLeft && (AdcData1.Channel2.u16 < SWITCH_LEVEL_TOWARD_CLOSED)? true : ButtonLeft);
			ButtonRight = (ButtonRight && (AdcData1.Channel3.u16 > SWITCH_LEVEL_TOWARD_OPEN))? false : (!ButtonRight && (AdcData1.Channel3.u16 < SWITCH_LEVEL_TOWARD_CLOSED)? true : ButtonRight);
		}

    	KeyDataL = UnsignedFilter( AdcData1.Channel0.u16, &FilterCartridgeKeyL);
    	KeyDataR = UnsignedFilter( AdcData1.Channel1.u16, &FilterCartridgeKeyR);
    	if (CalibPhase && UnsignedFilterInitialized(&FilterCartridgeKeyL))
    	{
    		AdcMinLeft = (KeyDataL < AdcMinLeft) ? KeyDataL : AdcMinLeft;
    		AdcMaxLeft = (KeyDataL > AdcMaxLeft) ? KeyDataL : AdcMaxLeft;
         	AdcMinRight = (KeyDataR < AdcMinRight) ? KeyDataR : AdcMinRight;
        	AdcMaxRight = (KeyDataR > AdcMaxRight) ? KeyDataR : AdcMaxRight;
    		if (ButtonLeft && (KeyDataL == AdcMinLeft))
    		{
    			KeyL_Reverse = 0x00;
    		}
    		if (ButtonLeft && (KeyDataL == AdcMaxLeft))
    		{
    			KeyL_Reverse = 0xff;
    		}

    		if (ButtonRight && (KeyDataR == AdcMinRight))
    		{
    			KeyR_Reverse = 0x00;
    		}
    		if (ButtonRight && (KeyDataR == AdcMaxRight))
    		{
    			KeyR_Reverse = 0xff;
    		}
    	}
 	}
	else
	{
		ButtonLeftError = true;
		ButtonRightError = true;
	}

	if (AdcConvert(I2C_Channel_Satellite, BASE_ADDR_ADC2, &AdcData2) == I2C_PRESENT)
	{
		ButtonFrontError = false;
		if (AdcData2.Channel2.u16 < ERROR_LO)	ButtonFrontError = true;
		if (AdcData2.Channel2.u16 > ERROR_HI)	ButtonFrontError = true;
		if (AdcData2.Channel3.u16 < ERROR_LO)	ButtonFrontError = true;
		if (AdcData2.Channel3.u16 > ERROR_HI)	ButtonFrontError = true;
		if (!ButtonFrontError)
		{
			ButtonFront1 = (ButtonFront1 && (AdcData2.Channel2.u16 > SWITCH_LEVEL_TOWARD_OPEN))? false : (!ButtonFront1 && (AdcData2.Channel2.u16 < SWITCH_LEVEL_TOWARD_CLOSED)? true : ButtonFront1);
			ButtonFront2 = (ButtonFront2 && (AdcData2.Channel3.u16 > SWITCH_LEVEL_TOWARD_OPEN))? false : (!ButtonFront2 && (AdcData2.Channel3.u16 < SWITCH_LEVEL_TOWARD_CLOSED)? true : ButtonFront2);
			ButtonFront = (ButtonFront1 || ButtonFront2);
		}
    	KeyDataT = UnsignedFilter( AdcData2.Channel0.u16, &FilterCartridgeKeyT);
    	KeyDataB = UnsignedFilter( AdcData2.Channel1.u16, &FilterCartridgeKeyB);
    	if (CalibPhase && UnsignedFilterInitialized(&FilterCartridgeKeyT))
     	{
    		AdcMinTop = (KeyDataT < AdcMinTop) ? KeyDataT : AdcMinTop;
    		AdcMaxTop = (KeyDataT > AdcMaxTop) ? KeyDataT : AdcMaxTop;
    		AdcMinBottom = (KeyDataB < AdcMinBottom) ? KeyDataB : AdcMinBottom;
    		AdcMaxBottom = (KeyDataB > AdcMaxBottom) ? KeyDataB : AdcMaxBottom;
    		if (ButtonFront1 && (KeyDataT == AdcMinTop))
    		{
    			KeyT_Reverse = 0x00;
    		}
    		if (ButtonFront1 && (KeyDataT == AdcMaxTop))
    		{
    			KeyT_Reverse = 0xff;
    		}

    		if (ButtonFront2 && (KeyDataB == AdcMinBottom))
    		{
    			KeyB_Reverse = 0x00;
    		}
    		if (ButtonFront2 && (KeyDataB == AdcMaxBottom))
    		{
    			KeyB_Reverse = 0xff;
    		}
     	}
	}
	else
	{
		ButtonFrontError = true;
	}
	ButtonError = ButtonLeftError || ButtonRightError || ButtonFrontError;
}

void FullEvalButtons(bool CalibPhase)
{
	SampleButtons(CalibPhase);
	FrontButtonState =	(FrontButtonState == PB_ABSENT) ? (ButtonFront) ? PB_NEW_EVENT: PB_ABSENT : (ButtonFront) ? PB_PRESENT : PB_ABSENT;
	LeftButtonState =	(LeftButtonState == PB_ABSENT) ? (ButtonLeft) ? PB_NEW_EVENT: PB_ABSENT : (ButtonLeft) ? PB_PRESENT : PB_ABSENT;
	RightButtonState =	(RightButtonState == PB_ABSENT) ? (ButtonRight) ? PB_NEW_EVENT: PB_ABSENT : (ButtonRight) ? PB_PRESENT : PB_ABSENT;

	FrontButton1State =	(FrontButton1State == PB_ABSENT) ? (ButtonFront1) ? PB_NEW_EVENT: PB_ABSENT : (ButtonFront1) ? PB_PRESENT : PB_ABSENT;
	FrontButton2State =	(FrontButton2State == PB_ABSENT) ? (ButtonFront2) ? PB_NEW_EVENT: PB_ABSENT : (ButtonFront2) ? PB_PRESENT : PB_ABSENT;
	BootButtonAtStartup = (ButtonFront || ButtonLeft || ButtonRight || ButtonError || !C_BUS) ? true : false;
	AllButtonsAtStartup = (ButtonFront && ButtonLeft && ButtonRight && !ButtonError) ? true : false;

}

uint8_t UpdateAbstractKeyState(uint8_t *AbstractButtonState, bool Button) {
	SampleButtons(false);
	*AbstractButtonState =
			(*AbstractButtonState == PB_ABSENT) ? (Button) ? PB_NEW_EVENT: PB_ABSENT : (Button) ? PB_PRESENT : PB_ABSENT;
	return (*AbstractButtonState);
}

void CheckForUpdate(bool DoIt) {
	if (DoIt)
	{
		// Start the USB-Bootloader
		// Write R0 and issue a software reset
		SFRPAGE = 0x00;
		*((uint8_t*) 0x00) = 0xA5;
		RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET;
	}
}

void WDT0_delay(uint32_t systemClkFreq, uint32_t wdtClkFreq, uint32_t wdtClkCycles) {
  uint32_t systemClkCycles, i;

  // Calculate how many system clock cycles to wait. This value is divided by 3 corresponding to the minimum cycles required to execute the wait loop
  systemClkCycles = (wdtClkCycles * systemClkFreq) / (wdtClkFreq * 3);

  // Wait loop

  for(i = 0; i < systemClkCycles; i++) {
    NOP();
  }
}

//-----------------------------------------------------------------------------
// SiLabs_Startup() Routine
// ----------------------------------------------------------------------------
// This function is called immediately after reset, before the initialization
// code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
// useful place to disable the watchdog timer, which is enable by default
// and may trigger before main() in some instances.
//-----------------------------------------------------------------------------
void SiLabs_Startup(void) {
	// $[SiLabs Startup]
	// [SiLabs Startup]$
	   // Disable watchdog with key sequence
	   SFRPAGE = 0x00;
	   WDTCN = 0xDE; //First key
	   WDTCN = 0xAD; //Second key
	   // Prepare for using (unused) USB-FIFO as normal XRAM (Init USB-Clock Domain)	
	   SFRPAGE = 0x10;
	   PFE0CN = PFE0CN_FLRT__SYSCLK_BELOW_50_MHZ;
	   SFRPAGE = 0x00;
	   CLKSEL = CLKSEL_CLKSL__HFOSC0 | CLKSEL_CLKDIV__SYSCLK_DIV_1;
	   while ((CLKSEL & CLKSEL_DIVRDY__BMASK) == CLKSEL_DIVRDY__NOT_READY)
	   CLKSEL = CLKSEL_CLKSL__HFOSC1 | CLKSEL_CLKDIV__SYSCLK_DIV_1;
	   while ((CLKSEL & CLKSEL_DIVRDY__BMASK) == CLKSEL_DIVRDY__NOT_READY);
	   SFRPAGE = 0x20;
	   USB0CF = USB0CF & ~0x07;
	   SFRPAGE = 0x00;
}

void fMulStretch(float *y, float const *factor)
{
	*y *= *factor;
}
void StretchXY(const float * const factor)
{
	fMulStretch(&TestMeterXY.xAxis.SlopeNeg, factor);
	fMulStretch(&TestMeterXY.yAxis.SlopeNeg, factor);
	fMulStretch(&TestMeterXY.xAxis.SlopePos, factor);
	fMulStretch(&TestMeterXY.yAxis.SlopePos, factor);
	fMulStretch(&TestSensorXY.xAxis.SlopeNeg, factor);
	fMulStretch(&TestSensorXY.yAxis.SlopeNeg, factor);
	fMulStretch(&TestSensorXY.xAxis.SlopePos, factor);
	fMulStretch(&TestSensorXY.yAxis.SlopePos, factor);
}

void Init3d(void)
{
    ClearAxisStuct(&TestMeterXY.xAxis); // axis unused
    ClearAxisStuct(&TestMeterXY.yAxis); // axis unused
    ClearAxisStuct(&TestMeterXY.zAxis); // axis unused
	TestMeterXY.BusAddress = BASE_ADDR_A0Hi_A1Lo; // Base Joystick Magnetometer
	TestMeterXY.Gain = VALUE_GAIN_7;  //Original 7
	TestMeterXY.Filter = VALUE_FILTER_5;
	TestMeterXY.Resolution = VALUE_RES_1; //  3
	TestMeterXY.OSR = VALUE_OSR_0;
	TestMeterXY.OperationMode = METER_SB_ZXYT;
	TestMeterXY.ReadMode = REGISTER_80;
	TestMeterXY.AutoTempCompensation = VALUE_TCMP_EN_1;
	TestMeterXY.retry = 0x00;
	TestMeterXY.BusChannel = I2C_Channel_Base;

    ClearAxisStuct(&TestMeterZ.xAxis); // axis unused
    ClearAxisStuct(&TestMeterZ.yAxis); // axis unused
    ClearAxisStuct(&TestMeterZ.zAxis); // axis unused
	TestMeterZ.BusAddress = BASE_ADDR_A0Lo_A1Hi; // 3th Axis Magnetometer
	TestMeterZ.Gain = VALUE_GAIN_0; // was 0 then 7
	TestMeterZ.Filter = VALUE_FILTER_5;
	TestMeterZ.Resolution = VALUE_RES_0; //  was 1
	TestMeterZ.OSR = VALUE_OSR_0;
	TestMeterZ.OperationMode = METER_SB_ZXYT;
	TestMeterZ.ReadMode = REGISTER_80;
	TestMeterZ.AutoTempCompensation = VALUE_TCMP_EN_1;
	TestMeterZ.retry = 0x00;
	TestMeterZ.BusChannel = I2C_Channel_Satellite;

    ClearAxisStuct(&TestSensorZ.xAxis); // axis unused
    ClearAxisStuct(&TestSensorZ.yAxis); // axis unused
    ClearAxisStuct(&TestSensorZ.zAxis); // axis unused
    TestSensorZ.BusAddress = HALLSENSOR1_ADDR; //  Base PCB 100700
	TestSensorZ.BusChannel = I2C_Channel_Satellite; // Satellite PCB 100634
	TestSensorZ.retry = 0x00;
	TestSensorZ.FirstRun = true; // Init Routine will reset this after successful initialization
	TestSensorZ.Mode = TLE493D_ModeMasterControlled; // not speed optimized but rugged and with less power consumption
	TestSensorZ.InterruptDisabled = true; // as we have other circuits on the same bus, we keep it simple and save this way
    TestSensorZ.LowPowerPeriod = TLE493D_Rate97Hz; // Don't care in Master Controlled Mode
	TestSensorZ.DisableTemperature = false;
	TestSensorZ.Angular = false;
	TestSensorZ.TriggerOption = TLE493D_TriggerAfterRef05; // Don't care in Protocol type TwoByteRead
	TestSensorZ.ShortRangeSensitivity = false;
	TestSensorZ.TempCompensation = TLE493D_TempCompP350;
	TestSensorZ.CollisionAvoidanceDisabled = false; // slow but save and rugged
	TestSensorZ.ProtocolOneByteReadEnabled = false; // keep it simple (on default) as otherwise protocol has to be changed on the fly

	ClearAxisStuct(&TestSensorXY.xAxis); // axis unused
	ClearAxisStuct(&TestSensorXY.yAxis); // axis unused
	ClearAxisStuct(&TestSensorXY.zAxis); // axis unused
	TestSensorXY.BusAddress = HALLSENSOR2_ADDR; //  Base PCB 100700
	TestSensorXY.BusChannel = I2C_Channel_Base; // Satellite PCB 100634
	TestSensorXY.retry = 0x00;
	TestSensorXY.FirstRun = true; // Init Routine will reset this after successful initialization
	TestSensorXY.Mode = TLE493D_ModeMasterControlled; // not speed optimized but rugged and with less power consumption
	TestSensorXY.InterruptDisabled = true; // as we have other circuits on the same bus, we keep it simple and save this way
    TestSensorXY.LowPowerPeriod = TLE493D_Rate97Hz; // Don't care in Master Controlled Mode
	TestSensorXY.DisableTemperature = false;
	TestSensorXY.Angular = false;
	TestSensorXY.TriggerOption = TLE493D_TriggerAfterRef05; // Don't care in Protocol type TwoByteRead
	TestSensorXY.ShortRangeSensitivity = true;
	TestSensorXY.TempCompensation = TLE493D_TempCompP350;
	TestSensorXY.CollisionAvoidanceDisabled = false; // slow but save and rugged
	TestSensorXY.ProtocolOneByteReadEnabled = false; // keep it simple (on default) as otherwise protocol has to be changed on the fly
}


//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
int main(void) {

#define M_PI acos(-1.0)
#define M_2PI 2*acos(-1.0)
#define TRIGGER_CHAR ('\n')
#define	STRETCH (0.085F) // in final Version  0.291 = sqrt(0.085)

#define	TEMP_COEFF_MAGNET (0.00F) // (-0.0012F) in fact, but done by MLX90395
#define	TEMP_COEFF_AMP    (0.00F) // (-0.0044F) if fact for MLX90393, but compensated automatically by MLX90395// 

#define	RANGE (2.18F)
#define	ANGLE_LIMIT (17)
#define ANGLE_TEST_LIMIT (10)
#define FRAM_READ_ATTEMPTS (5)
#define HALL_INIT_ATTEMPTS (10)
#define Z_RADIUS_MAX_LIMIT (28500.0F)

#define LO_LIMIT_Z_METER (2000)
#define HI_LIMIT_Z_METER (32767)
#define LO_LIMIT_Z_SENSOR (2000)
#define HI_LIMIT_Z_SENSOR (32767)
#define LO_LIMIT_Z_METER_FLIP ( -32768)
#define HI_LIMIT_Z_METER_FLIP ( -2000)
#define LO_LIMIT_Z_SENSOR_FLIP ( -32768)
#define HI_LIMIT_Z_SENSOR_FLIP ( -2000)

#define HIGH_TEMP_LIMIT (85)
#define LOW_TEMP_LIMIT (-20)

	uint8_t VibraDiag = 0;
	uint8_t xdata SwitchDirection = 0;
	uint8_t Red =1, Green = 1, Blue = 1;
	uint8_t Smooth;
	uint8_t DeflectionVector1;
	uint8_t DeflectionVector2;
	bool MagnetSensorRegular = false;
	bool MagnetSensorGoofy = false;
	bool MagnetMeterRegular = false;
	bool MagnetMeterGoofy = false;
    bool Calibration = false;
	bool StartBootloader = false;
    bool NormalOperation = false;
    bool ControlTest = false;
	bool LinkTestOnly = false;
	bool RawdataBaseTest = false;
	bool UsbPower = false;
    bool ExternalBusNotOk = true;
    bool InternalBusNotOk = true;
    bool CalibValid = false;
    float Range = RANGE;
	float StretchCoeff = STRETCH;
	int16_t AngleDegLimit = ANGLE_LIMIT;
	int16_t CorrectorXY = 0;
	int16_t NewAxisValue = 0;
	bool CenterStep = true;
	uint8_t CalibCounter = 0;
	int16_t TestCounter= 0;
	uint8_t RetryInit = 0;
	uint8_t LED_Values[9];
	uint16_t posX, negX, posY, negY;
	uint8_t j = 0;
	uint8_t status_HMT7742;
#if(WATCHDOG_ACTIVE == 1)
    // Check to see if the WDT was the last reset cause
    if (RSTSRC & RSTSRC_WDTRSF__SET)
    {
      WdtReset = true;
    }
    // Feed WDT timer before disabling (Erratum WDT_E102)
    WDTCN = 0xA5;
    // Watchdog is active now and will hit in 13s
#endif


	// Bootloader Activation if USB-Powered
	P0MDOUT = P0MDOUT_B2__PUSH_PULL | P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__PUSH_PULL;
	XBR2 += XBR2_XBARE__ENABLED;

	VCC_M = 1; // Activate Power for Base Magnetsensor

	NCS_IO = 1;
	LoopDelay150ms();


	NCS_IO = 0;
	status_HMT7742 = OneByteSPI(REG_CFG + REG_WRITE_BIT, false);
	OneByteSPI((CFG_UVT_7_2V + CFG_BD_38400 + CFG_RF_ABS + CFG_S5V_SS), false);
	NCS_IO = 1;

	NCS_IO = 0;
	status_HMT7742 = OneByteSPI(REG_LED + REG_WRITE_BIT, false);
	OneByteSPI((LED_LEVEL_3_GREEN + LED_LEVEL_3_RED), false); // Yellow Led during Power-up
	NCS_IO = 1;

	LoopDelay150ms();

	NCS_IO = 0;
	status_HMT7742 = OneByteSPI(REG_LED + REG_WRITE_BIT, false);
	if ((status_HMT7742 & STATUS_UV) == false) // if we are not powered from USB but from IO-Link
	{
		OneByteSPI(LED_LEVEL_1_GREEN + LED_LEVEL_OFF_RED, false); // green LED on bottom
		NCS_IO = 1;
		UsbPower = false;
	}
	else
	{
		//Undervoltage -> Enter USB-Bootloader as we assume USB-Connection
		OneByteSPI(LED_LEVEL_OFF_GREEN + LED_LEVEL_7_RED, false); // red LED on bottom
		NCS_IO = 1;
		UsbPower = true;
	}

#if INIT_SIGI == 1
	IolInitSigi();
#else
	IolInit();
#endif

	// Call hardware initialization routine
	enter_DefaultMode_from_RESET();
	VCC_M = 1; // Activate Power for Base Magnetsensor

	ResetHallSensors(I2C_Channel_Satellite);

	if (UsbPower)
		LedError = LedInitSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 50, 50, 50); //Set Max Current to 5mA per Color (Adequate for ROHM RGB-LED)
	else
		LedError = LedInitSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 100, 100, 100); //Set Max Current to 10mA per Color (Adequate for ROHM RGB-LED)
    if (WdtReset)
    {
    	LedError = true; // indicate that LED may be out of sync with application after Watchdog-Reset
    }
	AdcTopError = AdcConvert(I2C_Channel_Satellite, BASE_ADDR_ADC1, &AdcData1) ? true : AdcTopError;
	AdcFrontError = AdcConvert(I2C_Channel_Satellite, BASE_ADDR_ADC2, &AdcData2) ? true : AdcFrontError;

	FullEvalButtons(false);
	FullEvalButtons(false);
	FullEvalButtons(false);

	Init3d(); // Initalize all Parameters of Infineon and Melexis 3D Hall Sensors;
	// check presence of the four 3D-Hall chips;
	MeterXYError = InitMeterI2C(&TestMeterXY);
	MeterZError = InitMeterI2C(&TestMeterZ);
	SensorXYError = InitHallSensor(&TestSensorXY);
	SensorZError = InitHallSensor(&TestSensorZ);

	CalibValid = false;
	RetryInit = 0;
	do // Make sure FRAM is valid but keep a reasonable number of retries in mind as this prevents the unit form entering recalibration by accident
	{  // This should not happen by accident if some interferences are around during startup, as it blocks the whole machine
		RetryInit++;
		CalibValid = (CalibrationSettingsFRAM(READ_SETTINGS_FROM_FLASH) == SETTING_VALID) ? true : false;
	} while ((CrcError || (CalibValid == false)) && (RetryInit < FRAM_READ_ATTEMPTS));

	ExternalBusNotOk = (SensorZError && MeterZError && LedError && AdcTopError && AdcFrontError && !CalibValid); // show raw data instead

	InternalBusNotOk = (MeterXYError && SensorXYError);

	if (!UsbPower)
	{
		if (ExternalBusNotOk)
		{
			LinkTestOnly = InternalBusNotOk; // When IO-Link Powered but I2C-circuits are missed in general, we enter the diagnostic Link Test mode
		    RawdataBaseTest = !LinkTestOnly;// When IO-Link Powered but circuits in Grip are not connected, we provide raw data in diagnostic Link Test mode
		}
		else
		{
			FramError = FramRead(I2C_Channel_Satellite, BASE_ADDR_FRAM, sizeof(AllowReCalibrationTag), START_SCRATCHPAD_FRAM, &AllowReCalibrationTag);
			if (!FramError && (AllowReCalibrationTag == CALIB_COMPATIBILITY_VERSION))
			{
				AllowReCalibrationTag = ~CALIB_COMPATIBILITY_VERSION;
				FramError = FramWrite(I2C_Channel_Satellite, BASE_ADDR_FRAM, sizeof(AllowReCalibrationTag), START_SCRATCHPAD_FRAM, &AllowReCalibrationTag);
				// When recalibration was entered in USB-Mode before but not finished, then entering recalibration is possible the first time in IO-Link-Mode, given all threee buttons Pressed at startup
				CalibValid = (AllButtonsAtStartup) ? false : CalibValid;
			}

			NormalOperation = CalibValid; // When IO-Link Powered and calibration is Valid and everything is fine we enter normal operation
			Calibration = !NormalOperation; // When IO-Link Powered and no valid calibration found but everything else is fine, we enter calibration
		}
	}
	else
	{
		if (!ExternalBusNotOk)
		{
			if (!CalibValid || AllButtonsAtStartup)
			{
			    Calibration = true; // When USB-connected and everything works fine but there is no valid calib found or there are all buttons pressed, we enter calibration
			    if (AllButtonsAtStartup)
			    {
			    	AllowReCalibrationTag = CALIB_COMPATIBILITY_VERSION;
			    	FramError = FramWrite(I2C_Channel_Satellite, BASE_ADDR_FRAM, sizeof(AllowReCalibrationTag), START_SCRATCHPAD_FRAM, &AllowReCalibrationTag);
			    }
			}
			if (!AllButtonsAtStartup)
			{
				StartBootloader = BootButtonAtStartup; // When USB-Powered and something is not connected or one button is pressed at startup, we enter bootloader mode
			    ControlTest = !StartBootloader && CalibValid; // When USB-connected and everything works fine and there is no button pressed, and calib is valid, we enter the demonstration mode
			}
		}
		else
		{
			StartBootloader = true; // When USB-Powered and something is not connected or one button is pressed at startup, we enter bootloader mode
		}

	}


#if DIAGNOSTIC == 1
	if(NormalOperation){Red = 0; Blue = 0; Green = 255;} // green
	if(Calibration){Red = 0; Blue = 255; Green = 0;}     // blue
	if(StartBootloader){Red = 255; Blue = 0; Green = 0;} // red
	if(ControlTest){Red = 85; Blue = 85; Green = 85;} // white
	if(LinkTestOnly){Red = 128; Blue = 0; Green = 128;}  // yellow
	if(RawdataBaseTest){Red = 170; Blue = 0; Green = 85;}  // orange

	LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, Red, Green, Blue, LedSegment_All);
	StartBootloader = true;
#endif

	// Now we know which Mode we have to enter
	CheckForUpdate(StartBootloader);

	if (Calibration) // Init only in calibration, otherwise values come from vaild Calibration and should not be overwritten!
	  Init3d(); // Initialize all Register-Parameters of Infineon and Melexis 3D Hall Sensors before calibration again, as they may be overwritten with obsolete/Invalid Data from FRAM-read attempts before;

    if (!LinkTestOnly)
    {
    	RetryInit = 0;
    	do
    	{
    	   	MeterXYError = InitMeterI2C(&TestMeterXY); // Init Melexis Magnetsensor
 			SensorXYError = InitHallSensor(&TestSensorXY);
	    	if (RawdataBaseTest)
	    	{
	    		SprintForSerial();
	    	}
	    	else
	    	{
	    		if (MeterZError)
	    			MeterZError = InitMeterI2C(&TestMeterZ); // Init Melexis Magnetsensor
	    		if (SensorZError)
	    			SensorZError = InitHallSensor(&TestSensorZ);
	     	}
			RetryInit++;

    	} while ((MeterXYError || SensorXYError || MeterZError || SensorZError) && !RawdataBaseTest && (RetryInit < HALL_INIT_ATTEMPTS));
    }

    // Make sure Vibra is calibrated but keep a reasonable number of retries in mind as this prevents the unit form run into a watchdog reset
    if (!Calibration)
    	{
    		VibraDiag = LraVibraCalibrate(I2C_Channel_Satellite, BASE_ADDR_VIBRA);
    		LraVibraClearColdStartState();
    		VibraError = VibraDiag;
    		VibraOverCurrent = (VibraDiag & 0x01);
    		VibraOverTemp = (VibraDiag & 0x02);
    		VibraCalibTimeout = (VibraDiag & 0x08);
    	}
    else
       VibraError = LraVibraGetState(I2C_Channel_Satellite, BASE_ADDR_VIBRA, &LRA_State);

	do
	{
    	FullEvalButtons(false);
 	} while (BootButtonAtStartup && Calibration); //make sure all buttons are released before entering calibration

    memset(EABufferOut, 0xFF, sizeof(EABufferOut));
	memset(EABufferIn, 0xFF, sizeof(EABufferIn));
	memset(VibraBuffer, 0x00, sizeof(VibraBuffer));
	memset(V_InfoList, 0x00, sizeof(V_InfoList));

	AL_SetInputReq(EABufferIn, TRUE);

	LastVibra = 0;

	SignedFilterReset(&FilterCartridgeOne);
	SignedFilterReset(&FilterCartridgeTwo);

	UnsignedFilterReset(&FilterCartridgeKeyL);
	UnsignedFilterReset(&FilterCartridgeKeyR);
	UnsignedFilterReset(&FilterCartridgeKeyT);
	UnsignedFilterReset(&FilterCartridgeKeyB);

	if (!LinkTestOnly && !RawdataBaseTest)
	{

		LedError = LedSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 1, 1, 1); // Set max Current temporary to the value of 100uA
		LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 255, 255, 255, 0);

		LoopDelay3ms();
		for (j = 0; j<9; j++)
		{
			LedError = (LedGetAdc(I2C_Channel_Satellite, BASE_ADDR_ILLUM, j, &LED_Values[j])) ? true : LedError;
		}

		LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0, 0, 0, 0);
		LedError = LedSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 100, 100, 100); // Set max Current back to 10mA

		V_InfoList[2] += (LED_Values[0] > 150) ? 0x80 : 0x00; // Set Byte[2] Bit[0x80]Index 90 (BitOffset 23)	Subindex 37	Infolist-LED1 green wire open circuit bool
	    V_InfoList[2] += (LED_Values[0] < 100) ? 0x40 : 0x00; // Set Byte[2] Bit[0x40]Index 90 (BitOffset 22)	Subindex 38	Infolist-LED1 green wire short circuit bool
		V_InfoList[2] += (LED_Values[1] > 150) ? 0x20 : 0x00; // Set Byte[2] Bit[0x20]Index 90 (BitOffset 21)	Subindex 39	Infolist-LED1 blue wire open circuit bool
		V_InfoList[2] += (LED_Values[1] < 100) ? 0x10 : 0x00; // Set Byte[2] Bit[0x10]Index 90 (BitOffset 20)	Subindex 40	Infolist-LED1 blue wire short circuit bool
		V_InfoList[2] += (LED_Values[2] > 150) ? 0x02 : 0x00; // Set Byte[2] Bit[0x02]Index 90 (BitOffset 17)	Subindex 43	Infolist-LED2 green wire open circuit bool
		V_InfoList[2] += (LED_Values[2] < 100) ? 0x01 : 0x00; // Set Byte[2] Bit[0x01]Index 90 (BitOffset 16)	Subindex 44	Infolist-LED2 green wire short circuit bool
		V_InfoList[1] += (LED_Values[3] > 150) ? 0x80 : 0x00; // Set Byte[1] Bit[0x80]Index 90 (BitOffset 15)	Subindex 45	Infolist-LED2 blue wire open circuit bool
		V_InfoList[1] += (LED_Values[3] < 100) ? 0x40 : 0x00; // Set Byte[1] Bit[0x40]Index 90 (BitOffset 14)	Subindex 46	Infolist-LED2 blue wire short circuit bool
		V_InfoList[1] += (LED_Values[4] > 150) ? 0x08 : 0x00; // Set Byte[1] Bit[0x08]Index 90 (BitOffset 11)	Subindex 49	Infolist-LED3 green wire open circuit bool
		V_InfoList[1] += (LED_Values[4] < 100) ? 0x04 : 0x00; // Set Byte[1] Bit[0x04]Index 90 (BitOffset 10)	Subindex 50	Infolist-LED3 green wire short circuit bool
		V_InfoList[1] += (LED_Values[5] > 150) ? 0x02 : 0x00; // Set Byte[1] Bit[0x02]Index 90 (BitOffset 09)	Subindex 51	Infolist-LED3 blue wire open circuit bool
		V_InfoList[1] += (LED_Values[5] < 100) ? 0x01 : 0x00; // Set Byte[1] Bit[0x01]Index 90 (BitOffset 08)	Subindex 52	Infolist-LED3 blue wire short circuit bool
		V_InfoList[3] += (LED_Values[6] > 140) ? 0x02 : 0x00; // Set Byte[3] Bit[0x02]Index 90 (BitOffset 25)	Subindex 35	Infolist-LED1 red wire open circuit	bool
		V_InfoList[3] += (LED_Values[6] <  90) ? 0x01 : 0x00; // Set Byte[3] Bit[0x01]Index 90 (BitOffset 24)	Subindex 36	Infolist-LED1 red wire short circuit bool
		V_InfoList[2] += (LED_Values[7] > 140) ? 0x08 : 0x00; // Set Byte[2] Bit[0x08]Index 90 (BitOffset 19)	Subindex 41	Infolist-LED2 red wire open circuit	bool
		V_InfoList[2] += (LED_Values[7] <  90) ? 0x04 : 0x00; // Set Byte[2] Bit[0x04]Index 90 (BitOffset 18)	Subindex 42	Infolist-LED2 red wire short circuit bool
		V_InfoList[1] += (LED_Values[8] > 140) ? 0x20 : 0x00; // Set Byte[1] Bit[0x20]Index 90 (BitOffset 13)	Subindex 47	Infolist-LED3 red wire open circuit	bool
		V_InfoList[1] += (LED_Values[8] <  90) ? 0x10 : 0x00; // Set Byte[1] Bit[0x10]Index 90 (BitOffset 12)	Subindex 48	Infolist-LED3 red wire short circuit bool

		for (j = 20; j>0; j--)
		{
			MeterXYError = GetMagnetometer(&TestMeterXY);
			MeterZError = GetMagnetometer(&TestMeterZ);
			SensorXYError = GetMagnetSensor(&TestSensorXY);
			SensorZError = GetMagnetSensor(&TestSensorZ);
			LoopDelay3ms();
		}
	}

	if (Calibration)
	{ // begin calibration
		AdcMaxLeft = 0;
		AdcMinLeft = UINT_MAX;
		AdcMaxRight = 0;
		AdcMinRight = UINT_MAX;
		AdcMaxTop = 0;
		AdcMinTop = UINT_MAX;
		AdcMaxBottom = 0;
		AdcMinBottom = UINT_MAX;
		KeyL_Reverse = 0xAA;
		KeyR_Reverse = 0xAA;
		KeyT_Reverse = 0xAA;
		KeyB_Reverse = 0xAA;
		ClearAxisStuct(&TestMeterXY.xAxis);
		ClearAxisStuct(&TestMeterXY.yAxis);
		ClearAxisStuct(&TestMeterXY.zAxis);
		ClearAxisStuct(&TestMeterZ.xAxis);
		ClearAxisStuct(&TestMeterZ.yAxis);
		ClearAxisStuct(&TestMeterZ.zAxis);
		ClearAxisStuct(&TestSensorXY.xAxis);
		ClearAxisStuct(&TestSensorXY.yAxis);
		ClearAxisStuct(&TestSensorXY.zAxis);
		ClearAxisStuct(&TestSensorZ.xAxis);
		ClearAxisStuct(&TestSensorZ.yAxis);
		ClearAxisStuct(&TestSensorZ.zAxis);

		//AngleCenterSensor = INT_MAX;
		AngleMinSensor = INT_MAX;
		AngleMaxSensor = INT_MAX;

		//AngleCenter = INT_MAX;
		AngleMin = INT_MAX;
		AngleMax = INT_MAX;

		zRadiusMeter  = INT_MAX;
		zRadiusMinMeter = INT_MAX;
		zRadiusMaxMeter = 0;

		InitHallSensor(&TestSensorXY);
		InitHallSensor(&TestSensorZ);

		// CALIBRATION
		InitMeterI2C(&TestMeterXY); // Init Melexis Magnetsensor
		InitMeterI2C(&TestMeterZ); // Init Melexis Magnetsensor
		//PRESS LEVER IN CENTER POSITION TO PROCEED

		for (j = 150; j > 0; j--)
		{
			// This <warming-up-phase> is very important for good calibration results, as the filters
			// and oversampling-registers of the chips have to be prepared before
			// the obtained data can be considered to be stable.
			GetMagnetometer(&TestMeterXY);
			GetMagnetometer(&TestMeterZ);
			GetMagnetSensor(&TestSensorXY);
			GetMagnetSensor(&TestSensorZ);
			switch (j)
			{
			case 0:
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 255, 0, 0, 9);
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0, 255, 0, 10);
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0, 0, 255, 12);
				break;
			case 50:
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0, 255, 0, 9);
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0, 0, 255, 10);
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 255, 0, 0, 12);
			break;
			case 100:
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0, 0, 255, 9);
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 255, 0, 0, 10);
			    LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0, 255, 0, 12);
			break;
			}
		}

		MagnetSensorRegular = ((TestSensorXY.zAxis.RawValue.s16 >= LO_LIMIT_Z_SENSOR) && (TestSensorXY.zAxis.RawValue.s16 <= HI_LIMIT_Z_SENSOR));
		MagnetSensorGoofy = ((TestSensorXY.zAxis.RawValue.s16 >= LO_LIMIT_Z_SENSOR_FLIP) && (TestSensorXY.zAxis.RawValue.s16 <= HI_LIMIT_Z_SENSOR_FLIP));
		MagnetMeterRegular = ((TestMeterXY.zAxis.RawValue.s16 >= LO_LIMIT_Z_METER) && (TestMeterXY.zAxis.RawValue.s16 <= HI_LIMIT_Z_METER));
		MagnetMeterGoofy = ((TestMeterXY.zAxis.RawValue.s16 >= LO_LIMIT_Z_METER_FLIP) && (TestMeterXY.zAxis.RawValue.s16 <= HI_LIMIT_Z_METER_FLIP));

		if ((MagnetSensorGoofy == MagnetSensorRegular) || (MagnetMeterGoofy == MagnetMeterRegular))
		{
			LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 170, 85, 0, LedSegment_All); // yellow -> magnet field not in right range
		}
		else
		{
			if (MagnetSensorGoofy)
			{
				TestSensorXY.xAxis.SwapSign = MagnetSensorGoofy; // calibrate all axis with changed signs from now on. there
				TestSensorXY.yAxis.SwapSign = MagnetSensorGoofy; // is no way back
			}
			if (MagnetMeterGoofy)
			{
				TestMeterXY.xAxis.SwapSign = MagnetMeterGoofy; // calibrate all axis with changed signs from now on. there
				TestMeterXY.yAxis.SwapSign = MagnetMeterGoofy; // is no way back

			}
			if (UsbPower && (MagnetMeterGoofy || MagnetSensorGoofy))
				LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 85, 85, 85, LedSegment_All); // white -> in usb-mode, when joystick magnet mounted the flipped way
			else
				LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 255,0,0,7); // red -> calibrate center
		}

		TempBase2 = (int8_t)CalcCelsiusDegreeSensor(&TestSensorXY); // Temperature Base
		TempTop2 = (int8_t)CalcCelsiusDegreeSensor(&TestSensorZ); // Temperature Handle

		TempBase1 = (int8_t)CalcCelsiusDegreeMeter(&TestMeterXY);
		TempTop1 = (int8_t)CalcCelsiusDegreeMeter(&TestMeterZ);

		do {
			// Get Data from MLX90395 as x/y switching Joystick

			// Get Data from hall-element as redundant information
			MeterXYError = GetMagnetometer(&TestMeterXY);
			MeterZError = GetMagnetometer(&TestMeterZ);

			SensorXYError = GetMagnetSensor(&TestSensorXY);
			SensorZError = GetMagnetSensor(&TestSensorZ);

			FullEvalButtons(true); // CalibPhase

			if (CenterStep)  // Calibrate Centerpoint first
			{
				if (KeyR_Reverse != 0xAA)
				{
					LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 128, 128, LedSegment_All); // cyan -> Redundant Right Switch-Magnets calibrated
				}

				TeachAxisCenter(&TestMeterXY.xAxis);
				TeachAxisCenter(&TestMeterXY.yAxis);

				TeachAxisCenter(&TestSensorXY.xAxis);
				TeachAxisCenter(&TestSensorXY.yAxis);

				AngleMid = (uint16_t)((float)INT_MAX * ((atan2(TestMeterZ.xAxis.RawValue.s16 ,TestMeterZ.yAxis.RawValue.s16) / M_PI) + 1.0));
				AngleMidSensor = (uint16_t)((float)INT_MAX * ((atan2(TestSensorZ.zAxis.RawValue.s16, TestSensorZ.yAxis.RawValue.s16 * Z_RATIO_FOR_Y_AGAINST_Z) / M_PI) + 1.0));


#if (Z_RATIO_EVAL == 1)
				zIminX = (zIminX > TestSensorZ.xAxis.RawValue.s16) ? TestSensorZ.xAxis.RawValue.s16 : zIminX;
				zImaxX = (zImaxX < TestSensorZ.xAxis.RawValue.s16) ? TestSensorZ.xAxis.RawValue.s16 : zImaxX;
				zIminY = (zIminY > TestSensorZ.yAxis.RawValue.s16) ? TestSensorZ.yAxis.RawValue.s16 : zIminY;
				zImaxY = (zImaxY < TestSensorZ.yAxis.RawValue.s16) ? TestSensorZ.yAxis.RawValue.s16 : zImaxY;
				zIminZ = (zIminZ > TestSensorZ.zAxis.RawValue.s16) ? TestSensorZ.zAxis.RawValue.s16 : zIminZ;
				zImaxZ = (zImaxZ < TestSensorZ.zAxis.RawValue.s16) ? TestSensorZ.zAxis.RawValue.s16 : zImaxZ;
				zFactorStretchXtoY = ((float)zImaxY - (float)zIminY) / ((float)zImaxX - (float)zIminX);
				zFactorStretchYtoZ = ((float)zImaxZ - (float)zIminZ) / ((float)zImaxY - (float)zIminY);
				// This values can be used to derive an empirical factor to equalize the amplitudes of the sine- and cosine-waves.
				// Loosen the z-Axis magnet screw an turn it at least 360° to have all three values in their peaks, then calculate a factor
				// for Example: (zImaxZ - zIminZ) / (zImaxY - zIminY) leads to a factor of 2.66F for Y-Value when evaluated together with Z-Axis
#endif
				// -> O <-");
				if (LeftButtonState == PB_NEW_EVENT) {
					CenterStep = false;
					LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0,0,255, LedSegment_All);

					//DEFLECT LEVER TO ALL LIMITS, THEN PRESS TO PROCEED;
				}
			} else  // then calibrate maxima and minima
			{

				if ((KeyL_Reverse != 0xAA) && (KeyR_Reverse != 0xAA) && (KeyT_Reverse != 0xAA) && (KeyB_Reverse != 0xAA) && ButtonFront1 && ButtonFront2)
					LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 128, 64, LedSegment_All); // turquise -> all Redundant Switch-Magnets calibrated
				
		        AngleAbs = (uint16_t)((float)INT_MAX * ((atan2(TestMeterZ.xAxis.RawValue.s16 ,TestMeterZ.yAxis.RawValue.s16) / M_PI) + 1.0));
				AngleAbsSensor = (uint16_t)((float)INT_MAX * ((atan2(TestSensorZ.zAxis.RawValue.s16, TestSensorZ.yAxis.RawValue.s16 * Z_RATIO_FOR_Y_AGAINST_Z) / M_PI) + 1.0));

		        AngleRel = (AngleAbs - AngleMid) + INT_MAX;
		        AngleRelSensor = (AngleAbsSensor - AngleMidSensor) + INT_MAX;

				AngleMin = (AngleMin < AngleRel) ? AngleMin : AngleRel;
				AngleMax = (AngleMax > AngleRel) ? AngleMax : AngleRel;

				AngleMinSensor = (AngleMinSensor < AngleRelSensor) ? AngleMinSensor : AngleRelSensor;
				AngleMaxSensor = (AngleMaxSensor > AngleRelSensor) ? AngleMaxSensor : AngleRelSensor;

				if (AngleRel > INT_MAX)
				{
					AngleZ = (int16_t)(((float)AngleMax / 127.0f) * AngleRel);
					AngleZ = (AngleZ > 127) ? AngleZ : 127;
				}
				else
				{
					AngleZ = (int16_t)(((float)AngleMin / 128.0f) * AngleRel);
					AngleZ = (AngleZ < -128) ? AngleZ : -128;
				}
				zRadiusMeter = (uint32_t)sqrt((int32_t)TestMeterZ.xAxis.RawValue.s16 * (int32_t)TestMeterZ.xAxis.RawValue.s16 + (int32_t)TestMeterZ.yAxis.RawValue.s16 * (int32_t)TestMeterZ.yAxis.RawValue.s16);

				zRadiusMinMeter = (zRadiusMinMeter < zRadiusMeter) ? zRadiusMinMeter : zRadiusMeter;
				zRadiusMaxMeter = (zRadiusMaxMeter > zRadiusMeter) ? zRadiusMaxMeter : zRadiusMeter;

				if (zRadiusMaxMeter > Z_RADIUS_MAX_LIMIT) {
					LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 127,127,0, LedSegment_All); // YellowOrange Indicator
				}

				if (V_InfoList[3] || V_InfoList[4]) // any LED-Errors to declare -> visualize them
				{
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 255, 0, 0, LedSegment_1); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 255, 0, 0, LedSegment_2); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 255, 0, 0, LedSegment_3); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 255, 0, LedSegment_1); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 255, 0, LedSegment_2); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 255, 0, LedSegment_3); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 0, 255, LedSegment_1); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 0, 255, LedSegment_2); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 0, 255, LedSegment_3); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 85,85,85, LedSegment_All); // White Indicator (or what results out of defect)
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
						LoopDelay150ms();
				}

				TeachAxisCenterbalanced(&TestMeterXY.xAxis);
				TeachAxisCenterbalanced(&TestMeterXY.yAxis);

				TeachAxisCenterbalanced(&TestSensorXY.xAxis);
				TeachAxisCenterbalanced(&TestSensorXY.yAxis);

			}
		} while (CenterStep	|| (RightButtonState != PB_NEW_EVENT));


		TempBase1 = (int8_t)CalcCelsiusDegreeMeter(&TestMeterXY);
		TempTop1 = (int8_t)CalcCelsiusDegreeMeter(&TestMeterZ);

		TempBase2 = (int8_t)CalcCelsiusDegreeSensor(&TestSensorXY); // Temperature Base
		TempTop2 = (int8_t)CalcCelsiusDegreeSensor(&TestSensorZ); // Temperature Handle


		LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 127,0,127,LedSegment_All);

		do
		{
			FullEvalButtons(true);
		}
		while (FrontButton1State != PB_NEW_EVENT);
		LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0,255,0,LedSegment_All);

		do
			FullEvalButtons(true);
		while (FrontButton2State != PB_NEW_EVENT);
	   	SprintForSerial();
#ifndef HW_TEST
		CalibrationSettingsFRAM(STORE_SETTINGS_TO_FRAM);
#endif
		//CALIBRATION STORED
		Calibration = false;

		LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0,0,0,LedSegment_All);
    	VibraError = LraVibraCalibrate(I2C_Channel_Satellite, BASE_ADDR_VIBRA);
		LoopDelay150ms();

		if (UsbPower || VibraError)
		{
			ControlTest = true;
		}
			else
		{
			NormalOperation = true;
		    RestartFirmware = true;
		}
	}

	if (ControlTest)
	{

		InitMeterI2C(&TestMeterXY); // Init Melexis Magnetsensor
		InitMeterI2C(&TestMeterZ); // Init Melexis Magnetsensor

		InitHallSensor(&TestSensorXY); // Init Melexis Magnetsensor
		InitHallSensor(&TestSensorZ); // Init Melexis Magnetsensor

		while (1)
		{
			LedError = LedSetModeLogTC(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LedModeLog, LedModeLog, LedModeLog);
			for (Smooth  = 0; Smooth  < 255; Smooth ++)
			{
				LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, (VibraError)? Smooth : 0, (VibraError)? 0 : Smooth ,0 ,LedSegment_All);
				LoopDelay3ms();
			}

			for (Smooth = 255; Smooth > 0; Smooth--)
			{
				LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, (VibraError)? Smooth : 0, (VibraError)? 0 : Smooth ,0 ,LedSegment_All);
				LoopDelay3ms();

			}

		    Range = 2.0;
		    StretchXY(&Range);

			while (1) {
				// Get Data from MLX90393 as x/y switching Joystick
				MeterXYError = GetMagnetometer(&TestMeterXY);
				SensorXYError = GetMagnetSensor(&TestSensorXY);

				AdjustAxisCenterbalancedClamped(&TestMeterXY.xAxis, INT_MAX, INT_MIN);
			    AdjustAxisCenterbalancedClamped(&TestMeterXY.yAxis, INT_MAX, INT_MIN);
				AdjustAxisCenterbalancedClamped(&TestSensorXY.xAxis, INT_MAX, INT_MIN);
			    AdjustAxisCenterbalancedClamped(&TestSensorXY.yAxis, INT_MAX, INT_MIN);

				MeterZError = GetMagnetometer(&TestMeterZ);
				SensorZError = GetMagnetSensor(&TestSensorZ);

				AngleAbs = (uint16_t)((float)INT_MAX * ((atan2(TestMeterZ.xAxis.RawValue.s16 ,TestMeterZ.yAxis.RawValue.s16) / M_PI) + 1.0));
		        AngleRel = (AngleAbs - AngleMid) + INT_MAX;

		        if (UsbPower) // DemoMode does some live calibration. Not intended as final production test after calibration!
		        {
					AngleMin = (AngleMin < AngleRel) ? AngleMin : AngleRel; // maintain live calibration of left turn
					AngleMax = (AngleMax > AngleRel) ? AngleMax : AngleRel; // maintain live calibration of right turn

					AngleMinSensor = (AngleMinSensor < AngleRelSensor) ? AngleMinSensor : AngleRelSensor; // maintain live calibration of left turn
					AngleMaxSensor = (AngleMaxSensor > AngleRelSensor) ? AngleMaxSensor : AngleRelSensor; // maintain live calibration of right turn
					if (VibraError)
		        		LraVibraClearColdStartState();
		        }

				Converter16.u16 = abs(sqrt((int32_t)TestSensorXY.xAxis.FinalValue.s16 * (int32_t)TestSensorXY.xAxis.FinalValue.s16 + (int32_t)TestSensorXY.yAxis.FinalValue.s16 * (int32_t)TestSensorXY.yAxis.FinalValue.s16));
				DeflectionVector2 = Converter16.u8[0];

				Converter16.u16 = abs(TestMeterXY.xAxis.FinalValue.s16);
				if (TestMeterXY.xAxis.FinalValue.s16 > 0)
				{
					posX = Converter16.u8[0];
					negX = 0;
				}
				else
				{
					negX = Converter16.u8[0];
					posX = 0;
				}
				Converter16.u16 = abs(TestMeterXY.yAxis.FinalValue.s16);
				if (TestMeterXY.yAxis.FinalValue.s16 > 0)
				{
					posY = Converter16.u8[0];
					negY = 0;
				}
				else
				{
					negY = Converter16.u8[0];
					posY = 0;
				}

				AngleAbsSensor = (uint16_t)((float)INT_MAX * ((atan2(TestSensorZ.zAxis.RawValue.s16, TestSensorZ.yAxis.RawValue.s16 * Z_RATIO_FOR_Y_AGAINST_Z) / M_PI) + 1.0));
		        AngleRelSensor = (AngleAbsSensor - AngleMidSensor) + INT_MAX;

				Converter16.u16 = abs(sqrt((int32_t)TestMeterXY.xAxis.FinalValue.s16 * (int32_t)TestMeterXY.xAxis.FinalValue.s16 + (int32_t)TestMeterXY.yAxis.FinalValue.s16 * (int32_t)TestMeterXY.yAxis.FinalValue.s16));
				DeflectionVector1 = Converter16.u8[0];

				FullEvalButtons(false);
				Red=0;
				Green=0;
				Blue=0;

				if (ButtonFront1)
				{
					if (AngleRelSensor > INT_MAX)
					{
						Red += (uint8_t)((float)((float)(AngleRelSensor - INT_MAX) / (float)(AngleMaxSensor - INT_MAX)) * 127.0);
					}
					else
					{
						Green += (uint8_t)((float)((float)(INT_MAX - AngleRelSensor) / (float)(INT_MAX - AngleMinSensor)) * 127.0);
					}
				}
				if (ButtonFront2)
				{


					if (AngleRel > INT_MAX)
					{
						Red += (uint8_t)((float)((float)(AngleRel - INT_MAX) / (float)(AngleMax - INT_MAX)) * 127.0);
					}
					else
					{
						Green += (uint8_t)((float)((float)(INT_MAX - AngleRel) / (float)(INT_MAX - AngleMin)) * 127.0);
					}
				}

				if (FrontButtonState == PB_NEW_EVENT)
				{
					DataToRead[0] = (ButtonFront1) ? 13 : 11;
					DataToRead[1] = 0x00;
					VibraError = LraVibraSetPattern(I2C_Channel_Satellite, BASE_ADDR_VIBRA, DataToRead);
				}


				if (ButtonRight)
				{
					Blue = DeflectionVector2;
				}
				if (ButtonLeft)
				{
					Red = DeflectionVector1;
				}

				if ((ButtonRight || ButtonLeft || ButtonFront) == 0)
				{
					Red += negX;
					Blue += posX;
					Green += negY;

					Red += posY;
					Blue += posY;
					Green += posY;

				}


				LedError = LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, Red, Green, Blue, LedSegment_All);
			}
		} // end Calibration Test

	}	// Inititalisation stuff for direct startup of Calibration Test application

#if(WATCHDOG_ACTIVE == 1)
// Feed WDT timer before disabling (Erratum WDT_E102)
    WDTCN = 0xA5;

    // Add 2 LFO cycle delay before disabling WDT (Erratum WDT_E102)
    WDT0_delay(SYSTEM_CLOCK_FREQUENCY, LFOCLK_HZ, WDT_DELAY_CYCLES);

    // Disable WDT before changing interval
    ea_Val = IE_EA;
    IE_EA = 0;
    WDTCN = 0xDE;
    WDTCN = 0xAD;
    IE_EA = ea_Val;

    // Delay 2 LFO cycles to ensure WDT is completely disabled
    WDT0_delay(SYSTEM_CLOCK_FREQUENCY, LFOCLK_HZ, WDT_DELAY_CYCLES);

    // Change WDT interval
    WDTCN = 4;   // ~0.3s
    // Start WDT
    WDTCN = 0xA5;
#endif
    Range = RANGE;
    StretchXY(&Range);

#if INIT_SIGI == 1
	IE |= IE_EX0__ENABLED;  // Start with IO-Link by enabling interrupt
#endif

	AdcMidBottom = (AdcMaxBottom>>1) + (AdcMinBottom>>1);
	AdcMidTop = (AdcMaxTop>>1) + (AdcMinTop>>1);
    AdcMidRight = (AdcMaxRight>>1) + (AdcMinRight>>1);
   	AdcMidLeft = (AdcMaxLeft>>1) + (AdcMinLeft>>1);

	while (1)
	{
		if (!LinkTestOnly)
		{
			MeterXYError = GetMagnetometer(&TestMeterXY);
			SensorXYError = GetMagnetSensor(&TestSensorXY);
		}

		TempBase1 = (int8_t)CalcCelsiusDegreeMeter(&TestMeterXY);
		TempTop1 = (int8_t)CalcCelsiusDegreeMeter(&TestMeterZ);
		TempBase2 = (int8_t)CalcCelsiusDegreeSensor(&TestSensorXY); // Temperature Base
		TempTop2 = (int8_t)CalcCelsiusDegreeSensor(&TestSensorZ); // Temperature Handle

#define INFOLIST_USAGE_DEBUG 0
#define SMALLCODE 0
#if INFOLIST_USAGE_DEBUG == 1
		V_InfoList[0] = TestMeterXY.retry;
		V_InfoList[1] = TestMeterZ.retry;
		V_InfoList[2] = TestMeterXY.LastFrameCounter;
		V_InfoList[3] = TestMeterZ.LastFrameCounter;
		V_InfoList[4] = TestMeterXY.State;
		V_InfoList[5] = TestMeterZ.State;
		V_InfoList[6] = TestMeterXY.OperationMode;
		V_InfoList[7] = TestMeterZ.OperationMode;
#else
#if SMALLCODE == 1
//		Activate this <essential> section instead of the code-section below to free-up some FLASH-Memory in CODE for future modifications or fixes when needed
//		as the rest of the information can be derived with workarounds and is redundant form that point of view

		//	V_InfoList[0]..[2] is fully occupied by Cycle-Counter and LED-States. So Don't clear then here
		V_InfoList[3] &= ~(0x80 + 0x40 + 0x20 + 0x10 + 0x08 + 0x04); // don't clear the parts occupied by LED-States in V_InfoList[3]
		V_InfoList[4] = 0;
		V_InfoList[5] = 0;
		V_InfoList[6] = 0;
		V_InfoList[7] = 0;
		V_InfoList[7] |= (CrcError)                        ? 0x20 : 0;
		V_InfoList[7] |= (AdcFrontError)                   ? 0x10 : 0;
		V_InfoList[7] |= (KeyT_Reverse == 0xAA)            ? 0x08 : 0;
		V_InfoList[7] |= (KeyB_Reverse == 0xAA)            ? 0x04 : 0;
		V_InfoList[7] |= (AdcTopError)                     ? 0x01 : 0;
		V_InfoList[6] |= (KeyL_Reverse == 0xAA)            ? 0x80 : 0;
		V_InfoList[6] |= (KeyR_Reverse == 0xAA)            ? 0x40 : 0;
		V_InfoList[6] |= (TestMeterXY.xAxis.SwapSign)      ? 0x04 : 0;
		V_InfoList[6] |= (TestSensorXY.xAxis.SwapSign)     ? 0x01 : 0;
		V_InfoList[5] |= (SensorZError)                    ? 0x40 : 0;
		V_InfoList[4] |= (UsbPower)                        ? 0x02 : 0;
		V_InfoList[3] |= (RawdataBaseTest)                 ? 0x80 : 0;
		V_InfoList[3] |= (LinkTestOnly)                    ? 0x40 : 0;
#else
// this section here is a complete evaluation if most we can get out of hardware. repalce it by section above if more FLASH in CODE section is needed
		//	V_InfoList[0]..[2] is fully occupied by Cycle-Counter and LED-States. So Don't clear then here
		V_InfoList[3] &= ~(0x80 + 0x40 + 0x20 + 0x10 + 0x08 + 0x04); // don't clear the parts occupied by LED-States in V_InfoList[3]
		V_InfoList[4] = 0;
		V_InfoList[5] = 0;
		V_InfoList[6] = 0;
		V_InfoList[7] = 0;
		V_InfoList[7] |= (LedError)                        ? 0x80 : 0;
		V_InfoList[7] |= (FramError)                       ? 0x40 : 0;
		V_InfoList[7] |= (CrcError)                        ? 0x20 : 0;
		V_InfoList[7] |= (AdcFrontError)                   ? 0x10 : 0;
		V_InfoList[7] |= (KeyT_Reverse == 0xAA)            ? 0x08 : 0;
		V_InfoList[7] |= (KeyB_Reverse == 0xAA)            ? 0x04 : 0;
		V_InfoList[7] |= (ButtonFrontError)                ? 0x02 : 0;
		V_InfoList[7] |= (AdcTopError)                     ? 0x01 : 0;
		V_InfoList[6] |= (KeyL_Reverse == 0xAA)            ? 0x80 : 0;
		V_InfoList[6] |= (KeyR_Reverse == 0xAA)            ? 0x40 : 0;
		V_InfoList[6] |= (ButtonLeftError)                 ? 0x20 : 0;
		V_InfoList[6] |= (ButtonRightError)                ? 0x10 : 0;
		V_InfoList[6] |= (MeterXYError)                    ? 0x08 : 0;
		V_InfoList[6] |= (TestMeterXY.xAxis.SwapSign)      ? 0x04 : 0;
		V_InfoList[6] |= (SensorXYError)                   ? 0x02 : 0;
		V_InfoList[6] |= (TestSensorXY.xAxis.SwapSign)     ? 0x01 : 0;
		V_InfoList[5] |= (MeterZError)                     ? 0x80 : 0;
		V_InfoList[5] |= (SensorZError)                    ? 0x40 : 0;
		V_InfoList[5] |= (VibraError)                      ? 0x20 : 0;
		V_InfoList[5] |= (VibraOverCurrent)                ? 0x10 : 0;
		V_InfoList[5] |= (VibraCalibTimeout)               ? 0x08 : 0;
		V_InfoList[5] |= (VibraOverTemp)                   ? 0x04 : 0;
		V_InfoList[5] |= (TempBase1 > HIGH_TEMP_LIMIT)     ? 0x02 : 0;
		V_InfoList[5] |= (TempTop1 > HIGH_TEMP_LIMIT)      ? 0x01 : 0;
		V_InfoList[4] |= (TempBase1 < LOW_TEMP_LIMIT)      ? 0x80 : 0;
		V_InfoList[4] |= (TempTop1 < LOW_TEMP_LIMIT)       ? 0x40 : 0;
		V_InfoList[4] |= (TempBase2 > HIGH_TEMP_LIMIT)     ? 0x20 : 0;
		V_InfoList[4] |= (TempTop2 > HIGH_TEMP_LIMIT)      ? 0x10 : 0;
		V_InfoList[4] |= (TempBase2 < LOW_TEMP_LIMIT)      ? 0x08 : 0;
		V_InfoList[4] |= (TempTop2 < LOW_TEMP_LIMIT)       ? 0x04 : 0;
		V_InfoList[4] |= (UsbPower)                        ? 0x02 : 0;
		V_InfoList[4] |= (ExternalBusNotOk)                ? 0x01 : 0;
		V_InfoList[3] |= (RawdataBaseTest)                 ? 0x80 : 0;
		V_InfoList[3] |= (LinkTestOnly)                    ? 0x40 : 0;
		V_InfoList[3] |= (InternalFault)                   ? 0x20 : 0;
		V_InfoList[3] |= (StartBootloader)                 ? 0x10 : 0;
		V_InfoList[3] |= (!NormalOperation)                ? 0x08 : 0;
		V_InfoList[3] |= (Calibration)                     ? 0x04 : 0;
		V_InfoList[0] += 1; //= V_InfoList[0]-Byte used as Cycle-Counter;

		// Documentation for Info-List
		// Info-List Byte 7, Bit7 (0x80) : (Error) LED-Driver not present
		// Info-List Byte 7, Bit6 (0x40) : (Error) NV-Memory not present
		// Info-List Byte 7, Bit5 (0x20) : (Error) CRC-Error in calibration data
		// Info-List Byte 7, Bit4 (0x10) : (Error) Analog to Digital Converter for Front-Buttons not present
		// Info-List Byte 7, Bit3 (0x08) : (Warning/Info) System for redundant upper Front Key not present/not calibrated
		// Info-List Byte 7, Bit2 (0x04) : (Warning/Info) System for redundant lower Front Key not present/not calibrated
		// Info-List Byte 7, Bit1 (0x02) : (Warning) Analog Voltage-Level of at least one Front-button is not in allowed Range
		// Info-List Byte 7, Bit0 (0x01) : (Error) Analog to Digital Converter for Top-Left and Top-Right Buttons is not present
		// Info-List Byte 6, Bit7 (0x80) : (Warning/Info) System for redundant Top Left Key not present/not calibrated
		// Info-List Byte 6, Bit6 (0x40) : (Warning/Info) System for redundant Top Right Key not present/not calibrated
		// Info-List Byte 6, Bit5 (0x20) : (Warning) Analog Voltage-Level of Top Left Button is not in allowed Range
		// Info-List Byte 6, Bit4 (0x10) : (Warning) Analog Voltage-Level of Top Right Button is not in allowed Range
		// Info-List Byte 6, Bit3 (0x08) : (Warning) Hall Element for Joystick-1 XY-Axis not present
		// Info-List Byte 6, Bit2 (0x04) : (Info) Joystick-1 Signals inverted as Magnet is mounted the flipped way
		// Info-List Byte 6, Bit1 (0x02) : (Warning) Hall Element for Joystick-2 XY-Axis not present
		// Info-List Byte 6, Bit0 (0x01) : (Info) Joystick-2 Signals inverted as Magnet is mounted the flipped way
		// Info-List Byte 5, Bit7 (0x80) : (Error) Hall Element for Rotation-1 Y-Axis not present
		// Info-List Byte 5, Bit6 (0x40) : (Error) Hall Element for Rotation-2 Z-Axis not present
		// Info-List Byte 5, Bit5 (0x20) : (Error) Driver for Haptic Feedback not present
		// Info-List Byte 5, Bit4 (0x10) : (Warning) Overcurrent for Haptic Motor detected
		// Info-List Byte 5, Bit3 (0x08) : (Warning) Calibration of Haptic Motor failed / Motor not present
		// Info-List Byte 5, Bit2 (0x04) : (Warning) Temperature of haptic Motor Driver too high / Driver shut down
		// Info-List Byte 5, Bit1 (0x02) : (Warning) Temperature XY-Sensor1 too high
		// Info-List Byte 5, Bit0 (0x01) : (Warning) Temperature Z-Sensor1 too high
		// Info-List Byte 4, Bit7 (0x80) : (Warning) Temperature XY-Sensor1 too low
		// Info-List Byte 4, Bit6 (0x40) : (Warning) Temperature Z-Sensor1 too low
		// Info-List Byte 4, Bit5 (0x20) : (Warning) Temperature XY-Sensor2 too high
		// Info-List Byte 4, Bit4 (0x10) : (Warning) Temperature Z-Sensor2 too high
		// Info-List Byte 4, Bit3 (0x08) : (Warning) Temperature XY-Sensor2 too low
		// Info-List Byte 4, Bit2 (0x04) : (Warning) Temperature Z-Sensor2 too low
		// Info-List Byte 4, Bit1 (0x02) : (Error) Device Running on USB-Power
		// Info-List Byte 4, Bit0 (0x01) : (Error) Internal Bus-Connection to Circuit in Handle not present
		// Info-List Byte 3, Bit7 (0x80) : (Warning) Raw-Data-Mode/no valid Calibration-Data (in conjunction with other errors)
		// Info-List Byte 3, Bit6 (0x40) : (Warning) Mode for IO-Link testing active (in conjunction with other errors)
		// Info-List Byte 3, Bit5 (0x20) : (Error/Warning/Info) Alarm-Collection/Test-Mode-Indicator/Indicator for a pending Reset
		// Info-List Byte 3, Bit4 (0x10) : (Reserved/BLS-Stat) This bit should not be evaluated as it is subject to be changed
		// Info-List Byte 3, Bit3 (0x08) : (Reserved/NOK-Stat) This bit should not be evaluated as it is subject to be changed
		// Info-List Byte 3, Bit2 (0x04) : (Reserved/CAL-Stat) This bit should not be evaluated as it is subject to be changed
		// Info-List Byte 3, Bit1 (0x02) : (Warning) LED1 red wire open circuit
		// Info-List Byte 3, Bit0 (0x01) : (Warning) LED1 red wire short circuit
		// Info-List Byte 2, Bit7 (0x80) : (Warning) LED1 green wire open circuit
		// Info-List Byte 2, Bit6 (0x40) : (Warning) LED1 green wire short circuit
		// Info-List Byte 2, Bit5 (0x20) : (Warning) LED1 blue wire open circuit
		// Info-List Byte 2, Bit4 (0x10) : (Warning) LED1 blue wire short circuit
		// Info-List Byte 2, Bit3 (0x08) : (Warning) LED2 red wire open circuit
		// Info-List Byte 2, Bit2 (0x04) : (Warning) LED2 red wire short circuit
		// Info-List Byte 2, Bit1 (0x02) : (Warning) LED2 green wire open circuit
		// Info-List Byte 2, Bit0 (0x01) : (Warning) LED2 green wire short circuit
		// Info-List Byte 1, Bit7 (0x80) : (Warning) LED2 blue wire open circuit
		// Info-List Byte 1, Bit6 (0x40) : (Warning) LED2 blue wire short circuit
		// Info-List Byte 1, Bit5 (0x20) : (Warning) LED3 red wire open circuit
		// Info-List Byte 1, Bit4 (0x10) : (Warning) LED3 red wire short circuit
		// Info-List Byte 1, Bit3 (0x08) : (Warning) LED3 green wire open circuit
		// Info-List Byte 1, Bit2 (0x04) : (Warning) LED3 green wire short circuit
		// Info-List Byte 1, Bit1 (0x02) : (Warning) LED3 blue wire open circuit
		// Info-List Byte 1, Bit0 (0x01) : (Warning) LED3 blue wire short circuit
		// Info-List Byte 0, Bit0 - Bit7 : (Info) Internal Main-Loop Cycle-Counter

#endif // NOT SMALLCODE
#endif // NOT INFOLIST_USAGE_DEBUG

		AdjustAxisCenterbalancedClamped(&TestSensorXY.xAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!
	    AdjustAxisCenterbalancedClamped(&TestSensorXY.yAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!
	    CompensateGeometry(&TestSensorXY.xAxis, &TestSensorXY.yAxis, STRETCH, LIMIT_POS, LIMIT_NEG);
		AdjustAxisCenterbalancedClamped(&TestMeterXY.xAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!
	    AdjustAxisCenterbalancedClamped(&TestMeterXY.yAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!
	    CompensateGeometry(&TestMeterXY.xAxis, &TestMeterXY.yAxis, STRETCH, LIMIT_POS, LIMIT_NEG);
	    if (TestMode)
	    {
	    	SensorXYError = true;
	    	TestSensorXY.xAxis.FinalValue.s16 = -TestSensorXY.xAxis.FinalValue.s16;
	    	TestSensorXY.yAxis.FinalValue.s16 = -TestSensorXY.yAxis.FinalValue.s16;
	    	AngleDegLimit = ANGLE_TEST_LIMIT;
	    }


	    if (!LinkTestOnly && !RawdataBaseTest)
		{
			MeterZError = GetMagnetometer(&TestMeterZ);
			SensorZError = GetMagnetSensor(&TestSensorZ);
			// Check LRA-State
			VibraError = LraVibraGetState(I2C_Channel_Satellite, BASE_ADDR_VIBRA, &LRA_State);
		}

		AngleAbs = (uint16_t)((float)INT_MAX * ((atan2(TestMeterZ.xAxis.RawValue.s16 ,TestMeterZ.yAxis.RawValue.s16) / M_PI) + 1.0));
        AngleRel = (AngleAbs - AngleMid) + INT_MAX;

		if (AngleRel > INT_MAX)
		{
			AngleZ = (int16_t)((float)((float)(AngleRel - INT_MAX) / (float)(AngleMax - INT_MAX)) * 127.0);  // Like it' s done in Application
			AngleZ = AngleZ > 127 ? 127 : AngleZ;
			AngleZ = AngleZ < 0 ? 0 : AngleZ;
		}
		else
		{
			AngleZ = (int16_t)((float)((float)(INT_MAX - AngleRel) / (float)(INT_MAX - AngleMin)) * -127.0); // Like it' s done in Application
			AngleZ = AngleZ < -127 ? -127 : AngleZ;
			AngleZ = AngleZ > 0 ? 0 : AngleZ;
		}

		AngleDeg = (int16_t)(((float)((float)AngleRel / (float)UINT_MAX) * 360.0F) - 180.0F);

		SampleButtons(false); // no calib phase here!
		if (RawdataBaseTest || LinkTestOnly)
		{
			ButtonLeft = !AUX_A;
			ButtonRight = !AUX_B;
			ButtonFront = !AUX_C;
			TestMeterXY.xAxis.FinalValue.s16 = TestMeterXY.xAxis.RawValue.s16;
			TestMeterXY.yAxis.FinalValue.s16 = TestMeterXY.yAxis.RawValue.s16;
			TestMeterXY.xAxis.FinalValue.s16 = TestMeterXY.xAxis.RawValue.s16;
			TestMeterXY.yAxis.FinalValue.s16 = TestMeterXY.yAxis.RawValue.s16;
			AngleZ = TestMeterXY.zAxis.RawValue.s8[0];
			AngleDeg = TestSensorXY.zAxis.RawValue.s8[0];


			if (LinkTestOnly)
			{
				TestMeterXY.xAxis.FinalValue.s16 = TestCounter;
				TestMeterXY.yAxis.FinalValue.s16 = -TestCounter;
				TestCounter++;
			}
		}

		#if (USE_INPUTS==1)
		Converter16.s16 = TestMeterXY.xAxis.FinalValue.s16;
		EABufferIn[0] = Converter16.u8[0];
		EABufferIn[1] = Converter16.u8[1];
		Converter16.s16 = TestMeterXY.yAxis.FinalValue.s16 * -1;
		EABufferIn[2] = Converter16.u8[0];
		EABufferIn[3] = Converter16.u8[1];
		EABufferIn[4] = AngleZ; // fill in value for Axis Z 1

	    if (TestMode)
	    {
			if (AngleRel > INT_MAX)
			{
				EABufferIn[7] = (int8_t)((float)((float)(AngleRel - INT_MAX) / (float)(AngleMax - INT_MAX)) * -100.0);  // Like it' s done in Application
			}
			else
			{
				EABufferIn[7] = (int8_t)((float)((float)(INT_MAX - AngleRel) / (float)(INT_MAX - AngleMin)) * 100.0); // Like it' s done in Application
			}
	    }
	    else
	    {
	    	EABufferIn[7] = (int8_t)AngleDeg; // fill in value for Axis Z-Axis1 Meter in absolute Degrees
	    }

		if (abs(AngleDeg) > AngleDegLimit)
		{
			AngleZError = true;
		}
		else
		{
			AngleZError = false;
		}


    	Converter16.s16 = SignedFilter( TestSensorXY.yAxis.FinalValue.s16, &FilterCartridgeOne);
		EABufferIn[5] = Converter16.u8[0]; // fill in value for Axis X 2

		Converter16.s16 = SignedFilter( TestSensorXY.xAxis.FinalValue.s16 * -1, &FilterCartridgeTwo);
		EABufferIn[6] = Converter16.u8[0]; // fill in value for Axis Y 2

		AngleAbsSensor = (uint16_t)((float)INT_MAX * ((atan2(TestSensorZ.zAxis.RawValue.s16, TestSensorZ.yAxis.RawValue.s16 * Z_RATIO_FOR_Y_AGAINST_Z) / M_PI) + 1.0));
        AngleRelSensor = (AngleAbsSensor - AngleMidSensor) + INT_MAX;

		if (AngleRelSensor > INT_MAX)
		{
			AngleZPercentSensor = (int16_t)((float)((float)(AngleRelSensor - INT_MAX) / (float)(AngleMaxSensor - INT_MAX)) * 100.0);  // Like it' s done in Application
		}
		else
		{
			AngleZPercentSensor = (int16_t)((float)((float)(INT_MAX - AngleRelSensor) / (float)(INT_MAX - AngleMinSensor)) * -100.0); // Like it' s done in Application
		}

		Converter16.s16 = TestMeterXY.xAxis.FinalValue.s16;
		CompareX1 = Converter16.u8[0]; // fill in compare value for Axis X 1
		Converter16.s16 = -TestMeterXY.yAxis.FinalValue.s16;
		CompareY1 = Converter16.u8[0]; // fill in compare value for Axis Y 1

		Converter16.s16 = TestSensorXY.yAxis.FinalValue.s16;
		CompareX2 = Converter16.u8[0]; // fill in compare value for Axis X 2
		Converter16.s16 = -TestSensorXY.xAxis.FinalValue.s16;
		CompareY2 = Converter16.u8[0]; // fill in compare value for Axis Y 2

		AxisXError =  (abs(CompareX1 - CompareX2) > COMPARE_TRESHOLD);
		AxisYError =  (abs(CompareY1 - CompareY2) > COMPARE_TRESHOLD);
		PosZMeter = (int16_t)(AngleRel - INT_MAX);

		if (RawdataBaseTest || LinkTestOnly)
		{
			AxisXError = true;
			AxisYError = true;
			AngleZError = true;
		}

		// Button Flags and ErrorHandling
		Converter16.u8[0] =  0x10 *(uint8_t)ButtonLeftError
				      + 0x08 *(uint8_t)ButtonRightError
				      + 0x04 *(uint8_t)ButtonFrontError
	      	  	  	  + 0x02 *(uint8_t)AxisXError
					  + 0x01 *(uint8_t)AxisYError; // add flags for Button Left Error; Button Right Error; Button Front Error; X1 - Axis Error; Y1 - Axis Error

	    if (TestMode)
	    {
	    	VibraError = (LRA_State);
	    }


		Converter16.u8[1] = 0x80 *(uint8_t)AngleZError
				      + 0x40 *(uint8_t)MeterXYError
				      + 0x20 *(uint8_t)SensorXYError
		              + 0x10 *(uint8_t)MeterZError
				      + 0x08 *(uint8_t)LedError
				      + 0x04 *(uint8_t)VibraError
	      	  	  	  + 0x02 *(uint8_t)(FramError || CrcError);
		if (Converter16.u16)
		{
			WarningToErrorConverter += (WarningToErrorConverter < 0xff) ? 1 : 0;
			if (WarningToErrorConverter > WARNING_TRESHOLD)
			{
				InternalFault = true;
			}
		}
		else
		{
			WarningToErrorConverter = 0;
			if (!RestartFirmware)
				InternalFault = false;
		}
		EABufferIn[8] =  0x80 *(uint8_t)ButtonLeft + 0x40 *(uint8_t)ButtonRight + 0x20 *(uint8_t)ButtonFront;
		EABufferIn[9] =  0x01 *(uint8_t)InternalFault; // fill in flags for Z1 - Axis Error; X2 - Axis Error; Y2 - Axis Error; Z2 - Axis Error; LED Error; Haptic Engine Error; Internal Fault; Internal Warning

		if (WarningToErrorConverter > ERROR_TRESHOLD)
		{
			EABufferIn[8] += Converter16.u8[0];
			EABufferIn[9] += Converter16.u8[1];
		}

	    AL_SetInputReq(EABufferIn, TRUE );

		#endif //if (USE_INPUTS==1)

	    KeyDataB = (KeyB_Reverse) ? INT_MAX - KeyDataB + AdcMidBottom : KeyDataB - AdcMidBottom + INT_MAX;
		KeyDataT = (KeyT_Reverse) ? INT_MAX - KeyDataT + AdcMidTop : KeyDataT - AdcMidTop + INT_MAX;
	    KeyDataR = (KeyR_Reverse) ? INT_MAX - KeyDataR + AdcMidRight : KeyDataR - AdcMidRight + INT_MAX;
	   	KeyDataL = (KeyL_Reverse) ? INT_MAX - KeyDataL + AdcMidLeft : KeyDataL - AdcMidLeft + INT_MAX;
		Converter16.s16 = -TestMeterXY.xAxis.RawValue.s16;
		X_Axis1.u8[1] = Converter16.u8[0];
		X_Axis1.u8[0] = Converter16.u8[1];
		Y_Axis1.u8[1] = TestMeterXY.yAxis.RawValue.u8[0];
		Y_Axis1.u8[0] = TestMeterXY.yAxis.RawValue.u8[1];
		Converter16.s16 = PosZMeter;
		Z_Axis1.u8[1] = Converter16.u8[0];
		Z_Axis1.u8[0] = Converter16.u8[1];
		Converter16.u16 = AdcData1.Channel2.u16;
		SwitchTopL1.u8[1] = Converter16.u8[0];
		SwitchTopL1.u8[0] = Converter16.u8[1];
		Converter16.u16 = AdcData1.Channel3.u16;
		SwitchTopR1.u8[1] = Converter16.u8[0];
		SwitchTopR1.u8[0] = Converter16.u8[1];
		Converter16.u16 = AdcData2.Channel2.u16;
		SwitchFrontTop1.u8[1] = Converter16.u8[0];
		SwitchFrontTop1.u8[0] = Converter16.u8[1];
		Converter16.u16 = AdcData2.Channel3.u16;
		SwitchFrontBottom1.u8[1] = Converter16.u8[0];
		SwitchFrontBottom1.u8[0] = Converter16.u8[1];
		Converter16.s16 = SignedFilterReadonly(&FilterCartridgeOne);
		X_Axis2.u8[1] = Converter16.u8[0];
		X_Axis2.u8[0] = Converter16.u8[1];
		Converter16.s16 = SignedFilterReadonly(&FilterCartridgeTwo);
		Y_Axis2.u8[1] = Converter16.u8[0];
		Y_Axis2.u8[0] = Converter16.u8[1];
		Converter16.s16 = AngleZPercentSensor;
		Z_Axis2.u8[1] = Converter16.u8[0];
		Z_Axis2.u8[0] = Converter16.u8[1];
		Converter16.u16 = KeyDataL;
		SwitchTopL2.u8[1] = Converter16.u8[0];
		SwitchTopL2.u8[0] = Converter16.u8[1];
		Converter16.u16 = KeyDataR;
		SwitchTopR2.u8[1] = Converter16.u8[0];
		SwitchTopR2.u8[0] = Converter16.u8[1];
		Converter16.u16 = KeyDataT;
		SwitchFrontTop2.u8[1] = Converter16.u8[0];
		SwitchFrontTop2.u8[0] = Converter16.u8[1];
		Converter16.u16 = KeyDataB;
		SwitchFrontBottom2.u8[1] = Converter16.u8[0];
		SwitchFrontBottom2.u8[0] = Converter16.u8[1];
		IolRun();

#if(WATCHDOG_ACTIVE == 1)
		{
			WDTCN = 0xA5;
		}
#endif
	}
}

#pragma save
#pragma optimize (11, speed)

SI_INTERRUPT (INT0_ISR, INT0_IRQn)
{
	TUnsigned16 Status;
	uint8_t SFRPAGE_save = SFRPAGE;

    Status = HMT7742_RegRead(HMT7742_REG_LINK);
	if(Status & (HMT7742_STATUS_DAT<<8))
	{
	  if(!(Status & (HMT7742_STATUS_CHK<<8)))
	  {
	      TUnsigned8 Length;
	      TUnsigned8 Pos = 0;
	      Length = (TUnsigned8) Status;
	      Length >>= 2;
		      HMT7742_BufRead(Length);
	      do
	      {
	        BSP_ReceiveCallback(FrameBuffer[++Pos], 0);
	      }
	      while(--Length);
	      if(Answer)
	      {
	        do
	        {
	          BSP_TransmitCallback();
	        }
	        while(Answer);
	        Length = Position;
	        Position = 0;
	        Pos = 1;
   	        HMT7742_BufWrite(FrameBuffer, Length);
      	    HMT7742_RegWrite(HMT7742_REG_LINK, Length<<2 | HMT7742_LINK_SND);
	    	SFRPAGE = SFRPAGE_save;
	        return;
	      }
	   }
   	   HMT7742_RegWrite(HMT7742_REG_LINK, HMT7742_LINK_END);
	   FRAME_ResetReceiveBuffer();
	}
	SFRPAGE = SFRPAGE_save;
}



/*! @} */
/*     End Function Implementation
 **
 ****************************************************************************/
#pragma restore
