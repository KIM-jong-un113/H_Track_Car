#include "ti_msp_dl_config.h"
#include "my_lib/sys.h"
#include "my_lib/delay.h"
#include "user/app_motor.h"

int main(void)
{
    SYSCFG_DL_init();
    printf("=== Motor Test ===\r\n");

    AIN1_Low; AIN2_Low;
    BIN1_Low; BIN2_Low;

    /* 启动 PWM */
    DL_Timer_startCounter(PWM_0_INST);

    printf("Motor forward 50%%...\r\n");
    Set_Pwm(2000, 2000);
    delay_ms(2000);

    printf("Motor stop...\r\n");
    Set_Pwm(0, 0);
    delay_ms(1000);

    printf("Motor reverse 50%%...\r\n");
    Set_Pwm(-2000, -2000);
    delay_ms(2000);

    printf("Motor stop...\r\n");
    Set_Pwm(0, 0);

    while (1) {
        delay_ms(1000);
    }
}
