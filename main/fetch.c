/*
 * fetch.c
 *
 *  Created on: 26 giu 2020
 *      Author: UTPM9
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_http_client.h"
#include "cJSON.h"
#include "driver/ledc.h"
#include "fetch.h"







esp_err_t eventHandlerClient(esp_http_client_event_t *evt){
	struct FetchParams *fetchparams = (struct FetchParams*) evt->user_data;

	switch(evt->event_id){
	case HTTP_EVENT_ON_DATA:
		printf("contenuto fetchato %.*s\n",evt->data_len,(char*)evt->data);
		if(fetchparams->onGotData!=NULL){
			fetchparams->onGotData((char*)evt->data);
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
//tutto bene
	printf("contenuto di dimensione %d\n",esp_http_client_get_content_length(client));
}else{
	//male male
	printf("errore\n");

}


esp_http_client_cleanup(client);
}


