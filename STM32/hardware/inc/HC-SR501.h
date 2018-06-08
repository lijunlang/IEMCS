#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f10x.h>

/* 类型定义 --------------------------------------------------------------*/
typedef enum
{
  HC_SR501_LOW   = 0,
  HC_SR501_HIGH = 1,
}HC_SR501_State_TypeDef;

/* 宏定义 --------------------------------------------------------------------*/
#define HC_SR501_RCC_CLOCKGPIO            RCC_APB2Periph_GPIOA
#define HC_SR501_GPIO_PIN                 GPIO_Pin_1
#define HC_SR501_GPIO                     GPIOA
#define HC_SR501_ACTIVE_LEVEL             1

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void HC_SR501_GPIO_Init(void);
HC_SR501_State_TypeDef HC_SR501_StateRead(void);


#endif  // __BSP_KEY_H__

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
