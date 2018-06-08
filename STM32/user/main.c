/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		李俊朗
	*
	*	日期： 		2018-03-27
	*
	*	版本： 		V1.1
	*
	*	说明： 		完成单片机初始化、外接IC初始化和任务的创建及运行
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//OS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "freertosAPI.h"

//协议
#include "onenet.h"
#include "fault.h"

//网络设备
#include "net_device.h"

//硬件驱动
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "hwtimer.h"
#include "i2c.h"
//#include "sht20.h"
#include "iwdg.h"
#include "at24c02.h"
#include "selfcheck.h"
#include "info.h"
#include "dht12.h"
#include "pm2.5.h"
#include "HC-SR501.h"
#include "adc.h"
#include "adc_sensor.h"
#include "ctrl.h"
//C库
#include <string.h>






//看门狗任务
#define IWDG_TASK_PRIO		11
#define IWDG_STK_SIZE		64
TaskHandle_t IWDG_Task_Handler;
void IWDG_Task(void *pvParameters);

//串口任务
#define USART_TASK_PRIO		10
#define USART_STK_SIZE		256
TaskHandle_t USART_Task_Handler;
void USART_Task(void *pvParameters);

//心跳任务
#define HEART_TASK_PRIO		9
#define HEART_STK_SIZE		256
TaskHandle_t HEART_Task_Handler;
void HEART_Task(void *pvParameters);

//故障处理任务
#define FAULT_TASK_PRIO		8 //
#define FAULT_STK_SIZE		256
TaskHandle_t FAULT_Task_Handler;
void FAULT_Task(void *pvParameters);

//传感器任务
#define SENSOR_TASK_PRIO	7
#define SENSOR_STK_SIZE		512
TaskHandle_t SENSOR_Task_Handler;
void SENSOR_Task(void *pvParameters);

//数据发送任务
#define SEND_TASK_PRIO		6
#define SEND_STK_SIZE		512
TaskHandle_t SEND_Task_Handler;
void SEND_Task(void *pvParameters);

//网络初始化任务
#define NET_TASK_PRIO		4 //
#define NET_STK_SIZE		512
TaskHandle_t NET_Task_Handler;
void NET_Task(void *pvParameters);

//数据反馈任务
#define DATA_TASK_PRIO		3 //
#define DATA_STK_SIZE		512
TaskHandle_t DATA_Task_Handler;
void DATA_Task(void *pvParameters);

//信息更改任务
#define ALTER_TASK_PRIO		2 //
#define ALTER_STK_SIZE		256
TaskHandle_t ALTER_Task_Handler;
void ALTER_Task(void *pvParameters);



#define NET_TIME	60			//设定时间--单位秒
unsigned short timerCount = 0;	//时间计数--单位秒

TimerHandle_t t1_Thdl;






//数据流
DATA_STREAM dataStream[] = {
								{"Ctrl", &ctrlStatus.CtrlSta, TYPE_BOOL, 1},//警报
								{"Alarm_Led", &ledStatus.Led5Sta, TYPE_BOOL, 1},//警报
								{"Filter_Led", &ledStatus.Led4Sta, TYPE_BOOL, 1},//空气净化器
								{"Humidifier_Led", &ledStatus.Led3Sta, TYPE_BOOL, 1},//加湿器
								{"Light_Led", &ledStatus.Led2Sta, TYPE_BOOL, 1},//灯
								{"Forward", &ledStatus.Led11Sta, TYPE_BOOL, 1},//正
								{"Reverse", &ledStatus.Led12Sta, TYPE_BOOL, 1},//反
								{"temperature", &dht12Info.tempreture, TYPE_FLOAT, 1},
								{"humidity", &dht12Info.humidity, TYPE_FLOAT, 1},
								{"PM2.5", &PM2_5_Cncentration, TYPE_FLOAT, 1},
								{"gas", &Gas, TYPE_INT, 1},
								{"light", &Light, TYPE_INT, 1},
								{"errType", &faultTypeReport, TYPE_UCHAR, 1},
							};
unsigned char dataStreamLen = sizeof(dataStream) / sizeof(dataStream[0]);


