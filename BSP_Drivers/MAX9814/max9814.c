#include "max9814.h"
#include "adc.h"
#include "tim.h"
#include <stdio.h>
#include <math.h>
#include <cmsis_os.h>
#include "ui_record.h"
#define BUFF_SIZE_2	(MAX_DMA_BUFF_SIZE/2)

volatile uint16_t g_dma_buff[MAX_DMA_BUFF_SIZE];	// 采样数据缓冲
volatile uint16_t g_adv_val;		// 采样均值
volatile float g_max9814_db;		// 声音采集分贝数值

extern osThreadId_t defaultTaskHandle;

void StartSample(void)
{
//	printf("开始连续采集声音信号...\n");
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_dma_buff, MAX_DMA_BUFF_SIZE);
	HAL_TIM_Base_Start(&htim2);
}

void PauseSample(void)
{
//	printf("暂停声音信号采集...\n");
//	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)dma_buff1, MAX_DMA_BUFF_SIZE);
	HAL_TIM_Base_Stop(&htim2);
}

// 全局变量存储校准值
static float v_scale = 0.005f / 4095; // 根据实际硬件调整

float CalSpl(uint8_t bufidx) {
	uint16_t *adc_buffer = (uint16_t *)(bufidx ? g_dma_buff : (g_dma_buff + BUFF_SIZE_2));
	int64_t sum_squares = 0;
	uint32_t sum_val = 0;
	for (int i = 0; i < BUFF_SIZE_2; i++) {
		sum_val += adc_buffer[i];
	}
	g_adv_val = sum_val / BUFF_SIZE_2;

	for (int i = 0; i < BUFF_SIZE_2; i++) {
		int16_t sample_ac = adc_buffer[i] - g_adv_val; // 动态去除直流偏置（1.25V直流偏置约为1550）
		sum_squares += (int64_t)sample_ac * (int64_t)sample_ac;
	}
	float v_rms = sqrtf(sum_squares * 1.0f / BUFF_SIZE_2);
//	if (v_rms < 1e-6) return 0; // 避免log10(0)

	// 计算分贝
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
	
//		printf("采集缓冲满！\n");
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
//		printf("采集到缓冲一半！\n");
		}
		else
		  osThreadFlagsSet(defaultTaskHandle, 0x01);
	}
}

