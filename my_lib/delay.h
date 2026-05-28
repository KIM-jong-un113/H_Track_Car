#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdint.h>

void delay_init(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
uint32_t GetTick(void);
uint64_t GetUs(void);

#endif
