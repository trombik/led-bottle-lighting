# Example project

## Building

### ESP32

```console
export IDF_PATH=~/github/esp-idf
export PATH=${PATH}:${IDF_PATH}/tools
export PATH=${PATH}:/usr/local/xtensa-esp32s2-elf/bin
export PATH=${PATH}:/usr/local/xtensa-esp32-elf/bin
export PATH=${HOME}/.espressif/python_env/idf4.3_py3.7_env/bin:${PATH}
export IDF_CCACHE_ENABLE=y
idf.py menuconfig
idf.py all
````

```console
idf.py set-target esp32s2
```

### ESP8266

```console
export IDF_PATH=~/github/ESP8266_RTOS_SDK
export PATH=${PATH}:${IDF_PATH}/tools
export PATH=${PATH}:/usr/local/xtensa-lx106-elf/bin
idf.py menuconfig
idf.py all
```
