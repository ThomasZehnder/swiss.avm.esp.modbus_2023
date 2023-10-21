How to build PlatformIO based project
=====================================

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-espressif8266/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Change directory to example
$ cd platform-espressif8266/examples/arduino-webserver

# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Build specific environment
$ pio run -e nodemcuv2

# Upload firmware for the specific environment
$ pio run -e nodemcuv2 --target upload

# Clean build files
$ pio run --target clean
```


## exception decoding
see [https://www.dzombak.com/blog/2021/10/ESP8266-PlatformIO-Serial-Console-Monitoring-with-Exception-Decoding.html]

first add "monitor_filters = time, colorize, log2file, esp32_exception_decoder" to platformio.ini file
or only = esp8266_exception_decoder, default

run "pio device monitor" in Pio CLI

try to run  "pio run -t monitor"