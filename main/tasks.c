/*
 * Tasks.c
 *
 *  Created on: 10 nov 2020
 *      Author: Raul Rosa
 */

#include "fetch.h"
#include "string.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "camera.h"
#include "esp_wifi.h"
#include "cJSON.h"
struct timeval tv;

char startBody[]="{\"tokenname\": \"IOT";
char midBody[]="\",\"displayname\": \"test01\",\"description\": \"hello\",\"previewImageNft\":{\"mimetype\": \"image/jpeg\",\"fileFromBase64\":\"";
char endBody[]="\"}, \"priceInLovelace\": 4500000,\"isBlocked\": false}";
char startTangleBody[]="{\"iot2tangle\": [ { \"sensor\":\"Address\", \"data\": [ { \"name\": \"Tangle 2 Street\" } ] } ,{\"sensor\": \"Camera\",\"data\":[{\"cars\":";
char midTangleBody[]="}]}], \"device\": \"";
char endTangleBody[]="\", \"timestamp\": 0  }";

extern xSemaphoreHandle connectionSemaphore;

//arrays to store the values in NVS
char projectuidNVS[50],apikeyNVS[50],addressNVS[104];
char nftuid[40];

//number of detected elements by AI
int carNumber,animalNumber,bikeNumber,scooterNumber,personNumber,accidentsNumber,nftUid;
//flag to decide what detect and what not to detect
int flagAnimal,flagScooter,flagBike,flagPerson;


void onGotData(char* data) {
    cJSON *payload = cJSON_Parse(data);
    if (payload == NULL) {
        printf("Error parsing JSON\n");
        return;
    }

    cJSON *nftUidItem = cJSON_GetObjectItem(payload, "nftUid");
    if (!cJSON_IsString(nftUidItem) || (nftUidItem->valuestring == NULL)) {
        printf("nftUid not found or is not a string\n");
        cJSON_Delete(payload);
        return;
    }

    strncpy(nftuid, nftUidItem->valuestring, sizeof(nftuid) - 1);
    nftuid[sizeof(nftuid) - 1] = '\0'; // Ensure null-terminated string

    cJSON_Delete(payload);
}




extern uint8_t *buffer;
void ShootTask(void* params){


	while(1){


		 //if connection available make AI request

		 if (xSemaphoreTake(connectionSemaphore,100/portTICK_PERIOD_MS)){
			 camera_capture();
			 if (gettimeofday(&tv, NULL)!= 0) {
			     printf("Failed to obtain time");
			   }
			 struct  FetchParams AIparams;
			 AIparams.headerCount=4;
			 AIparams.onGotData=onGotData;
			 AIparams.method=POST;

			 Header hostAIHeader={
				 .key="Host",
				 .value="studio-api.nmkr.io"
			 };

			 Header typeHeader={
					 .key="Content-Type",
					 .value="application/json"

			 };

			 char Authnew[50];
			 strcpy(Authnew,"Bearer ");
			 strcat(Authnew,apikeyNVS);

			 Header AuthHeader={
					 .key="Authorization",
					 .value=Authnew

			 };
			 printf(Authnew,"auth string");

			 AIparams.header[0]=hostAIHeader;
			 AIparams.header[1]=AuthHeader;
			 AIparams.header[3]=typeHeader;

			 char bodyNew[50000];
			 strcpy(bodyNew,startBody);
			 char stringRandom[20];
			 sprintf(stringRandom,"%d",(int)tv.tv_sec+1000);
			 strcat(bodyNew,stringRandom);
			 strcat(bodyNew,midBody);
			 printf(bodyNew);
			 strcat(bodyNew,(char*)buffer);
			 printf(bodyNew);
			 printf(projectuidNVS);
			 strcat(bodyNew,endBody);
			 AIparams.body=bodyNew;

			 char str[10];
			 sprintf(str, "%d", sizeof(bodyNew));

			 Header lenghtAIHeader={
					 .key="Content-Length",
					 .value=str
			 };

			 AIparams.header[2]=lenghtAIHeader;

			 char request[500];
			 strcpy(request,"https://studio-api.nmkr.io/v2/UploadNft/");
			 strcat(request,projectuidNVS); //HERE I ATTACH the uid project
			 printf(request);
			 fetch(request,&AIparams);

			 //poi qua faccio il mint
			 printf(nftuid);

			 struct  FetchParams MintParams;

			 MintParams.headerCount=3;
			 MintParams.onGotData=onGotData;
			 MintParams.method=GET;
			 Header lenghtMintHeader={
					 .key="Content-Length",
					 .value="0"
			 };
			MintParams.header[0]=hostAIHeader;
			 MintParams.header[1]=AuthHeader;
			 MintParams.header[2]=typeHeader;



			char request2[500];
			snprintf(request2, sizeof(request2), "https://studio-api.nmkr.io/v2/MintAndSendSpecific/%s/%s/1/%s", projectuidNVS, nftuid, addressNVS);

			printf("Request URL: %s\n", request2);
			fetch_quote(request2, &MintParams);



			 esp_wifi_disconnect();
			 free(buffer);

		 }

		 vTaskDelay(10000/portTICK_PERIOD_MS);


	}

}
