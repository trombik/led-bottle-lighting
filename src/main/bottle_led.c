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

#include <esp_err.h>
#include <esp_log.h>

#include "bottle_led.h"

#if defined(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR)
#include "bottle_led_single_color.h"
#elif defined(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI)
#include "bottle_led_spi.h"
#else
#error "CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI or CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR must be defined"
#endif

esp_err_t bottle_led_init(void)
{
#if defined(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR)
    return bottle_led_single_color_init();
#elif defined(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI)
    return bottle_led_spi_init();
#else
#error "BUG: no fucntion defined in bottle_led_init()"
#endif
}

esp_err_t bottle_led_config(void)
{
#if defined(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR)
    return bottle_led_single_color_config(CONFIG_PROJECT_SINGLE_COLOR_GPIO_LED);
#elif defined(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI)
    return bottle_led_spi_config();
#else
#error "BUG: no fucntion defined in bottle_led_config()"
#endif
}

esp_err_t bottle_led_start()
{
#if defined(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR)
    return bottle_led_single_color_start();
#elif defined(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI)
    return bottle_led_spi_start();
#else
#error "BUG: no fucntion defined in bottle_led_start()"
#endif
}

void bottle_led_callback(void)
{
#if defined(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR)
    bottle_led_callback_single_color();
#elif defined(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI)
    bottle_led_callback_spi();
#else
#error "BUG: no callback defined in bottle_led_callback()"
#endif
}

esp_err_t bottle_led_off(void)
{
#if defined(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR)
    return bottle_led_single_color_off();
#elif defined(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI)
    return bottle_led_spi_off();
#else
#error "BUG: no function defined in bottle_led_off()"
#endif
}

esp_err_t bottle_led_on(void)
{
#if defined(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR)
    return bottle_led_single_color_on();
#elif defined(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI)
    return bottle_led_spi_on();
#else
#error "BUG: no function defined in bottle_led_on()"
#endif
}
