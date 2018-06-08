#ifndef __ADC_H__
#define	__ADC_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f10x.h>

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
/********************ADC输入通道（引脚）配置**************************/
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd

#define    ADCx                          ADC1
#define    ADC_CLK                       RCC_APB2Periph_ADC1
#define    ADC_DMA_CLK                   RCC_AHBPeriph_DMA1
#define    ADC_DMA_CHANNEL               DMA1_Channel1

//#define    ADCx                          ADC3
//#define    ADC_CLK                       RCC_APB2Periph_ADC3
//#define    ADC_DMA_CLK                   RCC_AHBPeriph_DMA2
//#define    ADC_DMA_CHANNEL               DMA2_Channel5

#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADC_PORT                      GPIOA

#define    ADC_PIN1                      GPIO_Pin_4      
#define    ADC_CHANNEL1                  ADC_Channel_4    
#define    ADC_PIN2                      GPIO_Pin_5      
#define    ADC_CHANNEL2                  ADC_Channel_5    
#define    ADC_PIN3                      GPIO_Pin_6       
#define    ADC_CHANNEL3                  ADC_Channel_6   
#define    ADC_PIN4                      GPIO_Pin_7       
#define    ADC_CHANNEL4                  ADC_Channel_7    

#define    ADC_NUMOFCHANNEL              4

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void ADCx_Init(void);

#endif /* __ADC_H__ */