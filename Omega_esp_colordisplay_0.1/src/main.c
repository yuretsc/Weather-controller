#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "main.h"
//#include "driver/adc.h"
#include "esp_adc/adc_cali.h"
#include "freertos/queue.h"
//#include "temperature.h"
#include "nvs_flash.h"
#include "nvs.h"
//#include "PID.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "pcf8563.h"
#include "st7789.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_log.h"
#include "functions.h"

#define CONFIG_WIDTH  240
#define CONFIG_HEIGHT 280
#define CONFIG_OFFSETX 0
#define CONFIG_OFFSETY 20
#define CONFIG_MOSI_GPIO SPI_MOSI
#define CONFIG_SCLK_GPIO SPI_SCLK
#define CONFIG_CS_GPIO I2C_SCL
#define CONFIG_DC_GPIO DC
#define CONFIG_RESET_GPIO 26
#define CONFIG_BL_GPIO LED
#define INTERVAL 10


    uint16_t xpos_=0;
	uint16_t ypos_=0;

static const char *TAG = "ST7789";

static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}





//QueueHandle_t Display_xQueue; // очередь для обновления дисплея
//QueueHandle_t Key_read_xQueue; // очередь для считывания клавиатуры
//QueueHandle_t Print_xQueue; // очередь для печати в консоль
key_DataType key_str; // структура для обработчика кнопок


void ArrowTest(TFT_t * dev, FontxFile *fx, int width, int height) {


	// get font width & height
	uint8_t buffer[FontxGlyphBufSize];
	uint8_t fontWidth;
	uint8_t fontHeight;
	GetFontx(fx, 0, buffer, &fontWidth, &fontHeight);
	//ESP_LOGI(__FUNCTION__,"fontWidth=%d fontHeight=%d",fontWidth,fontHeight);
	
	uint16_t xpos;
	uint16_t ypos;

	int	stlen;
	uint8_t ascii[24];
	uint16_t color;

	lcdFillScreen(dev, BLACK);


	strcpy((char *)ascii, uint32_to_string((int32_t)xTaskGetTickCount()));
	if (width < height) {
		xpos = ((width - fontHeight) / 2) - 1;
		ypos = (height - (strlen((char *)ascii) * fontWidth)) / 2;
		lcdSetFontDirection(dev, DIRECTION90);
	} else {
		ypos = ((height - fontHeight) / 2) - 1;
		xpos = (width - (strlen((char *)ascii) * fontWidth)) / 2;
		lcdSetFontDirection(dev, DIRECTION0);
	}
	color = WHITE;
	lcdDrawString(dev, fx, xpos, ypos, ascii, color);

	lcdSetFontDirection(dev, 0);
	color = RED;
	lcdDrawFillArrow(dev, 10, 10, 0, 0, 5, color);
	strcpy((char *)ascii, "0,0");
	lcdDrawString(dev, fx, 0, 30, ascii, color);

	color = GREEN;
	lcdDrawFillArrow(dev, width-11, 10, width-1, 0, 5, color);
	//strcpy((char *)ascii, "79,0");
	sprintf((char *)ascii, "%d,0",width-1);
	stlen = strlen((char *)ascii);
	xpos = (width-1) - (fontWidth*stlen);
	lcdDrawString(dev, fx, xpos, 30, ascii, color);

	color = GRAY;
	lcdDrawFillArrow(dev, 10, height-11, 0, height-1, 5, color);
	//strcpy((char *)ascii, "0,159");
	sprintf((char *)ascii, "0,%i",key_str.key_up);
	ypos = (height-11) - (fontHeight) + 5;
	lcdDrawString(dev, fx, 0, ypos, ascii, color);

	color = CYAN;
	lcdDrawFillArrow(dev, width-11, height-11, width-1, height-1, 5, color);
	//strcpy((char *)ascii, "79,159");
	sprintf((char *)ascii, "%u,%u",xpos_, ypos_);
	stlen = strlen((char *)ascii);
	xpos = (width-1) - (fontWidth*stlen);
	lcdDrawString(dev, fx, xpos, ypos, ascii, color);

    color = YELLOW;
    if (key_str.key_down_rise>0) ypos_++;
    if (key_str.key_up_rise>0) ypos_--;
    if (key_str.key_left_rise>0) xpos_--;
    if (key_str.key_right_rise>0) xpos_++;
    if (key_str.key_down>10) ypos_ += 3;
    if (key_str.key_up>10) ypos_ -= 3;
    if (key_str.key_left>10) xpos_ -= 3;
    if (key_str.key_right>10) xpos_ += 3;   
    if (xpos_>CONFIG_WIDTH) xpos_=CONFIG_WIDTH-10;
    if (ypos_>CONFIG_HEIGHT) ypos_=CONFIG_HEIGHT-10;
    lcdDrawFillRect(dev,xpos_,ypos_,xpos_+10,ypos_+10,color);
	lcdDrawFinish(dev);
  //  printf("xpos - %u \n", xpos_); 
  //  printf("ypos - %u \n", ypos_);
   // if (key_str.key_left_rise>0) printf("left");
}

