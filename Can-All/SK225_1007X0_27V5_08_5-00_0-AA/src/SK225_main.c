/* SK225_main.c
 ***********************************************
 *               (c) 2015-2022                 *
 *               GENGE & THOMA                 *
 *              Erlenstrasse 32                *
 *            CH-2555 Bruegg b.Biel            *
 *                                             *
 *      Author: Matthias Siegenthaler          *
 *                                             *
 *     LIBRARIES & CONCEPT (c) 2009-2022       *
 *           SIGITRONIC SOFTWARE               *
 *          Matthias Siegenthaler              *
 *              Kirchgaessli 3                 *
 *          CH-2502 Biel-Bienne                *
 *              Switzerland                    *
 *                                             *
 *         matthias@sigitronic.com             *
 ***********************************************
 */

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <si_toolchain.h>
#include <SI_EFM8UB3_Register_Enums.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <LIMITS.H>
#include "InitDevice.h"
#include "HalDef.h"
#include "I2cDispatcher.h"
#include "LedDriverLP55231.h"
#include "HallDriverTLE493D_W2B6.h"
#include "Cordic.h"
#include "NotchManager.h"
#include "SwitchManager.h"
#include "PersistSettings.h"
#include "SMBusSlave.h"
#include "Filter.h"
#include "CRC.h"


/////////////////////////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////////////////////////

#define LAG_X (160)
#define LAG_Y (32)
#define AMPLITUDE_X (34)
#define AMPLITUDE_Y (34)
#define COMPENSATE (1)
#define ANALOG_XY (1)         // If set to (1), Joystick X/Y-position is converted to analog signal instead of a binary on/off-bit for each direction

#define LED_R_MAX_CURRENT  120
#define LED_G_MAX_CURRENT  160
#define LED_B_MAX_CURRENT  140


#define TG_VersionIdOnI2C (0xAA) // 0xAA denotes Pointer to extended Protocol Type 
#define SW_VersionIdOnI2C (0x08) // 0x01 Firmware Version - 01 denotes Version 2 + 01 = 3V0 Compatibility
#define HW_VersionIdOnI2C (0xAA) // 0xAA denotes invalid dummy value which is overloaded by life distinguished version of 0x2C for EMV-Version - 0xAA for Pointer to new protocol type

#define FIRMWARE_REV SW_VersionIdOnI2C   // Increment for all productive builds
#define	SETTING_VALID (0x2705)  // all Settings using this number should be compatible, increment it otherwise. (do not exceed valid Range form 1...65534)
//#define	SETTING_VALID (0x5161)  // all Settings using this number should be compatible, increment it otherwise. (do not exceed valid Range form 1...65534)
#define	SETTING_INVALID (0xffff)
#define	STORE_SETTINGS_TO_FLASH (true)
#define	READ_SETTINGS_FROM_FLASH (false)
#define BUTTON_PRESS_THRESHOLD (92)
#define BUTTON_RELEASE_THRESHOLD (95)
#define	LIMIT_POS (32767)
#define	LIMIT_NEG (-32767)
#define FAST_LARGE_XRAM_MODE (1) // don't activate this USB-Clock domain based XRAM-Buffer when EMC problems show up!

#define WATCHDOG_ACTIVE (1)	  // This protects firmware from unintended hanging routines
#define ARMED_CALIBRATION (1) // set to this to "0" to prevent calibration from being stored to flash
#define REDUNDANCY_ACTIVE (0) // If not set to (1), pushbutton function is evaluated solely from snapdome-switch signal seen on ADC_in of LED-Diver

#define LFOCLK_HZ 10000UL         // LFOSC default frequency, in Hz
#define WDT_TICKS (LFOCLK_HZ*1)   // ~1 second worth of WDT ticks

// Default system clock for UB3 is 24.5 MHz HFOSC divided by 8
#define SYSTEM_CLOCK_FREQUENCY  (24500000)

// WDT delay cycles as defined by Erratum WDT_E102 is 2 clock cycles
#define WDT_DELAY_CYCLES        2


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

uint16_t CalibrationSettings(bool DirectionToFlash);
void WDT0_delay(uint32_t systemClkFreq, uint32_t wdtClkFreq, uint32_t wdtClkCycles);


/////////////////////////////////////////////////////////////////////////////
// type definitions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////


static TLE493D_BaseStruct xdata TestSensorBase;
static TLE493D_BaseStruct xdata TestSensorSatellite;
static RollingAverageSignedFilterStruct xdata FilterCartridgeSX;
static RollingAverageSignedFilterStruct xdata FilterCartridgeSY;

//static RollingAverageFilterStruct xdata FilterCartridgeX;
//static RollingAverageFilterStruct xdata FilterCartridgeY;


struct NotchPos xdata NotchPositions[NOTCH_COUNT];
struct NotchBackpackPos xdata NotchBackpackPositions[NOTCH_COUNT];

bool ea_Val;
bool WdtReset = false;
bool CrcError = false;
uint16_t Crc16Value = 0x0000;


uint8_t  SMB_DATA_IN[SMB_NUM_BYTES_WR];
uint8_t  SMB_DATA_OUT[SMB_NUM_BYTES_RD];
bool SMB_DATA_READY = 0;
bool SMB_TRANSFER_ONGOING = 0;
bool OffCalib = false;

uint8_t I2C_Channel_Base = I2C_Channel_Base_100750;
uint8_t I2C_Channel_Satellite = I2C_Channel_Satellite_100750;


const int16_t ConstLagX = LAG_X * 256;
const int16_t ConstLagY = LAG_Y * 256;
const int16_t ConstAmplitudeX = (float)AMPLITUDE_X;
const int16_t ConstAmplitudeY = (float)AMPLITUDE_Y;


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

#if(WATCHDOG_ACTIVE == 1)
			SFRPAGE = 0x00;
			WDTCN = 0xA5; // feed watchdog
#else
		   // Disable watchdog with key sequence
		   SFRPAGE = 0x00;
		   WDTCN = 0xDE; //First key
		   WDTCN = 0xAD; //Second key
#endif

#if (FAST_LARGE_XRAM_MODE == (1))
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
#endif

}

//-----------------------------------------------------------------------------
uint16_t CalibrationSettings(bool DirectionToFlash) {
	uint8_t FirmwareCompare;
	uint16_t SettingRelease;
	XRAMADDR CurrentSetting;
	uint16_t Crc16 = 0;
	uint16_t CrcCompare = 0;
	uint16_t CheckValue = 0xffff;
	bool CrcError = false;

	FirmwareCompare = FIRMWARE_REV;

	SettingRelease = SETTING_VALID;
	CurrentSetting = START_ADDRESS_CALIB_FLASH;

	CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &SettingRelease, sizeof(SettingRelease), DirectionToFlash, &Crc16);

	if (SettingRelease == SETTING_VALID)
	{
		CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &FirmwareCompare, sizeof(FirmwareCompare), DirectionToFlash, &Crc16);

		if (FirmwareCompare == FIRMWARE_REV) {
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &TestSensorSatellite.yAxis, sizeof(TestSensorSatellite.yAxis), DirectionToFlash, &Crc16);
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &TestSensorSatellite.xAxis, sizeof(TestSensorSatellite.xAxis), DirectionToFlash, &Crc16);
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &TestSensorBase.xAxis, sizeof(TestSensorBase.xAxis), DirectionToFlash, &Crc16);
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &TestSensorBase.yAxis, sizeof(TestSensorBase.yAxis), DirectionToFlash, &Crc16);
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &NotchPositions, sizeof(NotchPositions), DirectionToFlash, &Crc16);
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &NotchBackpackPositions, sizeof(NotchBackpackPositions), DirectionToFlash, &Crc16);
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &TestSensorSatellite.tValue, sizeof(TestSensorSatellite.tValue), DirectionToFlash, &Crc16);
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &TestSensorBase.tValue, sizeof(TestSensorSatellite.tValue), DirectionToFlash, &Crc16);
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &Crc16, sizeof(Crc16),DirectionToFlash, &Crc16);
			if (DirectionToFlash)
				CrcCompare = Crc16;
			else
				CheckValue = Crc16;
			CurrentSetting = BiStoreSettingFLASH(CurrentSetting, (uint8_t*) &CrcCompare, sizeof(CrcCompare),DirectionToFlash, &Crc16);

			if (Crc16 == 0 )
				CrcError = !(CrcCompare == CheckValue);
			else
				CrcError = true;
			if (CurrentSetting == START_ADDRESS_CALIB_FLASH)
				return (SETTING_INVALID);
			if (CurrentSetting >=  (START_ADDRESS_CALIB_FLASH + SIZE_CALIB_FLASH))
				return (SETTING_INVALID);
			return CrcError ? SETTING_INVALID : SETTING_VALID;
		}
	} else
		SettingRelease = SETTING_VALID;	// Restore the Value overwritten by accident
	return (SETTING_INVALID);
}



