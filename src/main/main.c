#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_err.h>
#include <esp_log.h>

#include "bottle_led.h"

#define TAG "main"
#define GPIO_TOUCH  5
#define ESP_INTR_FLAG_DEFAULT   0

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static esp_err_t io_init()
{
    esp_err_t err = ESP_FAIL;
    gpio_config_t io_config;

    io_config.intr_type = GPIO_INTR_POSEDGE;
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pin_bit_mask = 1ULL << GPIO_TOUCH;
    io_config.pull_up_en = 1;

    err = gpio_config(&io_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gpio_config(): %s", esp_err_to_name(err));
        goto fail;
    }
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    if (gpio_evt_queue == NULL) {
        err = ESP_FAIL;
        ESP_LOGE(TAG, "xQueueCreate()");
        goto fail;
    }
    err = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gpio_install_isr_service(): %s", esp_err_to_name(err));
        goto fail;
    }
    err = gpio_isr_handler_add(GPIO_TOUCH, gpio_isr_handler, (void*) GPIO_TOUCH);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gpio_isr_handler_add(): %s", esp_err_to_name(err));
        goto fail;
    }

fail:
    return err;
}

void task_handle_gpio_intr(void* arg)
{
    uint8_t duty = 0;
    esp_err_t err;
    uint32_t io_num;

    while (1) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            switch (io_num) {
            case GPIO_TOUCH:
                ESP_LOGI(TAG, "touch sensor became HIGH");
                duty += 25;
                duty = duty < 25 ? 0 : duty;
                ESP_LOGI(TAG, "duty: %d", duty);
                err = bottle_led_update_duty(duty);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "bottle_led_update_duty(): %s", esp_err_to_name(err));
                }
                break;
            default:
                ESP_LOGI(TAG, "unknown GPIO intrrupt: io_num = %d", io_num);
            }
        }
    }
}

void app_main(void)
{
    esp_err_t err = ESP_FAIL;

    ESP_LOGI(TAG, "io_init()");
    err = io_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "io_init()");
        goto fail;
    }

    ESP_LOGI(TAG, "bottle_led_init()");
    err = bottle_led_init();
    if (err != ESP_OK) {
        goto fail;
    }
    ESP_LOGI(TAG, "bottle_led_config()");
    err = bottle_led_config(4);
    if (err != ESP_OK) {
        goto fail;
    }

    err = bottle_led_update_duty(100);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "bottle_led_update_duty(): %s", esp_err_to_name(err));
        goto fail;
    }

    xTaskCreate(task_handle_gpio_intr, "task_handle_gpio_intr", 2048, NULL, 10, NULL);
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
fail:
    ESP_LOGE(TAG, "%s", esp_err_to_name(err));
}
