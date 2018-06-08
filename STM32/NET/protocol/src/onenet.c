/**
	************************************************************
	*	文件名： 	onenet.c
	*	作者： 		张继瑞
	*	日期： 		2016-12-07
	*	版本： 		V1.1
	*	说明： 		与onenet平台的数据交互，协议层
	*	修改记录：	V1.1：简化了开发过程中数据流的填写---相关函数：OneNet_SendData、OneNet_Load_DataStream
	*					  新增了连接方式2---相关函数：OneNet_DevLink
	*					  新增了PUSHDATA功能，设备与设备之间的通信---相关函数：OneNet_PushData
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "net_device.h"

//协议文件
#include "onenet.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "sht20.h"
#include "fault.h"
#include "iwdg.h"
#include "hwtimer.h"
#include "selfcheck.h"
#include "ctrl.h"

//图片数据文件
#include "image_2k.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




EdpPacket *send_pkg;	//协议包

ONETNET_INFO oneNetInfo = {"26021494", "Dp0zRJ=c2gFMs2au4K8GCprQNbQ=", 0, 0, 0, 0};//设备号，API-KEY
extern DATA_STREAM dataStream[];


/*
************************************************************
*	函数名称：	OneNet_DevLink
*
*	函数功能：	与onenet创建连接
*
*	入口参数：	devid：创建设备的devid
*				auth_key：创建设备的masterKey或apiKey
*
*	返回参数：	无
*
*	说明：		与onenet平台建立连接，成功或会标记oneNetInfo.netWork网络状态标志
************************************************************
*/
void OneNet_DevLink(const char* devid, const char* auth_key)
{

	unsigned char *dataPtr;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , devid, auth_key);

#if 1
	send_pkg = PacketConnect1(devid, auth_key);						//根据devid 和 apikey封装协议包
#else
	send_pkg = PacketConnect2(devid, auth_key);						//根据产品id 和 鉴权信息封装协议包
#endif
	
	NET_DEVICE_SendData(send_pkg->_data, send_pkg->_write_pos);		//上传平台
	
	dataPtr = NET_DEVICE_GetIPD(250);								//等待平台响应
	if(dataPtr != NULL)
	{
		oneNetInfo.netWork = OneNet_EDPKitCmd(dataPtr);				//解析数据
	}
	
	DeleteBuffer(&send_pkg);										//删包
	
	if(oneNetInfo.netWork)											//如果接入成功
	{
		UsartPrintf(USART_DEBUG, "Tips:	NetWork OK\r\n");
		
		oneNetInfo.errCount = 0;
		
		NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);					//设置为数据收发模式
	}
	else
	{
		UsartPrintf(USART_DEBUG, "Tips:	NetWork Fail\r\n");
		
		if(++oneNetInfo.errCount >= 5)								//如果超过设定次数后，还未接入平台
		{
			oneNetInfo.netWork = 0;
			faultType = faultTypeReport = FAULT_NODEVICE;			//标记为硬件错误
		}
	}
	
}

