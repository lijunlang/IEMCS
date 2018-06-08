/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	HC-SR501.c
	*
	*	作者： 		李俊朗
	*
	*	日期： 		2018-03-15
	*
	*	版本： 		V1.1
	*
	*	说明： 		 HC-SR501人体红外感应驱动实现
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//包含头文件
#include "HC-SR501.h"



//板载按键IO引脚初始化.
void HC_SR501_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 使能(开启)KEY1引脚对应IO端口时钟 */  
  RCC_APB2PeriphClockCmd(HC_SR501_RCC_CLOCKGPIO, ENABLE);
   
  /* 设定KEY1对应引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = HC_SR501_GPIO_PIN;  
  /* 设定KEY1对应引脚IO最大操作速度 ：GPIO_Speed_50MHz */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  
  /* 设定KEY1对应引脚IO为浮空输入模式 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  /* 初始化KEY1对应引脚IO */
  GPIO_Init(HC_SR501_GPIO, &GPIO_InitStructure);
}

/**
  * 函数功能: 简单粗暴的延时函数
  * 输入参数: time；延时时间设置
  * 返 回 值: 无
  * 说    明：软件消抖
  */
static void HC_SR501_ScanDelay(void)
{  
  uint32_t i,j;
  for(i=0;i<100;++i)
    for(j=0;j<1000;++j){ }		
}

/**
  * 函数功能: 读取按键KEY1的状态
  * 输入参数：无
  * 返 回 值: KEY_DOWN：按键被按下；
  *           KEY_UP  ：按键没被按下
  * 说    明：无。
  */
HC_SR501_State_TypeDef HC_SR501_StateRead(void)
{
  /* 读取此时按键值并判断是否是被按下状态，如果是被按下状态进入函数内 */
  if(GPIO_ReadInputDataBit(HC_SR501_GPIO,HC_SR501_GPIO_PIN)==HC_SR501_ACTIVE_LEVEL)
  {
    /* 延时一小段时间，消除抖动 */
    HC_SR501_ScanDelay();
    /* 延时时间后再来判断按键状态，如果还是按下状态说明按键确实被按下 */
    if(GPIO_ReadInputDataBit(HC_SR501_GPIO,HC_SR501_GPIO_PIN)==HC_SR501_ACTIVE_LEVEL)
    {      
       /* 按键扫描完毕，确定按键被按下，返回按键被按下状态 */
      return HC_SR501_HIGH;
    }
  }
  /* 按键没被按下，返回没被按下状态 */
  return HC_SR501_LOW;
}

