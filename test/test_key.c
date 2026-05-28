/**
 * test_key.c — S2 按键翻转 LED
 * 每按一次 S2 (PB1), LED (PA13) 翻转一次
 * 烧录后按 S2 看 LED 亮灭切换
 */

#include "ti_msp_dl_config.h"

volatile uint32_t tick = 0;

/* SysTick 每 1ms 进一次 */
void SysTick_Handler(void)
{
    tick++;
}

static void delay_ms(uint32_t ms)
{
    uint32_t start = tick;
    while ((tick - start) < ms);
}

int main(void)
{
    SYSCFG_DL_init();

    /* 配置 SysTick: 80MHz / 80000 = 1kHz (1ms) */
    SysTick_Config(80000);

    /* 初始关 LED */
    DL_GPIO_clearPins(LED_PORT, LED_PIN_LED_PIN);

    uint8_t led_state = 0;
    uint8_t key_last  = 1;  /* 上拉, 未按下为 1 */

    while (1) {
        uint8_t key_now = (uint8_t)DL_GPIO_readPins(KEY_PORT, KEY_S2_PIN);

        /* 下降沿检测: 上次高, 本次低 = 按下 */
        if (key_last == 1 && key_now == 0) {
            delay_ms(20);  /* 消抖 */
            key_now = (uint8_t)DL_GPIO_readPins(KEY_PORT, KEY_S2_PIN);
            if (key_now == 0) {
                led_state = !led_state;
                if (led_state)
                    DL_GPIO_setPins(LED_PORT, LED_PIN_LED_PIN);
                else
                    DL_GPIO_clearPins(LED_PORT, LED_PIN_LED_PIN);
            }
        }
        key_last = key_now;
    }
}
