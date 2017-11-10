/*
 * myTimer.c
 *
 *  Created on: Jun 23, 2015
 *      Author: NhatTan
 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h"

#include "myIO.h"
#include  "myTimer.h"
#include "SAM.h"

//#include "PPM.h"


volatile uint32_t seconds=0; //variable for capture second

uint32_t preMicroSecond_angle=0;
uint32_t preMicroSecond_position=0;
uint32_t preMicroSecond_sonar=0;
uint32_t preMicroSecond_camera=0;
float sampling_time_second=0;

//extern uint8_t toggle_led[];
extern uint16_t PositionSensor_count;

extern volatile struct{
	uint32_t rise_time;
	uint32_t pulse_width;
	unsigned char flag_update;
	unsigned char fail_signal_count;
	float attitude;
}MaxSonar;

volatile uint8_t safe_flag=0; // using for RF signal, Sonar signal

/* timer is used for setting costant frequency
 *
 */
void SysTick_Init()
{
	SysTickPeriodSet(SysCtlClockGet()/200);//200Hz interrupt
	SysTickIntEnable();
	SysTickEnable();
}

volatile struct{
	unsigned char Hz_1;
	unsigned char Hz_5;
	unsigned char Hz_10;
	unsigned char Hz_100;
	unsigned char Hz_50;
	unsigned char Hz_20;
}SysTick_Int_Count;
/*
 * COUNT is defined for systick interrupt each 1/200 ms
 */
#define COUNT_5_HZ_		40
#define COUNT_10_HZ_	20
#define COUNT_20_HZ_	10
#define COUNT_50_HZ_	4
#define COUNT_100_HZ_	2


volatile unsigned char SycTick_Int_Count=0;

void SycTick_Interrupt_Handler(void)
{
	SysTick_Int_Count.Hz_100++;
	SysTick_Int_Count.Hz_50++;
	SysTick_Int_Count.Hz_20++;
	SysTick_Int_Count.Hz_10++;
	SysTick_Int_Count.Hz_5++;

	FlagTimer.Hz_200=1;

	if(SysTick_Int_Count.Hz_100==COUNT_100_HZ_)
	{
		SysTick_Int_Count.Hz_100=0;
		FlagTimer.Hz_100=1;

	}
	if(SysTick_Int_Count.Hz_50==COUNT_50_HZ_)
	{
		SysTick_Int_Count.Hz_50=0;
		FlagTimer.Hz_50=1;
	}

	if(SysTick_Int_Count.Hz_20==COUNT_20_HZ_)
	{
		SysTick_Int_Count.Hz_20=0;
		FlagTimer.Hz_20=1;
	}
	if(SysTick_Int_Count.Hz_10==COUNT_10_HZ_)
	{
		SysTick_Int_Count.Hz_10=0;
		FlagTimer.Hz_10=1;

	}

	if(SysTick_Int_Count.Hz_5==COUNT_5_HZ_)
	{
		SysTick_Int_Count.Hz_5=0;
		FlagTimer.Hz_5=1;
	}
}

// timer is used for receiving data from SAM
void Timer1_init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE,TIMER_CFG_A_ONE_SHOT);
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()/10000);

	IntMasterEnable(); //enable processor interrupts
	IntEnable(INT_TIMER1A); //enable the GPIOD interrupt
	IntPrioritySet(INT_TIMER1A,0x00);// configurated in Timer0_init()

	TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);

	TimerEnable(TIMER1_BASE,TIMER_A);
}
void  Timer1_Interrupt_Handler(void){
	TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
	samReadBusy=0;
	toggle_led[1]^=1;
	led(LED_BLUE,toggle_led[1]);
}
void Timer1_Reset(){
	TimerEnable(TIMER1_BASE,TIMER_A);
}
// timer is used for real time clock
void Timer0_init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC_UP);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());

	IntMasterEnable(); //enable processor interrupts
	IntEnable(INT_TIMER0A); //enable the GPIOD interrupt
	IntPrioritySet(INT_TIMER0A,0x00);// configurated in Timer0_init()

	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);

	TimerEnable(TIMER0_BASE,TIMER_A);
}

/*
 * Interrupt at every 1 second
 */
void Timer0_Interrupt_Handler(void)
{
	FlagTimer.Hz_1=1;
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	seconds++;
}

uint32_t getMicroSecond()
{
	uint32_t time;
	time=seconds*1000000+TimerValueGet(TIMER0_BASE,TIMER_A)/40;
	return time; //change to micro second unit when use 40Mhz clock system
}
