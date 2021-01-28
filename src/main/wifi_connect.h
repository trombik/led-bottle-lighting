#if !defined(__WIFI_CONNECT__H__)
#define __WIFI_CONNECT__H__

#include <esp_err.h>

esp_err_t wifi_init(void);

esp_err_t wifi_connect(void);

esp_err_t wifi_disconnect(void);

#endif
