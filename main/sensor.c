/*
 * sensors.c
 *
 *  Created on: 29 giu 2020
 *      Author: Raul Rosa
 */
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "connect.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "server.h"
#include "cJSON.h"

#define DHT11_PIN 14

void DHT11_Start (void)
{
	 gpio_pad_select_gpio(DHT11_PIN);
	 gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
	 gpio_set_level(DHT11_PIN, 0);
	 ets_delay_us(18000);
	 gpio_set_level(DHT11_PIN, 1);
	 ets_delay_us(20);
	 gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);

}

char DHT11_Check_Response (void)
{
	char Response = 0;
	ets_delay_us(40);
	if(!gpio_get_level(DHT11_PIN)){
		ets_delay_us(80);
		if(gpio_get_level(DHT11_PIN)){
			Response=1;
		}else{
			Response=-1;
		}
	}else{
		Response=-1;
	}

	while(gpio_get_level(DHT11_PIN));
	return Response;
}

char DHT11_Read (void)
{
	char i,j;
	i=0;


	for (j=0;j<8;j++)
	{
		//wait pin to go high
		while (!gpio_get_level(DHT11_PIN));

		ets_delay_us(40);   // wait for 40 us
		if (!gpio_get_level(DHT11_PIN))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while (gpio_get_level(DHT11_PIN));

	}


	return i;
}
