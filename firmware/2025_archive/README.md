# Polaris Remote Sailing Buoy.

Archive copy from https://github.com/awinkletoes/Polaris

The goal of this project is to transmit weather data from a remote buoy 2 miles offshore in Lake Michigan to the Milwaukee Community Sailing Center

## Sensors being used:

### IMU (BNO055) 
PoC: IMU, IMU Calibration
https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview
Use: 
  - Wave height measurement
  - Obtain z-axis movement
  - Double integrate for position data over time
  - Filter before integrating for both velocity and position
  - Real time filtering using butterworth filter
  - I2C connection to LilyGo 1262 via Qwiic connect
  - IMU calibration code will save the state in which IMU was at full 3 calibration and upload using EEPROM
  - Next steps would be to perform spectral analysis in addition to time domain

### Anemometer (Ecowitt WS80)
PoC: LilyGoSerial
https://www.ecowitt.com/shop/goodsDetail/126
Use: 
  - Wind speed and direction
  - Humidity
  - Air temperature
  - Using LoRa w/ 1276 chip to create BLE server and obtain rtl_433 data
  - Communicate to LoRa w/1262 chip via UART with data

### Paddle Wheel (Garmin Current Speed Paddle Wheel)
PoC: paddlewheel
Use:
  - Determine current speed near the buoy
  - Uses pulse counter on the LilyGo 1262

### GPS:
PoC: TinyGPS_Example
Use: 
  - Redundancy for location of buoy
  - UART communication to LilyGo 1262
  - Task based approach (claudobahn) 

Receive/transmit as well as TinyGPS examples pulled from here:
https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/tree/master

