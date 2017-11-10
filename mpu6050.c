#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "utils/ringbuf.h"

#include "mpu6050.h"
#include "myI2C.h"
#include "myIO.h"
#include "mySerial.h"
#include "myFIFO.h"
#include "numManipulate.h"

volatile uint8_t flag_MPU6050_INTpin=0;
volatile uint8_t flag_I2C_connection=1;
MPU6050_type MPU6050;
/*------------------------config cho MPU6050--------------------------------------------
 *
 --------------------------------------------------------------------------------------*/
#define GYRO_NOISE_RANGE 10 //use with gyro range: 1000 deg/s
#define UART_DEBUG_CALIB_GYRO
#define ACC_NOISE_RANGE_ 0.15
uint8_t Calib_Gyro()
{

	int32_t sum[3]={0,0,0};
	uint16_t numOfsamples=200;

	int16_t buffer[3];
	int16_t max_value[3];
	int16_t min_value[3];
	uint8_t i,j;

	while(flag_MPU6050_INTpin==0);
	flag_MPU6050_INTpin=0;

	//	if(MPU6050DataGetRaw(buffer))
	if(MPU6050DataGyroGetRaw (&buffer[0],&buffer[1],&buffer[2]))
	{
#ifdef UART_DEBUG_CALIB_GYRO
		SerialPutStrLn(UART_PC_,"error I2C 0!");
#endif
		return 1;
	}

	for(i=0;i<=2;i++)
	{
		max_value[i]=buffer[i];
		min_value[i]=buffer[i];
	}

	for (i=1; i<=numOfsamples;i++)
	{
		while(flag_MPU6050_INTpin ==  0);
		flag_MPU6050_INTpin=0;

		if(MPU6050DataGyroGetRaw (&buffer[0],&buffer[1],&buffer[2]))
		{
#ifdef UART_DEBUG_CALIB_GYRO
			SerialPutStrLn(UART_PC_," error I2C!");
#endif
			return 1;
		}

		for(j=0;j<=2;j++)
		{
			if(buffer[j]>max_value[j]) max_value[j]=buffer[j];
			else if(buffer[j]<min_value[j])  min_value[j]=buffer[j];
			if((max_value[j]-min_value[j])>=GYRO_NOISE_RANGE)
			{
#ifdef UART_DEBUG_CALIB_GYRO
				//				SerialPutStrLn(UART_PC_,"error gyro noise!");
#endif
				return 1;
			}
			// code sum
			sum[j] += buffer[j];
		}

	}//end of for

	MPU6050.gyroX_0Rate=(float)sum[0]/numOfsamples;
	MPU6050.gyroY_0Rate=(float)sum[1]/numOfsamples;
	MPU6050.gyroZ_0Rate=(float)sum[2]/numOfsamples;
	//	toggle_led[2]^=1;
	//	led(LED_RED,toggle_led[2]);



	//#ifdef	USE_TXFIFO_
	//	FIFO_3_float_display(MPU6050.gyroX_0Rate,MPU6050.gyroY_0Rate,MPU6050.gyroZ_0Rate);
	//#else
	//	UART_3_float_display(UART_BASE,MPU6050.gyroX_0Rate,MPU6050.gyroY_0Rate,MPU6050.gyroZ_0Rate);
	//#endif
	SerialPutStrLn(UART_PC_,"Gyro offset: ");
	char uartBuffer[10];
	SerialPutStr_NonTer(UART_PC_,"X offset: ");
	float2str(MPU6050.gyroX_0Rate,uartBuffer);
	SerialPutStrLn(UART_PC_,uartBuffer);

	SerialPutStr_NonTer(UART_PC_,"Y offset: ");
	float2str(MPU6050.gyroY_0Rate,uartBuffer);
	SerialPutStrLn(UART_PC_,uartBuffer);

	SerialPutStr_NonTer(UART_PC_,"Z offset: ");
	float2str(MPU6050.gyroZ_0Rate,uartBuffer);
	SerialPutStrLn(UART_PC_,uartBuffer);

	return 0;
}



//void MPU6050_INTpin_Init()
//{
//	//
//	// Enable Peripheral Clocks
//	//
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//	//
//	// Enable pin PE3 for GPIOInput
//	//
//	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);
//
//	//l
//	//	// set interrupt when both edges happen
//	//
//	GPIOIntTypeSet(GPIO_PORTE_BASE,GPIO_PIN_3,GPIO_RISING_EDGE);
//
//	//enable processor interrupts
//	IntMasterEnable();
//
//	IntPrioritySet(INT_GPIOE,0x20);
//
//	//enable the GPIOD interrupt
//	IntEnable(INT_GPIOE);
//
//	//enable the GPIO interrupt for pinE3
//	GPIOIntEnable(GPIO_PORTE_BASE,GPIO_PIN_3);
//}

