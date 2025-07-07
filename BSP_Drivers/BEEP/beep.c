#include "beep.h"
#include "tim.h"
#include <stdio.h>
#include <math.h>
#include <cmsis_os.h>

// ���������������������
uint8_t beep_tune = 0;
// �������������ʱ����������λ����
uint16_t beep_time = 0;

#define L8(X)		(X + 12500)
#define M8(X)		(X + 8 + 12500)
#define H8(X)		(X + 16 + 12500)

#define L6(X)		(X + 37500)
#define M6(X)		(X + 8 + 37500)
#define H6(X)		(X + 16 + 37500)

#define L4(X)		(X + 25000)
#define M4(X)		(X + 8 + 25000)
#define H4(X)		(X + 16 + 25000)

#define L3(X)		(X + 75000)
#define M3(X)		(X + 8 + 75000)
#define H3(X)		(X + 16 + 75000)

#define L2(X)		(X + 50000)
#define M2(X)		(X + 8 + 50000)
#define H2(X)		(X + 16 + 50000)

#define L1(X)		(X + 100000)
#define M1(X)		(X + 8 + 100000)
#define H1(X)		(X + 16 + 100000)

#define L0(X)		(X + 200000)
#define M0(X)		(X + 8 + 200000)
#define H0(X)		(X + 16 + 200000)

// �й�����־Ը��ս��
uint32_t MIDI[] = {
	M4(1), M4(1), M2(1), 										// ������
	L4(5), L4(6), L2(5),										// ������
	M6(3), M8(2), M4(1), L4(6),	M1(2),			// ���Ѽ�̽�
	M4(3), M4(3), M2(3),										// ����ƽ
	M4(5), M4(5), M2(5),										// �����
	M6(6), M8(5), M4(1), M4(3),	M1(2),			// ���Ǳ�����
	M1(3), M1(5),														// �й�
	M6(3), M8(2), M4(1), M4(2),	L1(6),			// �ö�Ů
	M2(6), H2(1), M3(5), M4(3),							// ����
	M2(1), M2(3), M1(5),										// �Ž��
	M4(6), M4(0), M4(5), M4(0),							// ����
	M4(3), M4(0), M4(5), M4(0),							// Ԯ��
	H3(1), M4(6),														// ���
	M4(5), M4(0), M4(3), M4(0),							// ����
	M4(5), M4(0), M4(2), M4(0),	M2(1), M2(0), // Ұ����
	M4(1), M4(1), M2(1), 
	L4(5), L4(6), L2(5),
	M6(3), M8(2), M4(1), L4(6),
	M1(2),
	M4(3), M4(3), M2(3),
	M4(5), M4(5), M2(5),
	M6(6), M8(5), M4(1), M4(3),
	M1(2),
	M1(3), M1(5),
	M6(3), M8(2), M4(1), M4(2),
	L1(6),
	M2(6), H2(1), M3(5), M4(3),
	M2(1), M2(3), M1(5),
	M4(6), M4(0), M4(5), M4(0),
	M4(3), M4(0), M4(5), M4(0),
	H3(1), M4(6),
	M4(5), M4(0), M4(3), M4(0),
	H4(1), M4(0), M4(5), M4(0),
	H2(1), M2(0),
};

void Beep(uint8_t tune, uint16_t time) { // ���к���
	// ������ӦƵ�ʱ�C4��B4��
	const float tab[8] = {0, 261.6, 293.6, 329.6, 349.2, 392.0, 440.0, 493.9};
	int key = (tune / 8) * 2;
	if (key < 1)
		key = 1;
	
	tune %= 8; // ����������Χ0��7
	if (tune > 0) { // �������Ч����
		// ����Ŀ��Ƶ�ʼ���ARR��ֵ��(1M / tab[tune]) / 2 - 1
		float arr = (1000000 / (tab[tune] * key) ) / 2 - 1; 
		__HAL_TIM_SET_AUTORELOAD(&htim3, (uint16_t)arr); // ���µ�����ʱƵ��
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, arr / 2); 
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // ������ʱ��1��PWM�������
		beep_tune = tune; // ��������
		beep_time = time; // ��������ʱ��
	}
	else // ������Ч����ʱ��ֹͣ��ʱ�����PWM�ź�
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void BeepRun(void)	// ����������ʱ�����ƺ�������1���붨ʱ�ж��е���
{
	if (beep_tune > 0 && beep_time > 0) 
		-- beep_time; // ������ʱ�����ƣ�ÿ����ݼ�
	else 
	{
		if (beep_tune > 0)
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
		beep_tune = 0; // ʱ��Ϊ0ʱ��������
		HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
	}
}

// ʱ��Ƭ��ʽ����midi���֣��Ѻ����ŵ�����ѭ���У�������ʱ����С��10ms
// ����playtime����λms�����ڿ��Ʋ���ʱ���������ų�ʱ���߲���Ϊ0ʱ����ֹͣ����
void PlayMidi(uint32_t playtime)
{
	static int midi_idx = 0;				// ������������
	static uint32_t tune_tick = 0;	// �������ſ���ʱ���
	static uint32_t end_tick = 0;	// ���Ž���ʱ���
	
	uint32_t ct = osKernelGetTickCount();
	if (playtime == 0)
	{
		end_tick = 0;		// ���ò��Ž���ʱ���
		if (beep_time > 0 || beep_tune > 0)
			Beep(0, 0);		// �رշ���������
		return;
	}
	else if (0 == end_tick)
		end_tick = ct + playtime;		// ���ý���ʱ���
	else if (ct < end_tick && ct >= tune_tick)
	{
		int midi_len = sizeof(MIDI) / sizeof(MIDI[0]);
		if (midi_idx >= midi_len)
			midi_idx = 0;
		uint32_t tune = MIDI[midi_idx++];
		uint16_t time = tune / 100;
		Beep(tune % 100, time * 9 / 10);
		tune_tick = ct + time;
	}
	else if (ct >= end_tick)
		Beep(0, 0);			// �رշ���������
}