set(srcs "main.c" "bottle_led.c")

if(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR)
    list(APPEND srcs "bottle_led_single_color.c")
endif()

if(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI)
    list(APPEND srcs "bottle_led_spi.c")
endif()

if(CONFIG_PROJECT_CONNECT_WIFI)
    list(APPEND srcs "wifi_connect.c")
endif()

if(CONFIG_PROJECT_SNTP_ENABLE)
    list(APPEND srcs "task_sntp.c")
endif()

if(CONFIG_PROJECT_CRON_ENABLE)
    list(APPEND srcs "task_cron.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
