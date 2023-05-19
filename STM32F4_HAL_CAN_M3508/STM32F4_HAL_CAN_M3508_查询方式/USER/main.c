#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "can.h"
#include "motor.h"
#include "motor_pid.h"
#include "timer.h"

/***************************************************************************************************
 ������M3508 PID����ʵ��
 ���ܣ�ͨ��������������M3508������ٶȣ�����CANͨ�Ž��е���ٶȱջ����ƣ����ò�ѯ��ʽ����CAN����
 ���ԣ�����ԭ��̽����F4����/������F7���ӣ���������F4��F7���˿��ͷ�ļ���һ����������ͬ
 ע�⣺ʹ��F1��CAN�������ֻ᲻һ������Ӧ����Ҫ�޸�motor.c�����
			 ��ȡ���ݺ���(get_moto_measure)��
			 д�����ݵĺ���(set_moto_current)
 ���ߣ���Ϊ��Ȼ @CSDN (��ҳ��https://blog.csdn.net/qq_30267617)
 ʱ�䣺2021-08-12
***************************************************************************************************/

int main(void)
{
	u8 key;
	s8 key_cnt;
	u8 i;
	u16 retry;
	u8 flag_motor[motor_num];		   //�����Ϣ���ܳɹ���־
	pid_t pid_speed[motor_num];		   //����ٶ�PID��
	float set_speed_temp;			   //�Ӽ���ʱ����ʱ�趨�ٶ�
	int16_t delta;					   //�趨�ٶ���ʵ���ٶȵĲ�ֵ
	int16_t max_speed_change = 1000;   //����������仯�ٶȣ��Ӽ�����
									   // 500�����Բ�������������䣬�����������ٶȲ��쳣�����ֵ
	static float set_speed[motor_num]; //����ٶ�ȫ�ֱ���

	HAL_Init();																	 //��ʼ��HAL��
	Stm32_Clock_Init(360, 25, 2, 8);											 //����ʱ��,180Mhz
	delay_init(180);															 //��ʼ����ʱ����
	uart_init(115200);															 //��ʼ��USART
	LED_Init();																	 //��ʼ��LED
	KEY_Init();																	 //��ʼ������
	CAN1_Mode_Init(CAN_SJW_1TQ, CAN_BS2_6TQ, CAN_BS1_8TQ, 3, CAN_MODE_LOOPBACK); //CAN��ʼ��,������1000Kbps
	PWM_Init();
	//PID��ʼ��
	for (i = 0; i < 4; i++)
	{
		PID_struct_init(&pid_speed[i], POSITION_PID, 16384, 16384, 1.5f, 0.1f, 0.0f); //4 motos angular rate closeloop.
	}

	// �ȴ�CANͨѶ�ɹ�
	while (!get_moto_measure(&moto_info, &CAN1_Handler))
	{
		LED0 = ~LED0;
		delay_ms(200);
	}

	while (1)
	{

		// ���������ٶȼӼ�
		key = KEY_Scan(0);
		if (key == KEY0_PRES) //KEY0����,����һ������
		{
			key_cnt++;
		}
		else if (key == KEY1_PRES) //WK_UP���£��ı�CAN�Ĺ���ģʽ
		{
			key_cnt--;
		}

#if defined CAN_CONTROL
		if (key_cnt < -16)
			key_cnt = -16;
		if (key_cnt > 16)
			key_cnt = 16;
		set_speed[0] = set_speed[1] = set_speed[2] = set_speed[3] = key_cnt * 500; // -8000-8000��˫��ת

		//PID��������ȡ�������
		retry = 0;
		flag_motor[0] = flag_motor[1] = flag_motor[2] = flag_motor[3] = 0;
		while (retry < 20)
		{
			get_moto_measure(&moto_info, &CAN1_Handler);
			if (CAN1_Handler.pRxMsg->StdId == 0x201)
			{
				moto_chassis[0] = moto_info;
				flag_motor[0] = 1;
			}
			if (CAN1_Handler.pRxMsg->StdId == 0x202)
			{
				moto_chassis[1] = moto_info;
				flag_motor[1] = 1;
			}
			if (CAN1_Handler.pRxMsg->StdId == 0x203)
			{
				moto_chassis[2] = moto_info;
				flag_motor[2] = 1;
			}
			if (CAN1_Handler.pRxMsg->StdId == 0x204)
			{
				moto_chassis[3] = moto_info;
				flag_motor[3] = 1;
			}

			if (flag_motor[0] && flag_motor[1] && flag_motor[2] && flag_motor[3])
				break;
			else
				retry++;
		}

		//����״̬���ϴ�����λ��
		//				CAN_MotoInfo.motor1.speed_rpm = moto_chassis[0].speed_rpm;
		//				CAN_MotoInfo.motor2.speed_rpm = moto_chassis[1].speed_rpm;
		//				CAN_MotoInfo.motor3.speed_rpm = moto_chassis[2].speed_rpm;
		//				CAN_MotoInfo.motor1.given_current = moto_chassis[0].given_current;
		//				CAN_MotoInfo.motor2.given_current = moto_chassis[1].given_current;
		//				CAN_MotoInfo.motor3.given_current = moto_chassis[2].given_current;

		//PID���������д5 ������һ�����������һ����������㣬������һ��δ֪bug
		for (i = 0; i < 5; i++)
		{
			//PID����

			// �޼Ӽ���
			//pid_calc(&pid_speed[i], (float)moto_chassis[i].speed_rpm, set_speed[i]);

			//�Ӽ���
			delta = (int16_t)set_speed[i] - moto_chassis[i].speed_rpm;
			if (delta > max_speed_change)
				set_speed_temp = (float)(moto_chassis[i].speed_rpm + max_speed_change);
			else if (delta < -max_speed_change)
				set_speed_temp = (float)(moto_chassis[i].speed_rpm - max_speed_change);
			else
				set_speed_temp = set_speed[i];
			pid_calc(&pid_speed[i], (float)moto_chassis[i].speed_rpm, set_speed_temp);
		}

		// ͨ�����ڵ������ֹ۲�ʵ���ٶ����趨�ٶ�
		printf("PID out: %f\r\n", pid_speed[0].pos_out);
		printf("real speed: %d \r\n", moto_chassis[0].speed_rpm);
		printf("set speed: %f \r\n", set_speed[0]);

		//PID ���
		set_moto_current(&CAN1_Handler, 0x200, (s16)(pid_speed[0].pos_out),
						 (s16)(pid_speed[1].pos_out),
						 (s16)(pid_speed[2].pos_out),
						 (s16)(pid_speed[3].pos_out));

#elif defined PWM_CONTROL
		if (key_cnt < -10)
			key_cnt = -10;
		if (key_cnt > 10)
			key_cnt = 10;
		set_speed[0] = set_speed[1] = set_speed[2] = set_speed[3] = key_cnt * 50; // -500-500,˫��ת
		__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, 1500+set_speed[0]);				// PWM��Χ1000-2000,˫��ת
		__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, 1500+set_speed[1]);
		__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3, 1500+set_speed[2]);
		__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 1500+set_speed[3]);

#endif

		delay_ms(5); // ��������
	}
}
