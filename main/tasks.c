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
struct timeval tv;

char startBody[]="{\"tokenname\": \"IOT";
char midBody[]="\",\"displayname\": \"test01\",\"description\": \"hello\",\"previewImageNft\":{\"mimetype\": \"image/png\",\"fileFromBase64\":\"";
char endBody[]="\"}, \"priceInLovelace\": 4500000,\"isBlocked\": false}";
char startTangleBody[]="{\"iot2tangle\": [ { \"sensor\":\"Address\", \"data\": [ { \"name\": \"Tangle 2 Street\" } ] } ,{\"sensor\": \"Camera\",\"data\":[{\"cars\":";
char midTangleBody[]="}]}], \"device\": \"";
char endTangleBody[]="\", \"timestamp\": 0  }";

extern xSemaphoreHandle connectionSemaphore;

//arrays to store the values in NVS
char gatewayNVS[20],apikeyNVS[50],deviceidNVS[30];

//number of detected elements by AI
int carNumber,animalNumber,bikeNumber,scooterNumber,personNumber,accidentsNumber,nftUid;
//flag to decide what detect and what not to detect
int flagAnimal,flagScooter,flagBike,flagPerson;


 int getItems(char* response, char* item){
    char *ptr=response;
    int total=0;

    while(strstr(ptr,item)!=NULL){
    total++;
    ptr=strstr(ptr,item)+1;
    }

    return total;
 }

 void onGotData(char* data){

	nftUid=getItems(data,"nftUid");//this line detectes toy vehicles

    printf("There are %d cars\n",nftUid);
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

			 Header AuthHeader={
					 .key="Authorization",
					 .value="Bearer 641958a078434527a6e6388c07ca7dd8"

			 };

			 AIparams.header[0]=hostAIHeader;
			 AIparams.header[1]=AuthHeader;
			 AIparams.header[3]=typeHeader;

			 char bodyNew[40000];
			 strcpy(bodyNew,startBody);
			 char stringRandom[20];
			 sprintf(stringRandom,"%d",(int)tv.tv_sec);
			 strcat(bodyNew,stringRandom);
			 strcat(bodyNew,midBody);
			 printf(bodyNew);
			 strcat(bodyNew,(char*)buffer);
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
			 strcpy(request,"https://studio-api.nmkr.io/v2/UploadNft/5cc3bf1a-a628-4712-9bf4-1891170d5336");
			 //strcat(request,apikeyNVS); //HERE I ATTACH the uid project
			 fetch(request,&AIparams);

			 /*
			 struct  FetchParams TangleParams;
			 char strCars[10];
			 sprintf(strCars,"\"%d\"",carNumber);

			 TangleParams.headerCount=2;
			 TangleParams.onGotData=NULL;
			 TangleParams.method=POST;

			 char gatewayHost[30];
			 strcpy(gatewayHost,gatewayNVS);
			 //strcat(gatewayHost,":8080");

			 Header hostTangleHeader={
				 .key="Host",
				 .value=gatewayHost
			 };

			 Header hostTypeHeader={
					 .key="Content-Type",
					 .value="application/json"
			 };


			 char BodyTangle[5000];
			 strcpy(BodyTangle,startTangleBody);
			 strcat(BodyTangle,strCars);



			 if(flagAnimal){
				 char strAnimals[10];
				 strcat(BodyTangle,",\"Animals\":");
				 sprintf(strAnimals,"\"%d\"",animalNumber);
				 strcat(BodyTangle,strAnimals);

			 }
			 if(flagPerson){
				 char strPersons[10];
				 strcat(BodyTangle,",\"Pedestrians\":");
				 sprintf(strPersons,"\"%d\"",personNumber);
				 strcat(BodyTangle,strPersons);

			 }
			 if(flagBike){
				 char strBikes[10];
				 strcat(BodyTangle,",\"Bicycle\":");
				 sprintf(strBikes,"\"%d\"",bikeNumber);
				 strcat(BodyTangle,strBikes);

			 }
			 if(flagScooter){
				 char strScooter[10];
				 strcat(BodyTangle,",\"Scooter\":");
				 sprintf(strScooter,"\"%d\"",scooterNumber);
				 strcat(BodyTangle,strScooter);

			 }
			 char strAccidents[10];
			 strcat(BodyTangle,",\"accidents\":");
			 sprintf(strAccidents,"\"%d\"",accidentsNumber);
			 strcat(BodyTangle,strAccidents);

			 strcat(BodyTangle,midTangleBody);
			 strcat(BodyTangle,deviceidNVS);
			 strcat(BodyTangle,endTangleBody);
			 TangleParams.header[0]=hostTangleHeader;
			 TangleParams.header[1]=hostTypeHeader;

			 TangleParams.body=BodyTangle;
			 strcat(gatewayHost,"/messages");




			 strcpy(request,"http://");
			 strcat(request,gatewayHost);
			 fetch(request,&TangleParams);
			 printf("richiesta %s\n",request);
			 printf("\n%s il body\n",BodyTangle);
			 */






			 esp_wifi_disconnect();
			 free(buffer);

		 }

		 vTaskDelay(10000/portTICK_PERIOD_MS);


	}

}
