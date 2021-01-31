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
    if ((err = bottle_led_off()) != ESP_OK) {
        ESP_LOGE(tag, "bottle_led_off(): %s", esp_err_to_name(err));
    }
}

static
void callback_turn_on(cron_job *job)
{
    esp_err_t err;

    ESP_LOGI(tag, "Time to turn on");
    if ((err = bottle_led_on()) != ESP_OK) {
        ESP_LOGE(tag, "bottle_led_on(): %s", esp_err_to_name(err));
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
