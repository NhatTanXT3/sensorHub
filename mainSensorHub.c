/*
 * main.c
 * Readversion
 * Test
 */
#include <stdint.h>
#include <stdbool.h>
//#include <math.h>
//#include <stdlib.h>




#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "utils/ringbuf.h"

#include "mySerial.h"

#include "myFIFO.h"
#include "myTimer.h"
#include "myIO.h"
#include "myRS485.h"
#include "SAM.h"

#include "numManipulate.h"

#include "mpu6050.h"
#include "myI2C.h"
#include "IMU.h"
#include "ZMP.h"

void task_200Hz();
void task_100Hz();
void task_50Hz();
void task_20Hz();
void communication();
void display_com();
void task_IMU();
void ZMP_left_update();
void ZMP_right_update();
struct FlagType{
    unsigned char display:1;
}Flag;


void main(){
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); //40Mhz
    myIO_init();

    led(LED_RED,0);
    led(LED_BLUE,0);
    led(LED_GREEN,0);

    led(LED_BLUE,1);


    Timer0_init();
    Timer1_init();
    SysTick_Init();

    UART5_Init();
    //	UART7_Init();

    RS485_4_Init();
    RS485_2_Init();
    RS485_3_Init();

    UART1_Init();
    UART6_Init();
    //========sensor init========
    //    I2C1_Init();
    //    while(MPU6050_Init());

    SerialPutStrLn(UART_PC_,"calib gyroscope offset ...");
    //    while(Calib_Gyro());

    //	SerialPutStrLn(UART_PC_,"calib accelerometer offset ...");
    //	while(Calib_Accelerometer_Amplitude());

    //	RS485_4_Init();
    //	RS485_2_Init();

    SerialPutStrLn(UART_PC_,"config done!");
    led(LED_BLUE,0);
    SysCtlDelay(SysCtlClockGet()/300);
    Flag.display=0;


    while(1)
    {
        communication();
        //        ZMP_left_update();
        //        ZMP_right_update();

        //        task_IMU();
        task_20Hz();
        task_50Hz();
        task_100Hz();
        task_200Hz();
    }
}
void task_20Hz(){
    if(FlagTimer.Hz_20)
    {
        FlagTimer.Hz_20=0;
        /*
         * Your code begin from here
         */
    }


}

void task_100Hz(){
    if(FlagTimer.Hz_100)
    {
        FlagTimer.Hz_100=0;
        /*
         * Your code begin from here
         */

    }
}

void task_200Hz()
{
    if(FlagTimer.Hz_200)
    {
        FlagTimer.Hz_200=0;
        /*
         * Your code begin from here
         */
        //        if(Flag.display)
        //        {
        //                            getRawZMPData(UART_ZMP_LEFT_);
        //                    getRawZMPData(UART_ZMP_RIGHT_);
        //        getFilteredZMPData(UART_ZMP_LEFT_);
        //        getFilteredZMPData(UART_ZMP_RIGHT_);
        //        }
    }
}
void task_50Hz(){
    if(FlagTimer.Hz_50)
    {
        FlagTimer.Hz_50=0;
        /*
         * Your code begin from here
         */
        if(Flag.display)
        {
            display_com();
            toggle_led[1]^=1;
            led(LED_BLUE,toggle_led[1]);

        }


    }
}

void task_IMU()
{
    if(flag_MPU6050_INTpin==1)
    {
        flag_MPU6050_INTpin=0;
        /*
         * Your code begin from here
         */
        uint32_t microSecond=0;
        microSecond=getMicroSecond();
        sampling_time_second=(float)(microSecond- preMicroSecond_angle)/1000000.0;
        preMicroSecond_angle=microSecond;
        /*
         * update sensor's datas
         */


        while(MPU6050DataGetRaw(&MPU6050.accX_raw))
        {}
        //		{led(LED_BLUE,1);}// loop untill data is read
        //		led(LED_BLUE,0);

        /*
         * sensor processing
         */
        angle(sampling_time_second);
        //		update_accelerometer();
        /*
         *  running controller
         */
        //		if(Flag.run_controller)
        //			Stable_Controller(sampling_time_second);
    }// end of (flag_MPU6050_INTpin==1)
}
#define U

