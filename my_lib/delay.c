#include "delay.h"
#include "ti_msp_dl_config.h"

static volatile uint32_t tick_ms = 0;
static volatile uint64_t tick_us = 0;

void delay_init(void)
{
    tick_ms = 0;
    tick_us = 0;
}

void delay_ms(uint32_t ms)
{
    while(ms--)
        delay_cycles(CPUCLK_FREQ / 1000);
}

void delay_us(uint32_t us)
{
    while(us--)
        delay_cycles(CPUCLK_FREQ / 1000000);
}

uint32_t GetTick(void)
{
    return tick_ms;
}

uint64_t GetUs(void)
{
    return tick_us;
}

void Time_Increment_10us(void)
{
    tick_us += 10;
    tick_ms = (uint32_t)(tick_us / 1000);
}
