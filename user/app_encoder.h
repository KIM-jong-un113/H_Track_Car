#ifndef __APP_ENCODER_H__
#define __APP_ENCODER_H__

#include <stdint.h>

extern volatile int32_t EncoderA_VEL, EncoderB_VEL;

void Encoder_Init(void);
void Encodering(void);
void Read_Encoder(void);
void TIMER_Encoder_Read_INST_IRQHandler(void);

#endif
