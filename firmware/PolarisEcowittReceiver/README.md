Imported from https://github.com/awinkletoes/Polaris/tree/main/Isolated%20Sensor%20Programs/LilyGoSerial/polaris-poc-ecowitt-ble-gateway-main

This PoC is based on the receiver example provided by the rtl_433_ESP library: https://github.com/NorthernMan54/rtl_433_ESP/tree/main/example/OOK_Receiver

The default configuration is for a LilyGo board w/ SX1276 @ 915MHz.

This PoC uses PlatformIO and can be built and run using the PlatformIO CLI by connecting a Heltec V2 board and running
```shell
pio run -t upload -t monitor
```

# Transceiver Module Wiring

## SX127X (Heltec) Transceiver Module

For the sample I had purchased a Heltec LoRa 32 433-510 Module, which has the SX127X transceiver module included on the board.

For reference I found that the board had the following connections

| SX127X (Heltec) PIN | ESP32 GPIO | Compiler Definition | Notes |
|---------------------|------------|---------------------|-------|
| N/A                 | 25         | ONBOARD_LED         |       |
| DIO0                | 26         | RF_MODULE_DIO0      |       |
| DIO1                | 35         | RF_MODULE_DIO1      |       |
| DIO2                | 34         | RF_MODULE_DIO2      |       |
| RST                 | 14         | RF_MODULE_RST       |       |
| CS                  | 18         | RF_MODULE_CS        |       |
| MOSI                | 27         | RF_MODULE_MOSI      |       |
| MISO                | 19         | RF_MODULE_MISO      |       |
| SCK                 | 5          | RF_MODULE_SCK       |       |

## SX127X (LilyGo) Transceiver Module

For the sample I had purchased a TTGO LoRa32-OLED v2.1.6 433Mhz Module, which has the SX127X transceiver module included on the board.

# Sample Outputs

## Heltec SX127X Transceiver

```
���������������������������������������������������������������������������������������������������������������������N:  
N: ****** setup ******
rtl_433_ESP(6): SX1278 gpio receive pin: 34
rtl_433_ESP(6): SX1278 receive frequency: 433.920013
rtl_433_ESP(6): # of device(s) configured 101
rtl_433_ESP(6): ssizeof(r_device): 108
rtl_433_ESP(6): cfg->devices size: 10908
rtl_433_ESP(6): # of device(s) enabled 100
rtl_433_ESP(7): Enabling rtl_433_DecoderTask
rtl_433_ESP(6): SX1278 SPI Config SCK: 5, MISO: 19, MOSI: 27, CS: 18
M       SX127x
rtl_433_ESP(6): SX1278 radio.begin() success!
rtl_433_ESP(6): SX1278 setOOK - success!
rtl_433_ESP(6): SX1278 setDataShapingOOK - success!
rtl_433_ESP(6): SX1278 setOokThresholdType - success!
rtl_433_ESP(6): SX1278 setOokPeakThresholdDecrement - success!
rtl_433_ESP(6): SX1278 setOokFixedOrFloorThreshold - success!
rtl_433_ESP(6): SX1278 setRSSIConfig - success!
rtl_433_ESP(6): SX1278 PREAMBLE_DETECT - success!
rtl_433_ESP(6): SX1278 setBitRate - success!
rtl_433_ESP(6): SX1278 setRxBandwidth - success!
rtl_433_ESP(6): SX1278 setCrcFiltering - success!
rtl_433_ESP(6): SX1278 setDirectSyncWord - success!
rtl_433_ESP(6): SX1278 disableBitSync - success!
rtl_433_ESP(6): SX1278 receiveDirect - success!
----- SX127x Status -----
RegOpMode: 0x2c
RegPacketConfig1: 0x00
RegPacketConfig2: 0x00
RegBitrateMsb: 0x03
RegBitrateLsb: 0xd0
RegRxBw: 0x01
RegAfcBw: 0x02
-------------------------
RegOokPeak: 0x08
RegOokFix: 0x50
RegOokAvg: 0x72
-------------------------
RegLna: 0x20
RegRxConfig: 0x08
RegRssiConfig: 0x07
-------------------------
RegDioMapping1: 0x00
----- SX127x Status -----
rtl_433_ESP(3): SX1278 Enabling rtl_433_ReceiverTask
N: ****** setup complete ******
rtl_433_ESP(6): Signal length: 382996, Gap length: 1619894, Signal RSSI: -51, train: 0, messageCount: 0, pulses: 294
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"No","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-51,"duration":382996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"No","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-51,"duration":382996}
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"No","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-51,"duration":382996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"No","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-51,"duration":382996}
rtl_433_ESP(6): # of messages decoded 2
 
rtl_433_ESP(7): RegOokFix Threshold Adjust noise count 176, RegOokFix 0x51
rtl_433_ESP(6): Signal length: 513996, Gap length: 11653283, Signal RSSI: -49, train: 1, messageCount: 1, pulses: 477
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":24.6,"humidity":84,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-49,"duration":513996}
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-49,"duration":513996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-49,"duration":513996}
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-49,"duration":513996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-49,"duration":513996}
rtl_433_ESP(6): # of messages decoded 5
 
rtl_433_ESP(6): Signal length: 121996, Gap length: 1093746, Signal RSSI: -72, train: 0, messageCount: 2, pulses: 182
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":21.5,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-72,"duration":121996}
rtl_433_ESP(6): # of messages decoded 3
 
rtl_433_ESP(6): Signal length: 670996, Gap length: 10037719, Signal RSSI: -55, train: 1, messageCount: 3, pulses: 813
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-55,"duration":670996}
N: Received message : {"model":"LaCrosse-TX141Bv3","id":152,"channel":1,"battery_ok":1,"temperature_C":23.6,"test":"Yes","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-55,"duration":670996}
rtl_433_ESP(6): data_output {"model":"LaCrosse-TX141W","id":168901,"channel":0,"battery_ok":1,"wind_avg_km_h":2.7,"wind_dir_deg":0,"test":0,"mic":"CRC","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-55,"duration":670996}
N: Received message : {"model":"LaCrosse-TX141W","id":168901,"channel":0,"battery_ok":1,"wind_avg_km_h":2.7,"wind_dir_deg":0,"test":0,"mic":"CRC","protocol":"LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth sensor","rssi":-55,"duration":670996}
rtl_433_ESP(6): # of messages decoded 2
```

