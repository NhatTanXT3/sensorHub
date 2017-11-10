/*
 * mySerial.h
 *
 *  Created on: Mar 1, 2017
 *      Author: Nhat Tan
 */

#ifndef MYSERIAL_H_
#define MYSERIAL_H_

/*
 * Define uart for hardware
 */
#define UART_PC_	UART5_BASE
#define UART_HAND_    UART3_BASE
#define PC2MCU_TERMINATOR_ 	0xFE
#define PC2MCU_HEADER_		0xFF
#define MCU2PC_HEADER_		0xFF
#define MCU2PC_TERMINATOR_ 	0xFE
#define MCU2PC_MODE_1_ 	1

#define STR_TERMINATOR_ '\0' //string from MCU
/*
 * =========== channel for display===========
 */
#define CN_1_ '~'
#define CN_2_ '!'
#define CN_3_ '@'
#define CN_4_ '#'
#define CN_5_ '$'
#define CN_6_ '%'
#define CN_7_ '^'
#define CN_8_ '*'
#define CN_9_ '('
#define CN_10_ ')'

#define COM2CTL_DISPLAY_ON_				'e'
#define COM2CTL_DISPLAY_MODE_0_			   	'0'
#define COM2CTL_DISPLAY_MODE_1_				'1'
#define COM2CTL_DISPLAY_MODE_2_				'2'
#define COM2CTL_DISPLAY_MODE_3_				'3'
#define COM2CTL_DISPLAY_MODE_4_				'4'
#define COM2CTL_DISPLAY_MODE_5_				'5'
#define COM2CTL_DISPLAY_MODE_6_				'6'
#define COM2CTL_DISPLAY_MODE_7_				'7'
#define COM2CTL_DISPLAY_OFF_			'f'

//=====QUICK MODE DEFINE=====
#define PC_SUBSENSOR_MODE_0_ 0

//=====QUICK MODE DEFINE=====
#define PC_SAM_MODE_0_ 0
#define PC_SAM_MODE_1_ 1
#define PC_SAM_MODE_2_ 2
#define PC_SAM_MODE_3_ 3
#define PC_SAM_MODE_4_ 4
#define PC_SAM_MODE_5_ 5
#define PC_SAM_MODE_6_ 6
#define PC_SAM_MODE_7_ 7
#define PC_SAM_MODE_8_ 8
#define PC_SAM_MODE_9_ 9
#define PC_SAM_MODE_10_ 10
//#define PC_SAM_MODE_11_ 11

#define PC_SAM_MODE_1_DATALENGTH_ 6

//=====SPECIAL MODE DEFINE
#define PC_SUBSENSOR_SP_MODE_0_ 0xAA //get sensor data
#define PC_SUBSENSOR_SP_MODE_1_ 0xDD //get sensor data
#define PC_SUBSENSOR_SP_MODE_2_ 0xF0 //get sensor data
#define PC_SUBSENSOR_SP_MODE_3_ 0xAA //get sensor data
#define PC_SUBSENSOR_SP_MODE_4_  0x95 //get sensor data

#define PC_SUBSENSOR_SP_MODE_5_ 0xBB //hand function
#define HAND_MOTION_1 'm'
#define HAND_MOTION_2 'n'
#define HAND_MOTION_3 'f'
#define HAND_MOTION_4 'h'
#define HAND_MOTION_5 'l'


//#define PC_SAM_READ_1_ 2
//#define
#define SERIAL_BUFFER_SIZE_ 100
typedef volatile struct {
	unsigned char Flag_receive;
	unsigned char dataIndex;
	unsigned char dataCount;
	unsigned char Command_Data[SERIAL_BUFFER_SIZE_];
} MySerial;
extern unsigned char dataSendBuffer[];

extern MySerial serialPC;


extern void UART5_Init();



extern void SerialGetData(uint32_t ui32Base,char *uart_str);
extern void SerialSendData(uint32_t ui32Base,unsigned char *uart_str);
extern void SerialSend_1_Position12(uint32_t ui32Base,unsigned char ID,unsigned int value);
extern void SerialSend_1_Position8(uint32_t ui32Base,unsigned char ID,unsigned char value,unsigned char load);
extern void SerialSend_All_Position(uint32_t ui32Base,unsigned char ID,unsigned int *SamPos);
extern void SerialSend_PID(uint32_t ui32Base,unsigned char ID,unsigned char P, unsigned char I, unsigned char D);



extern void SerialPutData(uint32_t ui32Base,char *uart_data,unsigned char dataSize);
extern void SerialPutChar(uint32_t ui32Base,unsigned char uart_char);
extern void SerialPutStr(uint32_t ui32Base,char *uart_str);
extern void SerialPutStr_NonTer(uint32_t ui32Base,char *uart_str);
extern void SerialPutStrLn(uint32_t ui32Base,char *uart_str);
extern void SerialTerminator(uint32_t ui32Base);
#endif /* MYSERIAL_H_ */
