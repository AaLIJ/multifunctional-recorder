#ifndef __BEEP_H__
#define __BEEP_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void Beep(uint8_t tune, uint16_t time);	// 蜂鸣器鸣叫函数，音阶0～21，时长单位ms
void BeepRun(void);	// 蜂鸣器鸣叫时长控制函数，在1毫秒定时中断中调用
void PlayMidi(uint32_t playtime);				// 音乐播放函数，把函数放到任务循环中，任务延时尽量小于10ms

#ifdef __cplusplus
}
#endif

#endif /* __BEEP_H__ */
