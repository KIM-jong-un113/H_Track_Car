/**
 * test_mpu6050.c — 逐步诊断
 */
#include "ti_msp_dl_config.h"
#include "my_lib/delay.h"
#include "app_gyro.h"

int main(void)
{
    SYSCFG_DL_init();

    /* 启动：闪 3 下 */
    int i;
    for (i = 0; i < 3; i++) {
        DL_GPIO_setPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(100);
        DL_GPIO_clearPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(100);
    }
    delay_ms(1000);

    /* 阶段1：证明主循环在跑 → 1秒快闪 */
    for (i = 0; i < 5; i++) {
        DL_GPIO_setPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(100);
        DL_GPIO_clearPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(100);
    }
    delay_ms(1000);

    /* 阶段2：测试 I2C 初始化 */
    int ret = MPU6050_Init();
    if (ret != 0) {
        /* 失败 → 长亮 2 秒 */
        DL_GPIO_setPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(2000);
        DL_GPIO_clearPins(LED_PORT, LED_PIN_LED_PIN);
        while (1);
    }

    /* 阶段3：初始化成功 → 闪 5 下 */
    for (i = 0; i < 5; i++) {
        DL_GPIO_setPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(50);
        DL_GPIO_clearPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(50);
    }
    delay_ms(1000);

    MPU6050_Zero_Yaw();

    /* 阶段4：主循环——每 200ms 闪一次，同时更新角度 */
    while (1) {
        MPU6050_Update();
        DL_GPIO_setPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(50);
        DL_GPIO_clearPins(LED_PORT, LED_PIN_LED_PIN);
        delay_ms(200);
    }
}
