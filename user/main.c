#include "ti_msp_dl_config.h"
#include "my_lib/sys.h"
#include "my_lib/task.h"
#include "my_lib/delay.h"
#include "app_gyro.h"
#include "app_encoder.h"
#include "app_motor.h"
#include "app_sensor.h"
#include "app_control.h"

/* ---- 全局状态 ---- */
volatile int flag = 1;
volatile int mode = 0;
volatile int flag_en = 0;

volatile float LED_CNT = 0.0f;
volatile bool flag_LED = 0;

/* ---- 按键扫描 ---- */
static void Key_Scan(void)
{
    if (DL_GPIO_readPins(KEY_PORT, KEY_S2_PIN) == 0) {
        delay_ms(10);
        if (DL_GPIO_readPins(KEY_PORT, KEY_S2_PIN) == 0) {
            while (!DL_GPIO_readPins(KEY_PORT, KEY_S2_PIN));
            mode = (mode + 1) % 5;
        }
    }

    if (DL_GPIO_readPins(KEY_PORT, KEY_EN_PIN) == 0) {
        delay_ms(10);
        if (DL_GPIO_readPins(KEY_PORT, KEY_EN_PIN) == 0) {
            while (!DL_GPIO_readPins(KEY_PORT, KEY_EN_PIN));
            flag_en = 1 - flag_en;
        }
    }
}

/* ---- LED 闪烁 ---- */
static void LED_Sound(void)
{
    if (flag_LED) {
        LED_High;
        LED_CNT += 0.1f;
        if (LED_CNT >= 800) {
            LED_Low;
            LED_CNT = 0;
            flag_LED = 0;
        }
    }
}

int main(void)
{
    SYSCFG_DL_init();
    printf("Hello World\r\n");

    /* 陀螺仪初始化 */
    if (MPU6050_Init() != 0) {
        printf("MPU6050 Init FAIL!\r\n");
        while (1);
    }
    MPU6050_Zero_Yaw();
    delay_ms(500);

    /* 编码器 + PWM 初始化 */
    Encoder_Init();

    /* 控制算法初始化 */
    Control_Init();

    /* 使能控制定时器 */
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    DL_Timer_startCounter(TIMER_0_INST);

    while (1) {
        Key_Scan();
        MPU6050_Update();

        if (flag_en) {
            Follow_Route();
            LED_Sound();
        }
    }
}

/* ---- 定时器 0 中断：周期性调用 Control ---- */
void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
    case DL_TIMER_IIDX_ZERO:
        if (flag == 0 || flag_en == 0 || mode == 0)
            Set_Pwm(0, 0);
        else if (flag_en)
            Control();
        break;
    default:
        break;
    }
}

/* ---- GPIO 中断：编码器 4 倍频解码 ---- */
void GROUP1_IRQHandler(void)
{
    Encodering();
}
