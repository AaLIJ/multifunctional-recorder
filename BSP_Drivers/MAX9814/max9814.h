#ifndef __MAX9814_H__
#define __MAX9814_H__

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DMA_BUFF_SIZE  16000
extern volatile uint16_t g_dma_buff[MAX_DMA_BUFF_SIZE];	// �������ݻ���
extern volatile uint16_t g_adv_val;		// ������ֵ
extern volatile float g_max9814_db;		// �����ɼ��ֱ���ֵ

void StartSample(void);
void PauseSample(void);
float CalSpl(uint8_t bufidx);
void CalADVal(uint8_t bufidx);

#ifdef __cplusplus
}
#endif

#endif /* __MAX9814_H__ */
