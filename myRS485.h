/*
 * myRS485.h
 *
 *  Created on: Mar 17, 2017
 *      Author: Nhat Tan
 */

#ifndef MYRS485_H_
#define MYRS485_H_

#define UART_RS485_3_ UART3_BASE
#define UART_RS485_4_ UART4_BASE
#define UART_RS485_2_ UART2_BASE
//#define UART_RS485_1_ UART1_BASE
//#define UART_RS485_6_ UART6_BASE
//#define UART_RS485_7_ UART7_BASE

extern void RS485_4_Init();
extern void RS485_2_Init();
extern void RS485_3_Init();

//extern void RS485_1_Init();
//extern void RS485_6_Init();
//extern void RS485_7_Init();

#endif /* MYRS485_H_ */
