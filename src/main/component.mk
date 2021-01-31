#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_OBJS := main.o bottle_led.o

$(call compile_only_if,$(CONFIG_PROJECT_LED_TYPE_SINGLE_COLOR),bottle_led_single_color.o)
$(call compile_only_if,$(CONFIG_PROJECT_LED_TYPE_ADDRESSABLE_SPI),bottle_led_spi.o)
$(call compile_only_if,$(CONFIG_PROJECT_CONNECT_WIFI),wifi_connect.o)
$(call compile_only_if,$(CONFIG_PROJECT_SNTP_ENABLE),task_sntp.o)
$(call compile_only_if,$(CONFIG_PROJECT_CRON_ENABLE),task_cron.o)
