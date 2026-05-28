#ifndef __APP_GYRO_H__
#define __APP_GYRO_H__

#include "my_lib/sys.h"

extern float Pitch, Roll, Yaw;

int  MPU6050_Init(void);
void MPU6050_Zero_Yaw(void);
void MPU6050_Update(void);

#endif
