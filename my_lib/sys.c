#include "sys.h"

/* ---- PWM ---- */
void Set_Duty(GPTIMER_Regs *PWMA, DL_TIMER_CC_INDEX Channel, int ARR, float duty)
{
    uint32_t CompareValue;
    CompareValue = ARR * duty;
    DL_Timer_setCaptureCompareValue(PWMA, CompareValue, Channel);
}

void Set_Freq(GPTIMER_Regs *PWMA, uint32_t CLK_FREQ, uint32_t freq)
{
    uint32_t period;
    period = CLK_FREQ / freq;
    DL_Timer_setLoadValue(PWMA, period);
}

/* ---- printf 重定向到 UART0 ---- */
int fputc(int c, FILE* stream)
{
    DL_UART_Main_transmitDataBlocking(UART_0_INST, c);
    return c;
}

int fputs(const char* restrict s, FILE* restrict stream)
{
    uint16_t i, len;
    len = strlen(s);
    for(i = 0; i < len; i++)
        DL_UART_Main_transmitDataBlocking(UART_0_INST, s[i]);
    return len;
}

int puts(const char *_ptr)
{
    int count = fputs(_ptr, stdout);
    count += fputs("\n", stdout);
    return count;
}

/* ---- 取绝对值 ---- */
int myabs(int x)
{
    return (x < 0) ? -x : x;
}

/* ---- 限幅 ---- */
float PWM_Limit(float IN, float max, float min)
{
    float OUT = IN;
    if(OUT > max) OUT = max;
    if(OUT < min) OUT = min;
    return OUT;
}