void CheckForUpdate() {
	// Start the USB-Bootloader if C_BUS is LOW
	if (!PB_IN) {
		printf("\nUSB-LOADER STARTED...");
		// Write R0 and issue a software reset
		*((uint8_t*) 0x00) = 0xA5;
		RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET;
	}
}

void CheckForRecalibrate() {
	// Start the USB-Bootloader if C_BUS is LOW
	if (!PB_IN) {
		printf("\nUSB-LOADER STARTED...");
		// Write R0 and issue a software reset
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


uint8_t UpdateKeyState()
{
#define SWITCH_LIMIT (90)
	static uint8_t SwitchState = PB_ABSENT;
	uint8_t SwitchValue;
	if (LedGetAdc(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LedAdcInput_INT, &SwitchValue))
	{
		if (LedInitSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 120, 160, 140)) //Set Max Current per Color (Adequate for CREE RGB-LED)
			SwitchValue = (PB_IN)? 0xff : 0x00; //Use Redundant Information in case ADC evaluation and RESET causes problem
	}
	SwitchState = (SwitchState) ? (SwitchValue > SWITCH_LIMIT)? PB_ABSENT : PB_PRESENT : (SwitchValue > SWITCH_LIMIT)? PB_ABSENT : PB_NEW_EVENT;
	return (SwitchState);
}

void PrintLineUp(int8_t lines)
{
 	int8_t lineCount;
 	if (lines < 0)
 	{
 	 	for (lineCount = lines; lineCount; lineCount++)
 	 	{
 	 		printf("\n");
 	 	}
 	}
 	else
 	{
 	 	for (lineCount = lines; lineCount; lineCount--)
 	 	{
 	 		printf("\033[F");
 	 	}
 	} 	
}

void Delay(void)
{
   int16_t x;

   for (x = 0; x < 500; x)
   {
      x++;
   }
}

uint8_t ConsumeCmdNibble(void)
{
	uint8_t Nibble = 0x00;
	if (SMB_DATA_READY)
	{
		Nibble = SMB_DATA_IN[0] & 0xF0;
		SMB_DATA_READY = 0;
	}
	return Nibble;
}

uint8_t AdaptTarget(void)
{
	if (IsMeterAbsent(InitHallSensor(&TestSensorBase)))
	{
		I2C_Channel_Base = I2C_Channel_Base_100700;
		TestSensorBase.BusChannel = I2C_Channel_Base; // Base PCB 100700
		I2C_Channel_Satellite = I2C_Channel_Satellite_100700;
		TestSensorSatellite.BusChannel = I2C_Channel_Satellite; // Satellite PCB 100699
		VERS_100750 = IsMeterAbsent(InitHallSensor(&TestSensorBase));
	}
	else
	{
		VERS_100750 = !IsMeterAbsent(InitHallSensor(&TestSensorBase)); // Base PCB 100750 with Satellite PCB 100699
	}
	VERS_100750V8 = false;
	if (VERS_100750)
	{
		DBG_LED = 0;
		Delay();
		if ((VER_1 == 0) && (VER_2 == 0) && (VER_3 == 0))
		{
			DBG_LED = 1;
			Delay();
			if ((VER_1 == 1) && (VER_2 == 1) && (VER_3 == 1))
			{
				VERS_100750V8 = true;
			}
		}
	}
	
	return (VERS_100750) ? (VERS_100750V8) ? 0x02 : 01 : 0x00; // MSB set in Hardware-Version means PCB 100700
}

//-----------------------------------------------------------------------------

void main(void) {
#define M_PI acos(-1.0)
#define M_2PI 2*acos(-1.0)
#define TRIGGER_CHAR ('\n')
#define LO_LIMIT_Z_SENSOR (2000)
#define HI_LIMIT_Z_SENSOR (32767)
#define LO_LIMIT_Z_SENSOR_FLIP ( -32768)
#define HI_LIMIT_Z_SENSOR_FLIP ( -2000)
#define SLICE (float)((float)UINT_MAX / (float)NOTCH_COUNT)

#define COMMAND_INIT (0)
#define CALIB_STEP_KEY_1 (0x50)
#define CALIB_STEP_KEY_2 (0x60)
#define CALIB_STEP_KEY_3 (0x70)

#define BOOTLOADER_KEY_1 (0xD0)
#define BOOTLOADER_KEY_2 (0xE0)
#define BOOTLOADER_KEY_3 (0xF0)

#define CLEAR_KEY_1 (0x10)
#define CLEAR_KEY_2 (0x20)
#define CLEAR_KEY_3 (0x30)
#define CLEAR_KEY_4 (0x40)


	// Variables

	uint8_t serialNo[16];

//	volatile SI_UU16_t xdata Tilt_X_Value = { 0x0000 };
//	volatile SI_UU16_t xdata Tilt_Y_Value = { 0x0000 };
//	volatile SI_UU16_t xdata Tilt_Z_Value = { 0x0000 };
	uint8_t SW_Version = SW_VersionIdOnI2C;
	uint8_t PCB_Version = HW_VersionIdOnI2C;
	uint8_t TG_Version = TG_VersionIdOnI2C;
	
	bool CenterStep = true;
	bool MagnetSensorRegular = false;
	bool MagnetSensorGoofy = false;
	bool LedError = false;
	uint8_t CommandKey = COMMAND_INIT;
	uint16_t AngleAbs;
	uint16_t ActDelay = 0;
	uint8_t Notch;
	uint8_t AdcVoltageLED_R;
	uint8_t AdcVoltageLED_G;
	uint8_t AdcVoltageLED_B;
	uint8_t AdcVoltageOut;
	uint8_t AdcVoltageInt;
	uint8_t AdcVoltageDD;

	uint8_t InputVoltage;
	uint8_t xdata SwitchDirection = 0;
	uint8_t xdata SwitchState = 0;
	uint8_t xdata Deflection = 0;
	uint8_t ButtonState = PB_ABSENT;
	uint8_t index;
	uint16_t FirstNotchAngle = 0;
	uint8_t ErrorCase = 0;
	uint8_t ResetCause = 0;

	int16_t LastRawX = 0;
	int16_t LastRawY = 0;

	uint8_t DarkRed = 0, DarkGreen = 0, DarkBlue = 0, DarkCommand = 0;
	uint8_t ColorRed = 0, ColorGreen = 0, ColorBlue = 0, ColorCommand = 0;
	SI_UU16_t ConvertValue;
	uint8_t ThisNotch = 255, LastNotch = 255;
	uint8_t NotchIndex = 0;
	float DeflectionX, DeflectionY;
//	int32_t MeanPeakPosX, MeanPeakPosY, MeanPeakNegX, MeanPeakNegY, MeanCenterPosX, MeanCenterPosY, MeanCenterNegX, MeanCenterNegY;
//	uint16_t MeanAmplitude;

	ResetCause = RSTSRC;

	VDM0CN = VDM0CN_VDMEN__ENABLED;            // Enable VDD Monitor
	Delay ();                                  // Wait for VDD Monitor to stabilize
	RSTSRC = RSTSRC_PORSF__SET;                // Enable VDD Monitor as a reset source

	if (ResetCause & RSTSRC_PORSF__SET) // green = A power-on reset occurred.
	{
			ColorRed = 0;
			ColorGreen = 255;
			ColorBlue = 0;
	}
	else
	{
		if (ResetCause & RSTSRC_WDTRSF__SET)// blue = A watchdog timer overflow reset occurred.
		{
			ColorRed = 0;
			ColorGreen = 0;
			ColorBlue = 255;
		}
		else
		{
			if (ResetCause & RSTSRC_PINRSF__SET)// red = The RSTb pin caused the last reset.
			{
				ColorRed = 255;
				ColorGreen = 0;
				ColorBlue = 0;
			}
			else
			{
				if (ResetCause & RSTSRC_SWRSF__SET)// 1 11magenta = A software reset occurred.
				{
					ColorRed = 128;
					ColorGreen = 0;
					ColorBlue = 128;
				}
				else
				{
					if (ResetCause & RSTSRC_MCDRSF__SET)// yellow = A missing clock detector reset occurred.
					{
						ColorRed = 128;
						ColorGreen = 128;
						ColorBlue = 0;
					}
					else
					{
						if (ResetCause & RSTSRC_FERROR__SET)// cyan = A flash error reset occurred.
						{
							ColorRed = 0;
							ColorGreen = 128;
							ColorBlue = 128;
						}
						else
						{
							if (ResetCause & RSTSRC_C0RSEF__SET)// white =  A Comparator 0 reset occurred.
							{
								ColorRed = 85;
								ColorGreen = 85;
								ColorBlue = 85;
							}
							else
							{
								if (ResetCause & RSTSRC_USBRSF__SET)// pink = A USB0 reset occurred.
								{
									ColorRed = 128;
									ColorGreen = 64;
									ColorBlue = 64;
								}
								else
								{
									if (ResetCause)
									{
										ColorRed = 255;
										ColorGreen = 255;
										ColorBlue = 255;
									}
									else
									{
										ColorRed = 0;
										ColorGreen = 0;
										ColorBlue = 0;
									}
								}
							}
						}
					}
				}
			}
		}
	}


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
	BiStoreSettingFLASH(UUID_POS,(uint8_t*) &serialNo, sizeof(serialNo), READ_SETTINGS_FROM_FLASH, &Crc16Value);

	// initialization

    ClearAxisStuct(&TestSensorSatellite.xAxis); // axis unused
    ClearAxisStuct(&TestSensorSatellite.yAxis); // axis unused
    ClearAxisStuct(&TestSensorSatellite.zAxis); // axis unused
	TestSensorSatellite.BusAddress = SENSOR_TLE493D_W2B6_A1_ADDR; // Satellite PCB 100699
	TestSensorSatellite.BusChannel = I2C_Channel_Satellite; // Satellite PCB 100634
	TestSensorSatellite.retry = 0x00;
	TestSensorSatellite.FirstRun = true; // Init Routine will reset this after successful initialization
	TestSensorSatellite.Mode = TLE493D_ModeMasterControlled; // not speed optimized but rugged and with less power consumption
	TestSensorSatellite.InterruptDisabled = true; // as we have other circuits on the same bus, we keep it simple and save this way
    TestSensorSatellite.LowPowerPeriod = TLE493D_Rate97Hz; // Don't care in Master Controlled Mode
	TestSensorSatellite.DisableTemperature = false;
	TestSensorSatellite.Angular = false;
	TestSensorSatellite.TriggerOption = TLE493D_TriggerAfterRef05; // Don't care in Protocol type TwoByteRead
	TestSensorSatellite.ShortRangeSensitivity = true;
	TestSensorSatellite.TempCompensation = TLE493D_TempCompP350; //was TempCompN1500 before
	TestSensorSatellite.CollisionAvoidanceDisabled = false; // slow but save and rugged
	TestSensorSatellite.ProtocolOneByteReadEnabled = false; // keep it simple (on default) as otherwise protocol has to be changed on the fly

    ClearAxisStuct(&TestSensorBase.xAxis); // axis unused
    ClearAxisStuct(&TestSensorBase.yAxis); // axis unused
    ClearAxisStuct(&TestSensorBase.zAxis); // axis unused
	TestSensorBase.BusAddress = SENSOR_TLE493D_W2B6_A1_ADDR; //  Base PCB 100700
	TestSensorBase.BusChannel = I2C_Channel_Base; // Satellite PCB 100634
	TestSensorBase.retry = 0x00;
	TestSensorBase.FirstRun = true; // Init Routine will reset this after successful initialization
	TestSensorBase.Mode = TLE493D_ModeMasterControlled; // not speed optimized but rugged and with less power consumption
	TestSensorBase.InterruptDisabled = true; // as we have other circuits on the same bus, we keep it simple and save this way
    TestSensorBase.LowPowerPeriod = TLE493D_Rate97Hz; // Don't care in Master Controlled Mode
	TestSensorBase.DisableTemperature = false;
	TestSensorBase.Angular = false;
	TestSensorBase.TriggerOption = TLE493D_TriggerAfterRef05; // Don't care in Protocol type TwoByteRead
	TestSensorBase.ShortRangeSensitivity = true;
	TestSensorBase.TempCompensation = TLE493D_TempCompP350; //was TempCompN1500 before
	TestSensorBase.CollisionAvoidanceDisabled = false; // slow but save and rugged
	TestSensorBase.ProtocolOneByteReadEnabled = false; // keep it simple (on default) as otherwise protocol has to be changed on the fly
	 // Low on Signal C-Bus at Startup causes recalibration
#if (REDUNDANCY_ACTIVE == 1)
	if (CalibrationSettings(READ_SETTINGS_FROM_FLASH) != SETTING_VALID)
#else
	if ((!PB_IN) || (CalibrationSettings(READ_SETTINGS_FROM_FLASH) != SETTING_VALID))
#endif
	{	// Enter Calibration Mode
		enter_ApplictaionMode_from_RESET();
											// be set for prints to occur
		PCB_Version  = AdaptTarget(); // Old or new Hardware (100700 = 0) or (100750 = 1) or (100750V8 = 2) PCB
		DBG_LED = 1; //Show red LED in Calibration Mode

		LedInitSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LED_R_MAX_CURRENT, LED_G_MAX_CURRENT, LED_B_MAX_CURRENT);//Set Max Current per Color (Adequate for CREE RGB-LED)

#if (REDUNDANCY_ACTIVE != 1)
		while (!PB_IN)
#endif
		{
			CountDelay(200);
		}

		ResetHallSensors(I2C_Channel_Base); // This affects all Infineon TLV493D Magnet Sensors on first I2C Bus!
		ResetHallSensors(I2C_Channel_Satellite); // This affects all Infineon TLV493D Magnet Sensors on first I2C Bus!

		while (IsMeterError(InitHallSensor(&TestSensorSatellite)))
		{
			ResetHallSensors(I2C_Channel_Satellite); // This affects all Infineon TLV493D Magnet Sensors on first I2C Bus!
		}; // Init Infineon Magnetsensor
		ClearAxisStuct(&TestSensorSatellite.yAxis);
		ClearAxisStuct(&TestSensorSatellite.xAxis);
		ClearAxisStuct(&TestSensorBase.xAxis);
		ClearAxisStuct(&TestSensorBase.yAxis);

//		LedInitSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 120, 160, 140); //Set Max Current per Color (Adequate for CREE RGB-LED)

		SMB_DATA_OUT[SMB_TG_VERSION_POS] = CALIB_STEP_KEY_1;  // This position shows actual Calib Step


		UpdateKeyState();  	// dummy read to initialize adc converter
		while (IsMeterError(InitHallSensor(&TestSensorBase)))
		{
			ResetHallSensors(I2C_Channel_Base); // This affects all Infineon TLV493D Magnet Sensors on first I2C Bus!
		}; // Init Infineon Magnetsensor


		LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 255,255,255,0);  // show all leds at startup -> white until kex pressed


		do
		{
			LoopDelay3ms();
			LoopDelay3ms();
			LoopDelay3ms();
			ButtonState = UpdateKeyState();
		}
		while ((ButtonState != PB_NEW_EVENT) && (ConsumeCmdNibble() != CALIB_STEP_KEY_1));


		LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, ColorRed,ColorGreen,ColorBlue,0);  // show reset source at startup

		SMB_DATA_OUT[SMB_TG_VERSION_POS] = CALIB_STEP_KEY_2;  // This position shows actual Calib Step


		do
		{
			LoopDelay3ms();
			LoopDelay3ms();
			LoopDelay3ms();
			ButtonState = UpdateKeyState();

		}
		while ((ButtonState != PB_NEW_EVENT) && (ConsumeCmdNibble() != CALIB_STEP_KEY_2));

