#include "max9814.h"
#include "adc.h"
#include "tim.h"
#include <stdio.h>
#include <math.h>
#include <cmsis_os.h>
#include "ui_record.h"
#define BUFF_SIZE_2	(MAX_DMA_BUFF_SIZE/2)

volatile uint16_t g_dma_buff[MAX_DMA_BUFF_SIZE];	// �������ݻ���
volatile uint16_t g_adv_val;		// ������ֵ
volatile float g_max9814_db;		// �����ɼ��ֱ���ֵ

extern osThreadId_t defaultTaskHandle;

void StartSample(void)
{
//	printf("��ʼ�����ɼ������ź�...\n");
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_dma_buff, MAX_DMA_BUFF_SIZE);
	HAL_TIM_Base_Start(&htim2);
}

void PauseSample(void)
{
//	printf("��ͣ�����źŲɼ�...\n");
//	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)dma_buff1, MAX_DMA_BUFF_SIZE);
	HAL_TIM_Base_Stop(&htim2);
}

// ȫ�ֱ����洢У׼ֵ
static float v_scale = 0.005f / 4095; // ����ʵ��Ӳ������

float CalSpl(uint8_t bufidx) {
	uint16_t *adc_buffer = (uint16_t *)(bufidx ? g_dma_buff : (g_dma_buff + BUFF_SIZE_2));
	int64_t sum_squares = 0;
	uint32_t sum_val = 0;
	for (int i = 0; i < BUFF_SIZE_2; i++) {
		sum_val += adc_buffer[i];
	}
	g_adv_val = sum_val / BUFF_SIZE_2;

	for (int i = 0; i < BUFF_SIZE_2; i++) {
		int16_t sample_ac = adc_buffer[i] - g_adv_val; // ��̬ȥ��ֱ��ƫ�ã�1.25Vֱ��ƫ��ԼΪ1550��
		sum_squares += (int64_t)sample_ac * (int64_t)sample_ac;
	}
	float v_rms = sqrtf(sum_squares * 1.0f / BUFF_SIZE_2);
//	if (v_rms < 1e-6) return 0; // ����log10(0)

	// ����ֱ�
	const float s = 28.5f; 
	const float p0 = 5.2f;
	float spl = g_max9814_db = s * log10f(v_rms) - p0;
//	printf("%8.1f => %4.1fdB\n", v_rms, spl);
//	printf("%4.1fdB\n", spl);
	return spl;
}

void CalADVal(uint8_t bufidx) {
	uint16_t *adc_buffer = (uint16_t *)(bufidx ? g_dma_buff : (g_dma_buff + BUFF_SIZE_2));
	uint32_t sum_val = 0;
	for (int i = 0; i < BUFF_SIZE_2; i++) {
		sum_val += adc_buffer[i];
	}
	g_adv_val = sum_val / BUFF_SIZE_2;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) 
{
	if (hadc == &hadc1)
	{
	
//		printf("�ɼ���������\n");
		if(g_wsta !=STA_RECORD)
		{
			osThreadFlagsSet(defaultTaskHandle, 0x04|0x02);
		  HAL_TIM_Base_Stop(&htim2);
      HAL_ADC_Stop_DMA(&hadc1);	
		}
		else
		   osThreadFlagsSet(defaultTaskHandle, 0x02);
	}
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc == &hadc1)
	{
		if(g_wsta!=STA_RECORD)
		{
			osThreadFlagsSet(defaultTaskHandle, 0x04|0x01);
		  HAL_TIM_Base_Stop(&htim2);
		  HAL_ADC_Stop_DMA(&hadc1);
//		printf("�ɼ�������һ�룡\n");
		}
		else
		  osThreadFlagsSet(defaultTaskHandle, 0x01);
	}
}