/*
************************************************************
*	函数名称：	OneNet_PushData
*
*	函数功能：	PUSHDATA
*
*	入口参数：	dst_devid：接收设备的devid
*				data：数据内容
*				data_len：数据长度
*
*	返回参数：	无
*
*	说明：		设备与设备之间的通信
************************************************************
*/
_Bool OneNet_PushData(const char* dst_devid, const char* data, unsigned int data_len)
{
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
		return 1;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);										//设置为命令收发模式

	send_pkg = PacketPushdata(dst_devid, data, data_len);
	
	NET_DEVICE_SendData(send_pkg->_data, send_pkg->_write_pos);						//上传平台
	
	DeleteBuffer(&send_pkg);														//删包
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_Load_DataStream
*
*	函数功能：	数据流封装
*
*	入口参数：	type：发送数据的格式
*				send_buf：发送缓存指针
*				len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		封装数据流格式
************************************************************
*/
void OneNet_Load_DataStream(unsigned char type, char *send_buf, unsigned char len)
{
	
	unsigned char count = 0;
	char stream_buf[50];
	char *ptr = send_buf;

	switch(type)
	{
		case kTypeFullJson:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeFullJson\r\n");
		
			strcpy(send_buf, "{\"datastreams\":[");
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					switch((unsigned char)dataStream[count].dataType)
					{
						case TYPE_BOOL:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(_Bool *)dataStream[count].data);
						break;
						
						case TYPE_CHAR:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(char *)dataStream[count].data);
						break;
						
						case TYPE_UCHAR:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(unsigned char *)dataStream[count].data);
						break;
						
						case TYPE_SHORT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(short *)dataStream[count].data);
						break;
						
						case TYPE_USHORT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(unsigned short *)dataStream[count].data);
						break;
						
						case TYPE_INT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(int *)dataStream[count].data);
						break;
						
						case TYPE_UINT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(unsigned int *)dataStream[count].data);
						break;
						
						case TYPE_LONG:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%ld}]},", dataStream[count].name, *(long *)dataStream[count].data);
						break;
						
						case TYPE_ULONG:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%ld}]},", dataStream[count].name, *(unsigned long *)dataStream[count].data);
						break;
							
						case TYPE_FLOAT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%f}]},", dataStream[count].name, *(float *)dataStream[count].data);
						break;
						
						case TYPE_DOUBLE:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%f}]},", dataStream[count].name, *(float *)dataStream[count].data);
						break;
						
						case TYPE_GPS:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":{\"lon\":1.1,\"lat\":1.1}}]},", dataStream[count].name);
						break;
					}
					
					strcat(send_buf, stream_buf);
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strcat(send_buf, "]}");
		
			send_pkg = PacketSaveJson(NULL, send_buf, kTypeFullJson);				//封包
		
		break;
		
		case kTypeSimpleJsonWithTime:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeSimpleJsonWithTime\r\n");
		
			strcpy(send_buf, "{");
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					switch((unsigned char)dataStream[count].dataType)
					{
						case TYPE_BOOL:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(_Bool *)dataStream[count].data);
						break;
						
						case TYPE_CHAR:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(char *)dataStream[count].data);
						break;
						
						case TYPE_UCHAR:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(unsigned char *)dataStream[count].data);
						break;
						
						case TYPE_SHORT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(short *)dataStream[count].data);
						break;
						
						case TYPE_USHORT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(unsigned short *)dataStream[count].data);
						break;
						
						case TYPE_INT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(int *)dataStream[count].data);
						break;
						
						case TYPE_UINT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(unsigned int *)dataStream[count].data);
						break;
						
						case TYPE_LONG:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%ld},", dataStream[count].name, *(long *)dataStream[count].data);
						break;
						
						case TYPE_ULONG:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%ld},", dataStream[count].name, *(unsigned long *)dataStream[count].data);
						break;
						
						case TYPE_FLOAT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%f},", dataStream[count].name, *(float *)dataStream[count].data);
						break;
						
						case TYPE_DOUBLE:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%f},", dataStream[count].name, *(float *)dataStream[count].data);
						break;
					}
					
					strcat(send_buf, stream_buf);
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strcat(send_buf, "}");
			
			send_pkg = PacketSaveJson(NULL, send_buf, kTypeSimpleJsonWithTime);				//封包
		
		break;
		
		case kTypeSimpleJsonWithoutTime:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeSimpleJsonWithoutTime\r\n");
		
			strcpy(send_buf, "{");
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					switch((unsigned char)dataStream[count].dataType)
					{
						case TYPE_BOOL:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(_Bool *)dataStream[count].data);
						break;
							
						case TYPE_CHAR:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(char *)dataStream[count].data);
						break;
							
						case TYPE_UCHAR:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(unsigned char *)dataStream[count].data);
						break;
							
						case TYPE_SHORT:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(short *)dataStream[count].data);
						break;
							
						case TYPE_USHORT:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(unsigned short *)dataStream[count].data);
						break;
							
						case TYPE_INT:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(int *)dataStream[count].data);
						break;
							
						case TYPE_UINT:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(unsigned int *)dataStream[count].data);
						break;
							
						case TYPE_LONG:
							sprintf(stream_buf, "\"%s\":%ld,", dataStream[count].name, *(long *)dataStream[count].data);
						break;
							
						case TYPE_ULONG:
							sprintf(stream_buf, "\"%s\":%ld,", dataStream[count].name, *(unsigned long *)dataStream[count].data);
						break;
							
						case TYPE_FLOAT:
							sprintf(stream_buf, "\"%s\":%f,", dataStream[count].name, *(float *)dataStream[count].data);
						break;
							
						case TYPE_DOUBLE:
							sprintf(stream_buf, "\"%s\":%f,", dataStream[count].name, *(float *)dataStream[count].data);
						break;
					}
					
					strcat(send_buf, stream_buf);
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strcat(send_buf, "}");
			
			send_pkg = PacketSaveJson(NULL, send_buf, kTypeSimpleJsonWithoutTime);			//封包
		
		break;
		
		case kTypeString:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeString\r\n");
		
			strcpy(send_buf, ",;");
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					switch((unsigned char)dataStream[count].dataType)
					{
						case TYPE_BOOL:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(_Bool *)dataStream[count].data);
						break;
						
						case TYPE_CHAR:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(char *)dataStream[count].data);
						break;
						
						case TYPE_UCHAR:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(unsigned char *)dataStream[count].data);
						break;
						
						case TYPE_SHORT:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(short *)dataStream[count].data);
						break;
						
						case TYPE_USHORT:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(unsigned short *)dataStream[count].data);
						break;
						
						case TYPE_INT:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(int *)dataStream[count].data);
						break;
						
						case TYPE_UINT:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(unsigned int *)dataStream[count].data);
						break;
						
						case TYPE_LONG:
							sprintf(stream_buf, "%s,%ld;", dataStream[count].name, *(long *)dataStream[count].data);
						break;
						
						case TYPE_ULONG:
							sprintf(stream_buf, "%s,%ld;", dataStream[count].name, *(unsigned long *)dataStream[count].data);
						break;
						
						case TYPE_FLOAT:
							sprintf(stream_buf, "%s,%f;", dataStream[count].name, *(float *)dataStream[count].data);
						break;
						
						case TYPE_DOUBLE:
							sprintf(stream_buf, "%s,%f;", dataStream[count].name, *(float *)dataStream[count].data);
						break;
					}
				}
				
				strcat(send_buf, stream_buf);
			}
		
			send_pkg = PacketSavedataSimpleString(NULL, send_buf);							//封包
		
		break;
	}

}

