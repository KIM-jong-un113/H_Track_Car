#include "ti_msp_dl_config.h"
#include "my_lib/sys.h"
#include "my_lib/delay.h"
#include "user/app_sensor.h"

int main(void)
{
    SYSCFG_DL_init();
    printf("=== Sensor Test ===\r\n");

    while (1) {
        int val = Incremental_Quantity();
        printf("P1:%d P2:%d P3:%d P4:%d P5:%d P6:%d P7:%d P8:%d  bias:%d\r\n",
               P1 ? 1 : 0, P2 ? 1 : 0, P3 ? 1 : 0, P4 ? 1 : 0,
               P5 ? 1 : 0, P6 ? 1 : 0, P7 ? 1 : 0, P8 ? 1 : 0, val);
        delay_ms(50);
    }
}
