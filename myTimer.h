/*
 * MyTimer.h
 *
 *  Created on: May 26, 2015
 *      Author: NhatTan
 */
#include <stdint.h>

#ifndef MYTIMER_H_
#define MYTIMER_H_


//timer is used for setting constant frequencys
void SysTick_Init();

// timer is used for real time clock
void Timer0_init();
// timer is used for receiving data from SAM
void Timer1_init();
void Timer1_Reset();
//change to micro second unit when use 40Mhz clock system
uint32_t getMicroSecond();

#define SYSTEM_CLOCK_40MHZ


#ifdef SYSTEM_CLOCK_40MHZ

#define DELAY_500HZ_	26666
#define DELAY_200HZ_	66666
#define DELAY_100HZ_ 	133333
#define DELAY_50HZ_		266666
#define DELAY_10HZ_		1333333
#define DELAY_1HZ_		13333333
#define DELAY_200HZ_COMPESATOR_ 27000

#endif /*SYSTEM_CLOCK_40MHZ*/

volatile struct{
	unsigned char Hz_200:1;
	unsigned char Hz_100:1;
	unsigned char Hz_50:1;
	unsigned char Hz_20:1;
	unsigned char Hz_10:1;
	unsigned char Hz_5:1;
	unsigned char Hz_1:1;
}FlagTimer;


extern uint32_t preMicroSecond_angle;
extern uint32_t preMicroSecond_position;
extern uint32_t preMicroSecond_sonar;
extern uint32_t preMicroSecond_camera;
extern float sampling_time_second;

#endif /* MYTIMER_H_ */
