/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	pm2.5.c
	*
	*	作者： 		李俊朗
	*
	*	日期： 		2018-03-15
	*
	*	版本： 		V1.0
	*
	*	说明： 		完成UART3初始化、PM2.5浓度采集
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "pm2.5.h"
#include "usart.h"

//初始化IO 串口3（bound:波特率）	  
void PM2_5_USART_init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能

 	USART_DeInit(USART3);  //复位串口3
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx;	//收模式
  
	USART_Init(USART3, &USART_InitStructure); //初始化串口	3
  

	USART_Cmd(USART3, ENABLE);                    //使能串口 
	
	//使能接收中断
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

//GP2Y1050转换次数
u8 GP2Y1050_Times;
//PM2.5浓度缓存
float GP2Y1050_Quality;
//PM2.5实时浓度
float PM2_5_Cncentration;
////GP2Y1050模块转换系数
//u16 GP2Y1050_Coefficient;
//GP2Y1050接收缓存
u8 GP2Y1050_buf[5];
//GP2Y1050接收状态
//bit7 接收到数据头0XAA，说明可以开始接收数据
//bit6~0 接收数据数目
u8 GP2Y1050_STA=0;

//串口3中断服务程序
void USART3_IRQHandler(void)                	
{
	u8 Res;
	if((USART_GetITStatus(USART3, USART_IT_RXNE)!=RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res=USART3->DR;
		//GP2Y1050接收程序
		if(GP2Y1050_STA&(1<<7) && GP2Y1050_Times<GP2Y1050_COLLECT_TIMES)
		{
			if((GP2Y1050_STA&0x7f)<5)
			{
				GP2Y1050_buf[GP2Y1050_STA&0x7f]=Res;
				GP2Y1050_STA++;
			}
			else if(Res==0xff)
			{
				if(GP2Y1050_buf[4]==GP2Y1050_buf[0]+GP2Y1050_buf[1]+GP2Y1050_buf[2]+GP2Y1050_buf[3])  //数据校验
				{
					GP2Y1050_Times++;
					GP2Y1050_Quality += (GP2Y1050_buf[0]<<8) + GP2Y1050_buf[1];
//					printf("adc:%d\r\n",(GP2Y1050_buf[0]<<8) + GP2Y1050_buf[1]);
					if(GP2Y1050_Times==0) GP2Y1050_Quality=0;
					if(GP2Y1050_Times==GP2Y1050_COLLECT_TIMES) //达到采集次数后关闭中断
					{
//						USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);		//关闭接收中断
						GP2Y1050_Quality = GP2Y1050_Quality/GP2Y1050_COLLECT_TIMES*5/1023;  //转换成电压值

						
						PM2_5_Cncentration = GP2Y1050_Quality*172;
						GP2Y1050_Times=0;
//						UsartPrintf(USART_DEBUG, "PM2.5:  %f\r\n", PM2_5_Cncentration);
					}
				}
				GP2Y1050_STA=0;
			}
			else
			{
				GP2Y1050_STA=0;
			}
		}
		else if(Res==0xaa && GP2Y1050_Times<GP2Y1050_COLLECT_TIMES)
		{
			GP2Y1050_STA|=1<<7;
		}	 
	}	
} 
