#include "ui_start.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmsis_os.h>
//#include "spwm_led.h"
#include "gpio.h"
#include "beep.h"
#include "ui_set_para.h" 
#include "fatfs.h"
#include "dac.h"
#include "adc.h"
#include "ui_record.h"
#include "max9814.h" 
#include "tim.h"

extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_16;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_24;
extern GUI_CONST_STORAGE GUI_BITMAP bmIMG1;
extern GUI_CONST_STORAGE GUI_BITMAP bmIMG;
extern osThreadId_t defaultTaskHandle;
extern osSemaphoreId_t guiSemaHandle;
extern uint8_t open_record_file(FATFS*pfs,FIL*pfil,char*filename,uint8_t bplay);
extern FIL *g_pFil;//指针地址切换
extern FATFS *g_pFs;
extern char *path;
static uint32_t tick=0;

void goto_main(void)
{
	g_uista=UI_MAIN;
	tick=0;
	SetLEDS(0x01);
	PlayMidi(0);
}
void PlayRecordSound(void)
{
	char curFileName[40];
	sprintf(curFileName,"%s0.rec",path);
	if(open_record_file(g_pFs,g_pFil,curFileName,1))
				{
					HAL_ADC_Stop_DMA(&hadc1);
					printf("开始放音");
					g_wsta=STA_PLAY;
		      uint32_t br;
					f_read(g_pFil,(uint8_t*)g_dma_buff,MAX_DMA_BUFF_SIZE*2,&br);
					
					HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t*)g_dma_buff,MAX_DMA_BUFF_SIZE,DAC_ALIGN_12B_R);
					HAL_TIM_Base_Start(&htim2);
					
					
				}
}

void ui_start(void)
{
	char str[40];
	GUI_Clear();
	
	if(0==g_set.start_sound_select )
		PlayMidi(8000);
	
	if(tick==0)
	{
		tick=osKernelGetTickCount();
		SetLEDS(0);
		load_para ();
		if(1==g_set.start_sound_select )
		{
			PlayRecordSound();
		}
	}
	if(osKernelGetTickCount ()>=tick+8000)
	{
		goto_main();
	}
	
	if(osKernelGetTickCount ()>=tick+5000)
	{
		GUI_DrawBitmap (&bmIMG1,0,0);
		GUI_DrawBitmap (&bmIMG,64,0);
	}
	else if(osKernelGetTickCount ()>=tick+2000)
	{
		GUI_SetFont (&GUI_FontHZ_SimSun_16  );
		GUI_DispStringHCenterAt ("22届DXQ",S_W/2,16);
	}
	else 
	{	
		GUI_SetFont (&GUI_FontHZ_SimSun_24  );
		GUI_DispStringHCenterAt ("简易多功能\n录音机设计",S_W/2,8);
	}
	osSemaphoreAcquire(guiSemaHandle,100);
	GUI_Update();
	osSemaphoreRelease(guiSemaHandle);
	uint32_t key=osThreadFlagsWait (0xFFFF,osFlagsWaitAny,0);
	
	if(K5_Pin==key)
	{
		goto_main();
	}
	
}