/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);								//中断控制器分组设置

	delay_init();
	
	Delay_Init();																//Timer3初始化初始化
	
	Led_Init();																	//LED初始化
	
	HC_SR501_GPIO_Init();												//HC_SR501初始化
	
	ADCx_Init();                                //4通道ADC初始化
	
	IIC_Init();																	//软件IIC总线初始化
	
	Usart1_Init(115200); 														//初始化串口   115200bps
	
	PM2_5_USART_init(2400);										//初始化PM2.5串口
		
	Check_PowerOn(); 															//上电自检
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) 								//如果是看门狗复位则提示
	{
		UsartPrintf(USART_DEBUG, "WARN:	IWDG Reboot\r\n");
		
		RCC_ClearFlag();														//清除看门狗复位标志位
		
		faultTypeReport = faultType = FAULT_REBOOT; 							//标记为重启错误
		
		if(!Info_Check() && checkInfo.EEPROM_OK)								//如果EEPROM里有信息
			Info_Read();
	}
	else
	{
		//先读出ssid、pswd、devid、apikey
		if(!Info_Check() && checkInfo.EEPROM_OK)								//如果EEPROM里有信息
		{
			//AT24C02_Clear(0, 255, 256);Iwdg_Feed();
			UsartPrintf(USART_DEBUG, "1.ssid_pswd in EEPROM\r\n");
			Info_Read();
		}
		else //没有数据
		{
			UsartPrintf(USART_DEBUG, "1.ssid_pswd in ROM\r\n");
		}
		
		UsartPrintf(USART_DEBUG, "2.SSID: %s,   PSWD: %s\r\n"
								"DEVID: %s,     APIKEY: %s\r\n"
								, netDeviceInfo.staName, netDeviceInfo.staPass
								, oneNetInfo.devID, oneNetInfo.apiKey);
	}
	
	Iwdg_Init(4, 1250); 														//64分频，每秒625次，重载1250次，2s
	
	UsartPrintf(USART_DEBUG, "3.Hardware init OK\r\n");							//提示初始化完成

}

/*
************************************************************
*	函数名称：	OS_TimerCallBack
*
*	函数功能：	定时检查网络状态标志位
*
*	入口参数：	软件定时器句柄
*
*	返回参数：	无
*
*	说明：		定时器任务。定时检查网络状态，若持续超过设定时间无网络连接，则进行平台重连
************************************************************
*/
void OS_TimerCallBack(TimerHandle_t xTimer)
{
	
	if(oneNetInfo.netWork == 0)											//如果网络断开
	{
		if(++timerCount >= NET_TIME) 									//如果网络断开超时
		{
			UsartPrintf(USART_DEBUG, "Tips:		Timer Check Err\r\n");
			
			checkInfo.NET_DEVICE_OK = 0;								//置设备未检测标志
			
			NET_DEVICE_ReConfig(0);										//设备初始化步骤设置为开始状态
			
			oneNetInfo.netWork = 0;
		}
	}
	else
	{
		timerCount = 0;													//清除计数
	}

}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	完成初始化任务，创建应用任务并执行
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{
	
	Hardware_Init();								//硬件初始化

	//创建应用任务
	
	xTaskCreate((TaskFunction_t)IWDG_Task, "IWDG", IWDG_STK_SIZE, NULL, IWDG_TASK_PRIO, (TaskHandle_t*)&IWDG_Task_Handler);
	
	xTaskCreate((TaskFunction_t)USART_Task, "USART", USART_STK_SIZE, NULL, USART_TASK_PRIO, (TaskHandle_t*)&USART_Task_Handler);
	
	xTaskCreate((TaskFunction_t)HEART_Task, "HEART", HEART_STK_SIZE, NULL, HEART_TASK_PRIO, (TaskHandle_t*)&HEART_Task_Handler);
	
	xTaskCreate((TaskFunction_t)FAULT_Task, "FAULT", FAULT_STK_SIZE, NULL, FAULT_TASK_PRIO, (TaskHandle_t*)&FAULT_Task_Handler);
	
	xTaskCreate((TaskFunction_t)SENSOR_Task, "SENSOR", SENSOR_STK_SIZE, NULL, SENSOR_TASK_PRIO, (TaskHandle_t*)&SENSOR_Task_Handler);
	
	xTaskCreate((TaskFunction_t)SEND_Task, "SEND", SEND_STK_SIZE, NULL, SEND_TASK_PRIO, (TaskHandle_t*)&SEND_Task_Handler);
	
	xTaskCreate((TaskFunction_t)NET_Task, "NET", NET_STK_SIZE, NULL, NET_TASK_PRIO, (TaskHandle_t*)&NET_Task_Handler);
	
	xTaskCreate((TaskFunction_t)DATA_Task, "DATA", DATA_STK_SIZE, NULL, DATA_TASK_PRIO, (TaskHandle_t*)&DATA_Task_Handler);
	
	xTaskCreate((TaskFunction_t)ALTER_Task, "ALTER", ALTER_STK_SIZE, NULL, ALTER_TASK_PRIO, (TaskHandle_t*)&ALTER_Task_Handler);
	
	t1_Thdl = xTimerCreate("Timer1", 200, pdTRUE, (void *)1, (TimerCallbackFunction_t)OS_TimerCallBack);
	xTimerStart(t1_Thdl, 1);
	
	UsartPrintf(USART_DEBUG, "4.OSStart\r\n");		//提示任务开始执行
	
	vTaskStartScheduler();							//开始任务调度
	
	return 0;

}

