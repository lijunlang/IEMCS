/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	adc_sensor.c
	*
	*	作者： 		李俊朗
	*
	*	日期： 		2018-03-20
	*
	*	版本： 		V1.0
	*
	*	说明： 		获取数据
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/
//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "adc.h"
#include "adc_sensor.h"
#include "usart.h"
#include "led.h"
#include "delay.h"

/* 用于保存转换计算后的电压值 */	 
__IO float ADC_ConvertedValueLocal[ADC_NUMOFCHANNEL];   

/* 扩展变量 ------------------------------------------------------------------*/
extern __IO uint16_t ADC_ConvertedValue[ADC_NUMOFCHANNEL];
/* 私有函数原形 --------------------------------------------------------------*/
static void Delay(uint32_t time);

int Gas;
int Light;
//int Rain;


GAS_INFO gasInfo = {0};



ADCSENSOR_INFO adcsensorInfo;;

void sensor_adc_init(void)
{
	
}



void adcsensor_read()
{

	
		ADC_ConvertedValueLocal[0] =(float)ADC_ConvertedValue[0]*3.3/4096; 
		ADC_ConvertedValueLocal[1] =(float)ADC_ConvertedValue[1]*3.3/4096; 
		ADC_ConvertedValueLocal[2] =(float)ADC_ConvertedValue[2]*3.3/4096; 
		ADC_ConvertedValueLocal[3] =(float)ADC_ConvertedValue[3]*3.3/4096; 
	
		//UsartPrintf(USART_DEBUG,"CH1_PA4 value = %fV\n",ADC_ConvertedValueLocal[0]);
    //UsartPrintf(USART_DEBUG,"CH2_PA5 value = %fV\n",ADC_ConvertedValueLocal[1]);
    //UsartPrintf(USART_DEBUG,"CH3_PA6 value = %fV\n",ADC_ConvertedValueLocal[2]);
    //UsartPrintf(USART_DEBUG,"CH4_PA7 value = %fV\n",ADC_ConvertedValueLocal[3]);
    //UsartPrintf(USART_DEBUG,"\n");


/*---------------------------1.获取有害气体数据------------------------------**/		
		Gas=(unsigned int)(ADC_ConvertedValueLocal[0]*ADC_ConvertedValueLocal[0]*100-35);		
		
		//UsartPrintf(USART_DEBUG,"CH1_PA4 value = %dV\n",Gas);
	
/*---------------------------2.获取光照强度数据------------------------------**/		
		Light=10/ADC_ConvertedValueLocal[2]/ADC_ConvertedValueLocal[2];		////ADC_ConvertedValueLocal[1]/0.1
		
		//UsartPrintf(USART_DEBUG,"CH1_PA5 value = %dV\n",Light);
		
/*---------------------------3.获取雨数据------------------------------**/		
		//Rain=ADC_ConvertedValueLocal[1];	
		
		//UsartPrintf(USART_DEBUG,"CH1_PA6 value = %dV\n",Rain);

}

