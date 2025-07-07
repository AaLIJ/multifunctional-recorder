/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MAX9814_GAIN_GPIO_Port, MAX9814_GAIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, L1_Pin|L2_Pin|L3_Pin|L4_Pin
                          |L5_Pin|L6_Pin|L7_Pin|L8_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TEMP_GPIO_Port, TEMP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : K2_Pin K3_Pin K4_Pin K1_Pin */
  GPIO_InitStruct.Pin = K2_Pin|K3_Pin|K4_Pin|K1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : K5_Pin K6_Pin */
  GPIO_InitStruct.Pin = K5_Pin|K6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : MAX9814_GAIN_Pin */
  GPIO_InitStruct.Pin = MAX9814_GAIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAX9814_GAIN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : L1_Pin L2_Pin L3_Pin L4_Pin
                           L5_Pin L6_Pin L7_Pin L8_Pin
                           TEMP_Pin */
  GPIO_InitStruct.Pin = L1_Pin|L2_Pin|L3_Pin|L4_Pin
                          |L5_Pin|L6_Pin|L7_Pin|L8_Pin
                          |TEMP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */
void SetLEDS(uint8_t sta)
{
	HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, (sta & 0x01) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, (sta & 0x02) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L3_GPIO_Port, L3_Pin, (sta & 0x04) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L4_GPIO_Port, L4_Pin, (sta & 0x08) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L5_GPIO_Port, L5_Pin, (sta & 0x10) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L6_GPIO_Port, L6_Pin, (sta & 0x20) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L7_GPIO_Port, L7_Pin, (sta & 0x40) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L8_GPIO_Port, L8_Pin, (sta & 0x80) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

uint16_t KeyScan(void)
{
	static uint16_t oldkey = 0x00;
	static uint16_t keycnt = 0;
	static uint32_t key_tick = 0;
	uint16_t key = 0;
	key |= (HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_RESET) ? K1_Pin : 0;
	key |= (HAL_GPIO_ReadPin(K2_GPIO_Port, K2_Pin) == GPIO_PIN_RESET) ? K2_Pin : 0;
	key |= (HAL_GPIO_ReadPin(K3_GPIO_Port, K3_Pin) == GPIO_PIN_RESET) ? K3_Pin : 0;
	key |= (HAL_GPIO_ReadPin(K4_GPIO_Port, K4_Pin) == GPIO_PIN_RESET) ? K4_Pin : 0;
	key |= (HAL_GPIO_ReadPin(K5_GPIO_Port, K5_Pin) == GPIO_PIN_SET) ? K5_Pin : 0;
	key |= (HAL_GPIO_ReadPin(K6_GPIO_Port, K6_Pin) == GPIO_PIN_SET) ? K6_Pin : 0;
	
	if (key == oldkey)
		++keycnt;
	else
		keycnt = key_tick = 0;
	oldkey = key;
	
	if (1 == keycnt || (keycnt > 1 && HAL_GetTick() > key_tick))
	{
		key_tick = HAL_GetTick() + ((1 == keycnt) ? 500 : 20);
		return key;
	}

	return 0;
}

/* USER CODE END 2 */