void display(void *pvParameters)
{
	TFT_t dev;
	FontxFile fx16G[2];
	InitFontx(fx16G,"/spiffs/ILGH16XB.FNT",""); // 8x16Dot Gothic
	spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
	lcdInit(&dev, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);

	while(1) {

		ArrowTest(&dev, fx16G, CONFIG_WIDTH, CONFIG_HEIGHT);
		vTaskDelay(INTERVAL);

	} // end while
}


void print(void *pvParameter)
{
    for (;;)
    {
        printf("key down - %i \n", key_str.key_down);
        printf("key up - %i \n", key_str.key_up);
        printf("key left - %i \n", key_str.key_left);
        printf("key right - %i \n", key_str.key_right);
        vTaskDelay(10);
    }
}

void key_read(void *pvParameter)
{
    for (;;)
    {
        if (key_str.key_down_rise == 1)
            key_str.key_down_rise = 0; // сброс статуса фронта нажатия
        if (key_str.key_down_fall == 1)
            key_str.key_down_fall = 0; // сброс статуса спада нажатия
        // начало события, отсчет от первого фронта
        if (gpio_get_level(KeyDown) && key_str.key_down_time == 0)
        {
            key_str.key_down_time = xTaskGetTickCount();
            key_str.key_down = gpio_get_level(KeyDown);
        }
        // проверка состояния контакта через время дребезга контактов 50мс
        else if (gpio_get_level(KeyDown) && xTaskGetTickCount() > key_str.key_down_time + 50)
        {
            key_str.key_down = (xTaskGetTickCount() - key_str.key_down_time) / 10; // сохраняем время нажатой кнопки в мс*10
            key_str.key_down_rise = 1;                                             // формирование статуса фронт сигнала
        }
        else if (gpio_get_level(KeyDown) == 0 && key_str.key_down != 0)
        {
            key_str.key_down_time = 0;
            key_str.key_down = 0;
            key_str.key_down_fall = 1;
        }

        if (key_str.key_up_rise == 1)
            key_str.key_up_rise = 0; // сброс статуса фронта нажатия
        if (key_str.key_up_fall == 1)
            key_str.key_up_fall = 0; // сброс статуса спада нажатия
        // начало события, отсчет от первого фронта
        if (gpio_get_level(KeyUp) && key_str.key_up_time == 0)
        {
            key_str.key_up_time = xTaskGetTickCount();
            key_str.key_up = gpio_get_level(KeyUp);
        }
        // проверка состояния контакта через время дребезга контактов 50мс
        else if (gpio_get_level(KeyUp) && xTaskGetTickCount() > key_str.key_up_time + 50)
        {
            key_str.key_up = (xTaskGetTickCount() - key_str.key_up_time) / 10; // сохраняем время нажатой кнопки в мс*10
            key_str.key_up_rise = 1;                                           // формирование статуса фронт сигнала
        }
        else if (gpio_get_level(KeyUp) == 0 && key_str.key_up != 0)
        {
            key_str.key_up_time = 0;
            key_str.key_up = 0;
            key_str.key_up_fall = 1;
        }

        if (key_str.key_left_rise == 1)
            key_str.key_left_rise = 0; // сброс статуса фронта нажатия
        if (key_str.key_left_fall == 1)
            key_str.key_left_fall = 0; // сброс статуса спада нажатия
        // начало события, отсчет от первого фронта
        if (gpio_get_level(KeyLeft) && key_str.key_left_time == 0)
        {
            key_str.key_left_time = xTaskGetTickCount();
            key_str.key_left = gpio_get_level(KeyLeft);
        }
        // проверка состояния контакта через время дребезга контактов 50мс
        else if (gpio_get_level(KeyLeft) && xTaskGetTickCount() > key_str.key_left_time + 50)
        {
            key_str.key_left = (xTaskGetTickCount() - key_str.key_left_time) / 10; // сохраняем время нажатой кнопки в мс*10
            key_str.key_left_rise = 1;                                             // формирование статуса фронт сигнала
        }
        else if (gpio_get_level(KeyLeft) == 0 && key_str.key_left != 0)
        {
            key_str.key_left_time = 0;
            key_str.key_left = 0;
            key_str.key_left_fall = 1;
        }

        if (key_str.key_right_rise == 1)
            key_str.key_right_rise = 0; // сброс статуса фронта нажатия
        if (key_str.key_right_fall == 1)
            key_str.key_right_fall = 0; // сброс статуса спада нажатия
        // начало события, отсчет от первого фронта
        if (gpio_get_level(KeyRight) && key_str.key_right_time == 0)
        {
            key_str.key_right_time = xTaskGetTickCount();
            key_str.key_right = gpio_get_level(KeyRight);
        }
        // проверка состояния контакта через время дребезга контактов 50мс
        else if (gpio_get_level(KeyRight) && xTaskGetTickCount() > key_str.key_right_time + 50)
        {
            key_str.key_right = (xTaskGetTickCount() - key_str.key_right_time) / 10; // сохраняем время нажатой кнопки в мс*10
            key_str.key_right_rise = 1;                                              // формирование статуса фронт сигнала
        }
        else if (gpio_get_level(KeyRight) == 0 && key_str.key_right != 0)
        {
            key_str.key_right_time = 0;
            key_str.key_right = 0;
            key_str.key_right_fall = 1;
        }

        key_str.any_key = key_str.key_up | key_str.key_down | key_str.key_left | key_str.key_right;

        vTaskDelay(10);
    }
}

