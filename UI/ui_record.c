#include "ui_record.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmsis_os.h>
//#include "spwm_led.h"
#include "gpio.h"
#include "beep.h"
#include "fatfs.h"
#include "max9814.h" 
#include  "adc.h"
#include  "dac.h"
#include  "tim.h"
WORK_STA g_wsta=STA_READY;//录音放音状态
	
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_12;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_32;
extern uint8_t g_bInitFSOK;
extern osThreadId_t defaultTaskHandle;
extern osSemaphoreId_t guiSemaHandle;
static uint32_t tick=0;
static uint32_t rp_start_tick=0;
static uint32_t  k2_tick=0;
static int n_idx=1;//录音文件编号
char curFileName[20];//当前录放文件名
char* g_storageNames[] = {"FLASH", "TF卡", "U盘"};//当前卡的名字
uint8_t g_currentStorage = 0;//名字数组切换
FIL *g_pFil=&USERFile;//指针地址切换
FATFS *g_pFs = &USERFatFS;  // 文件切换
char *path=USERPath;
extern FRESULT InitFS(FATFS *pFs, FIL *pFil, char *path);
extern FRESULT g_fs_res;
int get_file_size(char*filename)
{
	FILINFO fi;
	if (f_stat(filename,&fi)!=FR_OK)
		return 0;
	
	return fi.fsize;
}


