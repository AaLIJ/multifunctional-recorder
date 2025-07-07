/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.h
  * @brief   This file contains all the function prototypes for
  *          the rtc.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN Private defines */
typedef struct {
	uint16_t Year;	// 年
	uint8_t Mon;		// 月
	uint8_t Dat;		// 日
	uint8_t Week;		// 星期（0：天，1～6：一～六）
	uint8_t Hour;		// 时
	uint8_t Min;		// 分
	uint8_t Sec;		// 秒
	uint8_t PSec; 	// 百分秒
} RTC_DT;

extern RTC_DT g_rtc_time;		// RTC日期时间

/* USER CODE END Private defines */

void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */
HAL_StatusTypeDef ReadRTCDateTime(void);			// 日期时间读取函数，结果保存在g_rtc_time
HAL_StatusTypeDef SetRTCTime(int hour, int min, int sec);	// 时间设置
HAL_StatusTypeDef SetRTCDate(int year, int mon, int date);	// 日期设置

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */

