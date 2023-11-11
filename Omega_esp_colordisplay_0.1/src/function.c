//
// Functions.c
// 
//   Created on: Apr 8, 2020
//       Author: Bondarenko Yrii

#include "functions.h"
#include "esp_adc/adc_cali.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/adc.h"

// Функция для конвертации int32_t в строку символов без sprintf
char* int32_to_string(int32_t value) {
    // Определяем максимальную длину строки для 32-битного числа
    int max_length = 12; // "-2147483648" имеет 11 символов, плюс нуль-терминатор

    // Выделяем память для строки
    char* str = (char*)malloc(max_length * sizeof(char));
    if (str == NULL) {
        return NULL; // В случае ошибки выделения памяти
    }

    // Индекс для записи символов
    int index = 0;

    // Проверка и обработка отрицательных чисел
    if (value < 0) {
        str[index++] = '-';
        value = -value;
    }

    // Конвертация числа в строку (с конца к началу)
    do {
        str[index++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    // Разворачиваем строку, чтобы получить правильный порядок цифр
    int start = (str[0] == '-') ? 1 : 0;
    int end = index - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

    // Добавляем нуль-терминатор
    str[index] = '\0';

    return str;
}

// Функция для конвертации uint32_t в строку символов без sprintf
char* uint32_to_string(uint32_t value) {
    // Определяем максимальную длину строки для 32-битного числа
    int max_length = 12; // "-2147483648" имеет 11 символов, плюс нуль-терминатор

    // Выделяем память для строки
    char* str = (char*)malloc(max_length * sizeof(char));
    if (str == NULL) {
        return NULL; // В случае ошибки выделения памяти
    }

    // Индекс для записи символов
    int index = 0;

    // Конвертация числа в строку (с конца к началу)
    do {
        str[index++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    // Разворачиваем строку, чтобы получить правильный порядок цифр
    int start = (str[0] == '-') ? 1 : 0;
    int end = index - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

    // Добавляем нуль-терминатор
    str[index] = '\0';

    return str;
}

void discr_reg (discr_reg_DataType *reg)
{
	if (reg->en!=0)
	{

		if ((reg->in > (reg->set + reg->hyst)) && xTaskGetTickCount() > reg->time_privat + reg->min_time)
			{
			reg->out = 0;
			}

		if (reg->in < (reg->set - reg->hyst))
			{
			reg->out = 1;
			reg->time_privat = xTaskGetTickCount();
			}
	}
	else reg->out = 0;
}


int16_t limit (int16_t data, int16_t min, int16_t max)
{
	if (data < min) return min;
	if (data > max) return max;
	return data;
}

// медиана на 3 значения со своим буфером
uint32_t median(uint32_t newVal)
{
  static int buf[3];
  static uint8_t count = 0;
  buf[count] = newVal;
  if (++count >= 3) count = 0;
  return (max(buf[0], buf[1]) == max(buf[1], buf[2])) ? max(buf[0], buf[2]) : max(buf[1], min(buf[0], buf[2]));
}

int32_t filter (int32_t in, uint32_t coeff_A, uint32_t coeff_k )
{
	static uint32_t filt;
	filt = (coeff_A * filt + in) >> coeff_k;
	return filt;
}

int32_t ADC_read (uint8_t func, int32_t chanel, uint32_t width, uint32_t atten, uint8_t messure )
{
	int read_raw;
	if (func == ADC_2)
	{
		adc2_config_channel_atten(chanel,atten);
		adc2_get_raw(chanel, width, &read_raw);
    }
    else
	{
		adc1_config_width (width);
		adc1_config_channel_atten(chanel,atten);

		read_raw = adc1_get_raw(chanel);
    }

	if (!messure)
		{
			return (read_raw*2750/4095);
		}
	return (read_raw);
	
}

uint8_t timer(uint32_t *var, uint16_t time_off, uint16_t time_on ) // таймер циклический
{
	if (*var == 0) //запись начального значения
			{
				*var = xTaskGetTickCount();
				return 1;// возврат 1
			}
	if (xTaskGetTickCount() > (*var + (uint32_t)time_on)) // глобальный счетчик больше чем время включения + последнее сохраненное время
		{
		if ((xTaskGetTickCount() > (*var +(uint32_t)time_on + (uint32_t)time_off))) *var = xTaskGetTickCount(); // выполнить условие если глобальный счетчик чем последнее сохраненное время +время включения+ время выключения
		return 0; // возврат 0
		}
	return 1; // возврат 1
}

void menu_string (void *p_var, const int16_t LLimit, const int16_t HLimit, int32_t Num_display, key_DataType key_str_func)
{
		//extern Number_disp;
		//extern uint8_t edit_string;
		extern QueueHandle_t DisplayName_xQueue;
		extern QueueHandle_t DisplayDot_xQueue;
		extern SemaphoreHandle_t Run_Name_blink;
		extern uint8_t Send_dot[3];
		int32_t Num_Queuesend;
	//	printf ("pvar = %i \n", (*(int16_t*) p_var));
		if (key_str_func.key_right_rise) (*(int16_t*) p_var)++;
		if (key_str_func.key_left_rise) (*(int16_t*) p_var)--;
        if (key_str_func.key_left_time > 1000) (*(int16_t*) p_var)--; // кнопка зажата
		if (key_str_func.key_right_time > 1000) (*(int16_t*) p_var)++; // кнопка зажата
		if ((*(int16_t*) p_var) <= LLimit) (*(int16_t*) p_var)++;
		if ((*(int16_t*) p_var) >= HLimit) (*(int16_t*) p_var)--;
		//if (key_str_func.key_set_rise) edit_string++;
		Num_Queuesend = *(int16_t*) p_var;
	//	printf ("Array_Queuesend0 = %i \n", Num_display );
	//	printf ("Array_Queuesend1 = %i \n", Num_Queuesend );
	//	xQueueReset(DisplayName_xQueue);
		xQueueSend(DisplayName_xQueue,( void *) &Num_display, (TickType_t) 0);
		xQueueSend(DisplayName_xQueue,( void *) &Num_Queuesend, (TickType_t) 0);
		xQueueReset(DisplayDot_xQueue);
        xQueueSend(DisplayDot_xQueue,( void *) &Send_dot, (TickType_t) 0);
		xSemaphoreGive(Run_Name_blink); // отправляем семафор


}