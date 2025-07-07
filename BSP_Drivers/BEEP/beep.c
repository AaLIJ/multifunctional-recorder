#include "beep.h"
#include "tim.h"
#include <stdio.h>
#include <math.h>
#include <cmsis_os.h>

// 定义蜂鸣器鸣叫音调变量
uint8_t beep_tune = 0;
// 定义蜂鸣器鸣叫时长变量，单位毫秒
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

// 中国人们志愿军战歌
uint32_t MIDI[] = {
	M4(1), M4(1), M2(1), 										// 雄赳赳
	L4(5), L4(6), L2(5),										// 气昂昂
	M6(3), M8(2), M4(1), L4(6),	M1(2),			// 跨过鸭绿江
	M4(3), M4(3), M2(3),										// 保和平
	M4(5), M4(5), M2(5),										// 卫祖国
	M6(6), M8(5), M4(1), M4(3),	M1(2),			// 就是保家乡
	M1(3), M1(5),														// 中国
	M6(3), M8(2), M4(1), M4(2),	L1(6),			// 好儿女
	M2(6), H2(1), M3(5), M4(3),							// 齐心
	M2(1), M2(3), M1(5),										// 团结紧
	M4(6), M4(0), M4(5), M4(0),							// 抗美
	M4(3), M4(0), M4(5), M4(0),							// 援朝
	H3(1), M4(6),														// 打败
	M4(5), M4(0), M4(3), M4(0),							// 美帝
	M4(5), M4(0), M4(2), M4(0),	M2(1), M2(0), // 野心狼
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

void Beep(uint8_t tune, uint16_t time) { // 鸣叫函数
	// 音调对应频率表（C4～B4）
	const float tab[8] = {0, 261.6, 293.6, 329.6, 349.2, 392.0, 440.0, 493.9};
	int key = (tune / 8) * 2;
	if (key < 1)
		key = 1;
	
	tune %= 8; // 限制音调范围0～7
	if (tune > 0) { // 如果是有效音调
		// 根据目标频率计算ARR的值：(1M / tab[tune]) / 2 - 1
		float arr = (1000000 / (tab[tune] * key) ) / 2 - 1; 
		__HAL_TIM_SET_AUTORELOAD(&htim3, (uint16_t)arr); // 重新调整定时频率
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, arr / 2); 
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // 启动定时器1的PWM输出功能
		beep_tune = tune; // 保存音调
		beep_time = time; // 设置鸣叫时长
	}
	else // 不是有效音调时，停止定时器输出PWM信号
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void BeepRun(void)	// 蜂鸣器鸣叫时长控制函数，在1毫秒定时中断中调用
{
	if (beep_tune > 0 && beep_time > 0) 
		-- beep_time; // 蜂鸣器时长控制，每毫秒递减
	else 
	{
		if (beep_tune > 0)
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
		beep_tune = 0; // 时长为0时音调清零
		HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
	}
}

// 时间片方式播放midi音乐，把函数放到任务循环中，任务延时尽量小于10ms
// 参数playtime，单位ms，用于控制播放时长，当播放超时或者参数为0时立即停止播放
void PlayMidi(uint32_t playtime)
{
	static int midi_idx = 0;				// 播放音符索引
	static uint32_t tune_tick = 0;	// 音符播放控制时间戳
	static uint32_t end_tick = 0;	// 播放结束时间戳
	
	uint32_t ct = osKernelGetTickCount();
	if (playtime == 0)
	{
		end_tick = 0;		// 重置播放结束时间戳
		if (beep_time > 0 || beep_tune > 0)
			Beep(0, 0);		// 关闭蜂鸣器声音
		return;
	}
	else if (0 == end_tick)
		end_tick = ct + playtime;		// 设置结束时间戳
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
		Beep(0, 0);			// 关闭蜂鸣器声音
}