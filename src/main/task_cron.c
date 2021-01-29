#include <esp_log.h>
#include <cron.h>

#include "task_cron.h"
#include "bottle_led.h"

static cron_job *cron_jobs[2];
static const char *tag = "task_cron";

static
void callback_turn_off(cron_job *job)
{
    esp_err_t err;

    ESP_LOGI(tag, "Time to turn off");
    if ((err = bottle_led_pause()) != ESP_OK) {
        ESP_LOGE(tag, "bottle_led_pause(): %s", esp_err_to_name(err));
    }
}

static
void callback_turn_on(cron_job *job)
{
    esp_err_t err;

    ESP_LOGI(tag, "Time to turn on");
    if ((err = bottle_led_resume()) != ESP_OK) {
        ESP_LOGE(tag, "bottle_led_resume(): %s", esp_err_to_name(err));
    }
}

esp_err_t cron_init(void)
{
    esp_err_t err = ESP_FAIL;

    cron_job_init();
    cron_jobs[0] = cron_job_create(CONFIG_PROJECT_CRON_JOB_TURN_ON, callback_turn_on, NULL);
    if (cron_jobs[0] == NULL) {
        ESP_LOGE(tag, "cron_job_create(): failed");
        err = ESP_FAIL;
        goto fail;
    }
    cron_jobs[1] = cron_job_create(CONFIG_PROJECT_CRON_JOB_TURN_OFF, callback_turn_off, NULL);
    if (cron_jobs[1] == NULL) {
        ESP_LOGE(tag, "cron_job_create(): failed");
        err = ESP_FAIL;
        goto fail;
    }
    err = ESP_OK;
fail:
    return err;
}
