#include "ti_msp_dl_config.h"
#include "my_lib/sys.h"
#include "my_lib/delay.h"
#include "user/app_encoder.h"

int main(void)
{
    SYSCFG_DL_init();
    printf("=== Encoder Test ===\r\n");

    Encoder_Init();
    NVIC_EnableIRQ(TIMER_Encoder_Read_INST_INT_IRQN);

    while (1) {
        printf("EncoderA: %d  EncoderB: %d\r\n", EncoderA_VEL, EncoderB_VEL);
        delay_ms(50);
    }
}
