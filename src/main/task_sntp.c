#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include <esp_sntp.h>

#include "task_sntp.h"

#define SETENV_OVERWRITE    (1)

static const char *tag = "task_sntp";

static void callback_time_sync(struct timeval *tv)
{
    char strftime_buf[64];
    time_t now_unix;
    struct tm timeinfo;

    if (time(&now_unix) == -1) {
        ESP_LOGE(tag, "time(): failed");
        goto fail;
    }
    if (localtime_r(&now_unix, &timeinfo) == NULL) {
        ESP_LOGE(tag, "localtime_r(): failed");
        goto fail;
    }
    if (strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo) == 0) {
        ESP_LOGE(tag, "strftime(): failed");
        goto fail;
    }
    ESP_LOGI(tag, "Time synced (TZ %s): %s", CONFIG_PROJECT_SNTP_TZ, strftime_buf);
fail:
    return;
}

static esp_err_t task_sntp_init(void) {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, CONFIG_PROJECT_SNTP_SERVER_NAME);
    sntp_set_time_sync_notification_cb(callback_time_sync);
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    sntp_init();
    return ESP_OK;
}

esp_err_t task_sntp_start(void)
{
    int retry = 0;
    const int retry_max = 10;
    esp_err_t err;

    if (setenv("TZ", CONFIG_PROJECT_SNTP_TZ, SETENV_OVERWRITE) != 0) {
        ESP_LOGE(tag, "setenv(): failed");
        err = ESP_FAIL;
        goto fail;
    }
    tzset();

    if ((err = task_sntp_init()) != ESP_OK) {
        ESP_LOGE(tag, "task_sntp_init(): %s", esp_err_to_name(err));
        goto fail;
    }
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_max) {
        ESP_LOGI(tag, "Waiting for system time to be set... (%d/%d)", retry, retry_max);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    if (retry >= retry_max) {
        ESP_LOGW(tag, "Failed to sync time within retry_max limit");
        err = ESP_FAIL;
        goto fail;
    }
    ESP_LOGI(tag, "Successfully synced");
fail:
    return err;
}
