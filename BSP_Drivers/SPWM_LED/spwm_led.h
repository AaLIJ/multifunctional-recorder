#ifndef __SPWM_LED_H__
#define __SPWM_LED_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float led_light;			// LED灯亮度，0～100有效
	uint16_t light_cnt;		// spwm计数
	uint8_t led_num;			// 亮灯个数，0～8有效
	uint8_t light_mode;					// 亮灯模式，0为直接亮灭，1为缓亮缓灭模式
	uint8_t mode_to_val;				// 缓冲模式目标亮度
	uint8_t mode_time;					// 模式1的缓冲时间，单位秒
	float mode_sval;						// 缓冲模式每次加减亮度值
	TIM_HandleTypeDef *phtim;		// 模拟pwm灯对应使用定时器
} SPWM_LED;

extern SPWM_LED g_spwm_led;	// spwm模式的LED灯对象

void set_leds(uint8_t sta);						// spwm内部函数，外部不直接调用
void spwm_led_init(TIM_HandleTypeDef *htim);	// 配置模拟PWM信号使用的定时器
void spwm_led_run(TIM_HandleTypeDef *htim);		// 在定时中断回调中执行该函数
void spwm_led_switch_light(uint8_t tolight);	// 设置指定亮度（缓亮缓灭模式）


#ifdef __cplusplus
}
#endif

#endif /* __SPWM_LED_H__ */
