/**
	************************************************************
	*
	*说明：模拟IIC配置程序
	*注意：DHT12的IIC的通讯频率不得超过50KHZ
	*
	************************************************************
**/
#include "myiic.h"
#include "delay.h"

//产生IIC起始信号
void IIC_start(void)
{
	SDA_OUT();     //sda线输出
	SDA_H;	  	  
	SCL_H;
	DelayUs(14);
 	SDA_L;//开始：当SCL为高时，数据从高到低变化
	DelayUs(14);
	SCL_L;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_stop(void)
{
	SDA_OUT();//sda线输出
	SCL_L;
	SDA_L;//停止:当SCL为高时，数据从低到高变化
 	DelayUs(14);
	SCL_H;
	DelayUs(14);	 
	SDA_H;//发送I2C总线结束信号
							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_ack(void)
{
	u8 ucErrTime=0;
	SCL_L;
	SDA_H;
	SDA_IN();      //SDA设置为输入  
	DelayUs(14);	   
	SCL_H;
	DelayUs(14);	 
	while(SDA_R)//等到SDA变低
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_stop();
			return 1;
		}
		DelayUs(1);
	}
	SCL_L;//时钟输出0
	DelayUs(10); 
	return 0;  
} 
//产生ACK应答
void IIC_ack(void)
{
	SCL_L;
	SDA_OUT();
	SDA_L;
	DelayUs(15);
	SCL_H;
	DelayUs(15);
	SCL_L;
}
//产生非ACK应答		    
void IIC_Nack(void)
{
	SCL_L;
	SDA_OUT();
	SDA_H;
	DelayUs(15);
	SCL_H;
	DelayUs(15);
	SCL_L;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    SCL_L;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		SCL_L;
		if((txd&0x80)>>7)
			SDA_H;
		else
			SDA_L;
		txd<<=1; 	  
		DelayUs(15);   //延时
		SCL_H;
		DelayUs(15); 	
    }
	SCL_L;	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        SCL_L; 
        DelayUs(50);
		SCL_H;
		DelayUs(50);
        receive<<=1;
        if(SDA_R)receive++;   
    }
        SCL_L;						 
    if (!ack)
        IIC_Nack();//发送nACK
    else
        IIC_ack(); //发送ACK   
    return receive;
}



























