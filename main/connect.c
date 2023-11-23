/*
 * connect.c
 *
 *  Created on: 29 giu 2020
 *      Author: Raul Rosa
 */
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "string.h"
#include <dirent.h>
#include "esp_log.h"
#include "esp_system.h"
#include "connect.h"

#define SSID "Seminja"
#define PASSWORD "12345678"


void connectSTA()
{
	    nvs_handle_t nvs;
	    nvs_open("ServerCreds", NVS_READWRITE, &nvs);

	    size_t ssidLen;
	    size_t passLen;
	    char *ssid = NULL;
	    char *pass=NULL;

nvs_get_str(nvs, "password", NULL, &passLen);
nvs_get_str(nvs, "server", NULL, &ssidLen);

	    if (((nvs_get_str(nvs, "password", NULL, &passLen)))== ESP_OK)
	    {

	      if (ssidLen > 0)
	      {

	        ssid = malloc(ssidLen);
	        nvs_get_str(nvs, "server", ssid, &ssidLen);

	        pass = malloc(passLen);
	        nvs_get_str(nvs, "password", pass, &passLen);

	        printf("provo a collegarmi STA %s %s\n",ssid,pass);

	      }
	    }else{

	    	nvs_close(nvs);
	    	printf("non ho trovati ssid e pass\n");
	    	connectAP();
	    	return;
	    }



  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  // wifi_config_t wifi_config =
  //     {
  //         .sta = {
  //             .ssid = CONFIG_WIFI_SSID,
  //             .password = CONFIG_WIFI_PASSWORD}};
  wifi_config_t wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char *)wifi_config.sta.ssid, ssid);
  strcpy((char *)wifi_config.sta.password, pass);
  esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);

  free(ssid);
  free(pass);
  nvs_close(nvs);

  ESP_ERROR_CHECK(esp_wifi_start());

}


void connectAP(){


	 ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));

	 tcpip_adapter_ip_info_t info;
	 memset(&info, 0, sizeof(info));
	 IP4_ADDR(&info.ip, 192, 168, 1, 1);
	 IP4_ADDR(&info.gw, 192, 168, 1, 1);
	 IP4_ADDR(&info.netmask, 255, 255, 255, 0);
	 ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
	 printf("- TCP adapter configured with IP 192.168.1.1/24\n");
	 ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	  wifi_config_t wifi_config =
	      {
	          .ap = {
	              .ssid = SSID,
	              .password = PASSWORD,
	              .max_connection = 4,
	              .authmode = WIFI_AUTH_WPA_WPA2_PSK}};




esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
ESP_ERROR_CHECK(esp_wifi_start());


}

