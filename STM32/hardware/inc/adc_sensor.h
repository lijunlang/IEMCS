#ifndef _ADC_SENSOR_H_
#define _ADC_SENSOR_H_

#include "stm32f10x.h"
#include "adc.h"

void sensor_adc_init(void);//初始化ADC传感器

void adcsensor_read();//读取数据

//float* Gas,float* Light,float* Rain

//void Continuous_Collect_adc(float* Gas,float* Light,float* Rain,u8 times);//多次采集取平均值

extern int Gas;
extern int Light;
extern int Rain;

typedef struct
{

	_Bool Gas_Status;

} GAS_INFO;



extern GAS_INFO gasInfo;



//extern float Temprature,Humidity;
typedef struct
{

	float gas;
	float light;
	float rain;

} ADCSENSOR_INFO;

extern ADCSENSOR_INFO adcsensorInfo;

#endif