void MPU6050_INTpin_Init()
{
	//
	// Enable Peripheral Clocks
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	//
	// Enable pin PD1 for GPIOInput
	//
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_1);

	//l
	//	// set interrupt when both edges happen
	//
	GPIOIntTypeSet(GPIO_PORTD_BASE,GPIO_PIN_1,GPIO_RISING_EDGE);

	//enable processor interrupts
	IntMasterEnable();

	IntPrioritySet(INT_GPIOD,0x20);

	//enable the GPIOD interrupt
	IntEnable(INT_GPIOD);

	//enable the GPIO interrupt for pinD1
	GPIOIntEnable(GPIO_PORTD_BASE,GPIO_PIN_1);
}

void GPIOD_Interrupt_Handler(void)
{
	// return: true-the masked interrupt status | false -the current interrupt status
	uint32_t interrupt_status;
	interrupt_status=GPIOIntStatus(GPIO_PORTD_BASE,true);
	//	toggle_led[2]^=1;
	//	led(LED_RED,toggle_led[2]);
	if(interrupt_status==GPIO_INT_PIN_1)
	{
		GPIOIntClear(GPIO_PORTD_BASE,GPIO_INT_PIN_1);
		flag_MPU6050_INTpin=1;
	}
}

uint8_t MPU6050_Init()
{

	MPU6050_INTpin_Init();

	uint8_t DataBuffer;
	/* Reset device*/
	DataBuffer=MPU6050_PWR_MGMT_1_DEVICE_RESET;
	if(I2C_Write(I2C_USE,MPU6050_ADDRESS,MPU6050_O_PWR_MGMT_1,&DataBuffer,1))
	{return 1;}
	SysCtlDelay(4000000); //delay 0.1s at system clock = 40Mhz

	/*wake up chip*/
	DataBuffer=MPU6050_PWR_MGMT_1_CLKSEL_S;
	if(I2C_Write(I2C_USE,MPU6050_ADDRESS,MPU6050_O_PWR_MGMT_1,&DataBuffer,1))
	{return 1;}

	/*set sample rate*/ // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz => MPU6050_O_SMPLRT_DIV=7
	DataBuffer=7;
	if(I2C_Write(I2C_USE,MPU6050_ADDRESS,MPU6050_O_SMPLRT_DIV,&DataBuffer,1))
	{return 1;}

	/*Set Gyro Full Scale Range to ±1000deg/s*/
	DataBuffer=MPU6050_GYRO_CONFIG_FS_SEL_1000;
	if(I2C_Write(I2C_USE,MPU6050_ADDRESS,MPU6050_O_GYRO_CONFIG,&DataBuffer,1))
	{return 1;}

	// Set Accelerometer Full Scale Range to ±8g
	DataBuffer=MPU6050_ACCEL_CONFIG_AFS_SEL_8G;
	if(I2C_Write(I2C_USE,MPU6050_ADDRESS,MPU6050_O_ACCEL_CONFIG,&DataBuffer,1))
	{return 1;}

	// Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering for 8 KHz sampling
	DataBuffer=MPU6050_CONFIG_DLPF_CFG_44_42;
	if(I2C_Write(I2C_USE,MPU6050_ADDRESS,MPU6050_O_CONFIG,&DataBuffer,1))
	{return 1;}

	// enable INT pin
	DataBuffer=MPU6050_INT_ENABLE_DATA_RDY_EN;
	if(I2C_Write(I2C_USE,MPU6050_ADDRESS,MPU6050_O_INT_ENABLE,&DataBuffer,1))
	{return 1;}

	return 0;
}

uint8_t MPU6050DataAccelGetRaw (int16_t *pui16AccelX,int16_t *pui16AccelY, int16_t *pui16AccelZ)
{
	unsigned char mpu6050_raw_data[6];
	if (I2C_Read(I2C_USE,MPU6050_ADDRESS,MPU6050_O_ACCEL_XOUT_H,mpu6050_raw_data,6)) return 1;
	*pui16AccelX = ((mpu6050_raw_data[0]<<8)|mpu6050_raw_data[1]);
	*pui16AccelY = (mpu6050_raw_data[2]<<8)|mpu6050_raw_data[3];
	*pui16AccelZ = (mpu6050_raw_data[4]<<8)|mpu6050_raw_data[5];
	return 0;
}

uint8_t MPU6050DataGyroGetRaw (int16_t *pui16AccelX,int16_t *pui16AccelY, int16_t *pui16AccelZ)
{
	unsigned char mpu6050_raw_data[6];
	if (I2C_Read(I2C_USE,MPU6050_ADDRESS,MPU6050_O_GYRO_XOUT_H,mpu6050_raw_data,6)) return 1;
	*pui16AccelX = (mpu6050_raw_data[0]<<8)|mpu6050_raw_data[1];
	*pui16AccelY = (mpu6050_raw_data[2]<<8)|mpu6050_raw_data[3];
	*pui16AccelZ = (mpu6050_raw_data[4]<<8)|mpu6050_raw_data[5];
	return 0;
}

