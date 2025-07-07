#ifndef __BEEP_H__
#define __BEEP_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void Beep(uint8_t tune, uint16_t time);	// ���������к���������0��21��ʱ����λms
void BeepRun(void);	// ����������ʱ�����ƺ�������1���붨ʱ�ж��е���
void PlayMidi(uint32_t playtime);				// ���ֲ��ź������Ѻ����ŵ�����ѭ���У�������ʱ����С��10ms

#ifdef __cplusplus
}
#endif

#endif /* __BEEP_H__ */
