#ifndef _UI_SET_PARA_H_
#define _UI_SET_PARA_H_

#include "main.h"
#include "GUI.h"


void ui_set_para(void);
	
typedef struct{
	int into_sleep_sec;//��������ʱ��
	int start_sound_select;//������������
	
}PARA_SET;

void load_para(void);
extern PARA_SET g_set;
#endif /*_UI_PARA_H_*/