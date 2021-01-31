/*
 * Copyright (c) 2021 Tomoyuki Sakurai <y@trombik.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "bottle_led.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <esp_err.h>

static char tag[] = "bottle_led";
uint8_t bottle_led_single_color_duty = 0;
xSemaphoreHandle bottle_led_single_color_duty_mutex = NULL;

static uint8_t get_next_duty()
{
    /* intentionally limit the max brightness to 200 to save the lifetime of
     * the LED, and save power */
#define N_DUTY  (6)
    const uint8_t duties[N_DUTY] = {
        0, 25, 50, 100, 175, 200
    };
    static int index = 0;

    index = index + 1 >= N_DUTY ? 0 : index + 1;
    return duties[index];
}

uint8_t bottle_led_single_color_get_duty()
{
    return bottle_led_single_color_duty;
}

esp_err_t bottle_led_single_color_init()
{
    esp_err_t err;

    if (bottle_led_single_color_duty_mutex == NULL) {
        bottle_led_single_color_duty_mutex = xSemaphoreCreateMutex();
        if (bottle_led_single_color_duty_mutex == NULL) {
            ESP_LOGE(tag, "xSemaphoreCreateMutex(): failed");
            err = ESP_FAIL;
            goto fail;
        }
    }
    err = ESP_OK;
fail:
    return err;
}

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
#include "hal/esp32/bottle_led_single_color.c"
#elif defined(CONFIG_IDF_TARGET_ESP8266)
#include "hal/esp8266/bottle_led_single_color.c"
#else
#error "The target is not supported"
#endif

esp_err_t bottle_led_single_color_set_duty_and_update(const uint8_t value)
{
    esp_err_t err;

    if (xSemaphoreTake(bottle_led_single_color_duty_mutex, portMAX_DELAY) == pdTRUE) {
        bottle_led_single_color_duty = value;
        err = bottle_led_single_color_update_duty(bottle_led_single_color_duty);
        xSemaphoreGive(bottle_led_single_color_duty_mutex);
    } else {
        ESP_LOGE(tag, "xSemaphoreTake(): timeout");
        err = ESP_FAIL;
    }
    return err;
}

void bottle_led_callback_single_color(void)
{
    uint8_t duty, current_duty;
    esp_err_t err;

    duty = get_next_duty();
    current_duty = bottle_led_single_color_get_duty();
    ESP_LOGI(tag, "duty: %d (cuurent: %d)", duty, current_duty);
    err = bottle_led_single_color_set_duty_and_update(duty);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "bottle_led_single_color_set_duty_and_update(): %s", esp_err_to_name(err));
    }
}

esp_err_t bottle_led_single_color_start(void)
{
    return ESP_OK;
}
