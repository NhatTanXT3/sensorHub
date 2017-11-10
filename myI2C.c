#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#include "myI2C.h"
#include "mySerial.h"
#include "myIO.h"
//extern uint8_t toggle_led[];

/* ------------------- viet mot byte vao dia chi trên slave------------------------
 tham so
 + I2C_SlaveAddress : dia chi cua I2C SLAVE
 + I2C_StartAddress : dia chi o nho truy cap de viet byte vào
 + i2c0data : du lieu muon gi vao

 ket qua
 + 0 neu khong loi
 + 1 neu co loi
 --------------------------------------------------------------------------------------------*/
uint8_t I2C_WriteByte(uint32_t I2C_ADDRESS_BASE,uint8_t I2C_SlaveAddress,uint8_t I2C_StartAddress, uint8_t i2c0data)
{
	// true=read; false =write (from master to slave)
	I2CMasterSlaveAddrSet(I2C_ADDRESS_BASE,I2C_SlaveAddress,false); // config 7bit slave_address + 1 bit r/w

	I2CMasterDataPut(I2C_ADDRESS_BASE,I2C_StartAddress);// prepare data to transmit
	I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_BURST_SEND_START);// transmit data: |7bit slave_address +1bit r/w|-|data to write|
	while(I2CMasterBusy(I2C_ADDRESS_BASE));
	if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE) return 1;


	I2CMasterDataPut(I2C_ADDRESS_BASE,(i2c0data));
	I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);
	// transmit data: |7bit slave_address +1bit r/w|-|data to write|
	while(I2CMasterBusy(I2C_ADDRESS_BASE));
	if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE) return (1);

	return 0; //no error

}


/* ------------------- viet nhieu byte vao dia chi trên slave-----------------------------
 tham so
 + I2C_SlaveAddress : dia chi cua I2C SLAVE
 + I2C_StartAddress : dia chi o nho truy cap de viet byte vào
 + i2c0data : bien co tro, tro vao du lieu gui
 + size: so byte can gui
 ket qua
 + 0 neu khong loi
 + 1 neu co loi
 --------------------------------------------------------------------------------------------*/
uint8_t I2C_Write(uint32_t I2C_ADDRESS_BASE,uint8_t I2C_SlaveAddress,uint8_t I2C_StartAddress, uint8_t *i2c0data,uint8_t size)
{
	// true=read; false =write (from master to slave)
	I2CMasterSlaveAddrSet(I2C_ADDRESS_BASE,I2C_SlaveAddress,false); // config 7bit slave_address + 1 bit r/w
	//	enable_highspeed();

	I2CMasterDataPut(I2C_ADDRESS_BASE,I2C_StartAddress);// prepare data to transmit
	I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_BURST_SEND_START);// transmit data: |7bit slave_address +1bit r/w|-|data to write|

	while(I2CMasterBusy(I2C_ADDRESS_BASE));
	if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE) return 1;

	uint8_t i;
	for (i=1;i<size;i++){
		I2CMasterDataPut(I2C_ADDRESS_BASE,*(i2c0data++));
		I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_BURST_SEND_CONT);// transmit data: |7bit slave_address +1bit r/w|-|data to write|

		while(I2CMasterBusy(I2C_ADDRESS_BASE));
		if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE) return (1);
	}

	I2CMasterDataPut(I2C_ADDRESS_BASE,*(i2c0data));
	I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_BURST_SEND_FINISH);// transmit data: |7bit slave_address +1bit r/w|-|data to write|

	while(I2CMasterBusy(I2C_ADDRESS_BASE));
	if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE) return (1);

	return 0; //no error
}


/*----------------------- doc nhieu byte tren slave i2c---------------------
 * doi so
 * 	+ I2C_SlaveAddress : dia chi slave device
 * 	+ I2C_StartAddress :di chi o nho trên slave de truy cap doc
 * 	+ i2c0data: bien con tro, tro toi bien chua du lieu
 * 	+ size: kich thuoc du lieu muon doc, bat dau tu dia chi quy dinh trong trong I2C_StartAddress
 *
 *  ket qua
 *  + tra ve 0 neu khong co loi
 *  + tra ve 1 neu co loi
 -----------------------------------------------------------------------------------------------------*/

