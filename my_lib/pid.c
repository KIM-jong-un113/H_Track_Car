/**
  ******************************************************************************
  * @file    pid.c
  * @author  铁头山羊stm32工作组
  * @version V1.0.0
  * @date    2023年2月24日
  * @brief   pid算法库
  ******************************************************************************
*/

#include "pid.h"

#define INVALID_TICK 0xffffffffffffffff

void PID_Init(PID_TypeDef *PID, PID_InitTypeDef *PID_InitStruct)
{
    PID->Init = *PID_InitStruct;

    PID->Setpoint = PID->Init.Setpoint;
    PID->Kp = PID->Init.Kp;
    PID->Ki = PID->Init.Ki;
    PID->Kd = PID->Init.Kd;
    PID->OutputLowerLimit = PID->Init.OutputLowerLimit;
    PID->OutputUpperLimit = PID->Init.OutputUpperLimit;
    PID->ITerm = PID->Init.DefaultOutput;
    PID->LastTime = INVALID_TICK;
    PID->LastInput = 0;

    PID->cmd = 0;
    PID->LpfCmd = 0;
}

void PID_LpfConfig(PID_TypeDef *PID, float Tf, uint8_t NewState)
{
    PID->LpfCmd = NewState;
    LPF_Init(&PID->Lpf, Tf);
}

void PID_Reset(PID_TypeDef *PID)
{
    PID->LastTime = INVALID_TICK;
    PID->ITerm = PID->Init.DefaultOutput;
    PID->LastError = 0;
    PID->LastInput = 0;
}

float PID_Compute1(PID_TypeDef *PID, float Input, uint64_t now)
{
    float error = PID->Setpoint - Input;
    float dt = (now - PID->LastTime) * 1.0e-6f;

    float output = PID->Kp * error;

    if(PID->LastTime != INVALID_TICK)
    {
        if(PID->Kd != 0)
        {
            PID->DTerm = PID->Kd * (error - PID->LastError) / dt;
            output += PID->DTerm;
        }

        if(PID->Ki != 0)
        {
            PID->ITerm += PID->Ki * (error + PID->LastError) * 0.5 * dt;

            if(PID->ITerm > PID->OutputUpperLimit)
                PID->ITerm = PID->OutputUpperLimit;
            else if(PID->ITerm < PID->OutputLowerLimit)
                PID->ITerm = PID->OutputLowerLimit;

            output += PID->ITerm;
        }
    }

    if(output > PID->OutputUpperLimit)
        output = PID->OutputUpperLimit;
    else if(output < PID->OutputLowerLimit)
        output = PID->OutputLowerLimit;

    PID->LastInput = Input;
    PID->LastTime = now;
    PID->LastError = error;

    if(PID->LpfCmd)
        output = LPF_Calc(&PID->Lpf, output, now);

    return output;
}

float PID_Compute2(PID_TypeDef *PID, float Input, float dInputDt, uint64_t now)
{
    float error = PID->Setpoint - Input;
    float dt = (now - PID->LastTime) * 1.0e-6f;

    float output = PID->Kp * error;

    if(PID->LastTime != INVALID_TICK)
    {
        if(PID->Kd != 0)
        {
            PID->DTerm = -PID->Kd * dInputDt;
            output += PID->DTerm;
        }

        if(PID->Ki != 0)
        {
            PID->ITerm += PID->Ki * (error + PID->LastError) * 0.5 * dt;

            if(PID->ITerm > PID->OutputUpperLimit)
                PID->ITerm = PID->OutputUpperLimit;
            else if(PID->ITerm < PID->OutputLowerLimit)
                PID->ITerm = PID->OutputLowerLimit;

            output += PID->ITerm;
        }
    }

    if(output > PID->OutputUpperLimit)
        output = PID->OutputUpperLimit;
    else if(output < PID->OutputLowerLimit)
        output = PID->OutputLowerLimit;

    PID->LastInput = Input;
    PID->LastTime = now;
    PID->LastError = error;

    if(PID->LpfCmd)
        output = LPF_Calc(&PID->Lpf, output, now);

    return output;
}

void PID_ChangeTunings(PID_TypeDef *PID, float NewKp, float NewKi, float NewKd)
{
    PID->Kp = NewKp;
    PID->Ki = NewKi;
    PID->Kd = NewKd;
}

void PID_ChangeSetpoint(PID_TypeDef *PID, float NewSetpoint)
{
    PID->Setpoint = NewSetpoint;
}

float PID_GetSetpoint(PID_TypeDef *PID)
{
    return PID->Setpoint;
}

void PID_GetTunings(PID_TypeDef *PID, float *pKpOut, float *pKiOut, float *pKdOut)
{
    *pKpOut = PID->Kp;
    *pKiOut = PID->Ki;
    *pKdOut = PID->Kd;
}

void PID_Cmd(PID_TypeDef *PID, uint8_t NewState)
{
    if(PID->cmd == 0 && NewState == 1)
    {
        PID->ITerm = PID->Init.DefaultOutput;

        if(PID->ITerm > PID->OutputUpperLimit)
            PID->ITerm = PID->OutputUpperLimit;

        if(PID->ITerm < PID->OutputLowerLimit)
            PID->ITerm = PID->OutputLowerLimit;
    }

    PID->cmd = NewState;
}
