/*
 * SAM.c
 *
 *  Created on: Feb 27, 2017
 *      Author: Nhat Tan
 */
#include <stdint.h>
#include <stdbool.h>

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

#include "SAM.h"
#include "myRS485.h"

SAM sam1;
volatile unsigned int samPosition12[24];
volatile unsigned int samAverageTorq[24];
volatile unsigned char samPosition8[24];
volatile unsigned char samLoad8[24];
volatile unsigned char samDataAvail[24];
volatile unsigned char samEnableRead[24]={1,1,1,1,1,1,1,1,1,1,1,1,
                                          1,1,1,1,1,1,1,1,1,1,1,1};

volatile unsigned char samP[24];
volatile unsigned char samI[24];
volatile unsigned char samD[24];
volatile unsigned char  samReadBusy;
unsigned char samReadMode;

// use this variable care fully, this variable for transparent of hardware
volatile unsigned char samReadCurrentID_C7;
volatile unsigned char samReadCurrentID_C6;
volatile unsigned char samReadCurrentID_C4;
volatile unsigned char samReadCurrentID_C3;
volatile unsigned char samReadCurrentID_C2;
volatile unsigned char samReadCurrentID_C1;
//RS485_Channel_State channel_1, channel_2, channel3;

//SAM sam1;

//FlagDataAvailSAM flagDataAvailSAM;
////FlagBusySAM flagBusySAM;
//
//volatile unsigned char Channel_1;
//volatile unsigned char flagReadSuccess;
//volatile unsigned char Channel_1_ReadID;


void SAM_init(){

}
void SAM_send_bytes(unsigned char ID, char *data, unsigned char size)
{
    uint32_t ui32Base;
    switch (ID){
    case 0:
        ui32Base=UART_RS485_ID_0_;
        break;
    case 1:
        ui32Base=UART_RS485_ID_1_;
        break;
    case 2:
        ui32Base=UART_RS485_ID_2_;
        break;
    case 3:
        ui32Base=UART_RS485_ID_3_;
        break;
    case 4:
        ui32Base=UART_RS485_ID_4_;
        break;
    case 5:
        ui32Base=UART_RS485_ID_5_;
        break;
    case 6:
        ui32Base=UART_RS485_ID_6_;
        break;
    case 7:
        ui32Base=UART_RS485_ID_7_;
        break;
    case 8:
        ui32Base=UART_RS485_ID_8_;
        break;
    case 9:
        ui32Base=UART_RS485_ID_9_;
        break;
    case 10:
        ui32Base=UART_RS485_ID_10_;
        break;
    case 11:
        ui32Base=UART_RS485_ID_11_;
        break;
    case 12:
        ui32Base=UART_RS485_ID_12_;
        break;
    case 13:
        ui32Base=UART_RS485_ID_13_;
        break;
    case 14:
        ui32Base=UART_RS485_ID_14_;
        break;
    case 15:
        ui32Base=UART_RS485_ID_15_;
        break;
    case 16:
        ui32Base=UART_RS485_ID_16_;
        break;
    case 17:
        ui32Base=UART_RS485_ID_17_;
        break;
    case 18:
        ui32Base=UART_RS485_ID_18_;
        break;
    case 19:
        ui32Base=UART_RS485_ID_19_;
        break;
    case 20:
        ui32Base=UART_RS485_ID_20_;
        break;
    case 21:
        ui32Base=UART_RS485_ID_21_;
        break;
    case 22:
        ui32Base=UART_RS485_ID_22_;
        break;
    case 23:
        ui32Base=UART_RS485_ID_23_;
        break;
    case 24:
        ui32Base=UART_RS485_ID_24_;
        break;
    case 25:
        ui32Base=UART_RS485_ID_25_;
        break;
    case 26:
        ui32Base=UART_RS485_ID_26_;
        break;
    case 27:
        ui32Base=UART_RS485_ID_27_;
        break;
    default:
        break;

    }

    switch(ui32Base){
    case UART_RS485_4_:
        samReadCurrentID_C4=ID;
        //        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_PIN_3);
//        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_PIN_4);
        break;
    case UART_RS485_2_:
        samReadCurrentID_C2=ID;
        //        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);
//        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_7,GPIO_PIN_7);
        break;
    case UART_RS485_3_:
        samReadCurrentID_C3=ID;
//        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_6,GPIO_PIN_6);
        break;
//    case UART_RS485_1_:
//           samReadCurrentID_C1=ID;
//           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_PIN_2);
//           break;
//    case UART_RS485_6_:
//           samReadCurrentID_C6=ID;
//           GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_PIN_3);
//           break;
//    case UART_RS485_7_:
//           samReadCurrentID_C7=ID;
//           GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);
//           break;
    default:
        break;
    }

        unsigned char i;
    for(i=0;i<size;i++){
        UARTCharPut(ui32Base,data[i]);
        //  UARTCharPut(ui32Base,data[1]);
        //  UARTCharPut(ui32Base,data[2]);
        //  UARTCharPut(ui32Base,data[3]);
        //  UARTCharPut(ui32Base,data[4]);
        //  UARTCharPut(ui32Base,data[5]);
        //  UARTCharPut(ui32Base,data[6]);
    }
    //  else if(size==4)
}
void SAM_set_avergTorque(unsigned char ID,unsigned int value){
    samReadMode=SAMREAD_AVRG_TORQUE_;
    char data[7];
    data[0]=0xFF;
    data[1]=0xE0;
    data[2]=186;
    data[3]=ID;
    data[4]=value>>7;
    data[5]=value&0x7F;
    data[6]=(data[1]^data[2]^data[3]^data[4]^data[5])&0x7F;
    SAM_send_bytes(ID,data,7);
}
void SAM_get_avergTorque(unsigned char ID){
    samReadMode=SAMREAD_AVRG_TORQUE_;
    char data[7];
    data[0]=0xFF;
    data[1]=0xE0;
    data[2]=187;
    data[3]=ID;
    data[4]=0;
    data[5]=0;
    data[6]=(data[1]^data[2]^data[3]^data[4]^data[5])&0x7F;
    SAM_send_bytes(ID,data,7);
}
void SAM_set_PD_RuntimeQuick(unsigned char ID, unsigned char P, unsigned char D){
    samReadMode=SAMREAD_PD_;
    char data[7];
    data[0]=0xFF;
    data[1]=(7<<5)+ID;
    data[2]=0x0B;
    data[3]=P;
    data[4]=D;
    data[5]=(data[1]^data[2]^data[3]^data[4])&0x7F;
    SAM_send_bytes(ID,data,6);
}
void SAM_set_PD_Runtime(unsigned char ID, unsigned char P, unsigned char D){
    samReadMode=SAMREAD_PD_;
    char data[7];
    data[0]=0xFF;
    data[1]=0xE0;
    data[2]=176;
    data[3]=ID;
    data[4]=P;
    data[5]=D;
    data[6]=(data[1]^data[2]^data[3]^data[4]^data[5])&0x7F;
    SAM_send_bytes(ID,data,7);

}
void SAM_set_I_Runtime(unsigned char ID, unsigned char I){
    samReadMode=SAMREAD_I_;
    char data[7];
    data[0]=0xFF;
    data[1]=0xE0;
    data[2]=183;
    data[3]=ID;
    data[4]=I;
    data[5]=I;
    data[6]=(data[1]^data[2]^data[3]^data[4]^data[5])&0x7F;
    SAM_send_bytes(ID,data,7);
}

