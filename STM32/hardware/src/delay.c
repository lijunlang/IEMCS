/**
	************************************************************
	*
	*	说明： 		利用Timer6做阻塞式延时
	*
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//delay头文件
#include "delay.h"

static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数

//延时系数
unsigned char UsCount = 0;
unsigned short MsCount = 0;


//初始化延迟函数
//SYSTICK的时钟固定为AHB时钟，基础例程里面SYSTICK时钟频率为AHB/8
//这里为了兼容FreeRTOS，所以将SYSTICK的时钟频率改为AHB的频率！
//SYSCLK:系统时钟频率
void delay_init()
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//选择外部时钟  HCLK
	fac_us=SystemCoreClock/1000000;				//不论是否使用OS,fac_us都需要使用
	reload=SystemCoreClock/1000000;				//每秒钟的计数次数 单位为M  
	reload*=1000000/configTICK_RATE_HZ;			//根据configTICK_RATE_HZ设定溢出时间
												//reload为24位寄存器,最大值:16777216,在72M下,约合0.233s左右	
	fac_ms=1000/configTICK_RATE_HZ;				//代表OS可以延时的最少单位	   

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 						//每1/configTICK_RATE_HZ秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    
}		



/*
************************************************************
*	函数名称：	Delay_Init
*
*	函数功能：	systick初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Delay_Init(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM3->CR1 |= 1UL << 3;									//单脉冲模式
	
	TIM3->EGR |= 1;											//更新生成
	
	TIM3->DIER = 0;											//禁止所有中断
	
	TIM3->CR1 &= (unsigned short)~TIM_CR1_CEN;				//停止计时

}

/*
************************************************************
*	函数名称：	DelayUs
*
*	函数功能：	微秒级延时
*
*	入口参数：	us：延时的时长
*
*	返回参数：	无
*
*	说明：		此时钟(21MHz)最大延时798915us
************************************************************
*/
void DelayUs(unsigned short us)
{

	TIM3->ARR = us;
	
	TIM3->PSC = 71;									//timer6为72MHz，设置为71+1分频，1MHz，1us计数一次
	
	TIM3->CR1 |= (unsigned short)TIM_CR1_CEN;		//开始计时
	
	while(!(TIM3->SR & 1));							//等待产生中断事件
	
	TIM3->SR &= ~(1UL << 0);						//清除标志

}

/*
************************************************************
*	函数名称：	DelayXms
*
*	函数功能：	毫秒级延时
*
*	入口参数：	ms：延时的时长
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void DelayXms(unsigned short ms)
{

	if(ms < 32768)
	{
		TIM3->ARR = (ms << 1);						//双倍计数值
		
		TIM3->PSC = 35999;							//timer6为72MHz，设置为35999+1分频，2KHz，500us计数一次
		
		TIM3->CR1 |= (unsigned short)TIM_CR1_CEN;	//开始计时
		
		while(!(TIM3->SR & 1));						//等待产生中断事件
		
		TIM3->SR &= ~(1UL << 0);					//清除标志
	}

}

/*
************************************************************
*	函数名称：	DelayMs
*
*	函数功能：	微秒级长延时
*
*	入口参数：	ms：延时的时长
*
*	返回参数：	无
*
*	说明：		多次调用DelayXms，做到长延时
************************************************************
*/
void DelayMs(unsigned short ms)
{

	unsigned char repeat = 0;
	unsigned short remain = 0;
	
	repeat = ms / 500;
	remain = ms % 500;
	
	while(repeat)
	{
		DelayXms(500);
		repeat--;
	}
	
	if(remain)
		DelayXms(remain);

}