uint8_t open_record_file(FATFS*pFs,FIL*pFil,char*filename,uint8_t bplay)
{
	extern FRESULT g_fs_res;
	if(g_bInitFSOK)
	{
		f_close(pFil);
		g_fs_res=f_open(pFil,filename,bplay?(FA_OPEN_ALWAYS|FA_READ):(FA_CREATE_ALWAYS|FA_WRITE));
		if(g_fs_res==FR_OK)
		{
			printf("open record file %s ok!\n",filename);
			return 1;
		}
		else
			printf("open record file %s error!\n",filename);
	}
	return 0;
		
}
void ui_record(void)
{
	if(0==tick)
	{
		sprintf(curFileName,"%s%d.rec",path,n_idx);
		tick=osKernelGetTickCount();
		
	}
	uint8_t bdot=0;
	if(osKernelGetTickCount( )%1000<700)
		bdot=1;
	if(STA_RECORD == g_wsta )
	  SetLEDS (0x02| (bdot?(0x01<<6):0));
	else if (STA_PLAY ==g_wsta)
		SetLEDS (0x02| (bdot?(0x01<<7):0));
	else
		SetLEDS (0x02);
		

	
	char str[40];
	GUI_Clear();
	GUI_SetFont (&GUI_FontHZ_SimSun_12);
//	GUI_DispStringHCenterAt("录音放音",S_W/2,0);
	sprintf(str,"#%d",n_idx);
	GUI_DispStringAt(str,0,0);
  GUI_DispStringAt(g_storageNames[g_currentStorage], S_W - 80, 0);
	GUI_SetFont (&GUI_FontHZ_SimSun_32);
	int nsec=0;
	if(g_wsta!=STA_RECORD)
	{
		int fsize=get_file_size(curFileName);
		nsec=fsize/16000;
		
	}
	else
		nsec=(osKernelGetTickCount()-rp_start_tick)/1000;
	sprintf(str,"%02d:%02d:%02d",nsec/3600,(nsec/60)%60,nsec%60);
//	printf("%s\n",str);
	GUI_DispStringHCenterAt(str,S_W/2,12);
	
	if(STA_PLAY ==g_wsta)
	{
		GUI_DrawHLine (50,0,S_W);
		GUI_DrawHLine (51,0,S_W);
		GUI_DrawHLine (52,0,S_W);
		int tnsec=(osKernelGetTickCount()-rp_start_tick)/1000;
		if(nsec>0)
		{
			int tx=tnsec*S_W/nsec;
			GUI_DrawRect(tx-2,49,tx+2,53);
		}
	}
	
	osSemaphoreAcquire(guiSemaHandle,100);
	GUI_Update();
	osSemaphoreRelease(guiSemaHandle);
	
	uint32_t key=osThreadFlagsWait (0xFFFF,osFlagsWaitAny,0);
	if (key!=K2_Pin)
		k2_tick=0;
	else if (k2_tick ==0)
		k2_tick =osKernelGetTickCount();
	
	switch(key){
		default:
			break;
		case K1_Pin://切换存储介质
			g_currentStorage = (g_currentStorage + 1) % 3;
      printf("切换到存储介质: %s\n", g_storageNames[g_currentStorage]);
      if (g_currentStorage ==0){
            g_pFil = &USERFile; 
			      g_pFs = &USERFatFS;
				    path=USERPath;
			}
      else if (g_currentStorage ==1){
            g_pFil = &SDFile;
			      g_pFs = &SDFatFS;
				    path=SDPath ;
			}
      else{
            g_pFil = &USBHFile;
				    g_pFs = &USBHFatFS;
				    path=USBHPath;
			}
			 if (InitFS(g_pFs, g_pFil, path) != FR_OK) {
        printf("介质初始化失败！\n");
        g_bInitFSOK = 0;
        } 
			 else {
        g_bInitFSOK = 1;
        }
    // 更新文件名
  		  sprintf(curFileName,"%s%d.rec",path,n_idx);
			break;
		case K2_Pin://长按删除文件，短按暂停录音或放音
			if(k2_tick >0 && osKernelGetTickCount()>k2_tick+2000)
			{
				//删除当前文件
				if (f_unlink (curFileName)==FR_OK)
				{
					printf("%s文件已删除\n",curFileName);
				}
				else
					printf("删除%s文件失败!\n",curFileName );
			}
			else
			{
				if (STA_RECORD==g_wsta  || STA_PLAY==g_wsta )
				{
					if(HAL_TIM_Base_GetState (&htim2)==HAL_TIM_STATE_READY)
					{
						HAL_TIM_Base_Start(&htim2);
						printf("继续录音/放音\n");
					}
					else
					{
					  HAL_TIM_Base_Stop(&htim2);
						printf("暂停录音/放音\n");
					}
				}
			}
			break;
		case K3_Pin://切换文件
			++n_idx;
		  if (n_idx>10)
				n_idx=1;
			sprintf(curFileName,"%s%d.rec",path,n_idx);
			break;
		case K4_Pin://返回主界面
			g_uista=UI_MAIN;
		  SetLEDS (0x01);
			break;
		case K5_Pin://开始结束录音
		  if(STA_READY==g_wsta )
			{
				if(open_record_file(g_pFs,g_pFil,curFileName,0))
				{
					HAL_ADC_Stop_DMA(&hadc1);
					printf("开始录音\n");
					g_wsta=STA_RECORD;
					StartSample();
					rp_start_tick =osKernelGetTickCount();
					
				}
				
			}
			else if(STA_RECORD==g_wsta)
			{
				g_wsta =STA_SAVE;
				
			}
			break;
		case K6_Pin://开始结束放音
			 if(STA_READY==g_wsta )
			{
				if(open_record_file(g_pFs,g_pFil,curFileName,1))
				{
					HAL_ADC_Stop_DMA(&hadc1);
					printf("开始放音\n6");
					g_wsta=STA_PLAY;
		      uint32_t br;
					f_read(g_pFil,(uint8_t*)g_dma_buff,MAX_DMA_BUFF_SIZE*2,&br);
					
					HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t*)g_dma_buff,MAX_DMA_BUFF_SIZE,DAC_ALIGN_12B_R);
					HAL_TIM_Base_Start(&htim2);
					
					rp_start_tick =osKernelGetTickCount();
					
				}
			
			 }
				else if(STA_PLAY==g_wsta)
			 {
				  f_close(g_pFil);
				  printf("停止放音\n");
				  HAL_TIM_Base_Stop(&htim2);
				  HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
				  g_wsta=STA_READY;
				
			}
			
			break;	
	}	
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) 
{
	
//		printf("采集缓冲满！\n");
	if(g_wsta !=STA_PLAY)
	{
		f_close(g_pFil);
		printf("停止放音\n");
		HAL_TIM_Base_Stop(&htim2);
		HAL_DAC_Stop_DMA(hdac,DAC_CHANNEL_1);
		g_wsta=STA_READY;	
	}
		else
		   osThreadFlagsSet(defaultTaskHandle, 0x02);
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
	if(g_wsta!=STA_PLAY)
	{
		f_close(g_pFil);
		printf("停止放音\n");
		HAL_TIM_Base_Stop(&htim2);
		HAL_DAC_Stop_DMA(hdac,DAC_CHANNEL_1);
		g_wsta=STA_READY;	
//  printf("采集到缓冲一半！\n");
	}
	else
		osThreadFlagsSet(defaultTaskHandle, 0x01);	
}

