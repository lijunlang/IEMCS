/**
	************************************************************
	*
	*	说明： 		LED初始化，亮灭LED
	*
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//LED头文件
#include "led.h"
#include "delay.h"



LED_STATUS ledStatus;


/*
************************************************************
*	函数名称：	Led_Init
*
*	函数功能：	LED初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		LED4-PB6	LED5-PB7	LED6-PB8	LED7-PB9
				高电平关灯		低电平开灯
************************************************************
*/
void Led_Init(void)
{
	
	GPIO_InitTypeDef gpioInitStrcut;

	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	//IO配置
	gpioInitStrcut.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 |GPIO_Pin_9;
	gpioInitStrcut.GPIO_Speed = GPIO_Speed_50MHz;
	//IO初始化
	GPIO_Init(GPIOB, &gpioInitStrcut);
	
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &gpioInitStrcut);
    
    Led2_Set(LED_OFF);
    Led3_Set(LED_OFF);
    Led4_Set(LED_OFF);
    Led5_Set(LED_OFF);
		Led11_Set(LED_ON);
    Led12_Set(LED_ON);
}

/*
************************************************************
*	函数名称：	Led2_Set
*
*	函数功能：	LED4控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led2_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOB, GPIO_Pin_6, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led2Sta = status;

}

/*
************************************************************
*	函数名称：	Led3_Set
*
*	函数功能：	LED5控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led3_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOB, GPIO_Pin_7, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led3Sta = status;

}

/*
************************************************************
*	函数名称：	Led4_Set
*
*	函数功能：	LED6控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led4_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOB, GPIO_Pin_8, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led4Sta = status;

}

/*
************************************************************
*	函数名称：	Led5_Set
*
*	函数功能：	LED7控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led5_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOB, GPIO_Pin_9, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led5Sta = status;

}


void Led11_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOA, GPIO_Pin_11, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led11Sta = status;

}

void Led12_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOA, GPIO_Pin_12, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led12Sta = status;

}

