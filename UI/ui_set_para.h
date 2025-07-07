#ifndef _UI_SET_PARA_H_
#define _UI_SET_PARA_H_

#include "main.h"
#include "GUI.h"


void ui_set_para(void);
	
typedef struct{
	int into_sleep_sec;//待机启动时间
	int start_sound_select;//开机播放声音
	
}PARA_SET;

void load_para(void);
extern PARA_SET g_set;
#endif /*_UI_PARA_H_*/