// this function only use for I2C1 module
uint8_t I2C_Read(uint32_t I2C_ADDRESS_BASE,uint8_t I2C_SlaveAddress, uint8_t I2C_StartAddress, uint8_t *i2c0data, uint8_t size)
{
	/* ---- send: slave_address + memory_address--------*/
	I2CMasterSlaveAddrSet(I2C_ADDRESS_BASE,I2C_SlaveAddress,false); // config 7bit slave_address + 1 bit r/w. true=read; false =write (from master to slave)
	I2CMasterDataPut(I2C_ADDRESS_BASE,I2C_StartAddress);// prepare data to transmit
	I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_SINGLE_SEND);// transmit data: |7bit slave_address +1bit r/w|-|data to write|
	while(I2CMasterBusy(I2C_ADDRESS_BASE));
	if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE)
	{
		I2CMasterDisable(I2C_ADDRESS_BASE);
		SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
		I2CMasterInitExpClk(I2C_ADDRESS_BASE, SysCtlClockGet(), true); //!I2C0, Systemclock, not Fastmode
//		if(!(UARTBusy(UART_BASE))){
//				UartPutStr(UART_BASE,"er_i2c");
//		}
//		toggle_led[2]^=1;
//		led(LED_RED,toggle_led[2]);
		return 1;
	}


	/*-------------neu chi doc mot byte---------------*/
	if (size==1)
	{

		I2CMasterSlaveAddrSet(I2C_ADDRESS_BASE,I2C_SlaveAddress,true);
		I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);
		while(I2CMasterBusy(I2C_ADDRESS_BASE));
		if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE)
		{
			I2CMasterControl(I2C_ADDRESS_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
			while(I2CMasterBusy(I2C_ADDRESS_BASE));
			return 1;
		}

		*i2c0data=(unsigned char)I2CMasterDataGet(I2C_ADDRESS_BASE);
		return 0; //no error
	}

	/*---------------doc 2 bytes tro len-----------------*/
	else if(size>1)
	{

		// true=read; false =write (from master to slave)
		I2CMasterSlaveAddrSet(I2C_ADDRESS_BASE,I2C_SlaveAddress,true); // config 7bit slave_address + 1 bit r/w

		I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_BURST_RECEIVE_START);
		while(I2CMasterBusy(I2C_ADDRESS_BASE));
		if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE)
		{
			I2CMasterControl(I2C_ADDRESS_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
			while(I2CMasterBusy(I2C_ADDRESS_BASE));
			return 1;
		}


		*(i2c0data++) = (unsigned char)I2CMasterDataGet(I2C_ADDRESS_BASE);

		int i;
		for(i=2;i<size;i++)
		{
			I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_BURST_RECEIVE_CONT);
			while(I2CMasterBusy(I2C_ADDRESS_BASE));
			if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE)
			{
				I2CMasterControl(I2C_ADDRESS_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
				while(I2CMasterBusy(I2C_ADDRESS_BASE));
				return 1;
			}

			*(i2c0data++) = (unsigned char)I2CMasterDataGet(I2C_ADDRESS_BASE);
		}

		I2CMasterControl(I2C_ADDRESS_BASE,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		while(I2CMasterBusy(I2C_ADDRESS_BASE));
		if (I2CMasterErr(I2C_ADDRESS_BASE)!=I2C_MASTER_ERR_NONE)
		{
			I2CMasterControl(I2C_ADDRESS_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
			while(I2CMasterBusy(I2C_ADDRESS_BASE));
			return 1;
		}

		*(i2c0data++) = (unsigned char)I2CMasterDataGet(I2C_ADDRESS_BASE);
		return 0;
	}
	else
		return 1;
}

/*-------I2C0 module---------------------
PIN CONFIG
	SCL-PB2
	SDA -PB3

---------------------------------------*/
void I2C0_Init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);
	GPIOPinTypeI2C(GPIO_PORTB_BASE,GPIO_PIN_3);
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE,GPIO_PIN_2);

	// master clock: true 400kbps --- false 100kbps---------
	I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
}

/*-------I2C2 module---------------------
PIN CONFIG
	SCL-PE4
	SDA -PE5

---------------------------------------*/
void I2C2_Init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	//
	// Enable pin PE5 for I2C2 I2C2SDA
	//
	GPIOPinConfigure(GPIO_PE5_I2C2SDA);
	GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);
	//
	// Enable pin PE4 for I2C2 I2C2SCL
	//
	GPIOPinConfigure(GPIO_PE4_I2C2SCL);
	GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);

	// master clock: true 400kbps --- false 100kbps---------
	I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);
}


/*-------I2C1 module---------------------
PIN CONFIG
	SCL-PA6
	SDA -PA7
---------------------------------------*/
void I2C1_Init()
{
	//
	// Enable Peripheral Clocks
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	//
	// Enable pin PA7 for I2C1 I2C1SDA
	//
	GPIOPinConfigure(GPIO_PA7_I2C1SDA);
	GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

	//
	// Enable pin PA6 for I2C1 I2C1SCL
	//
	GPIOPinConfigure(GPIO_PA6_I2C1SCL);
	GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);

	// master clock: true 400kbps --- false 100kbps---------
	I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(),true);
}
