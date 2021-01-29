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
#include <driver/gpio.h>
#include <driver/pwm.h>
#include <esp_err.h>
#include <esp_log.h>

#define PWM_FREQ    CONFIG_PROJECT_PWM_FREQ_HZ
#define PWM_T       (1000 * 1000 / PWM_FREQ) // in usec
#if PWM_T < 20
#error "PWM_T must be more than 20 usec"
#endif

/*
 * Number of PWM channel to use. must match the length of const variables in
 * bottle_led_config(). having no plan to support multiple LED strips, this
 * should always be 1.
 *
 * the official documentation is very misleading here: pwm_init() accepts
 * arrays of arguments except period, while claiming `pin_num` is _GPIO number
 * of PWM channel_.
 */
#define N_PWM_CHANNEL (1)

esp_err_t bottle_led_config(const gpio_num_t gpio_num)
{
    esp_err_t err = ESP_FAIL;
    uint32_t duty[N_PWM_CHANNEL] = {
        0
    };

    const uint32_t pin_num[N_PWM_CHANNEL] = {
        gpio_num
    };
    float phases[N_PWM_CHANNEL] = {
        0
    };

    err = pwm_init(PWM_T, duty, N_PWM_CHANNEL, pin_num);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "pwm_init(): %s", esp_err_to_name(err));
        goto fail;
    }

    err = pwm_set_phases(phases);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "pwm_set_phase(): %s", esp_err_to_name(err));
        goto fail;
    }

    err = pwm_start();
    if (err != ESP_OK) {
        ESP_LOGE(tag, "pwm_start(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

static esp_err_t bottle_led_update_duty(const uint8_t duty)
{
    esp_err_t err = ESP_FAIL;

    err = pwm_set_duty(0, duty);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "pwm_set_duty(): %s", esp_err_to_name(err));
        goto fail;
    }
    err = pwm_start();
    if (err != ESP_OK) {
        ESP_LOGE(tag, "pwm_start(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

esp_err_t bottle_led_resume()
{
    esp_err_t err;

    err = pwm_start();
    if (err != ESP_OK) {
        ESP_LOGE(tag, "pwm_start(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

esp_err_t bottle_led_pause()
{
    esp_err_t err;

    err = pwm_stop(1 << N_PWM_CHANNEL);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "pwm_stop(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}
