/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "DS_18B20.h"
#include "max9814.h"
#include "GUI.h"
#include "ui_start.h"
#include "ui_main.h"
#include "ui_set_time.h"
#include "ui_set_para.h"
#include "beep.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fatfs.h"
#include "w25qxx.h"
#include "ui_record.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
 BYTE work[SECTOR_SIZE]; // 格式化工作缓冲区
 uint8_t g_bInitFSOK=0;
 extern FIL *g_pFil;//指针地址切换
 extern FATFS *g_pFs;
 extern char *path;
 FRESULT g_fs_res = FR_OK;
//extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_12;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for guiTask */
osThreadId_t guiTaskHandle;
const osThreadAttr_t guiTask_attributes = {
  .name = "guiTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for keyTask */
osThreadId_t keyTaskHandle;
const osThreadAttr_t keyTask_attributes = {
  .name = "keyTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for uartTask */
osThreadId_t uartTaskHandle;
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for OtherTask */
osThreadId_t OtherTaskHandle;
const osThreadAttr_t OtherTask_attributes = {
  .name = "OtherTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/*Definitions for guiSema*/
osSemaphoreId_t guiSemaHandle;
const osSemaphoreAttr_t guiSema_attributes={
	.name="guiSema"
};
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
FRESULT InitFS(FATFS *pfs, FIL *pfil, char *path);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskGui(void *argument);
void StartTaskKey(void *argument);
void StartTaskUart(void *argument);
void StartTaskOther(void *argument);

extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	guiSemaHandle=osSemaphoreNew(1,1,&guiSema_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

//  /* creation of guiTask */
  guiTaskHandle = osThreadNew(StartTaskGui, NULL, &guiTask_attributes);

//  /* creation of keyTask */
  keyTaskHandle = osThreadNew(StartTaskKey, NULL, &keyTask_attributes);

//  /* creation of uartTask */
  uartTaskHandle = osThreadNew(StartTaskUart, NULL, &uartTask_attributes);

//  /* creation of OtherTask */
  OtherTaskHandle = osThreadNew(StartTaskOther, NULL, &OtherTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_HOST */
  MX_USB_HOST_Init();
//  /* USER CODE BEGIN StartDefaultTask */
//  /* Infinite loop */
//	StartSample();
	osDelay(100);
	ds18b20_init();
	osDelay(10);
	ds18b20_read();
	retUSER=InitFS(g_pFs,g_pFil,path);
	
		uint32_t temp_tick = 0;
	uint16_t *pbuff1=(uint16_t*)g_dma_buff;
	uint16_t *pbuff2=(uint16_t*)(g_dma_buff+(MAX_DMA_BUFF_SIZE/2));
	uint32_t rwsize=0;
  for(;;)
  {
		uint32_t ctick = osKernelGetTickCount();
		if (ctick >= temp_tick)
		{
			temp_tick = ctick + 1000;
			ds18b20_read();
		}
		
		uint32_t flag = osThreadFlagsWait(0x07, osFlagsWaitAny, 0);
		if (flag > 0 && flag <=0x07)
		{
			if(STA_RECORD==g_wsta || STA_SAVE==g_wsta)
			{
				uint32_t bw;
				osSemaphoreAcquire(guiSemaHandle,100);
				retUSER=f_write(g_pFil,(uint8_t*)((flag&0x01)?pbuff1:pbuff2),MAX_DMA_BUFF_SIZE,&bw);
				osSemaphoreRelease(guiSemaHandle);
				rwsize+=MAX_DMA_BUFF_SIZE;
	      if(retUSER!=FR_OK || bw!=MAX_DMA_BUFF_SIZE)
				{
					f_close(g_pFil);
					g_wsta=STA_READY;
					printf("write record file error!%d\n",retUSER);
				}
				else 
					printf("write record file size:%d\n",rwsize);
				if(STA_SAVE ==g_wsta)
				{
					g_wsta=STA_READY ;
					f_close (g_pFil);
					printf("停止录音!\n");
					rwsize=0;
				}
			}
			else if(STA_PLAY==g_wsta)
			{
				uint32_t br;
				osSemaphoreAcquire(guiSemaHandle,100);
				retUSER=f_read(g_pFil,(uint8_t*)((flag&0x01)?pbuff1:pbuff2),MAX_DMA_BUFF_SIZE,&br);
				osSemaphoreRelease(guiSemaHandle);
				rwsize+=MAX_DMA_BUFF_SIZE;
				if(retUSER!=FR_OK || br==0)
				{
					f_close(g_pFil);
					g_wsta=STA_READY;
					printf("read record file error!%d\n",retUSER);
				}
				else 
					printf("read record file size:%d\n",rwsize);
				
			}
		}
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskGui */
/**
* @brief Function implementing the guiTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskGui */
void StartTaskGui(void *argument)
{
  /* USER CODE BEGIN StartTaskGui */
  /* Infinite loop */
	osDelay(500);
	GUI_Init();
  for(;;)
  {
		switch(g_uista)
		{
			default:
			case UI_START:
				ui_start();
				break;
			case UI_MAIN:
				ui_main();
				break;
			case UI_RECORD:
				ui_record();
				break;
			case UI_SET_TIME:
				ui_set_time();
				break;
			case UI_SET_PARA:
				ui_set_para();
				break;
		}
    osDelay(1);
  }
  /* USER CODE END StartTaskGui */
}

/* USER CODE BEGIN Header_StartTaskKey */
/**
* @brief Function implementing the keyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskKey */
void StartTaskKey(void *argument)
{
  /* USER CODE BEGIN StartTaskKey */
  /* Infinite loop */
	uint16_t key;
  for(;;)
  {
		key = KeyScan();
		if(key>0)
		{
			osThreadFlagsSet (guiTaskHandle ,key);
		}
		
    osDelay(10);
  }
  /* USER CODE END StartTaskKey */
}

/* USER CODE BEGIN Header_StartTaskUart */
/**
* @brief Function implementing the uartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUart */
void StartTaskUart(void *argument)
{
  /* USER CODE BEGIN StartTaskUart */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskUart */
}

/* USER CODE BEGIN Header_StartTaskOther */
/**
* @brief Function implementing the OtherTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskOther */
void StartTaskOther(void *argument)
{
  /* USER CODE BEGIN StartTaskOther */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskOther */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
FRESULT InitFS(FATFS *g_pFs, FIL *g_pFil, char *path) {

 char filename[128]; // 文件路径名
 FRESULT g_fs_res; // 文件操作结果
 osDelay(100); // 延时等待外设稳定
 g_fs_res= f_mount(g_pFs, path, 1); // 立即挂载文件系统
	if(g_fs_res == FR_NO_FILESYSTEM){ // 重新格式化FLASH
	 printf("Disk Formatting...\n");// 开始格式化FLASH
	 g_fs_res = f_mkfs(path, FM_ANY, 0, work, sizeof(work));
	 if (g_fs_res != FR_OK) {
		 printf("mkfs error.\n");
		 g_bInitFSOK=0;
		 return g_fs_res; // 格式化错误，直接返回
	 }
	 g_fs_res=f_mount(g_pFs,path,1);
 }
	if (g_fs_res == FR_OK) // 打印初始化结果
	{
		printf("FATFS Init ok!\n");
		g_bInitFSOK=1;
	}
 else {
	 printf("FATFS Init error%d\n",g_fs_res);
	 g_bInitFSOK=0;
 }
 return g_fs_res; // 初始化失败，直接返回
 }
/* USER CODE END Application */