void time_pcf8563(void *pvParameters)
{
    i2c_dev_t dev;
    memset(&dev, 0, sizeof(i2c_dev_t));
    i2cdev_init();
    ESP_ERROR_CHECK(pcf8563_init_desc(&dev, 0, I2C_SDA, I2C_SCL));

    // setup datetime: 2020-04-03 12:35, Sunday
    struct tm time = {
        .tm_year = 123, // years since 1900
        .tm_mon  = 10,   // months since January
        .tm_mday = 13,
        .tm_hour = 9,
        .tm_min  = 30,
        .tm_sec  = 10,
        .tm_wday = 5    // days since Sunday
    };
    ESP_ERROR_CHECK(pcf8563_set_time(&dev, &time));

    while (1)
    {
        bool valid;
        esp_err_t r = pcf8563_get_time(&dev, &time, &valid);
        if (r == ESP_OK)
            printf("%04d-%02d-%02d %02d:%02d:%02d, %s\n", time.tm_year + 1900, time.tm_mon + 1,
                time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, valid ? "VALID" : "NOT VALID");
        else
            printf("Error %d: %s\n", r, esp_err_to_name(r));


        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void init_()
{
    gpio_reset_pin(KeyDown);
    gpio_reset_pin(KeyUp);
    gpio_reset_pin(KeyLeft);
    gpio_reset_pin(KeyRight);
    gpio_reset_pin(LED);
    gpio_reset_pin(Relay1);
    gpio_reset_pin(Relay2);
    gpio_reset_pin(Relay3);
    gpio_reset_pin(Relay4);
    gpio_reset_pin(Temp_1);
    gpio_reset_pin(Temp_2);
    gpio_reset_pin(Temp_3);
    gpio_reset_pin(Temp_4);
    gpio_reset_pin(WP);
    gpio_reset_pin(DC);

    gpio_set_direction(KeyDown, GPIO_MODE_INPUT);
    gpio_set_direction(KeyUp, GPIO_MODE_INPUT);
    gpio_set_direction(KeyLeft, GPIO_MODE_INPUT);
    gpio_set_direction(KeyRight, GPIO_MODE_INPUT);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(Relay1, GPIO_MODE_OUTPUT);
    gpio_set_direction(Relay2, GPIO_MODE_OUTPUT);
    gpio_set_direction(Relay3, GPIO_MODE_OUTPUT);
    gpio_set_direction(Relay4, GPIO_MODE_OUTPUT);
    gpio_set_direction(Temp_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(Temp_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(Temp_3, GPIO_MODE_OUTPUT);
    gpio_set_direction(Temp_4, GPIO_MODE_OUTPUT);
    gpio_set_direction(WP, GPIO_MODE_OUTPUT);
    gpio_set_direction(DC, GPIO_MODE_OUTPUT);

  //  vTaskDelay(pdMS_TO_TICKS(4000));
}

void app_main()
 {
   init_();

   ESP_LOGI(TAG, "Initializing SPIFFS");

	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/spiffs",
		.partition_label = NULL,
		.max_files = 12,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return;
	}

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(NULL, &total,&used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	SPIFFS_Directory("/spiffs/");
   // Display_xQueue = xQueueCreate( 1, sizeof( float ) );
   // Key_read_xQueue = xQueueCreate( 1, sizeof( float ) );
   // Print_xQueue = xQueueCreate( 1, sizeof( float ) );

    //    xTaskCreate(&display, "display", 2048,NULL,8,NULL );
    xTaskCreate(&key_read, "key_read", 2048,NULL,8,NULL );
   // xTaskCreate(&print, "print", 2048,NULL,8,NULL );
   // xTaskCreate(&time_pcf8563, "time_pcf8563", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);
    xTaskCreate(&display, "ST7789", 1024*6, NULL, 2, NULL);
 }