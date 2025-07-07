#ifndef __SPWM_LED_H__
#define __SPWM_LED_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float led_light;			// LED�����ȣ�0��100��Ч
	uint16_t light_cnt;		// spwm����
	uint8_t led_num;			// ���Ƹ�����0��8��Ч
	uint8_t light_mode;					// ����ģʽ��0Ϊֱ������1Ϊ��������ģʽ
	uint8_t mode_to_val;				// ����ģʽĿ������
	uint8_t mode_time;					// ģʽ1�Ļ���ʱ�䣬��λ��
	float mode_sval;						// ����ģʽÿ�μӼ�����ֵ
	TIM_HandleTypeDef *phtim;		// ģ��pwm�ƶ�Ӧʹ�ö�ʱ��
} SPWM_LED;

extern SPWM_LED g_spwm_led;	// spwmģʽ��LED�ƶ���

void set_leds(uint8_t sta);						// spwm�ڲ��������ⲿ��ֱ�ӵ���
void spwm_led_init(TIM_HandleTypeDef *htim);	// ����ģ��PWM�ź�ʹ�õĶ�ʱ��
void spwm_led_run(TIM_HandleTypeDef *htim);		// �ڶ�ʱ�жϻص���ִ�иú���
void spwm_led_switch_light(uint8_t tolight);	// ����ָ�����ȣ���������ģʽ��


#ifdef __cplusplus
}
#endif

#endif /* __SPWM_LED_H__ */