//		LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, ((!(bool) ButtonState) ? 0 : 255),0,0,0);

		GetHallSensorOnce(&TestSensorBase);

		MagnetSensorGoofy = ((TestSensorBase.zAxis.RawValue.s16 >= LO_LIMIT_Z_SENSOR) && (TestSensorBase.zAxis.RawValue.s16 <= HI_LIMIT_Z_SENSOR));
		MagnetSensorRegular = ((TestSensorBase.zAxis.RawValue.s16 >= LO_LIMIT_Z_SENSOR_FLIP) && (TestSensorBase.zAxis.RawValue.s16 <= HI_LIMIT_Z_SENSOR_FLIP));

		if (MagnetSensorGoofy == MagnetSensorRegular) // ambiguous result
		{
//			LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 170, 85, 0, LedSegment_All); // yellow -> magnet field not in right range
			ColorRed = 170;
			ColorGreen = 85;
			ColorBlue = 0;

		}
		else
		{
			if (MagnetSensorGoofy)
			{
				TestSensorBase.xAxis.SwapSign = MagnetSensorGoofy; // calibrate all axis with changed signs from now on. there
				TestSensorBase.yAxis.SwapSign = MagnetSensorGoofy; // is no way back
			//	LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 0, 85, 85, LedSegment_All); // cyan -> joystick magnet mounted the right way
				ColorRed = 0;
				ColorGreen = 85;
				ColorBlue = 85;

			}
			else
			{
//				LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 85, 0, 85, LedSegment_All); // violet -> joystick magnet mounted the flipped way
				ColorRed = 85;
				ColorGreen = 0;
				ColorBlue = 85;
			}
		}


		LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, ((!(bool) ButtonState) ? ColorRed : 255),((!(bool) ButtonState) ? ColorGreen : 255),((!(bool) ButtonState) ? ColorBlue : 255),0);

