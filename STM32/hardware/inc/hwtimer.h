#ifndef _HWTIMER_H_
#define _HWTIMER_H_

#include "stm32f10x.h"





typedef struct
{

	unsigned char timer3Out : 2;
	unsigned char reverse : 6;

} TIM_INFO;

extern TIM_INFO timInfo;

#define OS_TIMER	TIM3






void Timer3_4_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);

#endif
