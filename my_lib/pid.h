/**
  ******************************************************************************
  * @file    pid.h
  * @author  铁头山羊stm32工作组
  * @version V1.0.0
  * @date    2024年10月27日
  * @brief   pid算法库
  ******************************************************************************
*/

#ifndef _PID_H_
#define _PID_H_

#include <stdint.h>
#include "lpf.h"

typedef struct{
    float Kp;
    float Ki;
    float Kd;
    float Setpoint;
    float OutputUpperLimit;
    float OutputLowerLimit;
    float DefaultOutput;
}PID_InitTypeDef;

typedef struct{
    PID_InitTypeDef Init;
    uint8_t cmd;
    uint64_t LastTime;
    float ITerm;
    float DTerm;
    float LastInput;
    float LastError;
    float Kp;
    float Ki;
    float Kd;
    float OutputUpperLimit;
    float OutputLowerLimit;
    float Setpoint;
    LPF_TypeDef Lpf;
    uint8_t LpfCmd;
}PID_TypeDef;

void PID_Init(PID_TypeDef *PID, PID_InitTypeDef *PID_InitStruct);
void PID_LpfConfig(PID_TypeDef *PID, float Tf, uint8_t NewState);
void PID_Cmd(PID_TypeDef *PID, uint8_t NewState);
void PID_Reset(PID_TypeDef *PID);
float PID_Compute1(PID_TypeDef *PID, float Input, uint64_t Now);
float PID_Compute2(PID_TypeDef *PID, float Input, float dInput, uint64_t Now);
void PID_ChangeTunings(PID_TypeDef *PID, float NewKp, float NewKi, float NewKd);
void PID_ChangeSetpoint(PID_TypeDef *PID, float NewSetpoint);

#endif
