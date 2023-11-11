/*
 * Functions.h
 *
 *  Created on: Apr 8, 2022
 *      Author: Yurii Bondarenko
 */

#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define Digit_0 (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegD_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)
#define Digit_1 (1<<SegB_Pin)|(1<<SegC_Pin)
#define Digit_2 (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegD_Pin)|(1<<SegE_Pin)|(1<<SegG_Pin)
#define Digit_3 (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegD_Pin)|(1<<SegG_Pin)
#define Digit_4 (1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_5 (1<<SegA_Pin)|(1<<SegC_Pin)|(1<<SegD_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_6 (1<<SegC_Pin)|(1<<SegD_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_7 (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegC_Pin)
#define Digit_8 (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegD_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_9 (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_H (1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_L (1<<SegD_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)
#define Digit_Y (1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegD_Pin)|(1<<SegG_Pin)|(1<<SegF_Pin)
#define Digit_i (1<<SegC_Pin)
#define Digit_t (1<<SegD_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_S (1<<SegA_Pin)|(1<<SegC_Pin)|(1<<SegD_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_A (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_d (1<<SegB_Pin)|(1<<SegC_Pin)|(1<<SegE_Pin)|(1<<SegD_Pin)|(1<<SegG_Pin)
#define Digit_n (1<<SegC_Pin)|(1<<SegE_Pin)|(1<<SegG_Pin)
#define Digit_e (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegD_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_u (1<<SegC_Pin)|(1<<SegD_Pin)|(1<<SegE_Pin)
#define Digit_P (1<<SegA_Pin)|(1<<SegB_Pin)|(1<<SegE_Pin)|(1<<SegF_Pin)|(1<<SegG_Pin)
#define Digit_r (1<<SegE_Pin)|(1<<SegG_Pin)
#define Digit_minus (1<<SegG_Pin)
#define Digit_none

#define Num_H 10
#define Num_L 11
#define Num_Y 12
#define Num_i 13
#define Num_t 14
#define Num_S 15
#define Num_A 16
#define Num_d 17
#define Num_n 18
#define Num_minus 19
#define Num_none 20
#define Num_e 21
#define Num_u 22
#define Num_P 23
#define Num_r 24

#define Num_HH 101020
#define Num_LL 111120
#define Num_HYS 101215
#define Num_ti 201413
#define Num_Set 152114
#define Num_Add 161717
#define Num_nuL 182211
#define Num_sens 141223
#define Num_PID_out 231317
#define Num_Kp 202023
#define Num_Ki 202013
#define Num_Kd 202017
#define	Num_Pst 231529
#define	Num_Pdr 231724

#define ClearBit(A,k)   (A &= ~(1 << k))
#define SetBit(A,k)    (A |= (1 << k))
#define TestBit(A,k) ((A >> k) & 1)
#define ToggleBit(A,k)  ( A ^= 1 << k)

#define ADC_1 1
#define ADC_2 2

#include "main.h"

// обработка кнопок
// регулятор дискретный
typedef struct
{
	uint8_t out; /* выход регулятора */
	uint8_t en; /* разрешение */
	int16_t in; /*входная величина*/
	int16_t set; /*задание*/
	uint16_t hyst; /*гистерезис*/
	uint32_t min_time; /*минимальный сигнал в mс*/
	uint32_t time_privat; /*время вызова мс*/
} discr_reg_DataType;

// ошибки
typedef struct
{
	uint8_t code; // код ошибки
	int16_t T1_H; /* перегрев Т1 */
	int16_t T2_H; /* перегрев Т2 */
	int16_t T3_H; /* перегрев Т3 */
	int16_t T4_H; /* перегрев Т4 */
	int16_t T1_memory_privat; /*сохраненное значение Т1*/
	int16_t T2_memory_privat; /*сохраненное значение Т2*/
	int16_t T3_memory_privat; /*сохраненное значение Т3*/
	int16_t T4_memory_privat; /*сохраненное значение Т4*/
	int16_t R_trig_privat; /*триггер*/
	uint32_t dtime_privat; /*время сравнение прироста температуры мс*/
	uint32_t etime_privat; /*время длительности ошибки мс*/
	uint32_t time_privat[16];//*время для сохранения ошибки мс*/
} error_DataType;

typedef struct
{
	uint8_t Set_hyst;
	uint8_t Set_delay;
	uint8_t Set_sens_type;
	int16_t Set_add;
	int16_t Set_mul;
	int16_t Set_temp;

} Flash_DataType;

char* int32_to_string(int32_t value);
char* uint32_to_string(uint32_t value);
void discr_reg (discr_reg_DataType*);
void FLASH_Write (uint32_t *data, uint32_t adres, uint16_t length);
int16_t limit (int16_t data, int16_t min, int16_t max);
int32_t ADC_read (uint8_t func, int32_t chanel, uint32_t width, uint32_t atten, uint8_t messure);
uint32_t median (uint32_t newVal);
int32_t filter (int32_t in, uint32_t coeff_A, uint32_t coeff_k );
/*коэффициенты выбираются так:
k = 1, 2, 3…
A = (2^k) – 1
[k=2 A=3], [k=3 A=7], [k=4 A=15], [k=5 A=31]…
*/
uint8_t timer(uint32_t *var, uint16_t time_off, uint16_t time_on );
void menu_string (void *p_var, const int16_t LLimit, const int16_t HLimit, int32_t Num_display, key_DataType key_str_func);
#endif /* INC_FUNCTIONS_H_ */