//		LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, ColorRed,ColorGreen,ColorBlue,0);  // show reset source at startup



//		LedError = LedSetSegmentRGB(I2C_Channel_Satellite,BASE_ADDR_ILLUM, 255, 255, 255, LedSegment_All); // red -> calib value collector loop
		for (index = 0; index < 50; index ++)
		{

			GetMagnetSensor(&TestSensorSatellite);
			GetHallSensorOnce(&TestSensorBase);
			AngleAbs = (uint16_t) (32768.0	* ((atan2(TestSensorSatellite.xAxis.RawValue.s16,	TestSensorSatellite.yAxis.RawValue.s16) / M_PI) + 1.0));

#if (COMPENSATE == 1)

			TestSensorBase.xAxis.RawValue.s16 += (int16_t)(sin((float)((float)(AngleAbs + ConstLagX) / 256.0F * 2.0F * M_PI)) * (float)ConstAmplitudeX);
            TestSensorBase.yAxis.RawValue.s16 += (int16_t)(sin((float)((float)(AngleAbs + ConstLagY) / 256.0F * 2.0F * M_PI)) * (float)ConstAmplitudeY);
#endif
			ThisNotch = getNotch(AngleAbs);

			if (LastNotch == ThisNotch)
			{
				NotchBackpackPositions[ThisNotch].MaxCalX = (NotchBackpackPositions[ThisNotch].MaxCalX < TestSensorBase.xAxis.RawValue.s16) ? TestSensorBase.xAxis.RawValue.s16 : NotchBackpackPositions[ThisNotch].MaxCalX;
				NotchBackpackPositions[ThisNotch].MaxCalY = (NotchBackpackPositions[ThisNotch].MaxCalY < TestSensorBase.yAxis.RawValue.s16) ? TestSensorBase.yAxis.RawValue.s16 : NotchBackpackPositions[ThisNotch].MaxCalY;
				NotchBackpackPositions[ThisNotch].MinCalX = (NotchBackpackPositions[ThisNotch].MinCalX > TestSensorBase.xAxis.RawValue.s16) ? TestSensorBase.xAxis.RawValue.s16 : NotchBackpackPositions[ThisNotch].MinCalX;
				NotchBackpackPositions[ThisNotch].MinCalY = (NotchBackpackPositions[ThisNotch].MinCalY > TestSensorBase.yAxis.RawValue.s16) ? TestSensorBase.yAxis.RawValue.s16 : NotchBackpackPositions[ThisNotch].MinCalY;
			}
			LastNotch = ThisNotch;
		}

		FirstNotchAngle = AngleAbs - ((float)((uint16_t)((float)AngleAbs / SLICE)) * SLICE);


//		MeanAmplitude = 35; //(MeanPeakPosX + MeanPeakNegY)  + MeanPeakNegX + MeanPeakNegY;

		for (index = 0; index < NOTCH_COUNT; index ++)
		{
			addNotchComplete((uint16_t)(FirstNotchAngle + (uint16_t)((float)((float)index * SLICE))), !index);
			addNotchBackpack(TestSensorBase.xAxis.RawValue.s16,TestSensorBase.yAxis.RawValue.s16, !index);
/*
			NotchBackpackPositions[index].SlopePosX = (sin(index * (M_2PI /20) - (FirstNotchAngle / UINT_MAX * M_2PI) + M_2PI / 4) * MeanAmplitude);
			NotchBackpackPositions[index].SlopeNegX = (sin(index * (M_2PI /20) - (FirstNotchAngle / UINT_MAX * M_2PI) + M_2PI / 4) * MeanAmplitude);
			NotchBackpackPositions[index].SlopePosY = (sin(index * (M_2PI /20) - (FirstNotchAngle / UINT_MAX * M_2PI)) * MeanAmplitude);
			NotchBackpackPositions[index].SlopeNegY = (sin(index * (M_2PI /20) - (FirstNotchAngle / UINT_MAX * M_2PI)) * MeanAmplitude);
*/
		}

		if (validateNotchList())
		{
/*
			do
			{
				ButtonState = UpdateKeyState();
			}
			while (ButtonState != PB_NEW_EVENT);
*/


			LedError = (I2C_Channel_Satellite,BASE_ADDR_ILLUM, 255, 0, 0, LedSegment_All); // blue -> calib value collector loop finished
		}


		InitTrendSwitch();

		TestSensorBase.xAxis.SlopeNeg = TestSensorBase.xAxis.SlopeNeg * 2.18;
		TestSensorBase.yAxis.SlopeNeg = TestSensorBase.yAxis.SlopeNeg * 2.18;
		TestSensorBase.xAxis.SlopePos = TestSensorBase.xAxis.SlopePos * 2.18;
		TestSensorBase.yAxis.SlopePos = TestSensorBase.yAxis.SlopePos * 2.18;

