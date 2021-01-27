#if !defined(__BOTTLE_LED__H__)
#define __BOTTLE_LED__H__

#include <driver/gpio.h>
#include <esp_err.h>

esp_err_t bottle_led_init();

esp_err_t bottle_led_config(gpio_num_t gpio_num);

esp_err_t bottle_led_update_duty(uint8_t duty);

esp_err_t bottle_led_stop();

#endif
