#ifndef QUADROTOR_CURRENT_WORK_MYI2C_H_
#define QUADROTOR_CURRENT_WORK_MYI2C_H_

#define I2C_USE		I2C1_BASE
/* ------------------- viet mot byte vao dia chi trên slave------------------------
 tham so
 + I2C0_SlaveAddress : dia chi cua I2C SLAVE
 + I2C0_StartAddress : dia chi o nho truy cap de viet byte vào
 + i2c0data : du lieu muon gi vao

 ket qua
 + 0 neu khong loi
 + 1 neu co loi
 --------------------------------------------------------------------------------------------*/
uint8_t I2C_WriteByte(uint32_t I2C_ADDRESS_BASE,uint8_t I2C_SlaveAddress,uint8_t I2C_StartAddress, uint8_t i2c0data);

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
uint8_t I2C_Write(uint32_t I2C_ADDRESS_BASE,uint8_t I2C_SlaveAddress,uint8_t I2C_StartAddress, uint8_t *i2c0data,uint8_t size);

/*----------------------- doc nhieu byte tren slave i2c---------------------
 * doi so
 * 	+ I2C0_SlaveAddress : dia chi slave device
 * 	+ I2C0_StartAddress :di chi o nho trên slave de truy cap doc
 * 	+ i2c0data: bien con tro, tro toi bien chua du lieu
 * 	+ size: kich thuoc du lieu muon doc, bat dau tu dia chi quy dinh trong trong I2C0_StartAddress
 *
 *  ket qua
 *  + tra ve 0 neu khong co loi
 *  + tra ve 1 neu co loi
 -----------------------------------------------------------------------------------------------------*/
 uint8_t I2C_Read(uint32_t I2C_ADDRESS_BASE,uint8_t I2C_SlaveAddress, uint8_t I2C_StartAddress, uint8_t *i2c0data, uint8_t size);

/*-------I2C0 module---------------------
PIN CONFIG
	SCL-PB2
	SDA -PB3
---------------------------------------*/
void I2C0_Init();
void I2C2_Init();
void I2C1_Init();

#endif //QUADROTOR_CURRENT_WORK_MYI2C_H_
