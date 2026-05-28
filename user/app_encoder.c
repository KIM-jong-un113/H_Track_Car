#include "app_encoder.h"
#include "my_lib/sys.h"

volatile uint32_t EncoderA_Port, EncoderB_Port;
volatile int32_t EncoderA_CNT, EncoderB_CNT;
volatile int32_t EncoderA_VEL, EncoderB_VEL;

void Encoder_Init(void)
{
    AIN1_Low;
    AIN2_Low;
    BIN1_Low;
    BIN2_Low;

    NVIC_EnableIRQ(GPIO_EncoderA_INT_IRQN);
    NVIC_EnableIRQ(GPIO_EncoderB_INT_IRQN);
    NVIC_EnableIRQ(TIMER_Encoder_Read_INST_INT_IRQN);
    DL_Timer_startCounter(TIMER_Encoder_Read_INST);

    DL_Timer_startCounter(PWM_0_INST);
}

void TIMER_Encoder_Read_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_Encoder_Read_INST)) {
    case DL_TIMER_IIDX_ZERO:
        Read_Encoder();
        break;
    default:
        break;
    }
}

void Encodering(void)
{
    EncoderA_Port = DL_GPIO_getEnabledInterruptStatus(GPIO_EncoderA_PORT, GPIO_EncoderA_PIN_0_PIN | GPIO_EncoderA_PIN_1_PIN);
    EncoderB_Port = DL_GPIO_getEnabledInterruptStatus(GPIO_EncoderB_PORT, GPIO_EncoderB_PIN_2_PIN | GPIO_EncoderB_PIN_3_PIN);

    if ((EncoderA_Port & GPIO_EncoderA_PIN_0_PIN) == GPIO_EncoderA_PIN_0_PIN) {
        if (!DL_GPIO_readPins(GPIO_EncoderA_PORT, GPIO_EncoderA_PIN_1_PIN))
            EncoderA_CNT--;
        else
            EncoderA_CNT++;
    } else if ((EncoderA_Port & GPIO_EncoderA_PIN_1_PIN) == GPIO_EncoderA_PIN_1_PIN) {
        if (!DL_GPIO_readPins(GPIO_EncoderA_PORT, GPIO_EncoderA_PIN_0_PIN))
            EncoderA_CNT++;
        else
            EncoderA_CNT--;
    }
    DL_GPIO_clearInterruptStatus(GPIO_EncoderA_PORT, GPIO_EncoderA_PIN_0_PIN | GPIO_EncoderA_PIN_1_PIN);

    if ((EncoderB_Port & GPIO_EncoderB_PIN_2_PIN) == GPIO_EncoderB_PIN_2_PIN) {
        if (!DL_GPIO_readPins(GPIO_EncoderB_PORT, GPIO_EncoderB_PIN_3_PIN))
            EncoderB_CNT--;
        else
            EncoderB_CNT++;
    } else if ((EncoderB_Port & GPIO_EncoderB_PIN_3_PIN) == GPIO_EncoderB_PIN_3_PIN) {
        if (!DL_GPIO_readPins(GPIO_EncoderB_PORT, GPIO_EncoderB_PIN_2_PIN))
            EncoderB_CNT++;
        else
            EncoderB_CNT--;
    }
    DL_GPIO_clearInterruptStatus(GPIO_EncoderB_PORT, GPIO_EncoderB_PIN_2_PIN | GPIO_EncoderB_PIN_3_PIN);
}

void Read_Encoder(void)
{
    EncoderA_VEL = EncoderA_CNT;
    EncoderA_CNT = 0;
    EncoderB_VEL = EncoderB_CNT;
    EncoderB_CNT = 0;
}
