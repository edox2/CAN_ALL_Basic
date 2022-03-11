/*
 * LedDriverLP55231.h
 *********************************************
 *    (c)2019-2020 SIGITRONIC SOFTWARE       *
 *                                           *
 *      Author: Matthias Siegenthaler        *
 *                                           *
 *        matthias@sigitronic.com            *
 *********************************************
 */

#ifndef LEDDRIVER_LP55231_H_
#define LEDDRIVER_LP55231_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <si_toolchain.h>

#define IllumENABLE_ENGINE_CNTRL1          (0x00)
#define IllumENGINE_CNTRL2                 (0x01)
#define IllumOUTPUT_DIRECT_RATIOMETRIC_MSB (0x02)
#define IllumOUTPUT_DIRECT_RATIOMETRIC_LSB (0x03)
#define IllumOUTPUT_ON_OFF_CONTROL_MSB     (0x04)
#define IllumOUTPUT_ON_OFF_CONTROL_LSB     (0x05)
#define IllumD1_CONTROL                    (0x06)
#define IllumD2_CONTROL                    (0x07)
#define IllumD3_CONTROL                    (0x08)
#define IllumD4_CONTROL                    (0x09)
#define IllumD5_CONTROL                    (0x0A)
#define IllumD6_CONTROL                    (0x0B)
#define IllumD7_CONTROL                    (0x0C)
#define IllumD8_CONTROL                    (0x0D)
#define IllumD9_CONTROL                    (0x0E)
#define IllumD1_PWM                        (0x16)
#define IllumD2_PWM                        (0x17)
#define IllumD3_PWM                        (0x18)
#define IllumD4_PWM                        (0x19)
#define IllumD5_PWM                        (0x1A)
#define IllumD6_PWM                        (0x1B)
#define IllumD7_PWM                        (0x1C)
#define IllumD8_PWM                        (0x1D)
#define IllumD9_PWM                        (0x1E)
#define IllumD1_CURRENT_CONTROL            (0x26)
#define IllumD2_CURRENT_CONTROL            (0x27)
#define IllumD3_CURRENT_CONTROL            (0x28)
#define IllumD4_CURRENT_CONTROL            (0x29)
#define IllumD5_CURRENT_CONTROL            (0x2A)
#define IllumD6_CURRENT_CONTROL            (0x2B)
#define IllumD7_CURRENT_CONTROL            (0x2C)
#define IllumD8_CURRENT_CONTROL            (0x2D)
#define IllumD9_CURRENT_CONTROL            (0x2E)
#define IllumMISC                          (0x36)
#define IllumENGINE1_PC                    (0x37)
#define IllumENGINE2_PC                    (0x38)
#define IllumENGINE3_PC                    (0x39)
#define IllumSTATUS_INTERRUPT              (0x3A)
#define IllumINT_GPO                       (0x3B)
#define IllumVARIABLE                      (0x3C)
#define IllumRESET                         (0x3D)
#define IllumTEMP_ADC_CONTROL              (0x3E)
#define IllumTEMPERATURE_READ              (0x3F)
#define IllumTEMPERATURE_WRITE             (0x40)
#define IllumLED_TEST_CONTROL              (0x41)
#define IllumLED_TEST_ADC                  (0x42)
#define IllumENGINE1_VARIABLE_A            (0x45)
#define IllumENGINE2_VARIABLE_A            (0x46)
#define IllumENGINE3_VARIABLE_A            (0x47)
#define IllumMASTER_FADER1                 (0x48)
#define IllumMASTER_FADER2                 (0x49)
#define IllumMASTER_FADER3                 (0x4A)
#define IllumENG1_PROG_START_ADDR          (0x4C)
#define IllumENG2_PROG_START_ADDR          (0x4D)
#define IllumENG3_PROG_START_ADDR          (0x4E)
#define IllumPROG_MEM_PAGE_SEL             (0x4F)
#define IllumPROGRAM_MEMORY_X0H            (0x50)
#define IllumPROGRAM_MEMORY_X0L            (0x51)
#define IllumPROGRAM_MEMORY_X1H            (0x52)
#define IllumPROGRAM_MEMORY_X1L            (0x53)
#define IllumPROGRAM_MEMORY_X2H            (0x54)
#define IllumPROGRAM_MEMORY_X2L            (0x55)
#define IllumPROGRAM_MEMORY_X3H            (0x56)
#define IllumPROGRAM_MEMORY_X3L            (0x57)
#define IllumPROGRAM_MEMORY_X4H            (0x58)
#define IllumPROGRAM_MEMORY_X4L            (0x59)
#define IllumPROGRAM_MEMORY_X5H            (0x5A)
#define IllumPROGRAM_MEMORY_X5L            (0x5B)
#define IllumPROGRAM_MEMORY_X6H            (0x5C)
#define IllumPROGRAM_MEMORY_X6L            (0x5D)
#define IllumPROGRAM_MEMORY_X7H            (0x5E)
#define IllumPROGRAM_MEMORY_X7L            (0x5F)
#define IllumPROGRAM_MEMORY_X8H            (0x60)
#define IllumPROGRAM_MEMORY_X8L            (0x61)
#define IllumPROGRAM_MEMORY_X9H            (0x62)
#define IllumPROGRAM_MEMORY_X9L            (0x63)
#define IllumPROGRAM_MEMORY_XAH            (0x64)
#define IllumPROGRAM_MEMORY_XAL            (0x65)
#define IllumPROGRAM_MEMORY_XBH            (0x66)
#define IllumPROGRAM_MEMORY_XBL            (0x67)
#define IllumPROGRAM_MEMORY_XCH            (0x68)
#define IllumPROGRAM_MEMORY_XCL            (0x69)
#define IllumPROGRAM_MEMORY_XDH            (0x6A)
#define IllumPROGRAM_MEMORY_XDL            (0x6B)
#define IllumPROGRAM_MEMORY_XEH            (0x6C)
#define IllumPROGRAM_MEMORY_XEL            (0x6D)
#define IllumPROGRAM_MEMORY_XFH            (0x6E)
#define IllumPROGRAM_MEMORY_XFL            (0x6F)
#define IllumENG1_MAPPING_MSB              (0x70)
#define IllumENG1_MAPPING_LSB              (0x71)
#define IllumENG2_MAPPING_MS               (0x72)
#define IllumENG2_MAPPING_LSB              (0x73)
#define IllumENG3_MAPPING_MSB              (0x74)
#define IllumENG3_MAPPING_LSB              (0x75)
#define IllumGAIN_CHANGE_CTRL              (0x76)

