/*
 * IMU.h
 *
 *  Created on: Jun 20, 2015
 *      Author: NhatTan
 */

#ifndef QUADROTOR_CURRENT_WORK_IMU_H_
#define QUADROTOR_CURRENT_WORK_IMU_H_

#define  GYRO_CONVER_FACTOR 32.768 // change the unit from bit to degree per second(dsp) : range 1000dps
//#define  GYRO_CONVER_FACTOR 131.072 // change the unit from bit to degree per second(dsp) : range 00dps

#define rad_to_deg  57.29578
#define deg_to_rad  0.01745329
//#define SETPOINT_ROLL 0.07
//#define SETPOINT_PITCH -1.87
//#define MAX_ADJ_ANGLE 5.0


typedef struct{
    float roll_rate;
    float pitch_rate;
    float yaw_rate;
    float roll_rate_fil;
    float pitch_rate_fil;
    float yaw_rate_fil;

	float roll_acc;
	float pitch_acc;
	float roll_gyro;
	float pitch_gyro;
	float yaw_gyro;
	float roll;
	float pitch;
	float yaw;
	float yaw_ofset;

	float sin_roll;
	float cos_roll;
	float sin_pitch;
	float cos_pitch;
} IMU_type;

extern IMU_type IMU;
void IMU_init();
void angle(float Ts);

#endif /* QUADROTOR_CURRENT_WORK_IMU_H_ */
