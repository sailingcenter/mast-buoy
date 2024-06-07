# WeeWX docker container for MAST Buoy
## Pre-requisites
* Docker / Docker Compose: https://docs.docker.com/compose/install/

## Getting started
```shell
docker compose build
docker compose up
```

## Sample request
* http://localhost:8089/weatherstation/updateweatherstation.php?windSpeedKph=22.629213&windDirectionDegrees=301&waveHeightFt=0.341356&wavePeriodSeconds=54.090321&gpsLong=-87.846039&gpsLat=43.040272&waterSpeedKn=0.017173&waterDirection=301.558167&rxSNR=-5&rxRSSI=-90

## Connecting to weewx container
* Using shell
```shell
docker exec -it mast-buoy-weewx-1 /bin/bash
```
* or use the Docker Desktop's Exec functionality to get a terminal

## Patching weewx.conf
* Start up container
```shell
docker compose build
docker compose up
```
* Copy the pre-patch and post-patch weewx.conf files
```shell
docker exec -it mast-buoy-weewx-1 /bin/bash -c 'cp weewx.conf.pre-patch /root/weewx-data/logs/'
docker exec -it mast-buoy-weewx-1 /bin/bash -c 'cp /root/weewx-data/weewx.conf /root/weewx-data/logs/'
```
* Modify `state/weewx-logs/weewx.conf`
* Update `weewx/weewx.conf.patch` file
```shell
diff state/weewx-logs/weewx.conf.pre-patch state/weewx-logs/weewx.conf > weewx/weewx.conf.patch
```