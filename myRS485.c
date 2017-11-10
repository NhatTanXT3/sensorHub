/*
 * myRS485.c
 *
 *  Created on: Mar 17, 2017
 *      Author: Nhat Tan
 */
/*-----system include----------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"

#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "utils/ringbuf.h"


/*-----my include-------------------*/
#include "myRS485.h"
#include "myIO.h"
#include "SAM.h"
/*
 *  RS485 7 module using UART7 and PA2
 */

/*
 *  RS485 6 module using UART6 and PA3
 */

/*
 *  RS485 2 module using UART2 and PB7
 */
void RS485_2_Init(){
    //
    // Enable Peripheral Clocks
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    //
    // Enable pin PD7 for UART2 U2TX
    // First open the lock and select the bits we want to modify in the GPIO commit register.
    //
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0x80;

    //
    // Now modify the configuration of the pins that we unlocked.
    //
    GPIOPinConfigure(GPIO_PD7_U2TX);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_7);

    //
    // Enable pin PD6 for UART2 U2RX
    //
    GPIOPinConfigure(GPIO_PD6_U2RX);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6);

    UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 1500000,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTEnable(UART2_BASE);


    //	UARTIntEnable(UART5_BASE, UART_INT_RX|UART_INT_RT);
    //	UARTFIFODisable(UART5_BASE);

    UARTFIFOEnable(UART2_BASE);
    UARTFIFOLevelSet(UART2_BASE,UART_FIFO_TX1_8,UART_FIFO_RX2_8);
    UARTTxIntModeSet(UART2_BASE,UART_TXINT_MODE_EOT);
    UARTIntEnable(UART2_BASE,  UART_INT_RT|UART_INT_RX|UART_INT_TX);

    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART2); //enable the UART interrup

//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7);

    //    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);
}

void UART2_Interrupt_Handler(void)
{
    uint32_t interrupt_status;
    interrupt_status=UARTIntStatus(UART2_BASE,true);

    if((interrupt_status&UART_INT_TX))
    {
//        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_7,0);
        //      GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,0);
        UARTIntClear(UART2_BASE,UART_INT_TX);
    }
    //	else if(interrupt_status&UART_INT_RX)
    //	{
    //		UARTIntClear(UART2_BASE,UART_INT_RX);//|UART_INT_RX
    //		char charData;
    //		while(UARTCharsAvail(UART2_BASE))
    //			charData=(char)UARTCharGet(UART2_BASE);
    //	}
    else if(interrupt_status&(UART_INT_RT|UART_INT_RX))
    {
        UARTIntClear(UART2_BASE,UART_INT_RT|UART_INT_RX);
        char charData[3];
        unsigned char dataCount=0;
        while(UARTCharsAvail(UART2_BASE))
        {
            charData[dataCount]=(char)UARTCharGet(UART2_BASE);
            dataCount++;
        }
        if(dataCount==2)
        {
            if(samReadBusy)
            {
                switch(samReadMode){
                case SAMREAD_POS8_:
                    samLoad8[samReadCurrentID_C2]=(unsigned char)charData[0];
                    samPosition8[samReadCurrentID_C2]=(unsigned char)charData[1];
                    break;
                case SAMREAD_POS12_:
                    samPosition12[samReadCurrentID_C2]=((charData[0]&0x1F)<<7)+(charData[1]&0x7F);
                    break;
                case SAMREAD_PD_:
                    samP[samReadCurrentID_C2]=charData[0];
                    samD[samReadCurrentID_C2]=charData[1];
                    break;
                case SAMREAD_I_:
                    samI[samReadCurrentID_C2]=charData[0];
                    break;
                case SAMREAD_AVRG_TORQUE_:
                    samAverageTorq[samReadCurrentID_C2]=((charData[0]&0x1F)<<7)+(charData[1]&0x7F);
                    break;
                default:
                    break;
                }
                samDataAvail[samReadCurrentID_C2]=1;
            }

        }
    }
}


/*
 *  RS485 4 module using UART4 and PA4
 */
void RS485_4_Init(){
    //
    // Enable Peripheral Clocks
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //
    // Enable pin PC4 for UART4 U4RX
    //
    GPIOPinConfigure(GPIO_PC4_U4RX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4);

    //
    // Enable pin PC5 for UART4 U4TX
    //
    GPIOPinConfigure(GPIO_PC5_U4TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_5);

    UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 1500000,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTEnable(UART4_BASE);
    //	IntPrioritySet(INT_UART5,0xE0);
    IntPrioritySet(INT_UART4,0);



    UARTFIFOEnable(UART4_BASE);
    UARTFIFOLevelSet(UART4_BASE,UART_FIFO_TX1_8,UART_FIFO_RX2_8);
    UARTTxIntModeSet(UART4_BASE,UART_TXINT_MODE_EOT);
    UARTIntEnable(UART4_BASE,  UART_INT_RT|UART_INT_RX|UART_INT_TX);



    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART4); //enable the UART interrup

//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
//    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4);

    //    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
}

