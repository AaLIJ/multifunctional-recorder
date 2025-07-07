#include "ui_set_time.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "DS_18B20.h"
#include <cmsis_os.h>
#include "rtc.h"
#include "gpio.h"
#include "ssd1306.h"
#include "spwm_led.h" 

extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_24;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_48;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_12;

typedef enum {SET_YEAR,SET_MON,SET_DAY,SET_HOUR,SET_MIN,SET_SEC}SET_IDX;
static SET_IDX setidx=SET_YEAR;

void auto_check_datetime(void)
{
	uint8_t brn=0;
	
	if(g_rtc_time.Year>2099)
		g_rtc_time.Year=2000;
	if(g_rtc_time.Year<2000)
		g_rtc_time.Year=2099;
	if((g_rtc_time.Year%4==0&&g_rtc_time.Year%100!=0)||(g_rtc_time.Year%400==0))
		brn=1;
	if(g_rtc_time.Mon>12)
			g_rtc_time.Mon=1;
	if(g_rtc_time.Mon<1)
		g_rtc_time.Mon=12;
	
	uint8_t maxdays[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	maxdays[2]+=brn;
	if(g_rtc_time.Dat>maxdays[g_rtc_time.Mon])
		g_rtc_time.Dat=1;
	if(g_rtc_time.Dat<1)
		g_rtc_time.Dat=maxdays[g_rtc_time.Mon];
	if(g_rtc_time.Hour>24)
		g_rtc_time.Hour=23;
	else if(g_rtc_time.Hour>23)
		g_rtc_time.Hour=0;
	if(g_rtc_time.Min>60)
		g_rtc_time.Min=59;
	else if(g_rtc_time.Min>59)
		g_rtc_time.Min=0;
	if(g_rtc_time.Sec>60)
		g_rtc_time.Sec=59;
	else if(g_rtc_time.Sec>59)
		g_rtc_time.Sec=0;
		
}

void ui_set_time(void)
{
	char str[40];
	GUI_Clear();
	GUI_SetFont (&GUI_FontHZ_SimSun_12 );
	GUI_DispStringHCenterAt ("Ê±¼äÉèÖÃ",S_W/2,0);
	
	GUI_SetFont (&GUI_FontHZ_SimSun_24 );
	uint8_t bdot=(osKernelGetTickCount ()%1000)>700?1:0;
	if(SET_YEAR==setidx)
	{
		if(bdot)
			sprintf(str,"[%02d]-%2d-%2d",g_rtc_time.Year%100,g_rtc_time.Mon,g_rtc_time.Dat);
		else
			sprintf(str," %02d -%2d-%2d",g_rtc_time.Year%100,g_rtc_time.Mon,g_rtc_time.Dat);
	}
	else if(SET_MON==setidx)
	{
		if(bdot)
			sprintf(str,"%02d-[%2d]-%2d",g_rtc_time.Year%100,g_rtc_time.Mon,g_rtc_time.Dat);
		else
			sprintf(str,"%02d- %2d -%2d",g_rtc_time.Year%100,g_rtc_time.Mon,g_rtc_time.Dat);
	}
	else if(SET_DAY==setidx)
	{
		if(bdot)
			sprintf(str,"%02d-%2d-[%2d]",g_rtc_time.Year%100,g_rtc_time.Mon,g_rtc_time.Dat);
		else
			sprintf(str,"%02d-%2d- %2d ",g_rtc_time.Year%100,g_rtc_time.Mon,g_rtc_time.Dat);
	}
	else
			sprintf(str,"%02d-%2d- %2d ",g_rtc_time.Year%100,g_rtc_time.Mon,g_rtc_time.Dat);
	
	GUI_DispStringHCenterAt (str,S_W/2,14);
	
	if(SET_HOUR==setidx)
	{
		if(bdot)
			sprintf(str,"[%02d]:%2d:%2d",g_rtc_time.Hour,g_rtc_time.Min,g_rtc_time.Sec);
		else
			sprintf(str," %02d :%2d:%2d",g_rtc_time.Hour,g_rtc_time.Min,g_rtc_time.Sec);
	}
	else if(SET_MIN==setidx)
	{
		if(bdot)
			sprintf(str,"%02d:[%2d]:%2d",g_rtc_time.Hour,g_rtc_time.Min,g_rtc_time.Sec);
		else
			sprintf(str,"%02d: %2d :%2d",g_rtc_time.Hour,g_rtc_time.Min,g_rtc_time.Sec);
	}
	else if(SET_SEC==setidx)
	{
		if(bdot)
			sprintf(str,"%02d:%2d:[%2d]",g_rtc_time.Hour,g_rtc_time.Min,g_rtc_time.Sec);
		else
			sprintf(str,"%02d:%2d: %2d ",g_rtc_time.Hour,g_rtc_time.Min,g_rtc_time.Sec);
	}
	else
			sprintf(str,"%02d:%2d: %2d ",g_rtc_time.Hour,g_rtc_time.Min,g_rtc_time.Sec);
	
	GUI_DispStringHCenterAt (str,S_W/2,40);
	GUI_Update();
	
	
	uint32_t key=osThreadFlagsWait (0xFFFF,osFlagsWaitAny,0);
	switch(key)
	{
		default:
			break;
		case K1_Pin:
			switch(setidx)
			{
				default:
					break;
				case SET_YEAR:g_rtc_time.Year++;break;
				case SET_MON:g_rtc_time.Mon++;break;
				case SET_DAY:g_rtc_time.Dat++;break;
				case SET_HOUR:g_rtc_time.Hour++;break;
				case SET_MIN:g_rtc_time.Min++;break;
				case SET_SEC:g_rtc_time.Sec++;break;
			}
			break;
		case K4_Pin:
			switch(setidx)
			{
				default:
					break;
				case SET_YEAR:g_rtc_time.Year--;break;
				case SET_MON:g_rtc_time.Mon--;break;
				case SET_DAY:g_rtc_time.Dat--;break;
				case SET_HOUR:g_rtc_time.Hour--;break;
				case SET_MIN:g_rtc_time.Min--;break;
				case SET_SEC:g_rtc_time.Sec--;break;
			}
			break;
		case K2_Pin:
			if(setidx>SET_YEAR)
				--setidx;
			else
				setidx=SET_SEC;
			break;
		case K3_Pin:
			if(setidx<SET_SEC)
				++setidx;
			else
				setidx=SET_YEAR;
			break;
		
		case K5_Pin:
			SetRTCTime(g_rtc_time.Hour, g_rtc_time.Min, g_rtc_time.Sec); 
			SetRTCDate(g_rtc_time.Year, g_rtc_time.Mon, g_rtc_time.Dat);
			g_uista=UI_MAIN;

			break;
		case K6_Pin:
			g_uista=UI_MAIN;
			break;
	}
	if(key>0&&key<=0xFFFF)
	{
		auto_check_datetime();
	}
	
}