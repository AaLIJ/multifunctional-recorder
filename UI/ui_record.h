#ifndef _UI_RECORD_H_
#define _UI_RECORD_H_

#include "main.h"
#include "GUI.h"

typedef enum{STA_READY,STA_RECORD,STA_PLAY,STA_SAVE}WORK_STA;
extern WORK_STA g_wsta;//Â¼Òô·ÅÒô×´Ì¬
void ui_record(void);
	


#endif /*_UI_RECORD_H_*/