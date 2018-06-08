#ifndef __MYIIC_H
#define __MYIIC_H
#include "DHT12.h"
//

//IO方向设置
#define SDA_IN()  {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=0x00000080;}//上下拉输入
#define SDA_OUT() {GPIOB->CRL&=0XFFFFFF0F;GPIOB->CRL|=0x00000030;}//通用推挽输出


//IO操作函数	 
//#define IIC_SCL    PBout(0) //SCL输出
//#define IIC_SDA    PBout(1) //SDA输出	 
//#define READ_SDA   PBin(1)  //SDA输入 

#define SDA_H	GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define SDA_L	GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define SDA_R	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)

#define SCL_H	GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define SCL_L	GPIO_ResetBits(GPIOB, GPIO_Pin_0)

extern u8 time_flag;

//IIC所有操作函数
void IIC_init(void);                //初始化IIC的IO口				 
void IIC_start(void);				//发送IIC开始信号
void IIC_stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_ack(void); 				//IIC等待ACK信号
void IIC_ack(void);					//IIC发送ACK信号
void IIC_Nack(void);				//IIC不发送ACK信号
void TIM3_Int_Init(u16 arr,u16 psc);

#endif
















