#include "ui_set_para.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "DS_18B20.h"
#include <cmsis_os.h>
#include "rtc.h"
#include "gpio.h"
#include "ssd1306.h"
#include "spwm_led.h" 
#include "fatfs.h"
#include "dac.h"
#include "adc.h"
#include "ui_record.h"
#include "max9814.h" 
#include "tim.h"
#include "w25qxx.h"

extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_24;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_48;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_12;
extern uint8_t open_record_file(FATFS*pfs,FIL*pfil,char*filename,uint8_t bplay);
void save_para(void);
void load_para(void);

static uint32_t tick=0;
PARA_SET g_set={
	5,1
};
PARA_SET tset;


static int setidx=0;
void ui_set_para(void)
{
	if(0==tick)
	{
		tick=osKernelGetTickCount ();
		load_para ();
		//tset=g_set;
	}
	uint8_t bdot=(osKernelGetTickCount ()%1000)>700?1:0;
	
	char str[40];
	GUI_Clear();
	GUI_SetFont (&GUI_FontHZ_SimSun_12 );
	GUI_DispStringHCenterAt ("参数设置",S_W/2,0);
	
	GUI_DispStringAt ("待机启动时间:(5-99)",0,14);
	GUI_DispStringAt ("开机播放声音:",0,40);
	
	GUI_SetTextAlign (GUI_TA_RIGHT );
	if(0==setidx)
		sprintf(str,"%s %d秒",bdot?"*":"",tset.into_sleep_sec);
	else 
		sprintf(str,"%d秒",tset.into_sleep_sec);
	GUI_DispStringAt(str,S_W,26);
	
	if(1==setidx)
		sprintf(str,"%s %s",bdot?"*":"",tset.start_sound_select?"语音留言":"内置音乐");
	else 
		sprintf(str,"%s",tset.start_sound_select?"语音留言":"内置音乐");
	GUI_SetTextAlign (GUI_TA_RIGHT );
	GUI_DispStringAt(str,S_W,52);
	
	GUI_Update();
	
	uint32_t key=osThreadFlagsWait (0xFFFF,osFlagsWaitAny,0);
	switch(key)
	{
		default:
			break;
		case K1_Pin:
			setidx=!setidx;
			break;
		case K2_Pin:
			if(0==setidx)
			{
				--tset.into_sleep_sec ;
				if(tset.into_sleep_sec <5)
					tset.into_sleep_sec =99;
			}
			else
			{
				tset.start_sound_select=!tset.start_sound_select;
			}
			break;
		case K3_Pin:
			if(0==setidx)
			{
				++tset.into_sleep_sec ;
				if(tset.into_sleep_sec >99)
					tset.into_sleep_sec =5;
			}
			else
			{
				tset.start_sound_select=!tset.start_sound_select;
			}
			break;
		case K4_Pin:
			setidx=!setidx;
			break;
		case K5_Pin:
			g_set=tset;
			save_para ();
			if(1==setidx&&tset.start_sound_select !=0)
			{
				char curFileName[40];
				sprintf(curFileName,"%s0.rec",USERPath);
				if(open_record_file(&USERFatFS,&USERFile,curFileName,0))
				{
					HAL_ADC_Stop_DMA(&hadc1);
					printf("开始录音\n");
					g_wsta=STA_RECORD;
					StartSample();
					
					osDelay(5000);
					g_wsta =STA_SAVE;
				}
			}
			g_uista=UI_MAIN;
			break;
		case K6_Pin:
			g_uista=UI_MAIN;
			break;
	}
}

void save_para(void)
{
	printf("save para:%d %d\n",g_set.into_sleep_sec ,g_set.start_sound_select);
	W25QXX_Write ((uint8_t *)(&g_set),0,sizeof(g_set));
	
}
void load_para(void)
{
	
	W25QXX_Read ((uint8_t *)(&tset),0,sizeof(g_set));
	if(tset.into_sleep_sec >99||tset.into_sleep_sec <5)
	{
		tset.into_sleep_sec =10;
	}
	if(tset.start_sound_select <0||tset.start_sound_select >1)
	{
		tset.start_sound_select =0;
	}
	g_set=tset;
	printf("load para:%d %d\n",g_set.into_sleep_sec ,g_set.start_sound_select);
}