/*
************************************************************
*	函数名称：	IWDG_Task
*
*	函数功能：	清除看门狗
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		看门狗任务
************************************************************
*/
void IWDG_Task(void *pdata)
{

	while(1)
	{
	
		Iwdg_Feed(); 		//喂狗
		
		RTOS_TimeDly(50); 	//挂起任务250ms
	
	}

}

/*
************************************************************
*	函数名称：	HEART_Task
*
*	函数功能：	心跳检测
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		心跳任务。发送心跳请求并等待心跳响应，若在设定时间内没有响应则会进行平台重连
************************************************************
*/
void HEART_Task(void *pdata)
{

	while(1)
	{
		
		OneNet_HeartBeat();				//心跳检测
		
		RTOS_TimeDly(4000);				//挂起任务20s
	
	}

}

/*
************************************************************
*	函数名称：	SEND_Task
*
*	函数功能：	上传传感器数据
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		数据发送任务
************************************************************
*/
void SEND_Task(void *pdata)
{

	while(1)
	{
		
		OneNet_SendData(kTypeSimpleJsonWithoutTime, dataStreamLen);	//上传数据到平台
		
		RTOS_TimeDly(3000);											//挂起任务15s
		
	}

}

/*
************************************************************
*	函数名称：	USART_Task
*
*	函数功能：	处理平台下发的命令
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		串口接收任务。在数据模式下时，等待平台下发的命令并解析、处理
************************************************************
*/
void USART_Task(void *pdata)
{
	
	unsigned char *dataPtr;														//数据指针

	while(1)
	{
		if(oneNetInfo.netWork && NET_DEVICE_Get_DataMode() == DEVICE_DATA_MODE)	//当有网络 且 在命令接收模式时
		{
			dataPtr = NET_DEVICE_GetIPD(0);										//不等待，获取平台下发的数据
			if(dataPtr != NULL)													//如果数据指针不为空，则代表收到了数据
			{
				OneNet_App(dataPtr);											//集中处理
			}
		}
		
		RTOS_TimeDly(2);														//挂起任务10ms
	
	}

}

/*
************************************************************
*	函数名称：	SENSOR_Task
*
*	函数功能：	传感器数据采集、显示
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		传感器数据采集任务。进行外接传感器的数据采集、读取、显示
************************************************************
*/
void SENSOR_Task(void *pdata)
{

	while(1)
	{

		if(checkInfo.SHT20_OK == DEV_OK) 						//只有设备存在时，才会读取值和显示
		{
//			SHT20_GetValue();									//采集传感器数据
		
			sensor_read(&dht12Info.tempreture, &dht12Info.humidity);  //DHT12采集温湿度、
		}
		
		
    adcsensor_read();

		
		RTOS_TimeDly(100); 										//挂起任务500ms
	
	}

}

/*
************************************************************
*	函数名称：	DATA_Task
*
*	函数功能：	平台下发命令的数据反馈
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		数据反馈任务。这是平台下发指令后的反馈函数，透传模式的时候收到之后立即返回，非透传模式因为需要检索'>'符号，所以使用任务的方式来反馈。
************************************************************
*/
void DATA_Task(void *pdata)
{

	while(1)
	{
	
		if(oneNetInfo.sendData == 1)		//如果数据反馈标志位被设置
		{
			oneNetInfo.sendData = OneNet_SendData(kTypeSimpleJsonWithoutTime, dataStreamLen);	//上传数据到平台
		}
		
		RTOS_TimeDly(10);					//挂起任务50ms
	
	}

}