void SAM_get_PD(unsigned char ID){
    samReadMode=SAMREAD_PD_;
    char data[7];
    data[0]=0xFF;
    data[1]=0xE0;
    data[2]=175;
    data[3]=ID;
    data[4]=0;
    data[5]=0;
    data[6]=(data[1]^data[2]^data[3]^data[4]^data[5])&0x7F;
    SAM_send_bytes(ID,data,7);
}
void SAM_get_I(unsigned char ID){
    samReadMode=SAMREAD_I_;
    char data[7];
    data[0]=0xFF;
    data[1]=0xE0;
    data[2]=182;
    data[3]=ID;
    data[4]=0;
    data[5]=0;
    data[6]=(data[1]^data[2]^data[3]^data[4]^data[5])&0x7F;
    SAM_send_bytes(ID,data,7);
}
void SAM_get_jointAngle12bit(unsigned char ID){
    samReadMode=SAMREAD_POS12_;
    char data[7];
    data[0]=0xFF;
    data[1]=0xE0;
    //          0x22;
    data[2]=173;
    data[3]=ID;
    data[4]=0;
    data[5]=0;
    data[6]=(data[1]^data[2]^data[3]^data[4]^data[5])&0x7F;

    SAM_send_bytes(ID,data,7);
}

void SAM_set_jointAngle12bit(unsigned char ID,unsigned int value){
    if((value>=LOWER_12BIT_BOUNDARY_)&&(value<=UPPER_12BIT_BOUNDARY_))
    {
        char data[7];
        data[0]=0xFF;
        data[1]=0xE0;
        //          0x22;
        data[2]=200;
        data[3]=ID;

        data[4]=(unsigned char)((value>>7)&0x1F); // target position: upper 5bits
        data[5]=(unsigned char)(value&0x7F); //target position: lower 7 bits

        data[6]=(data[1]^data[2]^data[3]^data[4]^data[5])&0x7F;//check sum

        SAM_send_bytes(ID,data,7);
    }
}

void SAM_get_jointAngle8bit(unsigned char ID)
{
    samReadMode=SAMREAD_POS8_;
    char data[4];
    data[0]=0xFF;
    data[1]=(5<<5)|(ID);
    data[2]=0;
    data[3]=(data[1]^data[2])&0x7F;
    SAM_send_bytes(ID,data,4);
}

void SAM_set_jointAngle8bit(unsigned char ID,unsigned char Mode, unsigned char value)
{
    if(Mode>4)
        Mode=4;
    char data[4];
    data[0]=0xFF;
    data[1]=(Mode<<5)|(ID);
    data[2]=value;
    data[3]=(data[1]^data[2])&0x7F;

    SAM_send_bytes(ID,data,4);
}

void SAM_set_passiveMode(unsigned char ID)
{
    char data[4];
    data[0]=0xFF;
    data[1]=(6<<5)|(ID);
    data[2]=16;
    data[3]=(data[1]^data[2])&0x7F;
    SAM_send_bytes(ID,data,4);
}
