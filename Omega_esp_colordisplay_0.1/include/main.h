
/* USER CODE BEGIN Header */
/*
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "driver/gpio.h"
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H


/* Private defines -----------------------------------------------------------*/

#define KeyDown GPIO_NUM_39
#define KeyLeft GPIO_NUM_36
#define KeyRight GPIO_NUM_35
#define KeyUp GPIO_NUM_34
#define LED GPIO_NUM_33
#define ModbusTerminal GPIO_NUM_32
//#define PWM_Pin GPIO_NUM_27
//#define Dig2_Pin GPIO_NUM_26
#define DC GPIO_NUM_25
#define Temp_2 GPIO_NUM_23
#define I2C_SCL GPIO_NUM_22
#define I2C_SDA GPIO_NUM_21
#define Temp_3 GPIO_NUM_19
#define Temp_4 GPIO_NUM_18
#define Relay2 GPIO_NUM_17
#define Relay3 GPIO_NUM_16
#define WP GPIO_NUM_15
#define SPI_SCLK GPIO_NUM_14
#define SPI_MOSI GPIO_NUM_13
#define SPI_MISO GPIO_NUM_12
#define Relay1 GPIO_NUM_5
#define Relay4 GPIO_NUM_4
#define RXD GPIO_NUM_4
#define Temp_1 GPIO_NUM_2
#define TXD GPIO_NUM_2

#define uint32_t  unsigned int
#define uint16_t  unsigned short
#define uint8_t   unsigned char
#define int32_t   int
#define int16_t   short


#define GPIO_PIN_SET 1
#define GPIO_PIN_RESET 0

#define ADC_MV 0
#define ADC_RAW 1


#define TIME_TO_EDIT_EXIT 1500 // время выхода из режима редактирования

// обработка кнопок
typedef struct
{
	uint32_t key_up; /* кнопка вверх */
  uint32_t key_down; /* кнопка вниз */
	uint32_t key_left; /* кнопка влево */
	uint32_t key_right; /* кнопка вправо */
	uint32_t time_privat[8]; /*время нажатия мс*/
	uint32_t R_trig_privat;
	uint32_t key_up_time; /* время нажатия */
  uint32_t key_down_time; /* время нажатия */
	uint32_t key_left_time; /*время нажатия*/
	uint32_t key_right_time; /*время нажатия*/
	uint8_t key_up_fall; /* отпускание кнопки */
  uint8_t key_down_fall; /* отпускание кнопки */
	uint8_t key_left_fall; /* отпускание кнопки */
	uint8_t key_right_fall; /*отпускание кнопки*/
	uint8_t key_up_rise; /* нажатие кнопки */
  uint8_t key_down_rise; /* нажатие кнопки */
	uint8_t key_left_rise; /* нажатие кнопки */
	uint8_t key_right_rise; /*нажатие кнопки*/
	uint8_t any_key; /*нажата любая кнопка*/
} key_DataType;

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
