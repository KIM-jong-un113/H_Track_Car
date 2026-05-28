#include "app_gyro.h"
#include "ti_msp_dl_config.h"
#include "my_lib/delay.h"

/* ---- MPU6050 寄存器 ---- */
#define MPU6050_ADDR          0x68
#define SMPLRT_DIV            0x19
#define CONFIG                0x1A
#define GYRO_CONFIG           0x1B
#define ACCEL_CONFIG          0x1C
#define ACCEL_XOUT_H          0x3B
#define GYRO_XOUT_H           0x43
#define PWR_MGMT_1            0x6B
#define WHO_AM_I              0x75

/* 陀螺仪 ±2000°/s → 16.4 LSB/(°/s) */
#define GYRO_SENS             16.4f
/* 加速度计 ±2g → 16384 LSB/g */
#define ACCEL_SENS            16384.0f

/* 互补滤波系数 */
#define FILTER_ALPHA          0.98f

float Pitch, Roll, Yaw;

static float gGyrZ_Offset = 0.0f;
static float gGyroAngleYaw = 0.0f;
static float gCompPitch = 0.0f;
static float gCompRoll  = 0.0f;
static uint64_t gLastUpdate = 0;

/* ---- I2C 底层读写 ---- */

static int I2C_WaitBusy(uint32_t timeout_us)
{
    uint32_t waited = 0;
    while (DL_I2C_getControllerStatus(I2C_MPU6050_INST)
           & DL_I2C_CONTROLLER_STATUS_BUSY) {
        delay_us(1);
        waited++;
        if (waited > timeout_us) return -1;
    }
    return 0;
}

static int I2C_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    DL_I2C_fillControllerTXFIFO(I2C_MPU6050_INST, buf, 2);
    DL_I2C_startControllerTransfer(I2C_MPU6050_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 2);
    return I2C_WaitBusy(5000);
}

static int I2C_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    DL_I2C_fillControllerTXFIFO(I2C_MPU6050_INST, &reg, 1);
    DL_I2C_startControllerTransfer(I2C_MPU6050_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    if (I2C_WaitBusy(5000)) return -1;

    DL_I2C_startControllerTransfer(I2C_MPU6050_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_RX, len);
    if (I2C_WaitBusy(5000)) return -1;

    uint8_t i;
    for (i = 0; i < len; i++) {
        buf[i] = DL_I2C_receiveControllerData(I2C_MPU6050_INST);
    }
    return 0;
}

/* ---- MPU6050 初始化 ---- */

int MPU6050_Init(void)
{
    int ret;

    delay_ms(100);

    /* 唤醒 MPU6050 */
    ret = I2C_WriteReg(PWR_MGMT_1, 0x00);
    if (ret) return -1;
    delay_ms(10);

    /* DLPF 设置 */
    ret = I2C_WriteReg(CONFIG, 0x00);
    if (ret) return -2;
    /* 采样率 */
    ret = I2C_WriteReg(SMPLRT_DIV, 0x04);
    if (ret) return -3;
    /* 陀螺仪量程: 2000dps */
    ret = I2C_WriteReg(GYRO_CONFIG, 0x18);
    if (ret) return -4;
    /* 加速度计量程: 2g */
    ret = I2C_WriteReg(ACCEL_CONFIG, 0x00);
    if (ret) return -5;

    delay_ms(10);

    /* 校准陀螺仪 Z 轴零偏 */
    float sum = 0;
    int i;
    for (i = 0; i < 200; i++) {
        uint8_t buf[6];
        int16_t gz;
        if (I2C_ReadRegs(GYRO_XOUT_H, buf, 6)) return -6;
        gz = ((int16_t)buf[4] << 8) | buf[5];
        sum += (float)gz / GYRO_SENS;
        delay_us(500);
    }
    gGyrZ_Offset = sum / 200.0f;

    gLastUpdate = GetUs();
    return 0;
}

void MPU6050_Zero_Yaw(void)
{
    gGyroAngleYaw = 0.0f;
    Yaw = 0.0f;
}

/* ---- 读取传感器并更新角度 ---- */

void MPU6050_Update(void)
{
    uint8_t accel[6], gyro[6];
    int16_t ax, ay, az, gx, gy, gz;
    float accel_pitch, accel_roll, gyro_rate_yaw;
    float gyro_rate_pitch, gyro_rate_roll;
    uint64_t now;
    float dt;

    now = GetUs();
    dt = (float)(now - gLastUpdate) / 1000000.0f;
    gLastUpdate = now;

    /* 限制最大 dt，防止上电后首次 dt 过大 */
    if (dt > 0.05f) dt = 0.005f;

    I2C_ReadRegs(ACCEL_XOUT_H, accel, 6);
    I2C_ReadRegs(GYRO_XOUT_H,  gyro,  6);

    ax = ((int16_t)accel[0] << 8) | accel[1];
    ay = ((int16_t)accel[2] << 8) | accel[3];
    az = ((int16_t)accel[4] << 8) | accel[5];
    gx = ((int16_t)gyro[0]  << 8) | gyro[1];
    gy = ((int16_t)gyro[2]  << 8) | gyro[3];
    gz = ((int16_t)gyro[4]  << 8) | gyro[5];

    /* 角速度 (°/s) */
    gyro_rate_pitch =  (float)gy / GYRO_SENS;  /* 绕 Y  → Pitch */
    gyro_rate_roll  = -(float)gx / GYRO_SENS;  /* 绕 X  → Roll  (符号匹配) */
    gyro_rate_yaw   = -(float)gz / GYRO_SENS - gGyrZ_Offset; /* 绕 Z → Yaw */

    /* 加速度计角度 */
    accel_pitch = atan2f((float)ay, (float)az) * 57.2957795f;
    accel_roll  = atan2f(-(float)ax, (float)az) * 57.2957795f;

    /* 互补滤波: Pitch / Roll */
    gCompPitch = FILTER_ALPHA * (gCompPitch + gyro_rate_pitch * dt)
               + (1.0f - FILTER_ALPHA) * accel_pitch;
    gCompRoll  = FILTER_ALPHA * (gCompRoll  + gyro_rate_roll  * dt)
               + (1.0f - FILTER_ALPHA) * accel_roll;

    /* Yaw: 纯积分 (无磁力计, 会有漂移) */
    gGyroAngleYaw += gyro_rate_yaw * dt;
    if (gGyroAngleYaw >  180.0f) gGyroAngleYaw -= 360.0f;
    if (gGyroAngleYaw < -180.0f) gGyroAngleYaw += 360.0f;

    Pitch = gCompPitch;
    Roll  = gCompRoll;
    Yaw   = gGyroAngleYaw;
}
