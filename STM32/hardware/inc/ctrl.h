#ifndef _CTRL_H_
#define _CTRL_H_







typedef struct
{

	_Bool CtrlSta;

} CTRL_STATUS;

extern CTRL_STATUS ctrlStatus;

typedef enum
{

	CTRL_OFF = 0,
	CTRL_ON

} CTRL_ENUM;








void Ctrl_Init(void);

void Ctrl_On();

void Ctrl_Off();

void motor_stop();

#endif
