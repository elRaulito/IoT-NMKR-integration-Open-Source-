/*
 * server.c
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


#define TAG "server"


extern const char indiceStart[] asm("_binary_index_html_start");
extern const char wifiStart[] asm("_binary_wifi_html_start");
extern const char settsStart[] asm("_binary_settings_html_start");
extern const char sceltaStart[] asm("_binary_scelta_html_start");
extern const char numeroStart[] asm("_binary_number_html_start");

extern const char BootCssStart[] asm("_binary_bootstrap_min_css_start");
extern const char queryJsStart[] asm("_binary_jquery_min_js_start");
extern const char BootJsStart[] asm("_binary_bootstrap_min_js_start");

extern const char piantaStart[] asm("_binary_pianta_gif_start");
extern const char piantaFine[] asm("_binary_pianta_gif_end");


static esp_err_t on_url_hit(httpd_req_t *req)
{
	const unsigned int imgSize = piantaFine - piantaStart;




   printf("%s",req->uri);
    char path[600];
    sprintf(path, "/spiffs%s", req->uri);
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    if (strcmp(path, "/spiffs/") == 0)
    {
        sprintf(path, "/spiffs/%s", "index.html");
    }
    //style.css
    printf("1 punto\n");
    printf("%s\n",path);
    char *ptr = strrchr(path, '.');


    if (strcmp(ptr, ".css") == 0)
    {

        httpd_resp_set_type(req, "text/css");
    }

    if (strcmp(ptr, ".gif") == 0)
    {

        httpd_resp_set_type(req, "image/gif");
        httpd_resp_set_status(req,"200");

    }

    if (strcmp(ptr, ".map") == 0)
    {

        httpd_resp_send_404(req);
        return ESP_OK;
    }

    if (strcmp(req->uri, "/") == 0)
    {
    	printf("ti do il sito da flash\n");
        httpd_resp_sendstr(req,indiceStart);


        return ESP_OK;
    }

    if (strcmp(req->uri, "/index.html") == 0)
    {
    	printf("ti do il sito da flash\n");
        httpd_resp_sendstr(req,indiceStart);




        return ESP_OK;
    }

    if (strcmp(req->uri, "/wifi.html") == 0)
    {

        httpd_resp_sendstr(req,wifiStart);
        printf("ti do il sito da flash\n");




        return ESP_OK;
    }
    if (strcmp(req->uri, "/scelta.html") == 0)
    {

        httpd_resp_sendstr(req,sceltaStart);
        printf("ti do il sito da flash\n");




        return ESP_OK;
    }
    if (strcmp(req->uri, "/number.html") == 0)
    {

        httpd_resp_sendstr(req,numeroStart);
        printf("ti do il sito da flash\n");




        return ESP_OK;
    }
    if (strcmp(req->uri, "/img/pianta.gif") == 0)
    {

    //	httpd_resp_set_hdr(req,"Content-Length","100");
    	httpd_resp_send(req,piantaStart,imgSize);
    	printf("size=%d",imgSize);
        //httpd_resp_sendstr(req,piantaStart);
    	//httpd_resp_sendstr(req,NULL);

        printf("ti do il sito da flash\n");




        return ESP_OK;
    }

    if (strcmp(req->uri, "/settings.html") == 0)
    {

        httpd_resp_sendstr(req,settsStart);
        printf("ti do il sito da flash\n");




        return ESP_OK;
    }
    if (strcmp(req->uri, "/boot/css/bootstrap.min.css") == 0)
    {

        httpd_resp_sendstr(req,BootCssStart);
        printf("ti do il sito da flash\n");



        return ESP_OK;
    }



    if (strcmp(req->uri, "/boot/js/bootstrap.min.js") == 0)
    {

        httpd_resp_sendstr(req,BootJsStart);
        printf("ti do il sito da flash\n");



        return ESP_OK;
    }
    if (strcmp(req->uri, "/boot/jquery/jquery.min.js") == 0)
    {

        httpd_resp_sendstr(req,queryJsStart);
        printf("ti do il sito da flash\n");



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

    char *bufferserver=NULL;
    char *bufferpass=NULL;

    cJSON *server = cJSON_GetObjectItem(payload, "wifiserver");
    cJSON *pass = cJSON_GetObjectItem(payload, "passserver");



    bufferserver=malloc(sizeof(server->valuestring));
    strcpy(bufferserver, server->valuestring);


    bufferpass=malloc(sizeof(pass->valuestring));
    strcpy(bufferpass, pass->valuestring);





    nvs_flash_init();
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


static esp_err_t on_name_set(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[50];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    cJSON *payload = cJSON_Parse(buf);

    char *name=NULL;


    cJSON *nome = cJSON_GetObjectItem(payload, "name");


    name=malloc(sizeof(nome->valuestring));
    strcpy(name, nome->valuestring);


    nvs_flash_init();
    nvs_handle_t nvs;
    nvs_open("PlantCreds", NVS_READWRITE, &nvs);
    printf("la pianta si chiama %s\n",name);
    nvs_set_str(nvs, "nome", name);
    nvs_close(nvs);
    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);



    cJSON_Delete(payload);



    return ESP_OK;


}

static esp_err_t on_number_set(httpd_req_t *req)
{
    ESP_LOGI(TAG, "url %s was hit", req->uri);
    char buf[50];
    memset(&buf, 0, sizeof(buf));
    httpd_req_recv(req, buf, req->content_len);
    cJSON *payload = cJSON_Parse(buf);

    char *number=NULL;


    cJSON *numero = cJSON_GetObjectItem(payload, "numero");


    number=malloc(sizeof(numero->valuestring));
    strcpy(number, numero->valuestring);


    nvs_flash_init();
    nvs_handle_t nvs;
    nvs_open("PlantCreds", NVS_READWRITE, &nvs);
    printf("il numero di telefono e' %s\n",number);
    nvs_set_str(nvs, "numero", number);
    nvs_close(nvs);
    httpd_resp_set_status(req, "204 NO CONTENT");
    httpd_resp_send(req, NULL, 0);



    cJSON_Delete(payload);



    return ESP_OK;


}


char message[200];
int luce=0;
int soil=0;
static esp_err_t on_get_light(httpd_req_t *req)
{



    sprintf(message, "{\"light\":%d}", luce);
    httpd_resp_send(req, message, strlen(message));

    return ESP_OK;
}

extern int IQA;
extern float co,so2,o3,no2,pm1,pm25,pm10;
extern int limone;
static esp_err_t on_get_quality(httpd_req_t *req)
{



    sprintf(message, "{\"IQA\":%d,\"co\":%f,\"so2\":%f,\"o3\":%f,\"no2\":%f,\"pm1\":%f,\"pm25\":%f,\"pm10\":%f,\"limone\":%d   }", IQA,co,so2,o3,no2,pm1,pm25,pm10,limone);
    httpd_resp_send(req, message, strlen(message));

    return ESP_OK;
}


static esp_err_t on_get_soil(httpd_req_t *req)
{



    sprintf(message, "{\"soil\":%d}", soil);
    httpd_resp_send(req, message, strlen(message));

    return ESP_OK;
}




extern int Temperature;
extern int Humidity;
static esp_err_t on_get_temp(httpd_req_t *req)
{



    sprintf(message, "{\"temp\":%d}", Temperature);
    httpd_resp_send(req, message, strlen(message));

    return ESP_OK;
}

static esp_err_t on_get_hum(httpd_req_t *req)
{



    sprintf(message, "{\"hum\":%d}", Humidity);
    httpd_resp_send(req, message, strlen(message));

    return ESP_OK;
}

void RegisterEndPoints(void)
{

    httpd_handle_t server = NULL;


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




    httpd_uri_t light_end_point_config = {
        .uri = "/api/light",
        .method = HTTP_GET,
        .handler = on_get_light};
    httpd_register_uri_handler(server, &light_end_point_config);

    httpd_uri_t soil_end_point_config = {
        .uri = "/api/soil",
        .method = HTTP_GET,
        .handler = on_get_soil};
    httpd_register_uri_handler(server, &soil_end_point_config);


    httpd_uri_t temp_end_point_config = {
        .uri = "/api/temp",
        .method = HTTP_GET,
        .handler = on_get_temp};
    httpd_register_uri_handler(server, &temp_end_point_config);

    httpd_uri_t hum_end_point_config = {
        .uri = "/api/hum",
        .method = HTTP_GET,
        .handler = on_get_hum};
    httpd_register_uri_handler(server, &hum_end_point_config);

    httpd_uri_t quality_end_point_config = {
        .uri = "/api/quality",
        .method = HTTP_GET,
        .handler = on_get_quality};
    httpd_register_uri_handler(server, &quality_end_point_config);


    httpd_uri_t server_end_point_config = {
        .uri = "/api/server",
        .method = HTTP_POST,
        .handler = on_server_set};
    httpd_register_uri_handler(server, &server_end_point_config);



    httpd_uri_t name_end_point_config = {
        .uri = "/api/name",
        .method = HTTP_POST,
        .handler = on_name_set};
    httpd_register_uri_handler(server, &name_end_point_config);

    httpd_uri_t number_end_point_config = {
        .uri = "/api/numero",
        .method = HTTP_POST,
        .handler = on_number_set};
    httpd_register_uri_handler(server, &number_end_point_config);


    httpd_uri_t first_end_point_config = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_url_hit};
    httpd_register_uri_handler(server, &first_end_point_config);




}
