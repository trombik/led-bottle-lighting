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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_idf_lib_helpers.h>
#include <led_strip_spi.h>
#include <led_effect.h>

#include "bottle_led_spi.h"

#define N_PIXEL CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI_N_PIXEL
#define DELAY_MS    (100)

static char tag[] = "bottle_led_spi";
led_strip_spi_t strip = LED_STRIP_SPI_DEFAULT();
TaskHandle_t task_handle;
#if HELPER_TARGET_IS_ESP32
spi_device_handle_t device_handle;
#endif

static esp_err_t rainbow() {
    static uint8_t pos = 0;
    uint32_t c;
    esp_err_t err;
    rgb_t color;

    c = led_effect_color_wheel(pos);
    color.r = (c >> 16) & 0xff;
    color.g = (c >> 8)  & 0xff;
    color.b =  c        & 0xff;
    ESP_LOGD(tag, "r: 0x%02x g: 0x%02x b: 0x%02x", color.r, color.g, color.b);

    if ((err = led_strip_spi_fill(&strip, 0, strip.length, color)) != ESP_OK) {
        ESP_LOGE(tag, "led_strip_spi_fill(): %s", esp_err_to_name(err));
        goto fail;
    }
    pos += 1;
fail:
    return err;
}

static void task_led_strip_spi(void *pvParameters)
{
    esp_err_t err;

    while (1) {
        err = rainbow();
        if (err != ESP_OK) {
            ESP_LOGE(tag, "rainbow(): %s", esp_err_to_name(err));
            continue;
        }
        err = led_strip_spi_flush(&strip);
        if (err != ESP_OK) {
            ESP_LOGE(tag, "led_strip_spi_flush(): %s", esp_err_to_name(err));
            continue;
        }
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
    }
}

esp_err_t bottle_led_spi_init(void)
{
    /* NOOP. all the configuration is done in bottle_led_spi_config() */
    return ESP_OK;
}

esp_err_t bottle_led_spi_config(const gpio_num_t gpio_num)
{
    esp_err_t err;

    led_strip_spi_install();
    strip.length = N_PIXEL;
#if HELPER_TARGET_IS_ESP32
    strip.device_handle = device_handle;
    strip.max_transfer_sz = LED_STRIP_SPI_BUFFER_SIZE(N_PIXEL);
    strip.clock_speed_hz = 1000000 * 10;
    strip.mosi_io_num = gpio_num;
    strip.sclk_io_num = CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI_SCLK_PIN;
    strip.mosi_io_num = CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI_MOSI_PIN;
#endif // HELPER_TARGET_IS_ESP32

#if HELPER_TARGET_IS_ESP8266
    strip.clk_div = SPI_10MHz_DIV;
#endif // HELPER_TARGET_IS_ESP8266

    ESP_LOGI(tag, "Initializing LED strip");
    err = led_strip_spi_init(&strip);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "led_strip_spi_init(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

esp_err_t bottle_led_spi_start(void)
{
    ESP_LOGI(tag, "starting task_led_strip_spi()");
    if (xTaskCreate(task_led_strip_spi, "task_led_strip_spi", configMINIMAL_STACK_SIZE * 5, NULL, 5, &task_handle) != pdPASS) {
        ESP_LOGE(tag, "xTaskCreate(): failed to create task");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void bottle_led_callback_spi(void)
{
    /* not implemented yet */
    return;
}

esp_err_t bottle_led_spi_off(void)
{
    esp_err_t err = ESP_FAIL;
    rgb_t color = {
        .r = 0x00,
        .g = 0x00,
        .b = 0x00
    };

    if (task_handle == NULL) {
        ESP_LOGI(tag, "task_led_strip_spi is not running");
        goto fail;
    }
    vTaskSuspend(task_handle);

    err = led_strip_spi_fill(&strip, 0, strip.length, color);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "led_strip_spi_fill(): %s", esp_err_to_name(err));
        goto fail;
    }
    err = led_strip_spi_flush(&strip);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "led_strip_spi_flush(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

esp_err_t bottle_led_spi_on(void)
{
    vTaskResume(task_handle);
    return ESP_OK;
}
