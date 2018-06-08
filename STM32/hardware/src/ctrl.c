/**
	************************************************************
	*
	*	说明： 		
	*
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

#include "ctrl.h"
#include "led.h"
#include "delay.h"

#include "DHT12.h"
#include "pm2.5.h"
#include "adc_sensor.h"

CTRL_STATUS ctrlStatus;

int i;

void Ctrl_On()
{
	if(dht12Info.tempreture >= 32)
	{
		for(i=0;i<3;i++)//报警3次
		{
			Led5_Set(LED_ON);
			DelayMs(500);
			Led5_Set(LED_OFF);
			DelayMs(500);
		}
					
	}
	if(dht12Info.humidity <= 60)
	{
		Led3_Set(LED_ON);//开加湿器
	}
	if(PM2_5_Cncentration >= 50)
	{
		Led4_Set(LED_ON);//开空气净化器
	}
	if(Gas >= 50)
	{
		Led4_Set(LED_ON);//开空气净化器
		Led4_Set(LED_ON);
	}
	if(Light >= 100)
	{
		motor_stop();
		Led12_Set(LED_ON);//关窗帘
		DelayMs(2000);
		motor_stop();	
		
	}
	if(Light <= 20)
	{
		Led2_Set(LED_ON);//开灯
	}
}

void Ctrl_Off()
{
	Hardware_Init();
}

void motor_stop()
{
	Led11_Set(LED_OFF);
	DelayMs(20);
	Led12_Set(LED_OFF);
	DelayMs(20);
}
