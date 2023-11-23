#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "connect.h"
#include "esp_log.h"
#include "server.h"
#include "driver/adc.h"
#include "rom/ets_sys.h"
#include "sensor.h"
#include "fetch.h"
#include "cJSON.h"
#include "AI.h"
#include "string.h"

#define TAG "server"
xSemaphoreHandle connectionSemaphore;
int devicesConnected=0;

int contaSessioni=0;



static esp_err_t event_handler(void *ctx, system_event_t *event)
{

	  switch (event->event_id)
	  {
	  case SYSTEM_EVENT_STA_START:
	    if(esp_wifi_connect()!=ESP_OK){
	    	esp_wifi_stop();
	    	connectAP();
	    	ESP_LOGI(TAG,"Vado in AP mode, collegamento fallito...\n");
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

	    break;

	  case SYSTEM_EVENT_STA_DISCONNECTED:
	    ESP_LOGI(TAG,"disconnected, perch�?\n");
    	esp_wifi_stop();
    	connectAP();
	    break;

	  case SYSTEM_EVENT_AP_STACONNECTED:

	  devicesConnected++;
	  printf("%d disp collegati\n",devicesConnected);
	  break;

	  case SYSTEM_EVENT_AP_STADISCONNECTED:
	  devicesConnected--;
	  printf("%d disp collegati\n",devicesConnected);
	  break;

	  case SYSTEM_EVENT_AP_STOP:
      printf("qua finisce lo stop access point\n");
      break;

	  default:
	    break;
	  }
	  return ESP_OK;
}

extern int luce;
extern int soil;
void ADCtask(void* params){
	while(1){
    int val = adc1_get_raw(ADC1_CHANNEL_0);
    if(val<500){
    luce=val;
    }else{
    	luce=500;
    }

    //funzione per la soil
    val = adc1_get_raw(ADC1_CHANNEL_3);
    if(val<500){
        soil=val;
     }else{
        	soil=500;
     }
    //printf("soil = %d\n",soil);

    vTaskDelay(250 / portTICK_PERIOD_MS);
	}

}

void startADC(){
	     adc1_config_width(ADC_WIDTH_BIT_10);
	     adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
	     adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);

}

char *nome=NULL;
char *numero=NULL;
void getPlant(){
    nvs_handle_t nvs;
    nvs_open("PlantCreds", NVS_READWRITE, &nvs);
    size_t NameLen;
    size_t NumLen;

    nvs_get_str(nvs, "nome", NULL, &NameLen);
    nome=malloc(NameLen);
    nvs_get_str(nvs, "numero", NULL, &NumLen);
    numero=malloc(NumLen);


    nvs_get_str(nvs, "nome", nome, &NameLen);
    nvs_get_str(nvs, "numero", numero, &NumLen);
    printf("la pianta si chiama %s e il numero di telefono e'%s\n",nome,numero);
    nvs_close(nvs);
}


int Presence;
char Rh_byte1,Rh_byte2,Temp_byte1,Temp_byte2,SUM,TEMP,RH;
int Temperature,Humidity;

void meteoTask(void* params){



	while(1){

			  DHT11_Start();
			  Presence = DHT11_Check_Response();
			  if(Presence==1){
			  Rh_byte1 = DHT11_Read();
			  Rh_byte2 = DHT11_Read();
			  Temp_byte1 = DHT11_Read();
			  Temp_byte2 = DHT11_Read();
			  SUM = DHT11_Read();

			  TEMP = Temp_byte1;
			  RH = Rh_byte1;

			  Temperature = (int) TEMP;
			  Humidity = (int) RH;

			  // printf("Temp=%.2f Humidity=%.2f   SUM=%d\n",(float)TEMP,(float)RH,SUM);
			  }else{
				//  printf("sensore non rilevato\n");
			  }
		vTaskDelay(3000/portTICK_PERIOD_MS);
	}
}

TaskHandle_t meteoHandle;

void SMSBody(char* messaggio, char* nome, char* numero, char* out){

	sprintf(out,
	"{"
	" \"address\": \"tel:+39%s\", "
	" \"message\": \"Ciao sono %s %s\" "
	"}",numero,nome,messaggio
	);
printf("body= %s",out);

}

int IQA=0;
float co,so2,o3,no2,pm1,pm25,pm10;

void onGotData(char* data){
cJSON *payload= cJSON_Parse(data);
/*
Aggiungere codice in caso di dato ottenuto
*/
cJSON *quality=cJSON_GetObjectItem(payload,"IQAValue");
cJSON *gas= cJSON_GetObjectItem(payload,"pollutants");
IQA=(200-(int)quality->valuedouble)/20;

cJSON *solfuro=cJSON_GetObjectItem(gas,"SO2");
so2=solfuro->valuedouble;

cJSON *monossido=cJSON_GetObjectItem(gas,"CO");
co=monossido->valuedouble;

cJSON *ozono=cJSON_GetObjectItem(gas,"O3");
o3=ozono->valuedouble;

cJSON *azoto=cJSON_GetObjectItem(gas,"NO2");
no2=azoto->valuedouble;

cJSON *Pm1=cJSON_GetObjectItem(gas,"PM1");
pm1=Pm1->valuedouble;

cJSON *Pm25=cJSON_GetObjectItem(gas,"PM2_5");
pm25=Pm25->valuedouble;

cJSON *Pm10=cJSON_GetObjectItem(gas,"PM10");
pm10=Pm10->valuedouble;


printf("la qualita e' %d\n",IQA);
printf("Anidride solforosa %f\n",so2);
printf("monossido di carbonio %f\n",co);
printf("Ozono %f\n",o3);
printf("Diossido di azoto %f\n",no2);
printf("pm1 %f\n",pm1);
printf("pm25 %f\n",pm25);
printf("pm10 %f\n",pm10);

cJSON_Delete(payload);


}