/*
************************************************************
*	函数名称：	OneNet_SendData
*
*	函数功能：	上传数据到平台
*
*	入口参数：	type：发送数据的格式
*				len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_SendData(SaveDataType type, unsigned char len)
{
	
	char send_buf[SEND_BUF_SIZE];
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
		return 1;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);										//设置为命令收发模式
	memset(send_buf, 0, SEND_BUF_SIZE);
	
	if(type != kTypeBin)															//二进制文件吧全部工作做好，不需要执行这些
	{
		OneNet_Load_DataStream(type, send_buf, len);								//加载数据流
		NET_DEVICE_SendData(send_pkg->_data, send_pkg->_write_pos);					//上传数据到平台
		
		DeleteBuffer(&send_pkg);													//删包
		
		faultTypeReport = FAULT_NONE;												//发送之后清除标记
	}
	else
	{
		UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeBin\r\n");
		
		OneNet_SendData_EDPType2();
	}
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);
	
	return 0;
	
}

/*
************************************************************
*	函数名称：	OneNet_SendData_EDPType2
*
*	函数功能：	上传二进制数据到平台
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		若是低速设备，数据量大时，建议使用网络设备的透传模式
************************************************************
*/
#define PKT_SIZE 1024
void OneNet_SendData_EDPType2(void)
{

	char text[] = "{\"ds_id\":\"pic\"}";								//图片数据头
	unsigned int len = sizeof(Array);
	unsigned char *pImage = (unsigned char *)Array;					
	
	UsartPrintf(USART_DEBUG, "image len = %d", len);
	
	send_pkg = PacketSaveBin(NULL, text, len);							//一发完图片数据就HardFault_Handler，
										//原因是如果封包时填写了devid，平台会吧上传的图片数据在下发一次，导致串口buf溢出，从而进入HardFault_Handler
	//send_pkg = PacketSaveBin(oneNetInfo.devID, text, len);//我在串口接收里边做了防止数组访问越界的处理，这下不怕了。
	
	NET_DEVICE_SendData(send_pkg->_data, send_pkg->_write_pos);			//向平台上传数据点
	
	DeleteBuffer(&send_pkg);											//删包
	
	while(len > 0)
	{
		RTOS_TimeDly(10);												//传图时，时间间隔会大一点，这里额外增加一个延时
		
		if(len >= PKT_SIZE)
		{
			NET_DEVICE_SendData(pImage, PKT_SIZE);						//串口发送分片
			
			pImage += PKT_SIZE;
			len -= PKT_SIZE;
		}
		else
		{
			NET_DEVICE_SendData(pImage, (unsigned short)len);			//串口发送最后一个分片
			len = 0;
		}
	}

}