#define LedModeLin (0x00)
#define LedModeLog (0x20)

#define LedModeTC_1_5neg  (0x1F)
#define LedModeTC_1_4neg  (0x1E)
#define LedModeTC_1_3neg  (0x1D)
#define LedModeTC_1_2neg  (0x1C)
#define LedModeTC_1_1neg  (0x1B)
#define LedModeTC_1_0neg  (0x1A)
#define LedModeTC_0_9neg  (0x19)
#define LedModeTC_0_8neg  (0x18)
#define LedModeTC_0_7neg  (0x17)
#define LedModeTC_0_6neg  (0x16)
#define LedModeTC_0_5neg  (0x15)
#define LedModeTC_0_4neg  (0x14)
#define LedModeTC_0_3neg  (0x13)
#define LedModeTC_0_2neg  (0x12)
#define LedModeTC_0_1neg  (0x11)
#define LedModeTC_0_0off  (0x10)
#define LedModeTC_0_1pos  (0x01)
#define LedModeTC_0_2pos  (0x02)
#define LedModeTC_0_3pos  (0x03)
#define LedModeTC_0_4pos  (0x04)
#define LedModeTC_0_5pos  (0x05)
#define LedModeTC_0_6pos  (0x06)
#define LedModeTC_0_7pos  (0x07)
#define LedModeTC_0_8pos  (0x08)
#define LedModeTC_0_9pos  (0x09)
#define LedModeTC_1_0pos  (0x0A)
#define LedModeTC_1_1pos  (0x0B)
#define LedModeTC_1_2pos  (0x0C)
#define LedModeTC_1_3pos  (0x0D)
#define LedModeTC_1_4pos  (0x0E)
#define LedModeTC_1_5pos  (0x0F)



#define LedModeTC_1_5m  (0x10)


#define LedSegment_1  (0x01)
#define LedSegment_2  (0x02)
#define LedSegment_3  (0x04)
#define LedSegment_All (0x07)
#define LedColor_SetMaxCurrent 	(0x80)
#define LedColor_Red  			(0x10)
#define LedColor_Green 			(0x20)
#define LedColor_Blue 			(0x30)

#define LedAdcInput_D1   (0x00)
#define LedAdcInput_D2   (0x01)
#define LedAdcInput_D3   (0x02)
#define LedAdcInput_D4   (0x03)
#define LedAdcInput_D5   (0x04)
#define LedAdcInput_D6   (0x05)
#define LedAdcInput_D7   (0x06)
#define LedAdcInput_D8   (0x07)
#define LedAdcInput_D9   (0x08)
#define LedAdcInput_VOUT (0x0F)
#define LedAdcInput_VDD  (0x10)
#define LedAdcInput_INT  (0x11)


//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

uint8_t LedSetSegmentRGB(uint8_t Channel, uint8_t BaseAddress, uint8_t LedRed, uint8_t LedGreen, uint8_t LedBlue, uint8_t Segement);
uint8_t LedGetAdc(uint8_t Channel, uint8_t BaseAddress, uint8_t AdcInput, uint8_t * Value);
uint8_t LedInitSetMaxCurrent(uint8_t Channel, uint8_t BaseAddress, uint8_t LedRed789, uint8_t LedGreen135, uint8_t LedBlue246);
uint8_t LedSetMaxCurrent(uint8_t Channel, uint8_t BaseAddress, uint8_t LedRed789, uint8_t LedGreen135, uint8_t LedBlue246);
uint8_t LedSetModeLogTC(uint8_t Channel, uint8_t BaseAddress, uint8_t LedRed789, uint8_t LedGreen135, uint8_t LedBlue246);


#endif /* LEDDRIVER_LP55231_H_ */
