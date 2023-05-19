#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"


extern TIM_HandleTypeDef TIM3_Handler;      //��ʱ��3��� 
extern TIM_OC_InitTypeDef TIM3_CH1Handler;  //��ʱ��3ͨ��1���
extern TIM_OC_InitTypeDef TIM3_CH2Handler;  //��ʱ��3ͨ��2���
extern TIM_OC_InitTypeDef TIM3_CH3Handler;  //��ʱ��3ͨ��3���
extern TIM_OC_InitTypeDef TIM3_CH4Handler;  //��ʱ��3ͨ��4���

void TIM3_PWM_Init(u16 arr, u16 psc);
void PWM_Init(void);

#endif