uint8_t MPU6050DataTempGetRaw (uint16_t *pui16Temp)
{
	uint8_t mpu6050_raw_data[2];
	if (I2C_Read(I2C_USE,MPU6050_ADDRESS,MPU6050_O_TEMP_OUT_H,mpu6050_raw_data,2)) return 1;
	*pui16Temp = (mpu6050_raw_data[0]<<8)|mpu6050_raw_data[1];
	return 0;
}


uint8_t MPU6050DataGetRaw(int16_t *pui16Data)
{
	uint8_t mpu6050_raw_data[14];
	IntPriorityMaskSet(0xE0);
	if (I2C_Read(I2C_USE,MPU6050_ADDRESS,MPU6050_O_ACCEL_XOUT_H,mpu6050_raw_data,14))
	{
		IntPriorityMaskSet(0);
		return 1;//return 1;
	}
	IntPriorityMaskSet(0);

	*pui16Data++ = (mpu6050_raw_data[0]<<8)|mpu6050_raw_data[1];
	*pui16Data++ = (mpu6050_raw_data[2]<<8)|mpu6050_raw_data[3];
	*pui16Data++ = (mpu6050_raw_data[4]<<8)|mpu6050_raw_data[5];
	*pui16Data++ = (mpu6050_raw_data[6]<<8)|mpu6050_raw_data[7];
	*pui16Data++ = (mpu6050_raw_data[8]<<8)|mpu6050_raw_data[9];
	*pui16Data++ = (mpu6050_raw_data[10]<<8)|mpu6050_raw_data[11];
	*pui16Data = (mpu6050_raw_data[12]<<8)|mpu6050_raw_data[13];
	return 0;
}

void update_accelerometer(){
	MPU6050.accX=(float)MPU6050.accX_raw*ACCLEROMETER_SCALE_FACTOR;
	MPU6050.accY=(float)MPU6050.accY_raw*ACCLEROMETER_SCALE_FACTOR;
	MPU6050.accZ=(float)MPU6050.accZ_raw*ACCLEROMETER_SCALE_FACTOR;
	MPU6050.acc_amplitude=sqrt(MPU6050.accX*MPU6050.accX+MPU6050.accY*MPU6050.accY+MPU6050.accZ*MPU6050.accZ);
	MPU6050.z1_dot_dot=MPU6050.acc_amplitude-MPU6050.acc_amplitude_offset;
	MPU6050.z1_dot_dot_filter=MPU6050.z1_dot_dot_filter*0.95+MPU6050.z1_dot_dot*0.05;
}


//uint8_t Calib_Accelerometer_Amplitude()
//{
//
//	char uartBuffer[10];
//	float sum=0;
//	uint16_t numOfsamples=200;
//	//	float sumxy[2]={0,0};
//
//
//	float max_value;
//	float min_value;
//	uint8_t i;
//
//	while(flag_MPU6050_INTpin==0);
//	flag_MPU6050_INTpin=0;
//
//	if(MPU6050DataAccelGetRaw (&MPU6050.accX_raw,&MPU6050.accY_raw,&MPU6050.accZ_raw))
//	{
//#ifdef UART_DEBUG_CALIB_GYRO
//		SerialPutStrLn(UART_PC_,"error I2C 0!");
//#endif
//		return 1;
//	}
//
//	update_accelerometer();
//
//	max_value=MPU6050.acc_amplitude;
//	min_value=MPU6050.acc_amplitude;
//
//
//	for (i=1; i<=numOfsamples;i++)
//	{
//		while(flag_MPU6050_INTpin ==  0);
//		flag_MPU6050_INTpin=0;
//		if(MPU6050DataAccelGetRaw (&MPU6050.accX_raw,&MPU6050.accY_raw,&MPU6050.accZ_raw))
//		{
//#ifdef UART_DEBUG_CALIB_GYRO
//			SerialPutStrLn(UART_PC_," error I2C!");
//#endif
//			return 1;
//		}
//		if(MPU6050.acc_amplitude>max_value) max_value=MPU6050.acc_amplitude;
//		else if(MPU6050.acc_amplitude<min_value) min_value=MPU6050.acc_amplitude;
//
//		if((max_value-min_value)>=ACC_NOISE_RANGE_)
//		{
//#ifdef UART_DEBUG_CALIB_GYRO
//			SerialPutStrLn(UART_PC_,"error acc noise!");
//#endif
//			return 1;
//		}
//		sum+=MPU6050.acc_amplitude;
//	}//end of for
//
//	MPU6050.acc_amplitude_offset=sum/numOfsamples;
//
//
//	SerialPutStr_NonTer(UART_PC_,"accleration amplitude offset: ");
//	float2str(MPU6050.acc_amplitude_offset,uartBuffer);
//	SerialPutStrLn(UART_PC_,uartBuffer);
//
//	return 0;
//}