int limone=0;
void onGotAI(char* data){


if(strstr(data, "Lemon") != NULL) {
limone=1;
}

printf("limone=%d\n",limone);
}


void RESTtask(void* params){
while(1){

	if(devicesConnected==0){
		//spengo il sensore


		esp_wifi_stop();
		connectSTA();

		 if (xSemaphoreTake(connectionSemaphore, 10000/portTICK_PERIOD_MS)){








                 //GET dati qualita' aria
				 struct FetchParams AirQualityParams;
				 AirQualityParams.headerCount=2;
				 AirQualityParams.method=GET;
				 AirQualityParams.onGotData=onGotData;
				 AirQualityParams.body=NULL;
				 Header hostHeader={
					 .key="Host",
					 .value="hackathon.tim.it"
				 };
				 Header apiHeader={
						 .key="apikey",
						 .value="APIKEY"
				 };
				 AirQualityParams.header[0]=hostHeader;
				 AirQualityParams.header[1]=apiHeader;
				 fetch("https://hackathon.tim.it/airquality/latest?latitude=45.4642&longitude=9.1900",&AirQualityParams);


				 //POST MESSAGGIO SOLO IN CASO DI SCARSA LUCE
				 if(luce>450){
					 struct FetchParams SMSparams;
					 SMSparams.headerCount=3;
					 SMSparams.onGotData=NULL;
					 SMSparams.method=POST;


				     Header typeHeader={
						.key="Content-Type",
						.value="application/json"
				         };
					 Header hostHeader={
						 .key="Host",
						 .value="hackathon.tim.it"
					 };
					 Header apiHeader={
							 .key="apikey",
							 .value="APIKEY"
					 };
				     SMSparams.header[0]=hostHeader;
				     SMSparams.header[1]=apiHeader;
				     SMSparams.header[2]=typeHeader;
				     char body[1024];
				     SMSBody("La luminosita' della stanza non e' salutare per le mie foglie",nome,numero,body);
				     SMSparams.body=body;
				     fetch("https://hackathon.tim.it/sms/mt",&SMSparams);
				     printf("status= %d\n",SMSparams.status);


				 }


				 //MESSAGGIO IN CASO ABBIA SETE
				 //POST MESSAGGIO SOLO IN CASO DI SCARSA LUCE
				 if(soil>=500){
					 struct FetchParams SMSparams;
					 SMSparams.headerCount=3;
					 SMSparams.onGotData=NULL;
					 SMSparams.method=POST;


				     Header typeHeader={
						.key="Content-Type",
						.value="application/json"
				         };
					 Header hostHeader={
						 .key="Host",
						 .value="hackathon.tim.it"
					 };
					 Header apiHeader={
							 .key="apikey",
							 .value="APIKEY"
					 };
				     SMSparams.header[0]=hostHeader;
				     SMSparams.header[1]=apiHeader;
				     SMSparams.header[2]=typeHeader;
				     char body[1024];
				     SMSBody(" sto morendo di sete!!",nome,numero,body);
				     SMSparams.body=body;
				     fetch("https://hackathon.tim.it/sms/mt",&SMSparams);
				     printf("status= %d\n",SMSparams.status);


				 }

          //Artificial intelligence per vedere se ci sono i frutti

				 struct FetchParams AIparams;
				 AIparams.headerCount=3;
				 AIparams.onGotData=onGotAI;
				 AIparams.method=POST;
			     Header typeHeader={
					.key="Content-Type",
					.value="application/json"
			         };
				 Header hostAIHeader={
					 .key="Host",
					 .value="hackathon.tim.it"
				 };
				 Header apiAIHeader={
						 .key="apikey",
						 .value="APIKEY"
				 };
			     AIparams.header[0]=hostAIHeader;
			     AIparams.header[1]=apiAIHeader;
			     AIparams.header[2]=typeHeader;
			     AIparams.body=bodyAI;
			     fetch("https://hackathon.tim.it/gcloudvision/v1/images:annotate",&AIparams);



		 }



		 esp_wifi_disconnect();



	}
	vTaskDelay(10000/portTICK_PERIOD_MS);
}
}


void app_main(void)
{


     startADC();
	 ESP_ERROR_CHECK(nvs_flash_init());
     tcpip_adapter_init();
     connectionSemaphore=xSemaphoreCreateBinary();
     getPlant();


     ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	 wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	 ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	 ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

	 connectAP();


	 RegisterEndPoints();

	 xTaskCreatePinnedToCore(ADCtask,"ADC",1024*5,NULL,4,NULL,0);
	 xTaskCreatePinnedToCore(meteoTask,"Meteo",1024*5,NULL,3,&meteoHandle,0);

//enable this task only if you have wifi connection and api key of TIM 5g API
	 //xTaskCreatePinnedToCore(RESTtask,"RestTask",1024*20,NULL,2,NULL,1);

}

void esp_task_wdt_isr_user_handler(void)
{
vTaskDelete(meteoHandle);
meteoHandle=NULL;
ets_delay_us(10000);
if(meteoHandle==NULL){
xTaskCreatePinnedToCore(meteoTask,"Meteo",1024*5,NULL,3,&meteoHandle,0);
}
}
