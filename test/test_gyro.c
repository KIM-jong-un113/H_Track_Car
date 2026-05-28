#include "ti_msp_dl_config.h"
#include "my_lib/sys.h"
#include "my_lib/delay.h"
#include "user/app_gyro.h"

int main(void)
{
    SYSCFG_DL_init();
    printf("=== MPU6050 Gyro Test ===\r\n");

    MPU6050_Init();
    MPU6050_Zero_Yaw();
    delay_ms(500);

    while (1) {
        MPU6050_Update();
        printf("Yaw: %.1f  Pitch: %.1f  Roll: %.1f\r\n", Yaw, Pitch, Roll);
        delay_ms(50);
    }
}
