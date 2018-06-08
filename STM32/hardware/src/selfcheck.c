/**
	************************************************************
	*
	*	说明： 		外接设备检测
	*
	************************************************************
**/

//硬件驱动
#include "selfcheck.h"
#include "i2c.h"
#include "usart.h"
#include "delay.h"




CHECK_INFO checkInfo = {DEV_ERR, DEV_ERR, DEV_ERR};



/*
************************************************************
*	函数名称：	Check_PowerOn
*
*	函数功能：	外接设备检测
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		IIC设备可以读取寄存器来查看响应情况
*				主要检查sht20、adxl345、gy30、eeprom
************************************************************
*/
void Check_PowerOn(void)
{

	unsigned char value = 0;
	
//	//检测SH20
//	I2C_ReadByte(0X40, 0XE7, &value);					//读取用户寄存器
//	if(value)
//	{
//		UsartPrintf(USART_DEBUG, " SHT20 :Ok\r\n");
		checkInfo.SHT20_OK = DEV_OK;
//	}
//	else
//		UsartPrintf(USART_DEBUG, " SHT20 :Error\r\n");
//	DelayXms(1);
	
	//检测EEPROM
	if(!I2C_ReadByte(0x50, 255, &value))
	{
		UsartPrintf(USART_DEBUG, "EEPROM :Ok\r\n");
		checkInfo.EEPROM_OK = DEV_OK;
	}
	else
		UsartPrintf(USART_DEBUG, "EEPROM :Error\r\n");
	DelayXms(1);

}
