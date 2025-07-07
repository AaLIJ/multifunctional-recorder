#include "tim.h"
#include "spwm_led.h"
#include <stdio.h>
#include <math.h>
#include <cmsis_os.h>

SPWM_LED g_spwm_led = {
	.led_num = 1,			// Ĭ��8������
	.light_mode = 1,	// Ĭ�ϻ���ģʽ
	.light_cnt = 0, 	// ���Ƽ���Ĭ��Ϊ0
	.mode_time = 5,		// ����ģʽ�Ļ���ʱ�䣨5�룩
	.mode_sval = 0,
	.mode_to_val = 0,
};	// spwmģʽ��LED�ƶ���

// ��ʼ��spwm_led�ƣ�������Ӧ��ʱ��
void spwm_led_init(TIM_HandleTypeDef *htim)
{
	if (htim)
	{
		g_spwm_led.phtim = htim;
		HAL_TIM_Base_Start_IT(htim);
	}
}

void spwm_led_switch_light(uint8_t tolight)
{
	if (tolight > 100)
		tolight = 100;
	g_spwm_led.mode_to_val = tolight;
	if (g_spwm_led.light_mode && g_spwm_led.mode_time > 0)
		g_spwm_led.mode_sval = (g_spwm_led.mode_to_val - g_spwm_led.led_light) / (g_spwm_led.mode_time * 1000.0f);
	else
		g_spwm_led.led_light = tolight;
}

// ��ʱ�жϵ��õ�spwm_led�ƿغ���
void spwm_led_run(TIM_HandleTypeDef *htim)
{
	if (htim == g_spwm_led.phtim)
	{
		++g_spwm_led.light_cnt;
		if (g_spwm_led.light_cnt >= __HAL_TIM_GET_AUTORELOAD(g_spwm_led.phtim))
			g_spwm_led.light_cnt = 0;
		
		if (g_spwm_led.light_mode && g_spwm_led.led_light != g_spwm_led.mode_to_val)	// ����ģʽ
		{
			g_spwm_led.led_light += g_spwm_led.mode_sval;
			if (fabs(g_spwm_led.led_light - g_spwm_led.mode_to_val) < 0.01f)
				g_spwm_led.led_light = g_spwm_led.mode_to_val;
		}
		
		uint8_t sta = 0;
		uint8_t light = g_spwm_led.led_light + 0.1f;
		for (uint8_t i = 0; i < g_spwm_led.led_num && i < 8; ++i)
		{
			if (light > g_spwm_led.light_cnt)	// ����
				sta |= 0x01 << i;
		}
		set_leds(sta);
	}
}

void set_leds(uint8_t sta)
{
	HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, (sta & 0x01) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, (sta & 0x02) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L3_GPIO_Port, L3_Pin, (sta & 0x04) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L4_GPIO_Port, L4_Pin, (sta & 0x08) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L5_GPIO_Port, L5_Pin, (sta & 0x10) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L6_GPIO_Port, L6_Pin, (sta & 0x20) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L7_GPIO_Port, L7_Pin, (sta & 0x40) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L8_GPIO_Port, L8_Pin, (sta & 0x80) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void LoadSpwmParas(void)
{
}

void SaveSpwmParas(void)
{
}
