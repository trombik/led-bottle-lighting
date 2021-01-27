#include "bottle_led.h"

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
#include "hal/esp32/bottle_led.c"
#elif defined(CONFIG_IDF_TARGET_ESP8266)
#include "hal/esp8266/bottle_led.c"
#endif
