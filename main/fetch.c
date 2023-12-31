/*
 * fetch.c
 *
 *  Created on: 9 Nov 2020
 *      Author: Raul Rosa
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_http_client.h"
#include "cJSON.h"
#include "driver/ledc.h"
#include "fetch.h"
#include "esp_wifi.h"
#include "connect.h"




char *incomingData=NULL;
int indexBuffer=0;


esp_err_t eventHandlerClient(esp_http_client_event_t *evt){
	struct FetchParams *fetchparams = (struct FetchParams*) evt->user_data;

	switch(evt->event_id){
	case HTTP_EVENT_ON_DATA:


		if(incomingData==NULL){

			incomingData=(char*)malloc(evt->data_len);
		}else{

			incomingData=(char*)realloc(incomingData,evt->data_len+indexBuffer);
		}

		memcpy(&incomingData[indexBuffer],evt->data,evt->data_len);
		indexBuffer+=evt->data_len;

		break;


	case HTTP_EVENT_ON_FINISH:
		incomingData=(char*)realloc(incomingData,indexBuffer+1);
		memcpy(&incomingData[indexBuffer],"\0",1);
		printf("content fetched %.*s\n",indexBuffer,incomingData);
		if(fetchparams->onGotData!=NULL){
			fetchparams->onGotData(incomingData);
			free(incomingData);
			indexBuffer=0;
			incomingData=NULL;
		}

		break;


    default:
	break;

	}

	return ESP_OK;
}




void fetch(char* url, struct FetchParams *fetchparams){

esp_http_client_config_t clientConf={
		.url=url,
		.event_handler=eventHandlerClient,
		.user_data=fetchparams,
		.buffer_size=1024


};


esp_http_client_handle_t client=esp_http_client_init(&clientConf);

if(fetchparams->method==PATCH){
	esp_http_client_set_method(client,HTTP_METHOD_PATCH);
}
if(fetchparams->method==POST){
	esp_http_client_set_method(client,HTTP_METHOD_POST);
}

int i;
for(i=0;i<fetchparams->headerCount;i++){
	esp_http_client_set_header(client,fetchparams->header[i].key,fetchparams->header[i].value);
}
if(fetchparams->body!=NULL){
	esp_http_client_set_post_field(client,fetchparams->body,strlen(fetchparams->body));
}


esp_err_t err=esp_http_client_perform(client);
fetchparams->status=esp_http_client_get_status_code(client);
if(err==ESP_OK){

	printf("got content of lenght %d\n",esp_http_client_get_content_length(client));
}else{

	printf("error \n");


}


esp_http_client_cleanup(client);
}


