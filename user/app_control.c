#include "app_control.h"
#include "app_motor.h"
#include "app_sensor.h"
#include "app_gyro.h"
#include "my_lib/pid.h"
#include "my_lib/sys.h"
#include "my_lib/delay.h"

#define PWM_LIMIT  3800

/* ---- 速度环 PID 参数(与原项目一致) ---- */
#define SPD_Kp   40.0f
#define SPD_Ki   0.13f
#define SPD_Kd   0.1f

/* ---- 角度环 PID 参数 ---- */
#define YAW_Kp   1.0f
#define YAW_Ki   0.0f
#define YAW_Kd   0.0f

static PID_TypeDef gPidLeft, gPidRight, gPidYaw;

float Speed_Middle = 30;
int Motor_Left, Motor_Right;

extern float Yaw;
extern volatile int mode;
extern volatile int flag;
extern volatile int32_t EncoderA_VEL, EncoderB_VEL;

void Control_Init(void)
{
    PID_InitTypeDef cfg;

    /* 左电机速度环 */
    cfg.Kp = SPD_Kp;  cfg.Ki = SPD_Ki;  cfg.Kd = SPD_Kd;
    cfg.Setpoint = 0;
    cfg.OutputUpperLimit = PWM_LIMIT;
    cfg.OutputLowerLimit = -PWM_LIMIT;
    cfg.DefaultOutput = 0;
    PID_Init(&gPidLeft, &cfg);

    /* 右电机速度环 */
    PID_Init(&gPidRight, &cfg);

    /* 角度环 */
    cfg.Kp = YAW_Kp;  cfg.Ki = YAW_Ki;  cfg.Kd = YAW_Kd;
    cfg.Setpoint = 0;
    cfg.OutputUpperLimit = 40;
    cfg.OutputLowerLimit = -40;
    cfg.DefaultOutput = 0;
    PID_Init(&gPidYaw, &cfg);
}

void Control(void)
{
    float TargetA, TargetB;
    float bias = 0;

    /* ---- 偏航/巡线计算 ---- */
    if (mode == 1 || mode == 2) {
        if (flag == 1) {
            PID_ChangeSetpoint(&gPidYaw, 0);
            bias = PID_Compute1(&gPidYaw, Yaw, GetUs());
        } else if (flag == 2) {
            bias = Incremental_Quantity();
        } else if (flag == 3) {
            float MyYaw = (Yaw > 0) ? Yaw - 180 : Yaw + 180;
            PID_ChangeSetpoint(&gPidYaw, 5);
            bias = PID_Compute1(&gPidYaw, MyYaw, GetUs());
        } else if (flag == 4) {
            bias = Incremental_Quantity();
        }
    } else if (mode == 3 || mode == 4) {
        if (flag == 1) {
            PID_ChangeSetpoint(&gPidYaw, 0);
            bias = PID_Compute1(&gPidYaw, Yaw, GetUs());
        } else if (flag == 2) {
            bias = Incremental_Quantity();
        } else if (flag == 3) {
            float MyYaw = (Yaw > 0) ? Yaw - 180 : Yaw + 180;
            PID_ChangeSetpoint(&gPidYaw, -110);
            bias = PID_Compute1(&gPidYaw, Yaw, GetUs());
        } else if (flag == 4) {
            bias = Incremental_Quantity4();
        }
    }

    TargetA = Speed_Middle - bias;
    TargetB = Speed_Middle + bias;

    PID_ChangeSetpoint(&gPidLeft, TargetA);
    PID_ChangeSetpoint(&gPidRight, TargetB);

    Motor_Left  = (int)PID_Compute1(&gPidLeft, EncoderA_VEL, GetUs());
    Motor_Right = (int)PID_Compute1(&gPidRight, EncoderB_VEL, GetUs());

    Set_Pwm(Motor_Left, Motor_Right);
}
