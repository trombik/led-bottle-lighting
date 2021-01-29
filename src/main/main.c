#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_err.h>
#include <esp_log.h>

#include "bottle_led.h"

#if defined CONFIG_PROJECT_CONNECT_WIFI
#include "wifi_connect.h"
#endif // CONFIG_PROJECT_CONNECT_WIFI

#if defined(CONFIG_PROJECT_SNTP_ENABLE)
#include "task_sntp.h"
#endif // CONFIG_PROJECT_SNTP_ENABLE

#if defined(CONFIG_PROJECT_CRON_ENABLE)
#include <cron.h>
#include "task_cron.h"
#endif // CONFIG_PROJECT_CRON_ENABLE

#define GPIO_TOUCH  CONFIG_PROJECT_GPIO_TOUCH
#define GPIO_LED  CONFIG_PROJECT_GPIO_LED
#define ESP_INTR_FLAG_DEFAULT   0

static xQueueHandle queue_gpio_event = NULL;
static char tag[] = "main";

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(queue_gpio_event, &gpio_num, NULL);
}

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
        ESP_LOGE(tag, "gpio_config(): %s", esp_err_to_name(err));
        goto fail;
    }
    queue_gpio_event = xQueueCreate(10, sizeof(uint32_t));
    if (queue_gpio_event == NULL) {
        err = ESP_FAIL;
        ESP_LOGE(tag, "xQueueCreate()");
        goto fail;
    }
    err = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "gpio_install_isr_service(): %s", esp_err_to_name(err));
        goto fail;
    }
    err = gpio_isr_handler_add(GPIO_TOUCH, gpio_isr_handler, (void*) GPIO_TOUCH);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "gpio_isr_handler_add(): %s", esp_err_to_name(err));
        goto fail;
    }

fail:
    return err;
}

void task_handle_gpio_intr(void* arg)
{
    uint8_t duty = 0;
    uint8_t current_duty = 0;
    uint32_t io_num;
    esp_err_t err;

    while (1) {
        if(xQueueReceive(queue_gpio_event, &io_num, portMAX_DELAY)) {
            switch (io_num) {
            case GPIO_TOUCH:
                ESP_LOGI(tag, "touch sensor became HIGH");
                duty = get_next_duty();
                current_duty = bottle_led_get_duty();
                ESP_LOGI(tag, "duty: %d (cuurent: %d)", duty, current_duty);
                err = bottle_led_set_duty_and_update(duty);
                if (err != ESP_OK) {
                    ESP_LOGE(tag, "bottle_led_set_duty_and_update(): %s", esp_err_to_name(err));
                }
                break;
            default:
                ESP_LOGI(tag, "unknown GPIO intrrupt: io_num = %d", io_num);
            }
        }
    }
}

void app_main(void)
{
    esp_err_t err = ESP_FAIL;
    uint8_t duty;

    ESP_LOGI(tag, "io_init()");
    err = io_init();
    if (err != ESP_OK) {
        ESP_LOGE(tag, "io_init()");
        goto fail;
    }

    ESP_LOGI(tag, "bottle_led_init()");
    err = bottle_led_init();
    if (err != ESP_OK) {
        goto fail;
    }
    ESP_LOGI(tag, "bottle_led_config()");
    err = bottle_led_config(GPIO_LED);
    if (err != ESP_OK) {
        goto fail;
    }

    duty = get_next_duty();
    err = bottle_led_set_duty_and_update(duty);
    if (err != ESP_OK) {
        ESP_LOGE(tag, "bottle_led_set_duty_and_update(): %s", esp_err_to_name(err));
        goto fail;
    }

    xTaskCreate(task_handle_gpio_intr, "task_handle_gpio_intr", 2048, NULL, 10, NULL);

#if defined(CONFIG_PROJECT_CONNECT_WIFI)
    err = wifi_init();
    if (err != ESP_OK) {
        ESP_LOGE(tag, "wifi_init(): %s", esp_err_to_name(err));
    } else {
        err = wifi_connect();
        if (err != ESP_OK) {
            ESP_LOGE(tag, "wifi_connect(): %s", esp_err_to_name(err));
        }
    }
#endif // CONFIG_PROJECT_CONNECT_WIFI

#if defined(CONFIG_PROJECT_SNTP_ENABLE)
    task_sntp_start();
#endif // CONFIG_PROJECT_SNTP_ENABLE

#if defined(CONFIG_PROJECT_CRON_ENABLE)
    ESP_LOGI(tag, "Initializing cron jobs");
    err = cron_init();
    if (err != ESP_OK) {
        ESP_LOGE(tag, "cron_init(): %s", esp_err_to_name(err));
        goto fail;
    }
    ESP_LOGI(tag, "Starting cron");
    if (cron_start() != 0) {
        ESP_LOGE(tag, "cron_start(): failed");
        err = ESP_FAIL;
        goto fail;
    }
#endif // CONFIG_PROJECT_CRON_ENABLE

    ESP_LOGI(tag, "Finished initialization");
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
fail:
    ESP_LOGE(tag, "%s", esp_err_to_name(err));
}
