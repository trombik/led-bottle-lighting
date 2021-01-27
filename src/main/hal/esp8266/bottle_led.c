#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>
#include <driver/pwm.h>
#include <esp_err.h>
#include <esp_log.h>

#define TAG "hal/esp8266/bottle_led.c"

#define PWM_FREQ    (5000)
#define PWM_T       (1000 * 1000 / PWM_FREQ) // in usec
#if PWM_T < 20
#error "PWM_T must me more than 20 usec"
#endif
#define PWM_CHANNEL (1)

esp_err_t bottle_led_init()
{
    return ESP_OK;
}

esp_err_t bottle_led_config(const gpio_num_t gpio_num)
{
    esp_err_t err = ESP_FAIL;

    err = pwm_init(PWM_T, 0, PWM_CHANNEL, &gpio_num);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "pwm_init(): %s", esp_err_to_name(err));
        goto fail;
    }

    err = pwm_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "pwm_start(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

esp_err_t bottle_led_update_duty(const uint8_t duty)
{
    esp_err_t err = ESP_FAIL;

    err = pwm_set_duty(PWM_CHANNEL, duty);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "pwm_set_duty(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

esp_err_t bottle_led_stop()
{
    return bottle_led_update_duty(0);
}
