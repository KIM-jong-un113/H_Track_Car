#ifndef LPF_H
#define LPF_H

#include <stdint.h>

typedef struct
{
    float Tf;
    float LastOutput;
    uint64_t LastTime;
} LPF_TypeDef;

void LPF_Init(LPF_TypeDef *Lpf, float Tf);
float LPF_Calc(LPF_TypeDef *Lpf, float Input, uint64_t now);

#endif