/*
************************************************************
*	函数名称：	OneNet_HeartBeat
*
*	函数功能：	心跳检测
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void OneNet_HeartBeat(void)
{

	unsigned char heartBeat[2] = {PINGREQ, 0}, sCount = 5;
	unsigned char errType = 0;
	unsigned char *dataPtr;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)	//如果网络为连接 或 不为数据收发模式
		return;
	
	NET_DEVICE_Set_DataMode(DEVICE_HEART_MODE);									//设置为心跳收发模式
	
	while(--sCount)																//循环检测计数
	{
		NET_DEVICE_SendData(heartBeat, sizeof(heartBeat));						//向平台上传心跳请求

		dataPtr = NET_DEVICE_GetIPD(200);										//获取数据，等到2s
		if(dataPtr != NULL)														//如果数据指针不为空
		{
			if(dataPtr[0] == PINGRESP)											//第一个数据为心跳响应
			{
				errType = CHECK_NO_ERR;											//标记无错误
				UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
				break;
			}
		}
		else
		{
			if(dataPtr[0] != PINGRESP)											//心跳未响应
			{
				UsartPrintf(USART_DEBUG, "Check Device\r\n");
					
				errType = NET_DEVICE_Check();									//网络设备状态检查
			}
		}
			
		RTOS_TimeDly(2);														//延时等待
	}
	
	if(sCount == 0)																//超出
	{
		UsartPrintf(USART_DEBUG, "HeartBeat TimeOut\r\n");
		
		errType = NET_DEVICE_Check();											//网络设备状态检查
	}
	
	if(errType == CHECK_CONNECTED || errType == CHECK_CLOSED)
		faultTypeReport = faultType = FAULT_EDP;								//标记为协议错误
	else if(errType == CHECK_NO_DEVICE)
		faultTypeReport = faultType = FAULT_NODEVICE;							//标记为设备错误
	else
		faultTypeReport = faultType = FAULT_NONE;								//无错误
	
	NET_DEVICE_ClrData();														//情况缓存
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);									//设置为数据收发模式

}

/*
************************************************************
*	函数名称：	OneNet_EDPKitCmd
*
*	函数功能：	EDP协议命令解析
*
*	入口参数：	data：平台下发的数据
*
*	返回参数：	无
*
*	说明：		暂时只做了连接解析
************************************************************
*/
_Bool OneNet_EDPKitCmd(unsigned char *data)
{

	if(data[0] == CONNRESP) //连接响应
	{
		UsartPrintf(USART_DEBUG, "DevLink: %d\r\n", data[3]);
		
		//0		连接成功
		//1		验证失败：协议错误
		//2		验证失败：设备ID鉴权失败
		//3		验证失败：服务器失败
		//4		验证失败：用户ID鉴权失败
		//5		验证失败：未授权
		//6		验证失败：激活码未分配
		//7		验证失败：该设备已被激活
		//8		验证失败：重复发送连接请求包
		//9		验证失败：重复发送连接请求包
		
		if(data[3] == 0)
			return 1;
		else
			return 0;
	}
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_Replace
*
*	函数功能：	替换平台下发命令中的结束符
*
*	入口参数：	res：平台下发的命令
*
*	返回参数：	无
*
*	说明：		平台开关、旋钮下发的命令会有固定4个结束符，替换为其他不常用字符，以方便string库解析
************************************************************
*/
void OneNet_Replace(unsigned char *res, unsigned char num)
{
	
	unsigned char count = 0;

	while(count < num)
	{
		if(*res == '\0')		//找到结束符
		{
			*res = '~';			//替换
			count++;
		}
		
		res++;
	}

}

/*
************************************************************
*	函数名称：	OneNet_App
*
*	函数功能：	平台下发命令解析、处理
*
*	入口参数：	cmd：平台下发的命令
*
*	返回参数：	无
*
*	说明：		提取出命令，响应处理
************************************************************
*/
void OneNet_App(unsigned char *cmd)
{

	char *dataPtr;
	char numBuf[10];
	int num = 0;
	int i;
	
	if(cmd[0] == PUSHDATA)								//pushdata功能接收的数据
		OneNet_Replace(cmd, 1);
	else
		OneNet_Replace(cmd, 4);
	
	dataPtr = strstr((const char *)cmd, "}");			//搜索'}'

	if(dataPtr != NULL)									//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
		if(strstr((char *)cmd, "alarmled"))				//搜索"alarmled",报警
		{
			if(num == 1)
			{
				for(i=0;i<9;i++)
				{
					Led5_Set(LED_ON);
					DelayMs(500);
					Led5_Set(LED_OFF);
					DelayMs(500);
				}
			}
			else if(num == 0)
			{
				Led5_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;					//标记数据反馈
		}
														//下同
		else if(strstr((char *)cmd, "filterled"))
		{
			if(num == 1)
			{
				Led4_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led4_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
		else if(strstr((char *)cmd, "humidifierled"))
		{
			if(num == 1)
			{
				Led3_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led3_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
		else if(strstr((char *)cmd, "lightled"))
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				Led2_Set(LED_ON);
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				Led2_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
		
		
		//---------------------------------------//
		else if(strstr((char *)cmd, "forward"))
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				motor_stop();
				Led11_Set(LED_ON);
				DelayMs(2000);
				motor_stop();
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				Led11_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
		else if(strstr((char *)cmd, "reverse"))
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				motor_stop();
				Led12_Set(LED_ON);
				DelayMs(2000);
				motor_stop();
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				Led12_Set(LED_OFF);
			}
			
			oneNetInfo.sendData = 1;
		}
		
		//
		else if(strstr((char *)cmd, "ctrl"))
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				Ctrl_On();
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				Ctrl_Off();
			}
			
			oneNetInfo.sendData = 1;
		}
	}
	
	NET_DEVICE_ClrData();								//清空缓存

}

