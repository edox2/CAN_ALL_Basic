/*
 * TypeDefs.h
 *
 *  Created on: 13.03.2022
 *      Author: benja
 */
#ifndef INC_TYPEDEFS_H_
#define INC_TYPEDEFS_H_

#include "stdint.h"

typedef struct {
  int16_t current;
  uint16_t voltage;
  uint16_t temperature;
}HighSideSwitchData;

struct Battery_Status{
  uint8_t NodeActive;
  uint8_t Error;
  uint8_t ValidValues;
  uint8_t PrechargeOk;
  uint8_t HvRelaisEnable;
  uint8_t MasterActive;
  uint8_t Warning;
  uint8_t Balancing;
  uint8_t InternHeating;
  uint8_t ExternHeating;
};

enum Battery_State{
  Start = 0,
  Idle = 1,
  Balancing = 2,
  Drive = 3,
  Charge = 4,
  ChargeConversation = 5,
  BalancerSaveAndOff = 20,
  BalancerOffDelay = 21,
  BalancerOff = 22,
  Error = 99,
  Boot = 199,
};


struct Battery{
  struct  Battery_Status Status; //BIT0        BIT1  BIT2          BIT3            BIT4              BIT5          BIT6    BIT7      BIT8            BIT9            BIT10÷BIT15
                                //NODE AKTIV  ERROR VALID VALUES  PRECHARGE OK    HV RELAIS ENABLE  MASTER ACTIVE WARNING BALANCING INTERN HEATING  EXTERN HEATING

  uint8_t State;    //0       1       2           3       4         5                     20                      21                    22              99      199
                                //START   IDLE    BALANCING   DRIVE   CHARGE    CHARGE_CONSERVATION   BALANCER SAVE AND OFF   BALANCER OFF DELAY    BALANCER OFF    ERROR   BOOT
  uint16_t ErrorWarningNr;
  uint16_t SoC;
  uint16_t SoH;
  uint16_t CellDiff_mV;
  int16_t Current;
  int16_t MaxChargeCurrent;
};

struct Motor_Flags{
  uint8_t isLimitationActive_Motor1;
  uint8_t EncoderChannelA_Motor1;
  uint8_t EncoderChannelB_Motor1;
  uint8_t isOvertemperature_Motor1;
};

struct System_Flags{
  uint8_t isSoCLowForTraction;
  uint8_t isSoCLowForHydraulic;
  uint8_t isReverseActive;
  uint8_t isForwardActive;
  uint8_t isParkBrakeActive;
  uint8_t isPedalBrakeActive;
  uint8_t isControllerInOvertemperature;
  uint8_t isKeySwitchOvervoltage;
  uint8_t isKeySwitchUndervoltage;
  uint8_t isVehicleRunning_NotStill;
  uint8_t isTractionEnabled;
  uint8_t isHydraulicEnabled;
  uint8_t isPoweringEnabled;
  uint8_t isPoweringReady;
};

struct FaultLevel{
  uint8_t Ready;
  uint8_t Blocking;
  uint8_t Stopping;
  uint8_t Limiting;
  uint8_t Warning;
};

struct Invertor{
  uint8_t  Inverter_Temp; // [deg]
  uint8_t Motor_Temp;     // [deg]
  SI_UU16_t Tourque;       // [-100:100]
  struct Motor_Flags MotorFlags;     // BIT0                       BIT1                    BIT2                    BIT3                      BIT4÷BIT15
                                     // isLimitationActive_Motor1  EncoderChannelA_Motor1  EncoderChannelB_Motor1  isOvertemperature_Motor1  Motor2/Reserved

  struct System_Flags SystemFlags;    // BIT0                 BIT1                  BIT2            BIT3            BIT4              BIT5                BIT6                          BIT7                    BIT8                    BIT9                      BIT10             BIT11               BIT12             BIT13           BIT14÷BIT15
                                      // isSoCLowForTraction  isSoCLowForHydraulic  isReverseActive isForwardActive isParkBrakeActive isPedalBrakeActive  isControllerInOvertemperature isKeySwitchOvervoltage  isKeySwitchUndervoltage isVehicleRunning_NotStill isTractionEnabled isHydraulicEnabled  isPoweringEnabled isPoweringReady Reserved
  uint8_t FaultCode;      // See manual
  uint8_t FaultLevel;      // 0    1         2         3         4
                           // Ready Blocking  Stopping  Limiting  Warning

  SI_UU16_t DC_Bus_Current; // [dA] (1dA=0.1A)
};

struct ImcStatus{
  uint8_t IsolationError;     //0=NoError, 1=Error
  uint8_t ChassisError;       //0=NoError, 1=Error
  uint8_t SystemError;        //0=NoError, 1=Error
  uint8_t CalibratingActive;  //0=NotRunning, Running
  uint8_t SelbsttestActive;   //0=NotRunning, Running
  uint8_t IsolationWarning;   //0=NoWarning, 1=Warning
};

struct VifcStatus{
  uint8_t IsolationMeasurementActivated;  //0=Activated, 1=Deactivated
  uint8_t ImcAliveStaterecognition;       //0=OK, 1=error
  uint8_t OldMeasuremend;                 //0=IsolationResistor measurement valid, 1=old
  uint8_t OverallImcSelbsttest;           //0=Done, 1=not performed
  uint8_t ParameterImcSelbsttest;         //0=Done, 1=not performed
};

struct BenderIMC{
  uint16_t IMC_R_ISO;
  struct ImcStatus ImcStatus;
  struct VifcStatus VifcStatus;
};

#endif /* INC_TYPEDEFS_H_ */
