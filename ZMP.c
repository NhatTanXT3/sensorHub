/*
 * ZMP.c
 *
 *  Created on: Jun 22, 2017
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
#include "ZMP.h"
#include "mySerial.h"
#include "myIO.h"
#include "SAM.h"


MySerial serialZMPLeft;
MySerial serialZMPRight;
MyZMP zmpLeft;
MyZMP zmpRight;
/*
 *  UART7 init
 */
void UART7_Init(){
    //
    // Enable Peripheral Clocks
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    //
    // Enable pin PE0 for UART7 U7RX
    //
    GPIOPinConfigure(GPIO_PE0_U7RX);
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0);

    //
    // Enable pin PE1 for UART7 U7TX
    //
    GPIOPinConfigure(GPIO_PE1_U7TX);
    GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_1);

    UARTConfigSetExpClk(UART7_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


    UARTEnable(UART7_BASE);

    UARTFIFOEnable(UART7_BASE);
    UARTFIFOLevelSet(UART7_BASE,UART_FIFO_TX1_8,UART_FIFO_RX4_8);
    UARTIntEnable(UART7_BASE,  UART_INT_RT|UART_INT_RX);

    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART7); //enable the UART interrup

}

void UART7_Interrupt_Handler(void)
{
    uint32_t interrupt_status = UARTIntStatus(UART7_BASE,true);
    if(interrupt_status&(UART_INT_RX|UART_INT_RT))
    {
        UARTIntClear(UART7_BASE,UART_INT_RX|UART_INT_RT);
        while(UARTCharsAvail(UART7_BASE))
        {
            char charData=(char)UARTCharGet(UART7_BASE);
            if(charData==PC2MCU_HEADER_){
                serialZMPLeft.dataIndex=0;
            }
            else if(charData==PC2MCU_TERMINATOR_){
                serialZMPLeft.Flag_receive=1;
            }
            if(serialZMPLeft.dataIndex<SERIAL_BUFFER_SIZE_)
                serialZMPLeft.Command_Data[serialZMPLeft.dataIndex++]=charData;
            else
                serialZMPLeft.dataIndex=0;
        }
    }
    else{
        UARTIntClear(UART7_BASE,0xFF);
    }
}

/*
 *  RS485 6 module using UART6 and PA3
 */
void UART6_Init()
{
    //
    // Enable Peripheral Clocks
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    //
    // Enable pin PD5 for UART6 U6TX
    //
    GPIOPinConfigure(GPIO_PD5_U6TX);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_5);

    //
    // Enable pin PD4 for UART6 U6RX
    //
    GPIOPinConfigure(GPIO_PD4_U6RX);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4);

    UARTConfigSetExpClk(UART6_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTEnable(UART6_BASE);


    UARTFIFOEnable(UART6_BASE);
    UARTFIFOLevelSet(UART6_BASE,UART_FIFO_TX1_8,UART_FIFO_RX4_8);
    UARTIntEnable(UART6_BASE,  UART_INT_RT|UART_INT_RX);

    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART6); //enable the UART interrup
}

void UART6_Interrupt_Handler(void)
{
    uint32_t interrupt_status = UARTIntStatus(UART6_BASE,true);
    if(interrupt_status&(UART_INT_RX|UART_INT_RT))
    {
        UARTIntClear(UART6_BASE,UART_INT_RX|UART_INT_RT);
        while(UARTCharsAvail(UART6_BASE))
        {
            char charData=(char)UARTCharGet(UART6_BASE);
            if(charData==PC2MCU_HEADER_){
                serialZMPLeft.dataIndex=0;
            }
            else if(charData==PC2MCU_TERMINATOR_){
                serialZMPLeft.Flag_receive=1;
            }
            if(serialZMPLeft.dataIndex<SERIAL_BUFFER_SIZE_)
                serialZMPLeft.Command_Data[serialZMPLeft.dataIndex++]=charData;
            else
                serialZMPLeft.dataIndex=0;
        }
    }
    else{
        UARTIntClear(UART6_BASE,0xFF);
    }
}
/*
 *  RS485 1 module using UART1 and PB2
 */
void UART1_Init(){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0);

    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_1);


    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTEnable(UART1_BASE);


    //  UARTIntEnable(UART5_BASE, UART_INT_RX|UART_INT_RT);
    //  UARTFIFODisable(UART5_BASE);

    UARTFIFOEnable(UART1_BASE);
    UARTFIFOLevelSet(UART1_BASE,UART_FIFO_TX1_8,UART_FIFO_RX4_8);
    UARTIntEnable(UART1_BASE,  UART_INT_RT|UART_INT_RX);

    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART1); //enable the UART interrup
}

void UART1_Interrupt_Handler(void)
{
    uint32_t interrupt_status = UARTIntStatus(UART1_BASE,true);
    if(interrupt_status&(UART_INT_RX|UART_INT_RT))
    {
        UARTIntClear(UART1_BASE,UART_INT_RX|UART_INT_RT);
        while(UARTCharsAvail(UART1_BASE))
        {
            char charData=(char)UARTCharGet(UART1_BASE);
            if(charData==PC2MCU_HEADER_){
                serialZMPRight.dataIndex=0;
            }
            else if(charData==PC2MCU_TERMINATOR_){
                serialZMPRight.Flag_receive=1;
            }
            if(serialZMPRight.dataIndex<SERIAL_BUFFER_SIZE_)
                serialZMPRight.Command_Data[serialZMPRight.dataIndex++]=charData;
            else
                serialZMPRight.dataIndex=0;
        }
    }
    else{
        UARTIntClear(UART1_BASE,0xFF);
    }
}



void getRawZMPData(uint32_t ui32Base){
    char mdata[2];
    mdata[0]=SEND_RAW_DATA_;
    mdata[1]=MCU2ZMP_TERMINATOR_;
    ZMP_send_bytes(ui32Base,mdata,2);
}
void getFilteredZMPData(uint32_t ui32Base){
    char mdata[2];
    mdata[0]=SEND_FILTERED_DATA_;
    mdata[1]=MCU2ZMP_TERMINATOR_;
    ZMP_send_bytes(ui32Base,mdata,2);
}

void ZMP_send_bytes(uint32_t ui32Base_, char *data, unsigned char size)
{
    unsigned char i;
    for(i=0;i<size;i++){
        UARTCharPut(ui32Base_,*(data+i));
    }
}