void communication(){
    if(serialPC.Flag_receive){
        serialPC.Flag_receive=0;
        //=========== your code begin from here========
        if(serialPC.Command_Data[1]&0x80) // special command
        {
            switch(serialPC.Command_Data[1]){
            case PC_SUBSENSOR_SP_MODE_5_:
                if(serialPC.dataIndex==4){//check length of data
                    UARTCharPut(UART_HAND_,serialPC.Command_Data[2]);
                    UARTCharPut(UART_HAND_,'\n');
                }
                else{
                    SerialPutStrLn(UART_PC_,"e_l_5");
                }
                break;
            case PC_SUBSENSOR_SP_MODE_2_: // set position of SAM
                //                  SerialPutStrLn(UART_PC_,"sm1");
                if(((serialPC.dataIndex-3)%4)==0){ // check length of data
                    unsigned char numOfSam=serialPC.dataIndex/4;
                    unsigned char i;
                    unsigned char refIndex;
                    for(i=0;i<numOfSam;i++)
                    {
                        refIndex=4*i+2;
                        if(((serialPC.Command_Data[refIndex]^serialPC.Command_Data[refIndex+1]^serialPC.Command_Data[refIndex+2])&0x7F)==serialPC.Command_Data[refIndex+3])
                        {
                            sam1.id=serialPC.Command_Data[refIndex]&0x1F;
                            //sam1.mode=serialPC.Command_Data[refIndex]>>5;
                            sam1.SAMmode=((serialPC.Command_Data[refIndex]&0x60)>>3)+((serialPC.Command_Data[refIndex+1]&0x60)>>5);
                            sam1.position12=((serialPC.Command_Data[refIndex+1]&0x1F)<<7)+(serialPC.Command_Data[refIndex+2]&0X7F);
                            if(sam1.SAMmode==1){
                                SAM_set_jointAngle12bit(sam1.id,sam1.position12);
                                samEnableRead[sam1.id]=1;
                            }
                            else if(sam1.SAMmode==2){
                                SAM_set_passiveMode(sam1.id);
                                samEnableRead[sam1.id]=0;
                            }
                            //                          display_com();
                        }
                        else
                        {
                            UARTCharPut(UART_PC_,i+48);
                            SerialPutStrLn(UART_PC_,"e_cs");
                        }
                    }
                }
                else{
                    SerialPutStrLn(UART_PC_,"e_l_2");
                }
                break;
            case PC_SUBSENSOR_SP_MODE_3_:// set PID of 1 SAM
                SerialPutStrLn(UART_PC_,"sm3");
                if(serialPC.dataIndex==9){ //check length of data
                    if(((serialPC.Command_Data[2]^serialPC.Command_Data[3]^serialPC.Command_Data[4]^serialPC.Command_Data[5]^serialPC.Command_Data[6])&0x7F)==serialPC.Command_Data[7]){
                        sam1.id=serialPC.Command_Data[2]&0x1F;
                        sam1.P=serialPC.Command_Data[4]+((serialPC.Command_Data[3]<<5)&0x80);
                        sam1.I=serialPC.Command_Data[5]+((serialPC.Command_Data[3]<<6)&0x80);
                        sam1.D=serialPC.Command_Data[6]+((serialPC.Command_Data[3]<<7)&0x80);

                        while(samReadBusy);
                        samReadBusy=1;
                        SAM_set_PD_Runtime(sam1.id, sam1.P, sam1.D);
                        Timer1_Reset();

                        while(samReadBusy);
                        samReadBusy=1;
                        SAM_set_I_Runtime(sam1.id, sam1.I);
                        Timer1_Reset();
                        while(samReadBusy);

                    }
                    else{
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                }
                else{
                    SerialPutStrLn(UART_PC_,"e_l");
                }
                break;

            case PC_SUBSENSOR_SP_MODE_4_:// get PID of 1 SAM
                SerialPutStrLn(UART_PC_,"sm4");
                if(serialPC.dataIndex==4){ //check length of data
                    unsigned char update_count=0;
                    sam1.id=serialPC.Command_Data[2];
                    while(samReadBusy);
                    samReadBusy=1;
                    SAM_get_PD(sam1.id);
                    Timer1_Reset();

                    while(samReadBusy);
                    if(samDataAvail[sam1.id])
                    {
                        update_count++;
                        samDataAvail[sam1.id]=0;
                    }

                    samReadBusy=1;
                    SAM_get_I(sam1.id);
                    Timer1_Reset();

                    while(samReadBusy);
                    if(samDataAvail[sam1.id])
                    {
                        update_count++;
                        samDataAvail[sam1.id]=0;
                    }

                    if(update_count==2)
                    {
                        sam1.P=samP[sam1.id];
                        sam1.I=samI[sam1.id];
                        sam1.D=samD[sam1.id];
                        SerialSend_PID(UART_PC_,sam1.id,sam1.P, sam1.I,sam1.D);
                    }
                    else{
                        SerialPutStrLn(UART_PC_,"e_u");
                    }



                }
                else{
                    SerialPutStrLn(UART_PC_,"e_l");
                }
                break;

                //            case  PC_SUBSENSOR_SP_MODE_0_:
                //                if(serialPC.dataIndex==3){//check length of data
                //                    dataSendBuffer[0]=MCU2PC_HEADER_;
                //                    dataSendBuffer[1]=PC_SUBSENSOR_SP_MODE_0_;
                //
                //                    //                    unsigned char i;
                //                    unsigned char refIndex=2;
                //                    //                    for(i=0;i<4;i++){
                //                    //                        dataSendBuffer[refIndex++]=i&0x1F;
                //                    //                        dataSendBuffer[refIndex++]=( zmpLeft.rawForceSensor[i]>>7)&0x1F;
                //                    //                        dataSendBuffer[refIndex++]=( zmpLeft.rawForceSensor[i])&0x7F;
                //                    //                        dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //                    //                    }
                //                    //                    for(i=4;i<8;i++)
                //                    //                    {
                //                    //                        dataSendBuffer[refIndex++]=i&0x1F;
                //                    //                        dataSendBuffer[refIndex++]=( zmpRight.rawForceSensor[i-4]>>7)&0x1F;
                //                    //                        dataSendBuffer[refIndex++]=( zmpRight.rawForceSensor[i-4])&0x7F;
                //                    //                        dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //                    //                    }
                //
                //
                //                    dataSendBuffer[refIndex++]=(8&0x1F)+((MPU6050.accX_raw>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(MPU6050.accX_raw>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=(MPU6050.accX_raw)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(9&0x1F)+((MPU6050.accY_raw>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(MPU6050.accY_raw>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=(MPU6050.accY_raw)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(10&0x1F)+((MPU6050.accZ_raw>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(MPU6050.accZ_raw>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=(MPU6050.accZ_raw)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(11&0x1F)+((MPU6050.gyroX_raw>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(MPU6050.gyroX_raw>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=(MPU6050.gyroX_raw)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(12&0x1F)+((MPU6050.gyroY_raw>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(MPU6050.gyroY_raw>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=(MPU6050.gyroY_raw)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(13&0x1F)+((MPU6050.gyroZ_raw>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(MPU6050.gyroZ_raw>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=(MPU6050.gyroZ_raw)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex]=MCU2PC_TERMINATOR_;
                //                    SerialSendData(UART_PC_,dataSendBuffer);
                //                }else{
                //                    SerialPutStrLn(UART_PC_,"e_l_3");
                //                }
                //                break;
                //            case PC_SUBSENSOR_SP_MODE_1_:
                //                if(serialPC.dataIndex==3){//check length of data
                //                    dataSendBuffer[0]=MCU2PC_HEADER_;
                //                    dataSendBuffer[1]=PC_SUBSENSOR_SP_MODE_1_;
                //
                //                    //                    unsigned char i;
                //                    unsigned char refIndex=2;
                //                    //                    for(i=0;i<4;i++){
                //                    //                        dataSendBuffer[refIndex++]=i&0x1F;
                //                    //                        dataSendBuffer[refIndex++]=( zmpLeft.filterForceSensor[i]>>7)&0x1F;
                //                    //                        dataSendBuffer[refIndex++]=( zmpLeft.filterForceSensor[i])&0x7F;
                //                    //                        dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //                    //                    }
                //                    //                    for(i=4;i<8;i++)
                //                    //                    {
                //                    //                        dataSendBuffer[refIndex++]=i&0x1F;
                //                    //                        dataSendBuffer[refIndex++]=( zmpRight.filterForceSensor[i-4]>>7)&0x1F;
                //                    //                        dataSendBuffer[refIndex++]=( zmpRight.filterForceSensor[i-4])&0x7F;
                //                    //                        dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //                    //                    }
                //                    float imuRoll=IMU.roll*100.0;
                //                    float imuPitch=IMU.pitch*100.0;
                //                    float imuYaw=IMU.yaw_gyro*100.0;
                //
                //                    float imuRollRate=IMU.roll_rate_fil*100.0;
                //                    float imuPitchRate=IMU.pitch_rate_fil*100.0;
                //                    float imuYawRare=IMU.yaw_rate_fil*100.0;
                //
                //                    dataSendBuffer[refIndex++]=(8&0x1F)+((((int)imuRoll)>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(((int)imuRoll)>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=((int)imuRoll)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(9&0x1F)+((((int)imuPitch)>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(((int)imuPitch)>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=((int)imuPitch)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(10&0x1F)+((((int)imuYaw)>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(((int)imuYaw)>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=((int)imuYaw)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(11&0x1F)+((((int)imuRollRate)>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(((int)imuRollRate)>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=((int)imuRollRate)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(12&0x1F)+((((int)imuPitchRate)>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(((int)imuPitchRate)>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=((int)imuPitchRate)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex++]=(13&0x1F)+((((int)imuYawRare)>>9)&0x60);
                //                    dataSendBuffer[refIndex++]=(((int)imuYawRare)>>7)&0x7F;
                //                    dataSendBuffer[refIndex++]=((int)imuYawRare)&0x7F;
                //                    dataSendBuffer[refIndex++]=(dataSendBuffer[refIndex-3]^dataSendBuffer[refIndex-2]^dataSendBuffer[refIndex-1])&0x7F;
                //
                //                    dataSendBuffer[refIndex]=MCU2PC_TERMINATOR_;
                //                    SerialSendData(UART_PC_,dataSendBuffer);
                //                }else{
                //                    SerialPutStrLn(UART_PC_,"e_l_0");
                //                }
                //                break;
            default:
                break;
            }

        }else{
            if(serialPC.dataIndex==PC_SAM_MODE_1_DATALENGTH_)
            {
                unsigned char mode=((serialPC.Command_Data[1]&0x60)>>3)+((serialPC.Command_Data[2]&0x60)>>5);
                sam1.id=serialPC.Command_Data[1]&0x1F;
                switch(mode)
                {
                case PC_SAM_MODE_0_:
                    SerialPutStrLn(UART_PC_,"M0");
                    if(((serialPC.Command_Data[1]^serialPC.Command_Data[2]^serialPC.Command_Data[3])&0x7F)==serialPC.Command_Data[4])
                    {
                        sam1.mode=PC_SAM_MODE_0_;
                        sam1.position8=((serialPC.Command_Data[2]&0x01)<<7)+(serialPC.Command_Data[3]&0X7F);
                        SAM_set_jointAngle8bit(sam1.id,sam1.mode,sam1.position8);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                    break;
                case PC_SAM_MODE_1_:
                    SerialPutStrLn(UART_PC_,"M1");
                    if(((serialPC.Command_Data[1]^serialPC.Command_Data[2]^serialPC.Command_Data[3])&0x7F)==serialPC.Command_Data[4])
                    {
                        sam1.mode=PC_SAM_MODE_1_;
                        sam1.position8=((serialPC.Command_Data[2]&0x01)<<7)+(serialPC.Command_Data[3]&0X7F);
                        SAM_set_jointAngle8bit(sam1.id,sam1.mode,sam1.position8);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                    break;
                case PC_SAM_MODE_2_:
                    SerialPutStrLn(UART_PC_,"M2");
                    if(((serialPC.Command_Data[1]^serialPC.Command_Data[2]^serialPC.Command_Data[3])&0x7F)==serialPC.Command_Data[4])
                    {
                        sam1.mode=PC_SAM_MODE_2_;
                        sam1.position8=((serialPC.Command_Data[2]&0x01)<<7)+(serialPC.Command_Data[3]&0X7F);
                        SAM_set_jointAngle8bit(sam1.id,sam1.mode,sam1.position8);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                    break;
                case PC_SAM_MODE_3_:
                    SerialPutStrLn(UART_PC_,"M3");
                    if(((serialPC.Command_Data[1]^serialPC.Command_Data[2]^serialPC.Command_Data[3])&0x7F)==serialPC.Command_Data[4])
                    {
                        sam1.mode=PC_SAM_MODE_3_;
                        sam1.position8=((serialPC.Command_Data[2]&0x01)<<7)+(serialPC.Command_Data[3]&0X7F);
                        SAM_set_jointAngle8bit(sam1.id,sam1.mode,sam1.position8);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                    break;
                case PC_SAM_MODE_4_:
                    SerialPutStrLn(UART_PC_,"M4");
                    if(((serialPC.Command_Data[1]^serialPC.Command_Data[2]^serialPC.Command_Data[3])&0x7F)==serialPC.Command_Data[4])
                    {
                        sam1.mode=PC_SAM_MODE_4_;
                        sam1.position8=((serialPC.Command_Data[2]&0x01)<<7)+(serialPC.Command_Data[3]&0X7F);
                        SAM_set_jointAngle8bit(sam1.id,sam1.mode,sam1.position8);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                    break;

                case PC_SAM_MODE_5_:
                    //                        SerialPutStrLn(UART_PC_,"M5");
                    while(samReadBusy);
                    samReadBusy=1;
                    samReadCurrentID_C4=sam1.id;
                    SAM_get_jointAngle8bit(samReadCurrentID_C4);
                    Timer1_Reset();

                    while(samReadBusy);

                    if(samDataAvail[samReadCurrentID_C4])
                    {
                        samDataAvail[samReadCurrentID_C4]=0;
                        SerialSend_1_Position8(UART_PC_,sam1.id,samPosition8[sam1.id],samLoad8[sam1.id]);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"idNA");
                    }

                    break;
                case PC_SAM_MODE_6_:
                    SerialPutStrLn(UART_PC_,"M6"); // check if it have feedback or not.
                    if(((serialPC.Command_Data[1]^serialPC.Command_Data[2]^serialPC.Command_Data[3])&0x7F)==serialPC.Command_Data[4])
                    {
                        SAM_set_passiveMode(sam1.id);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                    break;
                case PC_SAM_MODE_7_:
                    //                        SerialPutStrLn(UART_PC_,"M7");
                    while(samReadBusy);
                    samReadBusy=1;
                    samReadCurrentID_C4=sam1.id;
                    SAM_get_jointAngle12bit(samReadCurrentID_C4);
                    Timer1_Reset();

                    while(samReadBusy);
                    if(samDataAvail[samReadCurrentID_C4])
                    {
                        samDataAvail[samReadCurrentID_C4]=0;
                        SerialSend_1_Position12(UART_PC_,samReadCurrentID_C4,samPosition12[sam1.id]);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"idNA");
                    }
                    break;
                case PC_SAM_MODE_8_:
                    SerialPutStrLn(UART_PC_,"M8");
                    if(((serialPC.Command_Data[1]^serialPC.Command_Data[2]^serialPC.Command_Data[3])&0x7F)==serialPC.Command_Data[4])
                    {
                        sam1.position12=((serialPC.Command_Data[2]&0x1F)<<7)+(serialPC.Command_Data[3]&0X7F);
                        SAM_set_jointAngle12bit(sam1.id,sam1.position12);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                    break;
                case PC_SAM_MODE_9_:
                    SerialPutStrLn(UART_PC_,"M9");
                    if(((serialPC.Command_Data[1]^serialPC.Command_Data[2]^serialPC.Command_Data[3])&0x7F)==serialPC.Command_Data[4])
                    {
                        sam1.averageTorq=((serialPC.Command_Data[2]&0x1F)<<7)+(serialPC.Command_Data[3]&0X7F);
                        SAM_set_avergTorque(sam1.id,sam1.averageTorq);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"e_cs");
                    }
                    break;

                case PC_SAM_MODE_10_:
                    SerialPutStrLn(UART_PC_,"M10");
                    while(samReadBusy);
                    samReadBusy=1;
                    samReadCurrentID_C4=sam1.id;
                    SAM_get_avergTorque(samReadCurrentID_C4);
                    Timer1_Reset();

                    while(samReadBusy);
                    if(samDataAvail[samReadCurrentID_C4])
                    {
                        samDataAvail[samReadCurrentID_C4]=0;
                        SerialSend_1_AverageTorq(UART_PC_,samReadCurrentID_C4,samAverageTorq[sam1.id]);
                    }
                    else
                    {
                        SerialPutStrLn(UART_PC_,"idNA");
                    }
                    break;


                default:
                    SerialPutStrLn(UART_PC_,"M?");
                    break;
                }
            }
            else
            {
                SerialPutStrLn(UART_PC_,"e_l_1");
            }

            //            switch(serialPC.Command_Data[1]){
            //            case COM2CTL_DISPLAY_ON_:
            //                Flag.display=1;
            //                //			display_mode=Uart.Command_Data[1];
            //                break;
            //            case COM2CTL_DISPLAY_OFF_:
            //                Flag.display=0;
            //                break;
            //            default:
            //                break;
            //            }
        }
    }
}

void ZMP_left_update(){
    if(serialZMPLeft.Flag_receive){
        serialZMPLeft.Flag_receive=0;
        //=========== your code begin from here========
        if(serialZMPLeft.Command_Data[1]==SENDING_KEY_LEFT_){
            if(((serialZMPLeft.dataIndex-3)%4)==0){ // check length of data
                unsigned char numOfPack=serialZMPLeft.dataIndex/4;
                unsigned char i;
                unsigned char refIndex;
                for(i=0;i<numOfPack;i++)
                {
                    refIndex=4*i+2;
                    if(((serialZMPLeft.Command_Data[refIndex]^serialZMPLeft.Command_Data[refIndex+1]^serialZMPLeft.Command_Data[refIndex+2])&0x7F)==serialZMPLeft.Command_Data[refIndex+3])
                    {
                        unsigned int sensorReaded=(serialZMPLeft.Command_Data[refIndex+1]<<7)+serialZMPLeft.Command_Data[refIndex+2];
                        switch (serialZMPLeft.Command_Data[refIndex]){
                        case ZMP_ID_RAW_P0:
                            zmpLeft.rawForceSensor[0]=sensorReaded;
                            break;
                        case ZMP_ID_RAW_P1:
                            zmpLeft.rawForceSensor[1]=sensorReaded;
                            break;
                        case ZMP_ID_RAW_P2:
                            zmpLeft.rawForceSensor[2]=sensorReaded;
                            break;
                        case ZMP_ID_RAW_P3:
                            zmpLeft.rawForceSensor[3]=sensorReaded;
                            break;
                        case ZMP_ID_FILTERED_P0:
                            zmpLeft.filterForceSensor[0]=sensorReaded;
                            break;
                        case ZMP_ID_FILTERED_P1:
                            zmpLeft.filterForceSensor[1]=sensorReaded;
                            break;
                        case ZMP_ID_FILTERED_P2:
                            zmpLeft.filterForceSensor[2]=sensorReaded;
                            break;
                        case ZMP_ID_FILTERED_P3:
                            zmpLeft.filterForceSensor[3]=sensorReaded;
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        UARTCharPut(UART_PC_,i+48);
                        SerialPutStrLn(UART_PC_,"e_zmp_l2");
                    }
                }
            }
            else{
                SerialPutStrLn(UART_PC_,"e_zmp_l1");
            }
        }
        else
        {
            SerialPutStrLn(UART_PC_,"e_zmp_l0");
        }
    }
}

void ZMP_right_update(){
    if(serialZMPRight.Flag_receive){
        serialZMPRight.Flag_receive=0;
        //=========== your code begin from here========
        if(serialZMPRight.Command_Data[1]==SENDING_KEY_RIGHT_){
            if(((serialZMPRight.dataIndex-3)%4)==0){ // check length of data
                unsigned char numOfPack=serialZMPRight.dataIndex/4;
                unsigned char i;
                unsigned char refIndex;
                for(i=0;i<numOfPack;i++)
                {
                    refIndex=4*i+2;
                    if(((serialZMPRight.Command_Data[refIndex]^serialZMPRight.Command_Data[refIndex+1]^serialZMPRight.Command_Data[refIndex+2])&0x7F)==serialZMPRight.Command_Data[refIndex+3])
                    {
                        unsigned int sensorReaded=(serialZMPRight.Command_Data[refIndex+1]<<7)+serialZMPRight.Command_Data[refIndex+2];
                        switch (serialZMPRight.Command_Data[refIndex]){
                        case ZMP_ID_RAW_P4:
                            zmpRight.rawForceSensor[0]=sensorReaded;
                            break;
                        case ZMP_ID_RAW_P5:
                            zmpRight.rawForceSensor[1]=sensorReaded;
                            break;
                        case ZMP_ID_RAW_P6:
                            zmpRight.rawForceSensor[2]=sensorReaded;
                            break;
                        case ZMP_ID_RAW_P7:
                            zmpRight.rawForceSensor[3]=sensorReaded;
                            break;
                        case ZMP_ID_FILTERED_P4:
                            zmpRight.filterForceSensor[0]=sensorReaded;
                            break;
                        case ZMP_ID_FILTERED_P5:
                            zmpRight.filterForceSensor[1]=sensorReaded;
                            break;
                        case ZMP_ID_FILTERED_P6:
                            zmpRight.filterForceSensor[2]=sensorReaded;
                            break;
                        case ZMP_ID_FILTERED_P7:
                            zmpRight.filterForceSensor[3]=sensorReaded;
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        UARTCharPut(UART_PC_,i+48);
                        SerialPutStrLn(UART_PC_,"e_zmp2");
                    }
                }
            }
            else{
                SerialPutStrLn(UART_PC_,"e_zmp_1");
            }
        }
        else
        {
            SerialPutStrLn(UART_PC_,"e_zmp_0");
        }
    }
}

unsigned char display_mode=COM2CTL_DISPLAY_MODE_1_;

void display_com(){
    char buffer[20];

    /*
     * khao sat PIDx
     */
    //	SerialPutChar(UART_PC_ ,PC2MCU_HEADER_);
    float2str(IMU.pitch,buffer);
    SerialPutChar(UART_PC_ ,CN_1_);
    SerialPutStr_NonTer(UART_PC_,buffer);

    float2str(IMU.roll,buffer);
    SerialPutChar(UART_PC_ ,CN_2_);
    SerialPutStr_NonTer(UART_PC_,buffer);
    //
    float2str(zmpLeft.filterForceSensor[0],buffer);
    SerialPutChar(UART_PC_ ,CN_3_);
    SerialPutStr_NonTer(UART_PC_,buffer);
    //	//Dte
    float2str(zmpLeft.filterForceSensor[1],buffer);
    SerialPutChar(UART_PC_ ,CN_4_);
    SerialPutStr_NonTer(UART_PC_,buffer);
    //		//
    float2str(zmpLeft.filterForceSensor[2],buffer); //acc_term
    SerialPutChar(UART_PC_ ,CN_5_);
    SerialPutStr_NonTer(UART_PC_,buffer);

    float2str(zmpLeft.filterForceSensor[3],buffer);
    SerialPutChar(UART_PC_ ,CN_6_);
    SerialPutStr_NonTer(UART_PC_,buffer);
    //

    float2str(zmpRight.filterForceSensor[0],buffer);
    SerialPutChar(UART_PC_ ,CN_7_);
    SerialPutStr_NonTer(UART_PC_,buffer);

    float2str(zmpRight.filterForceSensor[1],buffer);
    SerialPutChar(UART_PC_ ,CN_8_);
    SerialPutStr_NonTer(UART_PC_,buffer);
    //
    float2str(zmpRight.filterForceSensor[2],buffer);
    SerialPutChar(UART_PC_ ,CN_9_);
    SerialPutStr_NonTer(UART_PC_,buffer);

    float2str(zmpRight.filterForceSensor[3],buffer);
    SerialPutChar(UART_PC_ ,CN_10_);
    SerialPutStrLn(UART_PC_,buffer);
    //    SerialTerminator(UART_PC_);
    //	SerialPutChar(UART_PC_ ,PC2MCU_TERMINATOR_);
    SerialTerminator(UART_PC_);
}


