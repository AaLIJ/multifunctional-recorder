#include "ui_start.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "DS_18B20.h"
#include <cmsis_os.h>
#include "rtc.h"
#include "gpio.h"
#include "ssd1306.h"
#include "spwm_led.h" 
#include "ui_set_para.h"

extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_24;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_48;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_12;

static char WeekStr[][4]={"天","一","二","三","四","五","六"};
static uint32_t read_tick=0;
static uint32_t key_tick=0;

extern void SystemClock_Config(void);
extern osThreadId_t defaultTaskHandle;
extern osSemaphoreId_t guiSemaHandle;
void EnterStopMode(void);//进入停机模式

void ui_main(void)
{
	char str[40];
	uint8_t bdot=0;
	if(osKernelGetTickCount()>=read_tick)
	{
		read_tick =osKernelGetTickCount ()+100;
		ReadRTCDateTime();
	}
	if(g_rtc_time.PSec <70)
		bdot=1;

	GUI_Clear();
	//显示时间小时数、分钟数
	GUI_SetFont (&GUI_FontHZ_SimSun_48  );
	if(bdot)
		sprintf(str,"%02d:%02d",g_rtc_time.Hour,g_rtc_time.Min);
	else
		sprintf(str,"%02d %02d",g_rtc_time.Hour,g_rtc_time.Min);
	GUI_DispStringHCenterAt(str,S_W/2,8);
	
	GUI_SetFont (&GUI_FontHZ_SimSun_12  );
	//显示当前温度
	sprintf(str,"%.1f℃",g_temp);
	GUI_DispStringAt (str,0,0);
	
	//显示日期信息
	sprintf(str,"%02d年%02d月%02d日 星期%s",g_rtc_time.Year,g_rtc_time.Mon,g_rtc_time.Dat,WeekStr[g_rtc_time.Week]);
	GUI_DispStringHCenterAt(str,S_W/2,S_H-12);
	osSemaphoreAcquire(guiSemaHandle,100);
	GUI_Update();
	osSemaphoreRelease(guiSemaHandle);
	uint32_t key=osThreadFlagsWait (0xFFFF,osFlagsWaitAny,0);
	
  switch(key)
	{
		default:
			break;
		case K1_Pin:
			g_uista=UI_SET_TIME;
			key_tick =0;
			return;
		case K3_Pin:
			g_uista=UI_RECORD;
			key_tick =0;
			return;
		case K4_Pin:
			g_uista=UI_SET_PARA;
			key_tick =0;
			return;
	}
  if (key>0 && key<0xFFFF)
		  key_tick=osKernelGetTickCount();
  if (key_tick==0)
		  key_tick=osKernelGetTickCount();
	if(osKernelGetTickCount()>=key_tick +g_set.into_sleep_sec *1000)
	{
		EnterStopMode ();
		key_tick=osKernelGetTickCount();
		osDelay(100);
		osThreadFlagsClear(0xFFFF);
	}
}


void EnterStopMode(void){//进入停机模式
	SSD1306_OFF();//OLED熄灭
	spwm_led_switch_light(0);//渐灭
	osDelay(3000);
	//SetLEDS(0);//LED灯熄灭
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//关闭SysTick
	HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_STOPENTRY_WFE);
}

void ReInitSysClock(void){
	SysTick->CTRL |=SysTick_CTRL_ENABLE_Msk;//恢复SysTick
	SystemClock_Config();//恢复CPU时钟
	HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_A );//关闭RTC闹钟
	
	spwm_led_switch_light(80);//渐亮
	//SetLEDS(0x01);//LED灯点亮
	SSD1306_ON();//OLED打开
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) { // 按键对应的外部中断回调函数
 ReInitSysClock(); 
}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) { // RTC闹铃中断回调函数
	ReInitSysClock(); 
}