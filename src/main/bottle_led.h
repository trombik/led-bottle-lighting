#if !defined(__BOTTLE_LED__H__)
#define __BOTTLE_LED__H__

#include <driver/gpio.h>
#include <esp_err.h>

esp_err_t bottle_led_init();

esp_err_t bottle_led_config(const gpio_num_t gpio_num);

esp_err_t bottle_led_set_duty_and_update(const uint8_t value);

uint8_t bottle_led_get_duty();

esp_err_t bottle_led_resume();

esp_err_t bottle_led_pause();

#endif