//		LedInitSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 120, 160, 140); //Set Max Current per Color (Adequate for CREE RGB-LED)
		do
		{
			/*

			if (SMB_DATA_READY)
		    {   // SMBus data received

		    	ColorCommand = SMB_DATA_IN[0];
		    	ColorRed = SMB_DATA_IN[1];
		    	ColorGreen = SMB_DATA_IN[2];
		    	ColorBlue = SMB_DATA_IN[3];

		    	switch (ColorCommand & 0xF0)
		    	{
					case 0x90:
						if (CalibCounter == CALIB_INITSTEP)
							CalibCounter = BOOTLOADER_1;
						else
							CalibCounter = CALIB_INITSTEP;
						break;
					case 0xA0:
						if (CalibCounter == BOOTLOADER_1)
							CalibCounter = BOOTLOADER_2;
						else
							CalibCounter = CALIB_INITSTEP;
						break;
					case 0xB0:
						if (CalibCounter == BOOTLOADER_2)
							CalibCounter = BOOTLOADER_3;
						else
							CalibCounter = CALIB_INITSTEP;
						break;
					case 0xC0:
						if (CalibCounter == BOOTLOADER_3)
							CalibCounter = BOOTLOADER_START;
						else
							CalibCounter = CALIB_INITSTEP;
						break;
					default:
						CalibCounter = CALIB_INITSTEP;
						break;
				}

		    	ErrorCase += (LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, ColorRed, ColorGreen, ColorBlue, ColorCommand)) ? 1 : 0;
		    	SMB_DATA_READY = 0;
		    }
*/
			ButtonState = UpdateKeyState();
			SMB_DATA_OUT[SMB_NOTCH_POS] = 0; 	   // Notch
			SMB_DATA_OUT[SMB_BUTTON_POS] = ButtonState & PB_EDGE; // Button State
			SMB_DATA_OUT[SMB_DEFLECTION_POS] = 0;  // Direction
			SMB_DATA_OUT[SMB_ANGLE_POS] = 0; // Rotation Angle
			SMB_DATA_OUT[SMB_SUPPLY_POS] = 0; // Supply Voltage
			SMB_DATA_OUT[SMB_TEMPBASE_POS] = 0; // Temperature Base
			SMB_DATA_OUT[SMB_TEMPHANDLE_POS] = 0; // Temperature Handle
//			SMB_DATA_OUT[SMB_TG_VERSION_POS] = 0;  // Firmware and Hardware Version
			SMB_DATA_OUT[SMB_UID_01_POS] = 0xde;
			SMB_DATA_OUT[SMB_UID_02_POS] = 0xad;
			SMB_DATA_OUT[SMB_UID_03_POS] = 0xbe;
			SMB_DATA_OUT[SMB_UID_04_POS] = 0xef;
			SMB_DATA_OUT[SMB_UID_05_POS] = 0;
			SMB_DATA_OUT[SMB_UID_06_POS] = 0;
			SMB_DATA_OUT[SMB_UID_07_POS] = 0;
			SMB_DATA_OUT[SMB_UID_08_POS] = 0;
			SMB_DATA_OUT[SMB_UID_09_POS] = 0;
			SMB_DATA_OUT[SMB_UID_10_POS] = 0;
			SMB_DATA_OUT[SMB_UID_11_POS] = 0;
			SMB_DATA_OUT[SMB_UID_12_POS] = 0;
			SMB_DATA_OUT[SMB_UID_13_POS] = 0;
			SMB_DATA_OUT[SMB_UID_14_POS] = 0;
			SMB_DATA_OUT[SMB_UID_15_POS] = 0;
			SMB_DATA_OUT[SMB_UID_16_POS] = 0;
			SMB_DATA_OUT[SMB_SW_VERSION_POS] = 0;  // Firmware and Hardware Version
			SMB_DATA_OUT[SMB_HW_VERSION_POS] = 0;  // Firmware and Hardware Version
			SMB_DATA_OUT[SMB_ROT_ENTRY_POS_H] = 0;
			SMB_DATA_OUT[SMB_ROT_ENTRY_POS_L] = 0;
			SMB_DATA_OUT[SMB_ROT_CENTER_POS_H] = 0;
			SMB_DATA_OUT[SMB_ROT_CENTER_POS_L] = 0;
			SMB_DATA_OUT[SMB_ROT__DEV_POS_H] = 0;
			SMB_DATA_OUT[SMB_ROT__DEV_POS_L] = 0;
			SMB_DATA_OUT[SMB_ROT_ABS_POS_H] = 0;
			SMB_DATA_OUT[SMB_ROT_ABS_POS_L] = 0;
			SMB_DATA_OUT[SMB_ROT_EXIT_POS_H] = 0;
			SMB_DATA_OUT[SMB_ROT_EXIT_POS_L] = 0;
			SMB_DATA_OUT[SMB_VOLTAGE_LED_R] = 0;
			SMB_DATA_OUT[SMB_VOLTAGE_LED_G] = 0;
			SMB_DATA_OUT[SMB_VOLTAGE_LED_B] = 0;
			SMB_DATA_OUT[SMB_VOLTAGE_OUT] = 0;
			SMB_DATA_OUT[SMB_VOLTAGE_SUP] = 0;
			SMB_DATA_OUT[SMB_VOLTAGE_PBIN] = 0;  // Firmware and Hardware Version
			SMB_DATA_OUT[SMB_RAW_BAS_X_H] = 0;
			SMB_DATA_OUT[SMB_RAW_BAS_X_L] = 0;
			SMB_DATA_OUT[SMB_RAW_BAS_Y_H] = 0;
			SMB_DATA_OUT[SMB_RAW_BAS_Y_L] = 0;
			SMB_DATA_OUT[SMB_RAW_BAS_Z_H] = 0;
			SMB_DATA_OUT[SMB_RAW_BAS_Z_L] = 0;
			SMB_DATA_OUT[SMB_RAW_ROT_X_H] = 0;
			SMB_DATA_OUT[SMB_RAW_ROT_X_L] = 0;
			SMB_DATA_OUT[SMB_RAW_ROT_Y_H] = 0;
			SMB_DATA_OUT[SMB_RAW_ROT_Y_L] = 0;
			SMB_DATA_OUT[SMB_RAW_ROT_Z_H] = 0;
			SMB_DATA_OUT[SMB_RAW_ROT_Z_L] = 0;
			SMB_DATA_OUT[SMB_CRC16_H] = 0;
			SMB_DATA_OUT[SMB_CRC16_L] = 0;


			// Start the USB-Bootloader if C_BUS is LOW
			CheckForUpdate();


			GetMagnetSensor(&TestSensorSatellite);

			AdjustAxisGlobal(&TestSensorSatellite.yAxis);
			AdjustAxisGlobal(&TestSensorSatellite.xAxis);

//			ButtonState = UpdateKeyState();
//			LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, ((!(bool) ButtonState) ? 0 : 255),0,0,0);

			AngleAbs = (uint16_t) (32768.0	* ((atan2(TestSensorSatellite.xAxis.RawValue.s16,	TestSensorSatellite.yAxis.RawValue.s16) / M_PI) + 1.0));
			ThisNotch = getNotch(AngleAbs);

			// Get Data from TLV493D as x/y switching Joystick
			GetHallSensorOnce(&TestSensorBase);

#if (COMPENSATE == 1)

			TestSensorBase.xAxis.RawValue.s16 += (int16_t)(sin((float)((float)(AngleAbs + ConstLagX) / 256.0F * 2.0F * M_PI)) * (float)ConstAmplitudeX);
            TestSensorBase.yAxis.RawValue.s16 += (int16_t)(sin((float)((float)(AngleAbs + ConstLagY) / 256.0F * 2.0F * M_PI)) * (float)ConstAmplitudeY);
#endif

			AdjustAxisCenterbalancedClamped(&TestSensorBase.xAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!
		    AdjustAxisCenterbalancedClamped(&TestSensorBase.yAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!

			// Convert To SwitchState
			SwitchDirection = combineAxisToTrendSwitch(&TestSensorBase.xAxis, &TestSensorBase.yAxis);

			// Get Data from TLV493D as rotary Dial (Off-Axis-Configuration)



			if (LastNotch == ThisNotch)
			{
				NotchBackpackPositions[ThisNotch].MaxCalX = (NotchBackpackPositions[ThisNotch].MaxCalX < TestSensorBase.xAxis.RawValue.s16) ? TestSensorBase.xAxis.RawValue.s16 : NotchBackpackPositions[ThisNotch].MaxCalX;
				NotchBackpackPositions[ThisNotch].MaxCalY = (NotchBackpackPositions[ThisNotch].MaxCalY < TestSensorBase.yAxis.RawValue.s16) ? TestSensorBase.yAxis.RawValue.s16 : NotchBackpackPositions[ThisNotch].MaxCalY;
				NotchBackpackPositions[ThisNotch].MinCalX = (NotchBackpackPositions[ThisNotch].MinCalX > TestSensorBase.xAxis.RawValue.s16) ? TestSensorBase.xAxis.RawValue.s16 : NotchBackpackPositions[ThisNotch].MinCalX;
				NotchBackpackPositions[ThisNotch].MinCalY = (NotchBackpackPositions[ThisNotch].MinCalY > TestSensorBase.yAxis.RawValue.s16) ? TestSensorBase.yAxis.RawValue.s16 : NotchBackpackPositions[ThisNotch].MinCalY;

				for (index = 0; index < NOTCH_COUNT; index ++)
				{

					NotchBackpackPositions[index].MaxCalX = NotchBackpackPositions[ThisNotch].MaxCalX;
					NotchBackpackPositions[index].MaxCalY = NotchBackpackPositions[ThisNotch].MaxCalY;
					NotchBackpackPositions[index].MinCalX = NotchBackpackPositions[ThisNotch].MinCalX;
					NotchBackpackPositions[index].MinCalY = NotchBackpackPositions[ThisNotch].MinCalY;
				}
			}
			LastNotch = ThisNotch;

			/*
			MeanPeakPosX = 0;
			MeanPeakPosY = 0;
			MeanPeakNegX = 0;
			MeanPeakNegY = 0;
			MeanCenterPosX = 0;
			MeanCenterPosY = 0;
			MeanCenterNegX = 0;
			MeanCenterNegY = 0;

			for (NotchIndex = 0; NotchIndex < NOTCH_COUNT; NotchIndex++)
			{
				MeanPeakPosX += (int32_t)NotchBackpackPositions[NotchIndex].MaxCalX;
				MeanPeakPosY += (int32_t)NotchBackpackPositions[NotchIndex].MaxCalY;
				MeanPeakNegX += (int32_t)NotchBackpackPositions[NotchIndex].MinCalX;
				MeanPeakNegY += (int32_t)NotchBackpackPositions[NotchIndex].MinCalY;
			}

			MeanPeakPosX /= NOTCH_COUNT;
			MeanPeakPosY /= NOTCH_COUNT;
			MeanPeakNegX /= NOTCH_COUNT;
			MeanPeakNegY /= NOTCH_COUNT;
			MeanCenterPosX /= NOTCH_COUNT;
			MeanCenterPosY /= NOTCH_COUNT;
			MeanCenterNegX /= NOTCH_COUNT;
			MeanCenterNegY /= NOTCH_COUNT;
			*/

			if (TestSensorBase.xAxis.RawValue.s16 >= NotchBackpackPositions[ThisNotch].CenterCalX)
			{
				DeflectionX = 100.0f / (((float)NotchBackpackPositions[ThisNotch].MaxCalX - (float)NotchBackpackPositions[ThisNotch].CenterCalX) / ((float)TestSensorBase.xAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalX));
			}
			else
			{
				DeflectionX = -100.0f /(((float)NotchBackpackPositions[ThisNotch].MinCalX - (float)NotchBackpackPositions[ThisNotch].CenterCalX) / ((float)TestSensorBase.xAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalX));
			}

			if (TestSensorBase.yAxis.RawValue.s16 >= NotchBackpackPositions[ThisNotch].CenterCalY)
			{
				DeflectionY = 100.0f /(((float)NotchBackpackPositions[ThisNotch].MaxCalY - (float)NotchBackpackPositions[ThisNotch].CenterCalY) / ((float)TestSensorBase.yAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalY));
			}
			else
			{
				DeflectionY = -100.0f /(((float)NotchBackpackPositions[ThisNotch].MinCalY - (float)NotchBackpackPositions[ThisNotch].CenterCalY) / ((float)TestSensorBase.yAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalY));
			}

			SMB_DATA_OUT[SMB_TG_VERSION_POS] = CALIB_STEP_KEY_3;  // This position shows actual Calib Step
		}
		while ((UpdateKeyState() != PB_NEW_EVENT)  && (ConsumeCmdNibble() != CALIB_STEP_KEY_3));

//		LedInitSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 120, 160, 140); //Set Max Current per Color (Adequate for CREE RGB-LED)

#if(ARMED_CALIBRATION == 1)
		if (ClearAllSettingsFLASH(START_ADDRESS_CALIB_FLASH, SIZE_CALIB_FLASH))
				CalibrationSettings(STORE_SETTINGS_TO_FLASH);
#endif
		DBG_LED = 0; //no red LED in normal operation Mode denotes: Calibration Stored
		RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET; // software reset
	}
	else
	{	// Enter Application Mode

		//////////////////////////////////////////////////// BEGIN SMB ///////////////////
		enter_ApplictaionMode_from_RESET(); // Setup MCU Peripherials

#if(WATCHDOG_ACTIVE == 1)
	SFRPAGE = 0x00;

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

        PCB_Version = AdaptTarget(); // Old or new Hardware 100700 or 100750 PCB
		DBG_LED = 0; //no red LED in normal operation Mode

		SignedFilterReset(&FilterCartridgeSX);
		SignedFilterReset(&FilterCartridgeSY);

//		FilterReset(&FilterCartridgeX);
//		FilterReset(&FilterCartridgeY);

//		index = 0;
//		do
//		{
//			index++;
//			SignedFilter(index, &FilterCartridgeSX);
//			SignedFilter(index, &FilterCartridgeSY);
//		}
//		while (!(SignedFilterInitialized(&FilterCartridgeSX) && SignedFilterInitialized(&FilterCartridgeSY)));
/*
 		if (FilterCartridgeX.FilterSum == FilterCartridgeY.FilterSum)
		{
			if (FilterReadonly(&FilterCartridgeX) == FilterTake(&FilterCartridgeX))
			{
				if (FilterReadonly(&FilterCartridgeY) == FilterTake(&FilterCartridgeY))
				{
					FilterCartridgeX.FilterSum = FilterCartridgeY.FilterSum;
				}
			}
		}
*/
    	for (index = 0; index < SMB_NUM_BYTES_RD; index++)
		{
			SMB_DATA_OUT[index] = 0xff;
		}

		ResetHallSensors(I2C_Channel_Satellite); // This affects all Infineon TLV493D Magnet Sensors on first I2C Bus!
		while (IsMeterError(InitHallSensor(&TestSensorSatellite))); // Init Infineon Magnetsensor

		ResetHallSensors(I2C_Channel_Base); // This affects all Infineon TLV493D Magnet Sensors on 2nd I2C Bus!
		while (IsMeterError(InitHallSensor(&TestSensorBase)));

		LedInitSetMaxCurrent(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LED_R_MAX_CURRENT, LED_G_MAX_CURRENT, LED_B_MAX_CURRENT);//Set Max Current per Color (Adequate for CREE RGB-LED)
		LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0,0,0,0); // Run for the first time



		InitTrendSwitch();

		TestSensorBase.xAxis.SlopeNeg = TestSensorBase.xAxis.SlopeNeg * 2.18;
		TestSensorBase.yAxis.SlopeNeg = TestSensorBase.yAxis.SlopeNeg * 2.18;
		TestSensorBase.xAxis.SlopePos = TestSensorBase.xAxis.SlopePos * 2.18;
		TestSensorBase.yAxis.SlopePos = TestSensorBase.yAxis.SlopePos * 2.18;

		while (1) //(VERS_100750 && AUX_E_100750)
		{

			ErrorCase = 0;
			ErrorCase += (GetMagnetSensor(&TestSensorSatellite)) ? 1 : 0;

			AdjustAxisGlobal(&TestSensorSatellite.yAxis);
			AdjustAxisGlobal(&TestSensorSatellite.xAxis);

#if(REDUNDANCY_ACTIVE == 1)

			fzAxis = (200.0 / 32768.0) * sqrt((float) (((int32_t) TestSensorSatellite.xAxis.FinalValue * (int32_t) TestSensorSatellite.xAxis.FinalValue) + ((int32_t) TestSensorSatellite.yAxis.FinalValue * (int32_t) TestSensorSatellite.yAxis.FinalValue)));

			if (ButtonState)
			{
				ButtonState = (BUTTON_RELEASE_THRESHOLD > fzAxis)? 1 : 0;
			}
			else
			{
				ButtonState = (BUTTON_PRESS_THRESHOLD > fzAxis)? 3 : 0;
			}
#endif

	// -> ButtonState
			AngleAbs = (uint16_t) (32768.0	* ((atan2(TestSensorSatellite.xAxis.RawValue.s16,	TestSensorSatellite.yAxis.RawValue.s16) / M_PI) + 1.0));


			// Get Data from switching Joystick
			ErrorCase += (GetMagnetSensor(&TestSensorBase)) ? 1 : 0;

#if (COMPENSATE == 1)
			TestSensorBase.xAxis.RawValue.s16 += (int16_t)(sin((float)((float)(AngleAbs + ConstLagX) / 256.0F * 2.0F * M_PI)) * (float)ConstAmplitudeX);
            TestSensorBase.yAxis.RawValue.s16 += (int16_t)(sin((float)((float)(AngleAbs + ConstLagY) / 256.0F * 2.0F * M_PI)) * (float)ConstAmplitudeY);
#endif

			LastRawX = TestSensorBase.xAxis.RawValue.s16;
			LastRawY = TestSensorBase.yAxis.RawValue.s16;

			AdjustAxisCenterbalancedClamped(&TestSensorBase.xAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!
		    AdjustAxisCenterbalancedClamped(&TestSensorBase.yAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!

			if (TestSensorBase.xAxis.RawValue.s16 >= NotchBackpackPositions[ThisNotch].CenterCalX)
			{
				DeflectionX = 32767.0f / (((float)NotchBackpackPositions[ThisNotch].MaxCalX - (float)NotchBackpackPositions[ThisNotch].CenterCalX) / ((float)TestSensorBase.xAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalX));
				TestSensorBase.xAxis.FinalValue.s16 = (DeflectionX >= 32767.0f) ? 32767 : (int16_t)DeflectionX;
			}
			else
			{
				DeflectionX = -32767.0f /(((float)NotchBackpackPositions[ThisNotch].MinCalX - (float)NotchBackpackPositions[ThisNotch].CenterCalX) / ((float)TestSensorBase.xAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalX));
				TestSensorBase.xAxis.FinalValue.s16 = (DeflectionX <= -32767.0f) ? -32767 : (int16_t)DeflectionX;
			}

			if (TestSensorBase.yAxis.RawValue.s16 >= NotchBackpackPositions[ThisNotch].CenterCalY)
			{
				DeflectionY = 32767.0f /(((float)NotchBackpackPositions[ThisNotch].MaxCalY - (float)NotchBackpackPositions[ThisNotch].CenterCalY) / ((float)TestSensorBase.yAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalY));
				TestSensorBase.yAxis.FinalValue.s16 = (DeflectionY >= 32767.0f) ? 32767 : (int16_t)DeflectionY;
			}
			else
			{
				DeflectionY = -32767.0f /(((float)NotchBackpackPositions[ThisNotch].MinCalY - (float)NotchBackpackPositions[ThisNotch].CenterCalY) / ((float)TestSensorBase.yAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalY));
				TestSensorBase.yAxis.FinalValue.s16 = (DeflectionY <= -32767.0f) ? -32767 : (int16_t)DeflectionY;
			}

		    SwitchDirection = combineAxisToTrendSwitch(&TestSensorBase.xAxis, &TestSensorBase.yAxis);
//		    SwitchDirection = combineAxisToTrend(&TestSensorBase.xAxis, &TestSensorBase.yAxis, true);


			TestSensorBase.xAxis.RawValue.s16 = (int16_t)SignedFilter( TestSensorBase.xAxis.RawValue.s16, &FilterCartridgeSX);
		    TestSensorBase.yAxis.RawValue.s16 = (int16_t)SignedFilter( TestSensorBase.yAxis.RawValue.s16, &FilterCartridgeSY);

			AdjustAxisCenterbalancedClamped(&TestSensorBase.xAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!
		    AdjustAxisCenterbalancedClamped(&TestSensorBase.yAxis, LIMIT_POS, LIMIT_NEG); // Allow only values in range that can be mirrored without overflow!

		    // Convert To SwitchState
			//SwitchDirection = combineAxisToTrend(&TestSensorBase.xAxis, &TestSensorBase.yAxis);
			// -> SwitchDirection

			// Get Data from rotary Dial (On-Axis-Configuration)
			// -> Angle

			ThisNotch = getNotch(AngleAbs);
			Notch = (NOTCH_COUNT - ThisNotch);
	// -> Notch

			if (TestSensorBase.xAxis.RawValue.s16 >= NotchBackpackPositions[ThisNotch].CenterCalX)
			{
				DeflectionX = 32767.0f / (((float)NotchBackpackPositions[ThisNotch].MaxCalX - (float)NotchBackpackPositions[ThisNotch].CenterCalX) / ((float)TestSensorBase.xAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalX));
				TestSensorBase.xAxis.FinalValue.s16 = (DeflectionX >= 32767.0f) ? 32767 : (int16_t)DeflectionX;
			}
			else
			{
				DeflectionX = -32767.0f /(((float)NotchBackpackPositions[ThisNotch].MinCalX - (float)NotchBackpackPositions[ThisNotch].CenterCalX) / ((float)TestSensorBase.xAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalX));
				TestSensorBase.xAxis.FinalValue.s16 = (DeflectionX <= -32767.0f) ? -32767 : (int16_t)DeflectionX;
			}

			if (TestSensorBase.yAxis.RawValue.s16 >= NotchBackpackPositions[ThisNotch].CenterCalY)
			{
				DeflectionY = 32767.0f /(((float)NotchBackpackPositions[ThisNotch].MaxCalY - (float)NotchBackpackPositions[ThisNotch].CenterCalY) / ((float)TestSensorBase.yAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalY));
				TestSensorBase.yAxis.FinalValue.s16 = (DeflectionY >= 32767.0f) ? 32767 : (int16_t)DeflectionY;
			}
			else
			{
				DeflectionY = -32767.0f /(((float)NotchBackpackPositions[ThisNotch].MinCalY - (float)NotchBackpackPositions[ThisNotch].CenterCalY) / ((float)TestSensorBase.yAxis.RawValue.s16 - (float)NotchBackpackPositions[ThisNotch].CenterCalY));
				TestSensorBase.yAxis.FinalValue.s16 = (DeflectionY <= -32767.0f) ? -32767 : (int16_t)DeflectionY;
			}

			if (SMB_DATA_READY)
		    {   // SMBus data received
				ADC0CN1 = (ADC0CN1 & ~ADC0CN1_ADCM__FMASK) | ADC0CN1_ADCM__ADBUSY;
				// Clear the conversion complete flag
				ADC0CN0_ADINT = 0;
				// Start a conversion by setting ADBUSY
				ADC0CN0_ADBUSY = 1;

		    	ColorCommand = SMB_DATA_IN[0];
		    	ColorRed = SMB_DATA_IN[1];
		    	ColorGreen = SMB_DATA_IN[2];
		    	ColorBlue = SMB_DATA_IN[3];

		    	switch (ColorCommand & 0xF0)
		    	{
					case CLEAR_KEY_1:
						if (CommandKey == COMMAND_INIT)
						{
					    	ColorRed = 0;
					    	ColorGreen = 255;
					    	ColorBlue = 0;
					    	CommandKey = CLEAR_KEY_1;
						}
						else
							CommandKey = COMMAND_INIT;
						break;
					case CLEAR_KEY_2:
						if (CommandKey == CLEAR_KEY_1)
						{
					    	ColorRed = 0;
					    	ColorGreen = 0;
					    	ColorBlue = 255;
					    	CommandKey = CLEAR_KEY_2;
						}
						else
							CommandKey = COMMAND_INIT;
						break;
					case CLEAR_KEY_3:
						if (CommandKey == CLEAR_KEY_2)
						{
					    	ColorRed = 255;
					    	ColorGreen = 0;
					    	ColorBlue = 0;
					    	CommandKey = CLEAR_KEY_3;
						}
						else
							CommandKey = COMMAND_INIT;
						break;
					case CLEAR_KEY_4:
						if (CommandKey == CLEAR_KEY_3)
						{
					    	ColorRed = 80;
					    	ColorGreen = 80;
					    	ColorBlue = 80;
					    	CommandKey = CLEAR_KEY_4;

#if(ARMED_CALIBRATION == 1)
#if(WATCHDOG_ACTIVE == 1)
								SFRPAGE = 0x00; // makesure the watchdog is not hitting during flash erase cycle
								WDTCN = 0xA5;
#endif
							if (ClearAllSettingsFLASH(START_ADDRESS_CALIB_FLASH, SIZE_CALIB_FLASH))
							{
								RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET; // software reset
							}
#endif
						}
						else
							CommandKey = COMMAND_INIT;
						break;
					case BOOTLOADER_KEY_1:
						if (CommandKey == COMMAND_INIT)
							CommandKey = BOOTLOADER_KEY_1;
						else
							CommandKey = COMMAND_INIT;
						break;
					case BOOTLOADER_KEY_2:
 						if (CommandKey == BOOTLOADER_KEY_1)
 							CommandKey = BOOTLOADER_KEY_2;
						else
							CommandKey = COMMAND_INIT;
						break;
					case BOOTLOADER_KEY_3:
						if (CommandKey == BOOTLOADER_KEY_2)
						{
					    	ErrorCase += (LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 0x00, 0x00, 0x00, 0x00)) ? 1 : 0;
							CommandKey = COMMAND_INIT;
							*((uint8_t*) 0x00) = 0xA5; // prepare for entering usb bootloader
							RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET; // software reset
						}
						else
							CommandKey = COMMAND_INIT;
						break;

					default:
							CommandKey = COMMAND_INIT;
						break;
				}

		    	ErrorCase += (LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, ColorRed, ColorGreen, ColorBlue, ColorCommand)) ? 1 : 0;


				while (!ADC0CN0_ADINT); // wait for conversion complete

				InputVoltage = ADC0H;

		    	SMB_DATA_READY = 0;
		    }

			ButtonState = UpdateKeyState();

			LedGetAdc(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LedAdcInput_D7, &AdcVoltageLED_R);
			LedGetAdc(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LedAdcInput_D1, &AdcVoltageLED_G);
			LedGetAdc(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LedAdcInput_D2, &AdcVoltageLED_B);

			LedGetAdc(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LedAdcInput_VOUT, &AdcVoltageOut);
			LedGetAdc(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LedAdcInput_VDD,  &AdcVoltageDD);
			LedGetAdc(I2C_Channel_Satellite, BASE_ADDR_ILLUM, LedAdcInput_INT,  &AdcVoltageInt);

			Deflection = combineAxisToTrend(&TestSensorBase.xAxis, &TestSensorBase.yAxis, false);

			if (!SMB_TRANSFER_ONGOING) // Atomic write of volatile Multi-Byte Values
			{
				IE_EA = 0; //disable Interrupts globally

			SMB_DATA_OUT[SMB_NOTCH_POS] = Notch; 	   // Notch
			SMB_DATA_OUT[SMB_BUTTON_POS] = ButtonState & PB_EDGE; // Button State
#if (ANALOG_XY == 1)

			if 	(!(SignedFilterInitialized(&FilterCartridgeSX) && SignedFilterInitialized(&FilterCartridgeSY)))
			{
				SMB_DATA_OUT[SMB_DEFLECTION_POS] = SwitchDirection;  // Direction
			}
			else
			{
				SMB_DATA_OUT[SMB_DEFLECTION_POS] = (Deflection)? (Deflection) : SwitchDirection;  // Direction
			}
//			SMB_DATA_OUT[SMB_DEFLECTION_POS] = Deflection;  // Direction

#else
			SMB_DATA_OUT[SMB_DEFLECTION_POS] = SwitchDirection; // Firmware and Hardware Version
#endif
			ConvertValue.u16 = AngleAbs;
			SMB_DATA_OUT[SMB_ANGLE_POS] = ConvertValue.u8[0]; // Rotation Angle

			SMB_DATA_OUT[SMB_SUPPLY_POS] = InputVoltage; // Supply Voltage

//			SMB_DATA_OUT[SMB_SUPPLY_POS] = SwitchDirection; // Supply Voltage

			SMB_DATA_OUT[SMB_TEMPBASE_POS] = CalcCelsiusDegreeSensor(&TestSensorBase); // Temperature Base
			SMB_DATA_OUT[SMB_TEMPHANDLE_POS] = CalcCelsiusDegreeSensor(&TestSensorSatellite); // Temperature Handle

			SMB_DATA_OUT[SMB_TG_VERSION_POS] = TG_Version;  // Firmware and Hardware Version

			SMB_DATA_OUT[SMB_UID_01_POS] = serialNo[15];
			SMB_DATA_OUT[SMB_UID_02_POS] = serialNo[14];
			SMB_DATA_OUT[SMB_UID_03_POS] = serialNo[13];
			SMB_DATA_OUT[SMB_UID_04_POS] = serialNo[12];
			SMB_DATA_OUT[SMB_UID_05_POS] = serialNo[11];
			SMB_DATA_OUT[SMB_UID_06_POS] = serialNo[10];
			SMB_DATA_OUT[SMB_UID_07_POS] = serialNo[9];
			SMB_DATA_OUT[SMB_UID_08_POS] = serialNo[8];
			SMB_DATA_OUT[SMB_UID_09_POS] = serialNo[7];
			SMB_DATA_OUT[SMB_UID_10_POS] = serialNo[6];
			SMB_DATA_OUT[SMB_UID_11_POS] = serialNo[5];
			SMB_DATA_OUT[SMB_UID_12_POS] = serialNo[4];
			SMB_DATA_OUT[SMB_UID_13_POS] = serialNo[3];
			SMB_DATA_OUT[SMB_UID_14_POS] = serialNo[2];
			SMB_DATA_OUT[SMB_UID_15_POS] = serialNo[1];
			SMB_DATA_OUT[SMB_UID_16_POS] = serialNo[0];

			SMB_DATA_OUT[SMB_SW_VERSION_POS] = SW_Version;  // Firmware and Hardware Version
			SMB_DATA_OUT[SMB_HW_VERSION_POS] = PCB_Version;  // Firmware and Hardware Version

//			if (!SMB_TRANSFER_ONGOING) // Atomic write of volatile Multi-Byte Values
//			{
//				IE_EA = 0; //disable Interrupts globally
				ConvertValue.u16 = NotchPositions[ThisNotch].entry;
				SMB_DATA_OUT[SMB_ROT_ENTRY_POS_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_ROT_ENTRY_POS_L] = ConvertValue.u8[1];

				ConvertValue.u16 = NotchPositions[ThisNotch].center;
				SMB_DATA_OUT[SMB_ROT_CENTER_POS_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_ROT_CENTER_POS_L] = ConvertValue.u8[1];

				ConvertValue.s16 = (int16_t)((int32_t)NotchPositions[ThisNotch].center - (int32_t)AdjustToNotchDP(AngleAbs));
				SMB_DATA_OUT[SMB_ROT__DEV_POS_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_ROT__DEV_POS_L] = ConvertValue.u8[1];

				ConvertValue.u16 = AdjustToNotchDP(AngleAbs);
				SMB_DATA_OUT[SMB_ROT_ABS_POS_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_ROT_ABS_POS_L] = ConvertValue.u8[1];

				ConvertValue.u16 = NotchPositions[ThisNotch].exit;
				SMB_DATA_OUT[SMB_ROT_EXIT_POS_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_ROT_EXIT_POS_L] = ConvertValue.u8[1];

//				IE_EA = 1; //enable Interrupts globally
//			}

			SMB_DATA_OUT[SMB_VOLTAGE_LED_R] = AdcVoltageLED_R;
			SMB_DATA_OUT[SMB_VOLTAGE_LED_G] = AdcVoltageLED_G;
			SMB_DATA_OUT[SMB_VOLTAGE_LED_B] = AdcVoltageLED_B;
			SMB_DATA_OUT[SMB_VOLTAGE_OUT] = AdcVoltageOut;
			SMB_DATA_OUT[SMB_VOLTAGE_SUP] = AdcVoltageDD;
			SMB_DATA_OUT[SMB_VOLTAGE_PBIN] = AdcVoltageInt;  // Firmware and Hardware Version

//			if (!SMB_TRANSFER_ONGOING) // Atomic write of volatile Multi-Byte Values
//			{
//				IE_EA = 0; //disable Interrupts globally

//				ConvertValue.s16 = TestSensorBase.xAxis.RawValue.s16;
//				ConvertValue.s16 = LastRawX;
				ConvertValue.s16 = LastRawX - TestSensorBase.xAxis.CenterCal;

				SMB_DATA_OUT[SMB_RAW_BAS_X_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_RAW_BAS_X_L] = ConvertValue.u8[1];

//				ConvertValue.u16 = TestSensorBase.yAxis.RawValue.s16;
//				ConvertValue.s16 = LastRawY;
				ConvertValue.s16 = LastRawY - TestSensorBase.yAxis.CenterCal;

				SMB_DATA_OUT[SMB_RAW_BAS_Y_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_RAW_BAS_Y_L] = ConvertValue.u8[1];

				ConvertValue.s16 = TestSensorBase.zAxis.RawValue.s16;
//				ConvertValue.u8[0] = FilterCartridgeX.FilterPos; // todo remove
//				ConvertValue.u8[1] = FilterCartridgeY.FilterPos; // todo remove
				SMB_DATA_OUT[SMB_RAW_BAS_Z_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_RAW_BAS_Z_L] = ConvertValue.u8[1];

				ConvertValue.s16 = TestSensorSatellite.xAxis.RawValue.s16;
//				ConvertValue.s16 = TestSensorBase.xAxis.RawValue.s16;  // todo remove
				SMB_DATA_OUT[SMB_RAW_ROT_X_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_RAW_ROT_X_L] = ConvertValue.u8[1];

				ConvertValue.s16 = TestSensorSatellite.yAxis.RawValue.s16;
//				ConvertValue.s16 = TestSensorBase.yAxis.RawValue.s16;  // todo remove
				SMB_DATA_OUT[SMB_RAW_ROT_Y_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_RAW_ROT_Y_L] = ConvertValue.u8[1];

				ConvertValue.s16 = TestSensorSatellite.zAxis.RawValue.s16;
//				ConvertValue.s16 = (int16_t)FilterTake(&FilterCartridgeX); // todo remove
				SMB_DATA_OUT[SMB_RAW_ROT_Z_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_RAW_ROT_Z_L] = ConvertValue.u8[1];

//				IE_EA = 1; //enable Interrupts globally
//			}

				ConvertValue.u16 = calcCRC16Array(54, SMB_DATA_OUT, 0x0000);
//				ConvertValue.s16 = (int16_t)FilterTake(&FilterCartridgeY); // todo remove
				SMB_DATA_OUT[SMB_CRC16_H] = ConvertValue.u8[0];
				SMB_DATA_OUT[SMB_CRC16_L] = ConvertValue.u8[1];

				IE_EA = 1; //enable Interrupts globally
			}

	#if(WATCHDOG_ACTIVE == 1)
			if (!ErrorCase)
			{
				SFRPAGE = 0x00;
				WDTCN = 0xA5;
			}
			else
			{
			    LedSetSegmentRGB(I2C_Channel_Satellite, BASE_ADDR_ILLUM, 50, 50, 50, 0);
			}
	#endif

		}

	}

	//////////////////////////////////////////////////// END SMB ///////////////////
	// Wait for Firmware-Update on USB-Port
//	*((uint8_t*) 0x00) = 0xA5;
	RSTSRC = RSTSRC_SWRSF__SET | RSTSRC_PORSF__SET;
}



