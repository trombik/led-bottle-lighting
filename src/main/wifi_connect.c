/*
 * include sdkconfig.h for `CONFIG_*`.
 *
 * XXX esp_err.h in esp-idf pulls sdkconfig, but one in ESP8266 RTOS SDK does
 * not.
 */
#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32S2) && !defined(CONFIG_IDF_TARGET_ESP8266)
#include <sdkconfig.h>
#endif

#include "wifi_connect.h"

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
#include "hal/esp32/wifi_connect.c"
#elif defined(CONFIG_IDF_TARGET_ESP8266)
#include "hal/esp8266/wifi_connect.c"
#else
#error "The target is not supported"
#endif
