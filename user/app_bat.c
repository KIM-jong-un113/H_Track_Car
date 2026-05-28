#include "app_bat.h"
#include "ti_msp_dl_config.h"

void Bat_Init(void)
{
    /* ADC 初始化在 SysConfig 中已完成，此处预留扩展 */
}

float Bat_GetVoltage(void)
{
    /* TODO: 根据实际 ADC 通道读取电池电压 */
    return 0.0f;
}
