#include "app_motor.h"
#include "my_lib/sys.h"

void Set_Pwm(int Left, int Right)
{
    if (Left > 0) {
        AIN1_High;
        AIN2_Low;
    } else {
        AIN1_Low;
        AIN2_High;
    }
    DL_Timer_setCaptureCompareValue(PWM_0_INST, myabs(Left), DL_TIMER_CC_0_INDEX);

    if (Right > 0) {
        BIN1_High;
        BIN2_Low;
    } else {
        BIN1_Low;
        BIN2_High;
    }
    DL_Timer_setCaptureCompareValue(PWM_0_INST, myabs(Right), DL_TIMER_CC_1_INDEX);
}
