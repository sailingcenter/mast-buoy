# WeeWX docker container for MAST Buoy
## Pre-requisites
* Docker / Docker Compose: https://docs.docker.com/compose/install/

## Getting started
```shell
docker compose build
docker compose up
```

## Sample request
* http://localhost:8089/weatherstation/updateweatherstation.php?windspeedmph=15&winddir=45

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