## LilyGo SX127X Module

```
N: ****** setup ******
rtl_433_ESP(6): SX1278 gpio receive pin: 32
rtl_433_ESP(6): SX1278 receive frequency: 433.920013
rtl_433_ESP(6): # of device(s) configured 101
rtl_433_ESP(6): ssizeof(r_device): 108
rtl_433_ESP(6): cfg->devices size: 10908
rtl_433_ESP(6): # of device(s) enabled 100
M       SX127x
----- SX127x Status -----
RegOpMode: 0x24
RegPacketConfig1: 0x00
RegPacketConfig2: 0x00
RegBitrateMsb: 0x03
RegBitrateLsb: 0xd0
RegRxBw: 0x01
RegAfcBw: 0x02
-------------------------
RegOokPeak: 0x08
RegOokFix: 0x50
RegOokAvg: 0x72
-------------------------
RegLna: 0x23
RegRxConfig: 0x08
RegRssiConfig: 0x07
-------------------------
RegDioMapping1: 0x00
----- SX127x Status -----
N: ****** setup complete ******
rtl_433_ESP(6): Signal length: 180996, Gap length: 2825872, Signal RSSI: -66, train: 0, messageCount: 0, pulses: 182
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
N: Received message : {"model":"Acurite-Tower","id":2043,"channel":"B","battery_ok":1,"temperature_C":1.3,"humidity":94,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-66,"duration":180996}
rtl_433_ESP(6): # of messages decoded 3
rtl_433_ESP(6): Signal length: 182000, Gap length: 1508260, Signal RSSI: -63, train: 1, messageCount: 1, pulses: 182
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
rtl_433_ESP(6): data_output {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
N: Received message : {"model":"Acurite-Tower","id":5989,"channel":"A","battery_ok":1,"temperature_C":0.9,"humidity":127,"mic":"CHECKSUM","protocol":"Acurite 592TXR Temp/Humidity, 5n1 Weather Station, 6045 Lightning, 3N1, Atlas","rssi":-63,"duration":182000}
rtl_433_ESP(6): # of messages decoded 3
```
