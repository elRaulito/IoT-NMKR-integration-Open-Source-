/*
 * camera.c
 *
 *  Created on: 10 nov 2020
 *      Author: Raul Rosa
 */

#include "mbedtls/base64.h"
#include "esp_err.h"
#include "esp_camera.h"
#include "esp_log.h"
#define TAG_CAM "camera"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

uint8_t *buffer;

esp_err_t camera_capture(){
        //capture a frame
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG_CAM, "Frame buffer could not be acquired");
            return ESP_FAIL;

        }

        buffer = calloc((fb->len + 2 - ((fb->len + 2) % 3)) / 3 * 4 + 1, sizeof(char));

        size_t buff_size = 0;
        	int err = mbedtls_base64_encode(buffer, (fb->len + 2 - ((fb->len + 2) % 3)) / 3 * 4 + 1, &buff_size, fb->buf, fb->len);
        	if (err != 0) {
        		ESP_LOGE(TAG_CAM, "error base64 encoding, error %d, buff size: %d", err, buff_size);

        	}else{
        		//all fine
        	}


        esp_camera_fb_return(fb);
        return ESP_OK;
}

void camera_start(){




    //settings for the camera
    static camera_config_t camera_config = {
        .pin_pwdn       = PWDN_GPIO_NUM,
        .pin_reset      = RESET_GPIO_NUM,
        .pin_xclk       = XCLK_GPIO_NUM,
        .pin_sscb_sda   = SIOD_GPIO_NUM,
        .pin_sscb_scl   = SIOC_GPIO_NUM,
        .pin_d7         = Y9_GPIO_NUM,
        .pin_d6         = Y8_GPIO_NUM,
        .pin_d5         = Y7_GPIO_NUM,
        .pin_d4         = Y6_GPIO_NUM,
        .pin_d3         = Y5_GPIO_NUM,
        .pin_d2         = Y4_GPIO_NUM,
        .pin_d1         = Y3_GPIO_NUM,
        .pin_d0         = Y2_GPIO_NUM,
        .pin_vsync      = VSYNC_GPIO_NUM,
        .pin_href       = HREF_GPIO_NUM,
        .pin_pclk       = PCLK_GPIO_NUM,

        .xclk_freq_hz   = 20000000,
        .ledc_timer     = LEDC_TIMER_0,
        .ledc_channel   = LEDC_CHANNEL_0,
        .pixel_format   = PIXFORMAT_JPEG,
        .frame_size     = FRAMESIZE_HVGA,//YOU CAN CHANGE THE SIZE OF THE FRAME FROM HERE
        .jpeg_quality   = 10,
        .fb_count       = 2
    };

//initialize camera
    esp_camera_init(&camera_config);
}