/*
************************************************************
*	函数名称：	FAULT_Task
*
*	函数功能：	网络状态错误处理
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		故障处理任务。当发生网络错误、设备错误时，会标记对应标志位，然后集中进行处理
************************************************************
*/
void FAULT_Task(void *pdata)
{

	while(1)
	{
		
		if(faultType != FAULT_NONE)									//如果错误标志被设置
		{
			UsartPrintf(USART_DEBUG, "WARN:	Fault Process\r\n");
			Fault_Process();										//进入错误处理函数
		}
	
		
		RTOS_TimeDly(10);											//挂起任务50ms
	
	}

}

/*
************************************************************
*	函数名称：	NET_Task
*
*	函数功能：	网络连接、平台接入
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		网络连接任务任务。会在心跳检测里边检测网络连接状态，如果有错，会标记状态，然后在这里进行重连
************************************************************
*/
void NET_Task(void *pdata)
{
	
	NET_DEVICE_IO_Init();													//网络设备IO初始化
	NET_DEVICE_Reset();														//网络设备复位
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);								//设置为命令收发模式(例如ESP8266要区分AT的返回还是平台下发数据的返回)

	while(1)
	{
		
		if(!oneNetInfo.netWork && (checkInfo.NET_DEVICE_OK == DEV_OK))		//当没有网络 且 网络模块检测到时
		{
			NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);						//设置为命令收发模式
			
			if(!NET_DEVICE_Init())											//初始化网络设备，能连入网络
				OneNet_DevLink(oneNetInfo.devID, oneNetInfo.apiKey);		//接入平台
		}
		
		if(checkInfo.NET_DEVICE_OK == DEV_ERR) 								//当网络设备未做检测
		{
			NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);						//设置为命令收发模式
			
			if(timerCount >= NET_TIME) 										//如果网络连接超时
			{
				NET_DEVICE_Reset();											//复位网络设备
				timerCount = 0;												//清零连接超时计数
				faultType = FAULT_NONE;										//清除错误标志
			}
			
			if(!NET_DEVICE_SendCmd("AT\r\n", "OK"))							//网络设备检测
			{
				UsartPrintf(USART_DEBUG, "NET Device :Ok\r\n");
				checkInfo.NET_DEVICE_OK = DEV_OK;							//检测到网络设备，标记
				NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);					//设置为数据收发模式
			}
			else
				UsartPrintf(USART_DEBUG, "NET Device :Error\r\n");
		}
		
		RTOS_TimeDly(5);													//挂起任务25ms
	
	}

}

/*
************************************************************
*	函数名称：	ALTER_Task
*
*	函数功能：	通过串口更改SSID、PSWD、DEVID、APIKEY
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		更改后会保存到EEPROM里
************************************************************
*/
void ALTER_Task(void *pdata)
{
    
    unsigned char usart1Count = 0;

    while(1)
    {
    
        memset(alterInfo.alterBuf, 0, sizeof(alterInfo.alterBuf));
		alterInfo.alterCount = 0;usart1Count = 0;
        while((strlen(alterInfo.alterBuf) != usart1Count) || (usart1Count == 0))	//等待接收完成
        {
            usart1Count = strlen(alterInfo.alterBuf);								//计算长度
            RTOS_TimeDly(20);														//每100ms检查一次
        }
        UsartPrintf(USART_DEBUG, "\r\nusart1Buf Len: %d, usart1Count = %d\r\n",
									strlen(alterInfo.alterBuf), usart1Count);
        
		if(checkInfo.EEPROM_OK == DEV_OK)											//如果EEPROM存在
		{
			if(Info_Alter(alterInfo.alterBuf))										//更改信息
			{
				oneNetInfo.netWork = 0;												//重连网络和平台
				NET_DEVICE_ReConfig(0);
			}
		}
    
    }

}

void PUSHDATA_Task(void *pdata)
{
	
	_Bool flag = 0;

	/*while(1)
	{
		
		if(flag)
			OneNet_PushData("4091363", "{redled}1", 9); //发到kylin-v2.2
		else
			OneNet_PushData("4091363", "{redled}0", 9); //发到kylin-v2.2
		
		flag = !flag;
	
		RTOS_TimeDly(2000);
	
	}*/

}
