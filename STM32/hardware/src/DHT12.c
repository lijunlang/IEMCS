/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	DHT12.c
	*
	*	作者： 		李俊朗
	*
	*	日期： 		2018-03-22
	*
	*	版本： 		V1.0
	*
	*	说明： 		完成DHT12初始化、温湿度采集
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

#include "DHT12.h"
#include "myiic.h"
//float Temprature,Humidity;//定义温湿度变量 ，此变量为全局变量
DHT12_INFO dht12Info;;

/********************************************\
|* 功能： DHT12_IIC初始化函数	        *|
\********************************************/
void sensor_iic_init(void)
{
	IIC_Init();
}


/********************************************\
|* 功能：DHT12读取温湿度函数       *|
\********************************************/
//变量：Humi_H：湿度高位；Humi_L：湿度低位；Temp_H：温度高位；Temp_L：温度低位；Temp_CAL：校验位
//数据格式为：湿度高位（湿度整数）+湿度低位（湿度小数）+温度高位（温度整数）+温度低位（温度小数）+ 校验位
//校验：校验位=湿度高位+湿度低位+温度高位+温度低位
u8 sensor_read(float* Temp,float* Humi)
{	
	u16 i=0;
	
	u8 Humi_H=0,Humi_L=0,Temp_H=0,Temp_L=0,Temp_CAL=0,temp=0;

	IIC_start();    //主机发送起始信号
	IIC_Send_Byte(0Xb8);    //发送IIC地址
	if(!IIC_Wait_ack())  //等待从机应答信号（如无应答：考虑IIC通讯频率是否太快，或者传感器接线错误）
	{
		i=0;
		IIC_Send_Byte(0);
		while(IIC_Wait_ack())//等待从机应答信号
		{
		 	if(++i>=500)
			{
			 	break;
			}		 
		} 
		i=0;
		IIC_start();       //主机发送起始信号
		IIC_Send_Byte(0Xb9);     //发送读指令
		while(IIC_Wait_ack())    //等待从机应答信号
		{
		 	if(++i>=500)
			{
			 	break;
			}			 
		}
	
		Humi_H=IIC_Read_Byte(1);   //读取湿度高位
		Humi_L=IIC_Read_Byte(1);   //读取湿度低位
		Temp_H=IIC_Read_Byte(1);   //读取温度高位
		Temp_L=IIC_Read_Byte(1);   //读取温度低位
		Temp_CAL=IIC_Read_Byte(0); //读取校验位
		IIC_stop();	   //发送停止信号	
		temp = (u8)(Humi_H+Humi_L+Temp_H+Temp_L);//只取低8位
//		printf("Humi_H:%d Humi_L:%d Temp_H:%d Temp_L:%d Temp_CAL:%d\r\n",Humi_H,Humi_L,Temp_H,Temp_L,Temp_CAL);
		if(Temp_CAL==temp)//如果校验成功，往下运行
		{
			*Humi=Humi_H*10+Humi_L; //湿度
			if(Temp_L&0X80)	//为负温度
			{
				*Temp =0-(Temp_H*10+((Temp_L&0x7F)));
			}
			else   //为正温度
			{
				*Temp=Temp_H*10+Temp_L;//为正温度
			} 	
			//判断温湿度是否超过测量范围（温度：-20℃~60摄氏度；湿度：20%RH~95%RH）		
			if(*Humi>950) 
			{
			  *Humi=950;
			}
			if(*Humi<200)
			{
				*Humi =200;
			}
			if(*Temp>600)
			{
			  *Temp=600;
			}
			if(*Temp<-200)
			{
				*Temp = -200;
			}
			*Temp=*Temp/10.0;//计算为温度值
			*Humi=*Humi/10.0; //计算为湿度值
			
//			printf("\r\n温度为:  %.1f  ℃\r\n",*Temp); //显示温度
//			printf("湿度为:  %2.1f  %%RH\r\n",*Humi);//显示湿度	
			return 1;
		}
		else //校验失败
		{
			UsartPrintf(USART_DEBUG, "CRC Error !!\r\n");
			return 0;
		}
	}else
	{
	  UsartPrintf(USART_DEBUG, "Sensor Error !!\r\n");
		return 0;
	}	

}

//多次采集温湿度取平均值，参数times为次数
//void Continuous_Collect(float* Temp,float* Humi,u8 times)
//{
//	float Temp_buf=0,Humi_buf=0,Temp_Buf=0,Humi_Buf=0;
//	u8 i=times;
//	for(;i;)
//	{
//		delay_ms(40);  //延时40ms，DHT12采集太频繁数据会错误
//		if(sensor_read(&Temp_buf,&Humi_buf))
//		{
//			Temp_Buf+=Temp_buf;
//			Humi_Buf+=Humi_buf;
//			i--;
//		}
//	}
//	*Temp=Temp_Buf/times;
//	*Humi=Humi_Buf/times;
//}

