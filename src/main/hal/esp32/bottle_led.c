#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_log.h>

#define TAG "hal/esp32/bottle_led.c"
#define GPIO_NUM    (4)
#define DUTY_RESOLUTION LEDC_TIMER_8_BIT
#define DUTY_RESOLUTION_MAX (1 << DUTY_RESOLUTION)

ledc_timer_config_t ledc_timer = {
    .duty_resolution = DUTY_RESOLUTION,
    .freq_hz = 5000,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .timer_num = LEDC_TIMER_1,
    .clk_cfg = LEDC_AUTO_CLK,
};

ledc_channel_config_t ledc_channel = {
    .channel    = LEDC_CHANNEL_0,
    .duty       = 0,
    .gpio_num   = 0,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER_1
};

esp_err_t bottle_led_init()
{
    return ESP_OK;
}

esp_err_t bottle_led_config(gpio_num_t gpio_num)
{
    esp_err_t err = ESP_FAIL;

    ledc_channel.gpio_num = gpio_num;

    err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_timer_config(): %s", esp_err_to_name(err));
        goto fail;
    }

    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_channel_config(): %s", esp_err_to_name(err));
        goto fail;
    }
    err = ledc_fade_func_install(0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_fade_func_install(): %s", esp_err_to_name(err));
        goto fail;
    }
fail:
    return err;
}

esp_err_t bottle_led_update_duty(uint8_t duty)
{
    esp_err_t err = ESP_FAIL;

    duty = duty > DUTY_RESOLUTION_MAX * 0.8 ? DUTY_RESOLUTION_MAX * 0.8 : duty;
    err = ledc_set_duty_and_update(ledc_channel.speed_mode, ledc_channel.channel, duty, DUTY_RESOLUTION_MAX);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_set_duty_and_update(): %s", esp_err_to_name(err));
        goto fail;
    }

fail:
    return err;
}

esp_err_t bottle_led_stop()
{
    return bottle_led_update_duty(0);
}
