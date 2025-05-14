# Building
```shell
pio run
```

# Flashing
```shell
PIO_WIFI_SSID=SomeSSID PIO_WIFI_PASSWORD=SomePassword pio run -t upload
```

# Serial Monitor
```shell
pio run -t monitor
```
can also be combined with flashing:
```shell
PIO_WIFI_SSID=SomeSSID PIO_WIFI_PASSWORD=SomePassword pio run -t upload -t monitor
```