void UART4_Interrupt_Handler(void)
{
    uint32_t interrupt_status;
    interrupt_status=UARTIntStatus(UART4_BASE,true);

    if((interrupt_status&UART_INT_TX))
    {

//        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0);
        //        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,0);
        UARTIntClear(UART4_BASE,UART_INT_TX);
    }
    //	else if(interrupt_status&UART_INT_RX)
    //	{
    //		UARTIntClear(UART4_BASE,UART_INT_RX);//|UART_INT_RX
    //		char charData;
    //		while(UARTCharsAvail(UART4_BASE))
    //			charData=(char)UARTCharGet(UART4_BASE);
    //	}
    else if(interrupt_status&(UART_INT_RT|UART_INT_RX))
    {
        UARTIntClear(UART4_BASE,UART_INT_RT|UART_INT_RX);

        char charData[3];
        unsigned char dataCount=0;
        //		unsigned int UART4_data=0;
        while(UARTCharsAvail(UART4_BASE))
        {
            charData[dataCount]=(char)UARTCharGet(UART4_BASE);
            dataCount++;
        }
        if(dataCount==3)
        {
            if(samReadBusy)
            {
                switch(samReadMode){
                case SAMREAD_POS8_:
                    samLoad8[samReadCurrentID_C4]=(unsigned char)charData[0];
                    samPosition8[samReadCurrentID_C4]=(unsigned char)charData[1];
                    break;
                case SAMREAD_POS12_:
                    samPosition12[samReadCurrentID_C4]=((charData[0]&0x1F)<<7)+(charData[1]&0x7F);
                    break;
                case SAMREAD_PD_:
                    samP[samReadCurrentID_C4]=charData[0];
                    samD[samReadCurrentID_C4]=charData[1];
                    break;
                case SAMREAD_I_:
                    samI[samReadCurrentID_C4]=charData[0];
                    break;
                case SAMREAD_AVRG_TORQUE_:
                    samAverageTorq[samReadCurrentID_C4]=((charData[0]&0x1F)<<7)+(charData[1]&0x7F);
                    break;
                default:
                    break;
                }
                samDataAvail[samReadCurrentID_C4]=1;
            }
        }
    }
}

/*
 *  RS485 3 module using UART3 and PB6
 */
void RS485_3_Init()
{
    //
    // Enable Peripheral Clocks
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //
    // Enable pin PC7 for UART3 U3TX
    //
    GPIOPinConfigure(GPIO_PC7_U3TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_7);

    //
    // Enable pin PC6 for UART3 U3RX
    //
    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6);

    UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTEnable(UART3_BASE);
    //	IntPrioritySet(INT_UART5,0xE0);
    IntPrioritySet(INT_UART3,0);

    //	UARTIntEnable(UART5_BASE, UART_INT_RX|UART_INT_RT);
    //	UARTFIFODisable(UART5_BASE);

    UARTFIFOEnable(UART3_BASE);
    UARTFIFOLevelSet(UART3_BASE,UART_FIFO_TX1_8,UART_FIFO_RX2_8);
    UARTTxIntModeSet(UART3_BASE,UART_TXINT_MODE_EOT);
    UARTIntEnable(UART3_BASE,  UART_INT_RT|UART_INT_RX|UART_INT_TX);
    //	UARTIntEnable(UART5_BASE,  UART_INT_RX|UART_INT_TX);


    IntMasterEnable(); //enable processor interrupts
//    IntEnable(INT_UART3); //enable the UART interrup

//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
//    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_6);
}

void UART3_Interrupt_Handler(void)
{
    uint32_t interrupt_status;
    interrupt_status=UARTIntStatus(UART3_BASE,true);

    if((interrupt_status&UART_INT_TX))
    {
//        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_6,0);
        //        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0);
        UARTIntClear(UART3_BASE,UART_INT_TX);
    }
    else if(interrupt_status&(UART_INT_RT|UART_INT_RX))
    {
        UARTIntClear(UART3_BASE,UART_INT_RT|UART_INT_RX);

        char charData[3];
        unsigned char dataCount=0;
        //      unsigned int UART4_data=0;
        while(UARTCharsAvail(UART3_BASE))
        {
            charData[dataCount]=(char)UARTCharGet(UART3_BASE);
            dataCount++;
        }
        if(dataCount==2)
        {
            if(samReadBusy)
            {
                switch(samReadMode){
                case SAMREAD_POS8_:
                    samLoad8[samReadCurrentID_C3]=(unsigned char)charData[0];
                    samPosition8[samReadCurrentID_C3]=(unsigned char)charData[1];
                    break;
                case SAMREAD_POS12_:
                    samPosition12[samReadCurrentID_C3]=((charData[0]&0x1F)<<7)+(charData[1]&0x7F);
                    break;
                case SAMREAD_PD_:
                    samP[samReadCurrentID_C3]=charData[0];
                    samD[samReadCurrentID_C3]=charData[1];
                    break;
                case SAMREAD_I_:
                    samI[samReadCurrentID_C3]=charData[0];
                    break;
                case SAMREAD_AVRG_TORQUE_:
                    samAverageTorq[samReadCurrentID_C3]=((charData[0]&0x1F)<<7)+(charData[1]&0x7F);
                    break;
                default:
                    break;
                }
                samDataAvail[samReadCurrentID_C3]=1;
            }
        }
    }
}
