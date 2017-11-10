/*
 * myFIFO.c
 *
 *  Created on: Oct 21, 2015
 *      Author: NhatTan
 */
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/uart.h"
#include "driverlib/gpio.h"

#include "inc/hw_memmap.h"
#include "utils/ringbuf.h"
//#include "serial.h"
#include "myFIFO.h"

#include "myIO.h"


//uint8_t TxPcBuffer[128],RxPcBuffer[128];
//tRingBufObject TxRingBuf;
//tRingBufObject RxRingBuf;
volatile unsigned char flag_change_mode =0;

void FIFO_Rx_clear(FIFO *fifo){
	RingBufFlush(&(fifo->Rx));
}
void FIFO_Tx_clear(FIFO *fifo){
	RingBufFlush(&(fifo->Tx));
}

void FIFO_Rx_DataGet(FIFO *fifo,unsigned char *data)
{
	RingBufRead(&(fifo->Rx),data,RingBufUsed(&(fifo->Rx)));
}

void myFIFO_init(FIFO *fifo)
{
	//
	// Initialize the ring buffer.
	//
	RingBufInit(&(fifo->Tx), fifo->TxBuffer, sizeof(fifo->TxBuffer));
	RingBufInit(&(fifo->Rx), fifo->RxBuffer, sizeof(fifo->RxBuffer));
}


void FIFO_Rx_CharPut(FIFO *fifo,char ucData)
{
	//	if(UARTCharsAvail(UART_BASE))
	RingBufWriteOne(&(fifo->Rx),ucData);
}

void FIFO_Rx_StrGet(FIFO *fifo,char *fifo_str)
{
	//	if(UARTCharsAvail(UART_BASE))
	*fifo_str=(char)RingBufReadOne(&(fifo->Rx));
	while (*fifo_str !=  FIFO_TERMINATOR_)
	{
		fifo_str++;
		*fifo_str= (char)RingBufReadOne(&(fifo->Rx));
	}
	*(fifo_str+1)='\0';
}

// hardwareFIFO belong to UART modules
void update_hardwareFIFO(FIFO *fifo,uint32_t ui32Base)
{
	if(!UARTBusy(ui32Base))//hardwareFIFO full 16 bytes
	{
		uint8_t SoftFIFO_availData;
		uint8_t buffer[20];
		uint8_t i;

		SoftFIFO_availData=RingBufUsed(&(fifo->Tx));//check how many data available in FIFO
		// hardware FIFO of UART must be confit as 16 bytes
		//
		if(SoftFIFO_availData>16)
		{
			SoftFIFO_availData=16;
		}

		RingBufRead(&(fifo->Tx), buffer, SoftFIFO_availData);

		for( i=0;i<SoftFIFO_availData;i++)
		{
			UARTCharPut(ui32Base,*(buffer+i) );
			if(*(buffer+i)=='\n')
			{
				flag_change_mode=1;
//				led(LED_GREEN,0);
			}
		}
	}
}


void FIFOCharPut(FIFO *fifo,uint32_t ui32Base,char ucData,bool update)
{
	RingBufWriteOne(&(fifo->Tx),ucData);
	if(update)
		update_hardwareFIFO(fifo,ui32Base);
}
//
void FIFO_PutStr(FIFO *fifo,uint32_t ui32Base,char *uart_str,bool update,bool terminator)
{
	while(*uart_str != '\0') {RingBufWriteOne(&(fifo->Tx),*uart_str++);}
	if(terminator)
		RingBufWriteOne(&(fifo->Tx),FIFO_TERMINATOR_);
	if(update)
		update_hardwareFIFO(fifo,ui32Base);
}



//void FIFO_3_float_display(float num1, float num2, float num3 )
//{
//	char strNum[10];
//	float2str(num1,strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,false,false);
//	FIFOCharPut(&communicationFIFO,UART_BASE,'a',false);
//	float2str(num2,strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,false,false);
//	FIFOCharPut(&communicationFIFO,UART_BASE,'a',false);
//	float2str(num3,strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,true,true);
//}
//
//void FIFO_float_display( float num)
//{
//	char strNum[10];
//	float2str(num,strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,true,true);
//}
//
//void FIFO_int_display( float num)
//{
//	char strNum[10];
//	int2str(num,strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,true,true);
//}
//
//void FIFO_3_int_display(int16_t num1, int16_t num2, int16_t num3 )
//{
//	char strNum[10];
//	int2str(num1,strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,false,false);
//	FIFOCharPut(&communicationFIFO,UART_BASE,'a',false);
//	int2str(num2,strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,false,false);
//	FIFOCharPut(&communicationFIFO,UART_BASE,'a',false);
//	int2str(num3,strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,true,true);
//}
//
//void FIFO_n_int_display(uint32_t *num,uint8_t size)
//{
//	char strNum[10];
//	uint8_t i;
//	FIFOCharPut(&communicationFIFO,UART_BASE,'a',false);
//	for (i=1;i<size;i++)
//	{
//		int2str(*(num++),strNum);
//		FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,false,false);
//		FIFOCharPut(&communicationFIFO,UART_BASE,'a',false);
//	}
//	int2str(*(num),strNum);
//	FIFO_PutStr(&communicationFIFO,UART_BASE,strNum,true,true);
//}
//
//
//void printf(char *uart_str)
//{
//	FIFO_PutStr(&communicationFIFO,UART_BASE,uart_str,true,true);
//}
//void writeByte(char ucData)
//{
//	FIFOCharPut(&communicationFIFO,UART_BASE,ucData,false);
//}
