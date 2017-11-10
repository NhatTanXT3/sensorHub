/*
 * IMU.c
 *
 *  Created on: Jun 20, 2015
 *      Author: NhatTan
 */
#include <math.h>

#include "IMU.h"
#include "mpu6050.h"
#include "myTimer.h"



// Using to find zero rate values of the gyro



IMU_type IMU;

void IMU_init(){
    IMU.roll=0;
    IMU.pitch=0;
}

void angle(float Ts)
{
    //	MPU6050DataGetRaw(&MPU6050);
    if(((MPU6050.accY_raw) != 0) && ((MPU6050.accZ_raw) != 0))
    {
        IMU.pitch_acc=-atan2(MPU6050.accY_raw,MPU6050.accZ_raw)*rad_to_deg;
    }
    IMU.roll_acc=atan2(-MPU6050.accX_raw,sqrt(MPU6050.accY_raw*MPU6050.accY_raw+MPU6050.accZ_raw*MPU6050.accZ_raw))*rad_to_deg;

    IMU.pitch_rate=(MPU6050.gyroX_raw - MPU6050.gyroX_0Rate)/GYRO_CONVER_FACTOR;
    IMU.roll_rate=(MPU6050.gyroY_raw - MPU6050.gyroY_0Rate)/GYRO_CONVER_FACTOR;
    IMU.yaw_rate=(MPU6050.gyroZ_raw - MPU6050.gyroZ_0Rate)/GYRO_CONVER_FACTOR;

    IMU.roll_rate_fil=0.99*IMU.roll_rate_fil+0.01*IMU.roll_rate;
    IMU.pitch_rate_fil=0.99*IMU.pitch_rate_fil+0.01*IMU.pitch_rate;
    IMU.yaw_rate_fil=0.99*IMU.yaw_rate_fil+0.01*IMU.yaw_rate;
    IMU.pitch_gyro -= IMU.pitch_rate*Ts;
    IMU.roll_gyro += IMU.roll_rate*Ts;
    IMU.yaw_gyro += IMU.yaw_rate*Ts;//+ IMU.yaw_ofset


    IMU.pitch=0.99*(IMU.pitch-(IMU.pitch_rate)*Ts)+0.01*IMU.pitch_acc;
    IMU.roll=0.99*(IMU.roll+(IMU.roll_rate)*Ts)+0.01*IMU.roll_acc;

    IMU.cos_roll=cos(IMU.roll*deg_to_rad);
    IMU.cos_pitch=cos(IMU.pitch*deg_to_rad);

    IMU.sin_roll=sin(IMU.roll*deg_to_rad);
    IMU.sin_pitch=sin(IMU.pitch*deg_to_rad);
}

