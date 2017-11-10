/*
 * myFIFO.h
 *
 *  Created on: Oct 21, 2015
 *      Author: NhatTan
 */

#ifndef MYFIFO_H_
#define MYFIFO_H_

#define FIFO_BASE
#define FIFO_TERMINATOR_ '\0'

typedef struct{
	tRingBufObject Tx;
	tRingBufObject Rx;
	uint8_t TxBuffer[300];
	uint8_t RxBuffer[300];
} FIFO;

extern FIFO serialPcFIFO;
extern void FIFO_Rx_DataGet(FIFO *fifo,unsigned char *data);

extern volatile unsigned char flag_change_mode;
extern char pcBuffer[128],pcData[20];


extern void myFIFO_init(FIFO *fifo);

extern void FIFO_Rx_clear(FIFO *fifo);
extern void FIFO_Tx_clear(FIFO *fifo);

extern void FIFO_Rx_CharPut(FIFO *fifo,char ucData);
extern void FIFO_Rx_StrGet(FIFO *fifo,char *fifo_str);

extern void update_hardwareFIFO(FIFO *fifo,uint32_t ui32Base);
extern void FIFOCharPut(FIFO *fifo,uint32_t ui32Base,char ucData,bool update);
extern void FIFO_PutStr(FIFO *fifo,uint32_t ui32Base,char *uart_str,bool update,bool terminator);

extern void FIFO_3_float_display(float num1, float num2, float num3 );
extern void FIFO_float_display( float num);
extern void FIFO_3_int_display(int16_t num1, int16_t num2, int16_t num3 );
extern void FIFO_n_int_display(uint32_t *num,uint8_t size);

extern void printf(char *uart_str);
extern void writeByte(char ucData);

#endif /* MYFIFO_H_ */
