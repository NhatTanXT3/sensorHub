/*
 * ZMP.h
 *
 *  Created on: Jun 22, 2017
 *      Author: Nhat Tan
 */

#ifndef ZMP_H_
#define ZMP_H_
#include "mySerial.h"

#define UART_ZMP_LEFT_ 	UART6_BASE
#define UART_ZMP_RIGHT_ 	UART1_BASE
//============== id define=========


#define MCU2ZMP_TERMINATOR_ 	'\n'
#define MCU2ZMP_HEADER_		0xFF
#define SEND_RAW_DATA_           'a'
#define SEND_FILTERED_DATA_      'b'
#define SEND_PROCESSED_DATA_     'c'
#define SENDING_KEY_LEFT_ 0xCC
#define SENDING_KEY_RIGHT_ 0xAA

#define ZMP_ID_RAW_P0 0
#define ZMP_ID_RAW_P1 1
#define ZMP_ID_RAW_P2 2
#define ZMP_ID_RAW_P3 3

#define ZMP_ID_RAW_P4   4
#define ZMP_ID_RAW_P5   5
#define ZMP_ID_RAW_P6   6
#define ZMP_ID_RAW_P7   7

#define ZMP_ID_FILTERED_P0  8
#define ZMP_ID_FILTERED_P1  9
#define ZMP_ID_FILTERED_P2  10
#define ZMP_ID_FILTERED_P3  11

#define ZMP_ID_FILTERED_P4  12
#define ZMP_ID_FILTERED_P5  13
#define ZMP_ID_FILTERED_P6  14
#define ZMP_ID_FILTERED_P7  15

typedef struct {
	unsigned int rawForceSensor[4];
	unsigned int filterForceSensor[4];
	unsigned int posX;
	unsigned int posY;
	unsigned int amp;
} MyZMP;

extern MySerial serialZMPLeft;
extern MySerial serialZMPRight;
extern MyZMP zmpLeft;
extern MyZMP zmpRight;

extern void getRawZMPData(uint32_t ui32Base);
extern void getFilteredZMPData(uint32_t ui32Base);

extern void ZMP_send_bytes(uint32_t ui32Base_, char *data, unsigned char size);
extern void UART7_Init();
extern void UART1_Init();
extern void UART6_Init();


#endif /* ZMP_H_ */
