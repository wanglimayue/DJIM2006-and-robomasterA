#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM3_Handler;     //��ʱ�����
TIM_OC_InitTypeDef TIM3_CH1Handler; //��ʱ��3ͨ��1���
TIM_OC_InitTypeDef TIM3_CH2Handler; //��ʱ��3ͨ��1���
TIM_OC_InitTypeDef TIM3_CH3Handler; //��ʱ��3ͨ��1���
TIM_OC_InitTypeDef TIM3_CH4Handler; //��ʱ��3ͨ��1���

//TIM4 PWM���ֳ�ʼ��
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u16 arr, u16 psc)
{
    TIM3_Handler.Instance = TIM3;                       //��ʱ��3
    TIM3_Handler.Init.Prescaler = psc;                  //��ʱ����Ƶ
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; //���ϼ���ģʽ
    TIM3_Handler.Init.Period = arr;                     //�Զ���װ��ֵ
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler); //��ʼ��PWM

    TIM3_CH1Handler.OCMode = TIM_OCMODE_PWM2;                                  //ģʽѡ��PWM1
    TIM3_CH1Handler.Pulse = arr / 2;                                           //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�
    TIM3_CH1Handler.OCPolarity = TIM_OCPOLARITY_LOW;                           //����Ƚϼ���Ϊ��
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH1Handler, TIM_CHANNEL_1); //����TIM3ͨ��2
    HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_1);                           //����PWMͨ��2

    TIM3_CH2Handler.OCMode = TIM_OCMODE_PWM2;                                  //ģʽѡ��PWM1
    TIM3_CH2Handler.Pulse = arr / 2;                                           //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�
    TIM3_CH2Handler.OCPolarity = TIM_OCPOLARITY_LOW;                           //����Ƚϼ���Ϊ��
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH2Handler, TIM_CHANNEL_2); //����TIM3ͨ��2
    HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_2);                           //����PWMͨ��2

    TIM3_CH3Handler.OCMode = TIM_OCMODE_PWM2;                                  //ģʽѡ��PWM1
    TIM3_CH3Handler.Pulse = arr / 2;                                           //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�
    TIM3_CH3Handler.OCPolarity = TIM_OCPOLARITY_LOW;                           //����Ƚϼ���Ϊ��
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH3Handler, TIM_CHANNEL_3); //����TIM3ͨ��2
    HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_3);                           //����PWMͨ��2

    TIM3_CH4Handler.OCMode = TIM_OCMODE_PWM2;                                  //ģʽѡ��PWM1
    TIM3_CH4Handler.Pulse = arr / 2;                                           //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�
    TIM3_CH4Handler.OCPolarity = TIM_OCPOLARITY_LOW;                           //����Ƚϼ���Ϊ��
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH4Handler, TIM_CHANNEL_4); //����TIM3ͨ��2
    HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_4);                           //����PWMͨ��2
}


//��ʱ���ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_PWM_Init()����
//htim:��ʱ�����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;

    if (htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();                                          //ʹ�ܶ�ʱ��3
        __HAL_RCC_GPIOC_CLK_ENABLE(); 
        __HAL_RCC_GPIOA_CLK_ENABLE();                                        //����GPIOBʱ��
        GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9; //PA6 PA7
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;                                  //�����������
        GPIO_Initure.Pull = GPIO_PULLUP;                                      //����
        GPIO_Initure.Speed = GPIO_SPEED_HIGH;                                 //����
        GPIO_Initure.Alternate = GPIO_AF2_TIM3;                               //����ΪTIM3
        HAL_GPIO_Init(GPIOC, &GPIO_Initure);
        GPIO_Initure.Pin = GPIO_PIN_6;
        HAL_GPIO_Init(GPIOA, &GPIO_Initure);
    }
    
}


void PWM_Init(void)
{
    TIM3_PWM_Init(20000 - 1, 108 - 1); //108M/108=1M�ļ���Ƶ�ʣ��Զ���װ��Ϊ20000����ôPWMƵ��Ϊ1M/2000=50HZ

    __HAL_TIM_SetCompare(&TIM3_Handler, TIM_CHANNEL_1, 1500);
		__HAL_TIM_SetCompare(&TIM3_Handler, TIM_CHANNEL_2, 1500);
		__HAL_TIM_SetCompare(&TIM3_Handler, TIM_CHANNEL_3, 1500);
		__HAL_TIM_SetCompare(&TIM3_Handler, TIM_CHANNEL_4, 1500);
}
