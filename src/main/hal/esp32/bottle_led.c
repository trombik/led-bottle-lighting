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
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_log.h>

#define DUTY_RESOLUTION LEDC_TIMER_8_BIT
#define DUTY_RESOLUTION_MAX (1 << DUTY_RESOLUTION)
#define PWM_FREQ    CONFIG_PROJECT_PWM_FREQ_HZ

#if defined(CONFIG_IDF_TARGET_ESP32S2)

/* ESP32S2 does not have high speed LED controller */
#define LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#else
#define LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
#endif

ledc_timer_config_t ledc_timer = {
    .duty_resolution = DUTY_RESOLUTION,
    .freq_hz = PWM_FREQ,
    .speed_mode = LEDC_SPEED_MODE,
    .timer_num = LEDC_TIMER_1,
    .clk_cfg = LEDC_AUTO_CLK,
};

ledc_channel_config_t ledc_channel = {
    .channel    = LEDC_CHANNEL_0,
    .duty       = 0,
    .gpio_num   = 0,
    .speed_mode = LEDC_SPEED_MODE,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER_1
};

esp_err_t bottle_led_config(const gpio_num_t gpio_num)
{
    esp_err_t err = ESP_FAIL;

    ledc_channel.gpio_num = gpio_num;

    err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "ledc_timer_config(): %s", esp_err_to_name(err));
        goto fail;
    }

    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "ledc_channel_config(): %s", esp_err_to_name(err));
        goto fail;
    }
    err = ledc_fade_func_install(0);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "ledc_fade_func_install(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

static esp_err_t bottle_led_update_duty(const uint8_t duty)
{
    esp_err_t err = ESP_FAIL;

    err = ledc_set_duty_and_update(ledc_channel.speed_mode, ledc_channel.channel, duty, DUTY_RESOLUTION_MAX);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "ledc_set_duty_and_update(): %s", esp_err_to_name(err));
        goto fail;
    }

fail:
    return err;
}

esp_err_t bottle_led_resume()
{
    esp_err_t err;
    uint8_t duty;

    duty = bottle_led_get_duty();
    ESP_LOGD(tag, "restoring duty to %d", duty);

    err = bottle_led_update_duty(duty);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "bottle_led_update_duty(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

esp_err_t bottle_led_pause()
{
    esp_err_t err;

    ESP_LOGD(tag, "Set LED duty to 0");
    err = bottle_led_update_duty(0);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "bottle_led_update_duty(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}
