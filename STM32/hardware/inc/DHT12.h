#ifndef _DHT12_H_
#define _DHT12_H_
#include "stm32f10x.h"
#include "stdio.h"
#include "usart.h"
void sensor_iic_init(void);//初始化DHT12_IIC
u8 sensor_read(float* Temp,float* Humi);//读取数据
void Continuous_Collect(float* Temp,float* Humi,u8 times);//多次采集温湿度取平均值


//extern float Temprature,Humidity;
typedef struct
{

	float tempreture;
	float humidity;

} DHT12_INFO;

extern DHT12_INFO dht12Info;

#endif	//_DHT12_H_
