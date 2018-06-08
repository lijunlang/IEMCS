#ifndef _LED_H_
#define _LED_H_







typedef struct
{

	_Bool Led2Sta;
	_Bool Led3Sta;
	_Bool Led4Sta;
	_Bool Led5Sta;
	_Bool Led11Sta;
	_Bool Led12Sta;

} LED_STATUS;

extern LED_STATUS ledStatus;

typedef enum
{

	LED_OFF = 0,
	LED_ON

} LED_ENUM;








void Led_Init(void);

void Alarm_Led(void);

void Led2_Set(LED_ENUM status);

void Led3_Set(LED_ENUM status);

void Led4_Set(LED_ENUM status);

void Led5_Set(LED_ENUM status);

void Led11_Set(LED_ENUM status);

void Led12_Set(LED_ENUM status);

#endif
