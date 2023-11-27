/*
 * server.c
 *
 *  Created on: 9 Nov 2020
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
#include "esp_camera.h"


#define TAG "server"

//address in flahs memory of the webpages to serve
extern const char indiceStart[] asm("_binary_index_html_start");
extern const char wifiStart[] asm("_binary_wifi_html_start");
extern const char gatewayStart[] asm("_binary_gateway_html_start");
extern const char apiStart[] asm("_binary_apikey_html_start");
extern const char IDStart[] asm("_binary_deviceid_html_start");
extern const char detectionStart[] asm("_binary_detection_html_start");

extern const char BootCssStart[] asm("_binary_bootstrap_min_css_start");
extern const char queryJsStart[] asm("_binary_jquery_min_js_start");
extern const char BootJsStart[] asm("_binary_bootstrap_min_js_start");
extern const char CocoJsStart[] asm("_binary_coco_js_start");
extern const char TensoflowJsStart[] asm("_binary_tfjs_js_start");
extern const char ModelStart[] asm("_binary_model_json_start");

extern const char coverStart[] asm("_binary_IOTMKR_png_start");
extern const char coverFine[] asm("_binary_IOTMKR_png_end");



static esp_err_t on_url_hit(httpd_req_t *req)
{


	const unsigned int imgSize = coverFine - coverStart;

    printf("%s",req->uri);
    char path[600];
    sprintf(path, "%s", req->uri);

    if (strcmp(path, "/") == 0)
    {
        sprintf(path, "%s", "index.html");
    }
    ESP_LOGI(TAG, "url %s was hit", req->uri);

    //style.css

    char *ptr = strrchr(path, '.');


    if (strcmp(ptr, ".css") == 0)
    {

        httpd_resp_set_type(req, "text/css");
    }



    if (strcmp(ptr, ".map") == 0)
    {

        httpd_resp_send_404(req);
        return ESP_OK;
    }

    if (strcmp(req->uri, "/") == 0)
    {

        httpd_resp_sendstr(req,indiceStart);


        return ESP_OK;
    }

    if (strcmp(req->uri, "/index.html") == 0)
    {

        httpd_resp_sendstr(req,indiceStart);




        return ESP_OK;
    }




    if (strstr(req->uri, "/snapshot.jpg") !=NULL)
    {

        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {

            return ESP_FAIL;
        }
    	httpd_resp_send(req,(char*)fb->buf,fb->len);
    	esp_camera_fb_return(fb);
        return ESP_OK;
    }


    if (strstr(req->uri, "/model.json") !=NULL)
        {
    	httpd_resp_sendstr(req,ModelStart);

    	return ESP_OK;
        }

    if (strcmp(req->uri, "/gateway.html") == 0)
    {

        httpd_resp_sendstr(req,gatewayStart);




        return ESP_OK;
    }

    if (strcmp(req->uri, "/wifi.html") == 0)
    {

        httpd_resp_sendstr(req,wifiStart);





        return ESP_OK;
    }

    if (strcmp(req->uri, "/apikey.html") == 0)
    {

        httpd_resp_sendstr(req,apiStart);





        return ESP_OK;
    }

    if (strcmp(req->uri, "/deviceid.html") == 0)
    {

        httpd_resp_sendstr(req,IDStart);





        return ESP_OK;
    }

    if (strcmp(req->uri, "/detection.html") == 0)
    {

        httpd_resp_sendstr(req,detectionStart);





        return ESP_OK;
    }



    if (strcmp(req->uri, "/boot/css/bootstrap.min.css") == 0)
    {

        httpd_resp_sendstr(req,BootCssStart);




        return ESP_OK;
    }



    if (strcmp(req->uri, "/boot/js/bootstrap.min.js") == 0)
    {

        httpd_resp_sendstr(req,BootJsStart);




        return ESP_OK;
    }


    if (strcmp(req->uri, "/boot/js/@tensorflow/tfjs.js") == 0)
    {

        httpd_resp_sendstr(req,TensoflowJsStart);




        return ESP_OK;
    }



    if (strcmp(req->uri, "/boot/js/@tensorflow-models/coco.js") == 0)
    {

        httpd_resp_sendstr(req,CocoJsStart);




        return ESP_OK;
    }


    if (strcmp(req->uri, "/boot/jquery/jquery.min.js") == 0)
    {

        httpd_resp_sendstr(req,queryJsStart);




        return ESP_OK;
    }

    if (strcmp(req->uri, "/img/IOTMKR.png") == 0)
    {


    	httpd_resp_send(req,coverStart,imgSize);


        return ESP_OK;
    }







        httpd_resp_send_404(req);


    return ESP_OK;
}


static esp_err_t on_server_set(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[50];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    cJSON *payload = cJSON_Parse(buf);

    char bufferserver[30];
    char bufferpass[30];

    cJSON *server = cJSON_GetObjectItem(payload, "wifiserver");
    cJSON *pass = cJSON_GetObjectItem(payload, "passserver");




    strcpy(bufferserver, server->valuestring);



    strcpy(bufferpass, pass->valuestring);






    nvs_handle_t nvs;
    nvs_open("ServerCreds", NVS_READWRITE, &nvs);
    nvs_set_str(nvs, "password", bufferpass);
    nvs_set_str(nvs, "server", bufferserver);
    nvs_close(nvs);
    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);



    cJSON_Delete(payload);



    return ESP_OK;


}


static esp_err_t on_gateway_set(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[50];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    printf("%s risposta JSON\n",buf);
    cJSON *payload = cJSON_Parse(buf);

    char buffergateway[30];


    cJSON *server = cJSON_GetObjectItem(payload, "gateway");





    strcpy(buffergateway, server->valuestring);
    printf("Set gateway to %s\n",buffergateway);



    nvs_handle_t nvs;
    nvs_open("ServerCreds", NVS_READWRITE, &nvs);
    printf("check 1\n");
    nvs_set_str(nvs, "gateway", buffergateway);
    printf("check 2\n");
    nvs_close(nvs);
    printf("check 3\n");

    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);



    cJSON_Delete(payload);
    printf("check 4\n");




    return ESP_OK;


}


static esp_err_t on_apikey_set(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[50];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    printf("%s risposta JSON\n",buf);
    cJSON *payload = cJSON_Parse(buf);

    char bufferkey[50];


    cJSON *server = cJSON_GetObjectItem(payload, "apikey");




    strcpy(bufferkey, server->valuestring);
    printf("Set api to to %s\n",bufferkey);



    nvs_handle_t nvs;
    nvs_open("ServerCreds", NVS_READWRITE, &nvs);
    nvs_set_str(nvs, "apikey", bufferkey);
    nvs_close(nvs);


    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);



    cJSON_Delete(payload);




    return ESP_OK;


}

static esp_err_t on_deviceid_set(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[50];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    printf("%s risposta JSON\n",buf);
    cJSON *payload = cJSON_Parse(buf);

    char bufferid[50];


    cJSON *server = cJSON_GetObjectItem(payload, "deviceid");




    strcpy(bufferid, server->valuestring);
    printf("Set device id  to %s\n",bufferid);



    nvs_handle_t nvs;
    nvs_open("ServerCreds", NVS_READWRITE, &nvs);
    nvs_set_str(nvs, "deviceid", bufferid);
    nvs_close(nvs);


    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);



    cJSON_Delete(payload);




    return ESP_OK;


}


static esp_err_t on_checkbox_set(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[50];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    printf("%s risposta JSON\n",buf);

    cJSON *payload = cJSON_Parse(buf);

    int person,animal,scooter,bicycle;


    cJSON *personChecked = cJSON_GetObjectItem(payload, "person");
    cJSON *animalChecked = cJSON_GetObjectItem(payload, "animal");
    cJSON *scooterChecked = cJSON_GetObjectItem(payload, "scooter");
    cJSON *bicycleChecked = cJSON_GetObjectItem(payload, "bicycle");



    nvs_handle_t nvs;
    nvs_open("ServerCreds", NVS_READWRITE, &nvs);
    nvs_set_i16(nvs,"person",personChecked->valueint);
    nvs_set_i16(nvs,"animal",animalChecked->valueint);
    nvs_set_i16(nvs,"scooter",scooterChecked->valueint);
    nvs_set_i16(nvs,"bicycle",bicycleChecked->valueint);
    nvs_close(nvs);


    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);



    cJSON_Delete(payload);




    return ESP_OK;


}




httpd_handle_t server = NULL;

extern char gatewayNVS[20];
extern char apikeyNVS[50];
extern char deviceidNVS[30];
extern int flagAnimal;
extern int flagScooter;
extern int flagBike;
extern int flagPerson;

void getNVS(){

	            nvs_handle_t nvs;
			    nvs_open("ServerCreds", NVS_READWRITE, &nvs);

			    size_t gateLen;
			    size_t apiLen;
			    size_t idLen;

			    nvs_get_str(nvs, "gateway", NULL, &gateLen);
			    nvs_get_str(nvs, "deviceid", NULL, &idLen);
			    nvs_get_str(nvs, "apikey", NULL, &apiLen);

			    if(nvs_get_str(nvs, "gateway", NULL, &gateLen)==ESP_OK){
			    	if(gateLen>0){
			    		nvs_get_str(nvs, "gateway", gatewayNVS, &gateLen);
			    		printf("gateway ottenuto %s\n",gatewayNVS);
			    	}

			    }


			    if(nvs_get_str(nvs, "deviceid", NULL, &idLen)==ESP_OK){
			    	if(idLen>0){
			    		nvs_get_str(nvs, "deviceid", deviceidNVS, &idLen);
			    		printf("id ottenuto ottenuto %s\n",deviceidNVS);
			    	}

			    }

			    if(nvs_get_str(nvs, "apikey", NULL, &apiLen)==ESP_OK){
			    	if(apiLen>0){
			    		nvs_get_str(nvs, "apikey", apikeyNVS, &apiLen);
			    		printf("apikey ottenuto %s\n",apikeyNVS);
			    	}

			    }

			    nvs_get_i16(nvs,"person",&flagPerson);
			    nvs_get_i16(nvs,"animal",&flagAnimal);
			    nvs_get_i16(nvs,"bicycle",&flagBike);
			    nvs_get_i16(nvs,"scooter",&flagScooter);

			    printf("values of checkbox are %d %d %d %d\n",flagAnimal,flagPerson,flagBike,flagScooter);

}

void resetWifi(void *params)
{
vTaskDelay(1000 / portTICK_PERIOD_MS);
esp_wifi_stop();
getNVS();
vTaskDelay(1000 / portTICK_PERIOD_MS);
connectSTA();
httpd_stop(server);

vTaskDelete(NULL);
}


static esp_err_t on_start_request(httpd_req_t *req){

	printf("I am here\n");
	xTaskCreate(resetWifi, "reset wifi", 1024 * 2, NULL, 15, NULL);
    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);
	return ESP_OK;
}




void RegisterEndPoints(void)
{




    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.lru_purge_enable=true;
    config.stack_size=1024*20;//*10
    config.core_id=1;
    config.max_uri_handlers=20;




//qui

    ESP_LOGI(TAG, "starting server");
    if (httpd_start(&server, &config) != ESP_OK)
    {
    ESP_LOGE(TAG, "COULD NOT START SERVER");

    }









    httpd_uri_t server_end_point_config = {
        .uri = "/api/server",
        .method = HTTP_POST,
        .handler = on_server_set};
    httpd_register_uri_handler(server, &server_end_point_config);



    httpd_uri_t apikey_end_point_config = {
        .uri = "/api/apikey",
        .method = HTTP_POST,
        .handler = on_apikey_set};
    httpd_register_uri_handler(server, &apikey_end_point_config);


    httpd_uri_t deviceid_end_point_config = {
        .uri = "/api/deviceid",
        .method = HTTP_POST,
        .handler = on_deviceid_set};
    httpd_register_uri_handler(server, &deviceid_end_point_config);


    httpd_uri_t start_end_point_config = {
        .uri = "/api/start",
        .method = HTTP_GET,
        .handler = on_start_request};
    httpd_register_uri_handler(server, &start_end_point_config);

    httpd_uri_t gateway_end_point_config = {
        .uri = "/api/gateway",
        .method = HTTP_POST,
        .handler = on_gateway_set};
    httpd_register_uri_handler(server, &gateway_end_point_config);

    httpd_uri_t checkbox_end_point_config = {
        .uri = "/api/checkbox",
        .method = HTTP_POST,
        .handler = on_checkbox_set};
    httpd_register_uri_handler(server, &checkbox_end_point_config);


    httpd_uri_t first_end_point_config = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_url_hit};
    httpd_register_uri_handler(server, &first_end_point_config);




}
