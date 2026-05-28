#include "lpf.h"

void LPF_Init(LPF_TypeDef *Lpf, float Tf)
{
    Lpf->Tf = Tf;
    Lpf->LastTime = 0xffffffffffffffff;
}

float LPF_Calc(LPF_TypeDef *Lpf, float Input, uint64_t now)
{
    float output;

    if(Lpf->LastTime == 0xffffffffffffffff)
    {
        output = Input;
    }
    else
    {
        float dt = (now - Lpf->LastTime) * 1e-6;
        output = Lpf->LastOutput + (Input - Lpf->LastOutput) / Lpf->Tf * dt;
    }

    Lpf->LastTime = now;
    Lpf->LastOutput = output;

    return output;
}
