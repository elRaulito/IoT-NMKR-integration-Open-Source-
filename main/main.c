#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "mbedtls/base64.h"


#include "connect.h"
#include "fetch.h"
#include "string.h"
#include "server.h"
#include "camera.h"
#include "tasks.h"




#define TAG "server"



xSemaphoreHandle connectionSemaphore;
int flagConnected;




 static esp_err_t event_handler(void *ctx, system_event_t *event)
 {

    	  switch (event->event_id)
    	  {
    	  case SYSTEM_EVENT_STA_START:
    	    if(esp_wifi_connect()!=ESP_OK){
    	    	esp_wifi_stop();
    	    	connectSTA();

    	    }else{
    	    ESP_LOGI(TAG,"connecting...\n");
    	    }
    	    break;

    	  case SYSTEM_EVENT_STA_CONNECTED:
    	    ESP_LOGI(TAG,"connected\n");
    	    break;

    	  case SYSTEM_EVENT_STA_GOT_IP:
    	    ESP_LOGI(TAG,"got ip\n");
    	    xSemaphoreGive(connectionSemaphore);
    	    flagConnected=1;
    	    break;

    	  case SYSTEM_EVENT_STA_DISCONNECTED:
    		if(xSemaphoreTake(connectionSemaphore, 10000/portTICK_PERIOD_MS)){
    			printf("semaphore removed\n");
    		}
    		flagConnected=0;
  	    	esp_wifi_stop();
  	    	connectSTA();
    	    break;

    	  case SYSTEM_EVENT_AP_STACONNECTED:


    	  break;

    	  case SYSTEM_EVENT_AP_STADISCONNECTED:

    	  break;

    	  case SYSTEM_EVENT_AP_STOP:
          break;

    	  default:
    	    break;
    	  }
    	  return ESP_OK;
    }





void app_main(void)
{
	//init not volatile storage
     nvs_flash_init();
     ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

     camera_start();

     //wifi start
     tcpip_adapter_init();
     //create binary that is activated when connection is enstablished
     connectionSemaphore=xSemaphoreCreateBinary();
	 wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	 ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	 ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));


     //at the starting it generates its own wifi for configuration
	 connectAP();
	 RegisterEndPoints();


	 //the following task activates every 3 second and sends picture to the AI
	 //the resposne is analized and the result is sent to the tangle
     xTaskCreatePinnedToCore(ShootTask,"shoot",1024*100,NULL,4,NULL,0);

}

