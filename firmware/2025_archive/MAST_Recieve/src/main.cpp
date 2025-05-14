#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include <RadioLib.h>
#include "LoRaBoards.h"
#include "utilities.h"

#include "buoy_data.pb.h"
#include "pb_decode.h"

///////////////// LoRa Configuration ////////////////////
// SX1262 Setup - Match your pinout
SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#define CONFIG_RADIO_FREQUENCY      915.0
#define CONFIG_RADIO_BW             500.0
#define CONFIG_RADIO_SF             12
#define CONFIG_RADIO_OUTPUT_POWER   22
#define CONFIG_RADIO_RX_BOOSTED     true

#ifndef WIFI_SSID
#define WIFI_SSID "SSID"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "PASSWORD"
#endif

WiFiMulti wifiMulti;
HTTPClient http;

volatile bool receivedFlag = false;
String receivedData = "";

// Callback when a packet is received
void setFlag(void) {
  receivedFlag = true;
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("Starting LoRa Receiver...");
  setupBoards();
  // TODO: setupBoards turns on the wifi access point, so we need to turn it off
  WiFi.softAPdisconnect(true);

  // Initialize the WiFi module
  Serial.println("Connecting to WiFi...");
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  http.setReuse(true);

  // Initialize the LoRa module
  int state = radio.begin();
  radio.setFrequency(CONFIG_RADIO_FREQUENCY);
  radio.setBandwidth(CONFIG_RADIO_BW);
  radio.setSpreadingFactor(CONFIG_RADIO_SF);
  radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER);
  radio.setRxBoostedGainMode(CONFIG_RADIO_RX_BOOSTED);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("[SX1262] Initialization successful!");
  } else {
    Serial.print("[SX1262] Initialization failed, code ");
    Serial.println(state);
    while (true);
  }

  // Set the function that gets called when a packet is received
  radio.setDio1Action(setFlag);

  // Start listening for packets
  Serial.printf("[SX1262] Starting receive mode... [%fMHz SF%d BW%.1f]:\n", CONFIG_RADIO_FREQUENCY, CONFIG_RADIO_SF, CONFIG_RADIO_BW);
  state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[SX1262] Receive mode failed, code ");
    Serial.println(state);
    while (true);
  }
}

void loop() {
  wifiMulti.run(5000);
  if (receivedFlag) {
    receivedFlag = false;

    String rssi;
    String snr;
    String frequencyError;

    // Read the received data
    rssi = String(radio.getRSSI()) + "dBm";
    snr = String(radio.getSNR()) + "dB";
    frequencyError = String(radio.getFrequencyError()) + "Hz";
    byte byteArr[256];
    int numBytes = radio.getPacketLength();
    int state = radio.readData(byteArr, numBytes);

    Serial.printf("[LoRa] Received %d bytes [%fMHz SF%d BW%.1f]:\n", numBytes, CONFIG_RADIO_FREQUENCY, CONFIG_RADIO_SF, CONFIG_RADIO_BW);

    //print RSSI
    Serial.print(F("Radio RSSI:\t\t"));
    Serial.println(rssi);

    // print SNR (Signal-to-Noise Ratio)
    Serial.print(F("Radio SNR:\t\t"));
    Serial.println(snr);

    Serial.print(F("Frequency error:\t"));
    Serial.println(frequencyError);

    String dataURI = "/weatherstation/updateweatherstation.php?";
    dataURI += "rxRSSI=" + String(radio.getRSSI(), 1);
    dataURI += "&rxSNR=" + String(radio.getSNR(), 1);
    dataURI += "&rxFrequencyError=" + String(radio.getFrequencyError(), 1);

    if (state == RADIOLIB_ERR_NONE) {
      Serial.println("[LoRa] Receive passed CRC");

      BuoyData decoded = BuoyData_init_zero;

      pb_istream_t istream = pb_istream_from_buffer(byteArr, numBytes);
      if (!pb_decode(&istream, &BuoyData_msg, &decoded))
      {
        Serial.println(F("Failed to decode message"));
      }

      if (decoded.has_windData)
      {
        if (decoded.windData.has_windSpeedAverage_kmh_scaled100)
        {
          Serial.println(F("Wind Speed: "));
          Serial.println(decoded.windData.windSpeedAverage_kmh_scaled100 / 100.0, 2);
          dataURI += "&windSpeedKph=" + String(decoded.windData.windSpeedAverage_kmh_scaled100 / 100.0, 2);
        }
        if (decoded.windData.has_windSpeedGust_kmh_scaled100)
        {
          Serial.println(F("Wind Gust: "));
          Serial.println(decoded.windData.windSpeedGust_kmh_scaled100 / 100.0, 2);
          dataURI += "&windGustKph=" + String(decoded.windData.windSpeedGust_kmh_scaled100 / 100.0, 2);
        }
        if (decoded.windData.has_windSpeedDirection_degrees)
        {
          Serial.println(F("Wind Direction: "));
          Serial.println(decoded.windData.windSpeedDirection_degrees);
          dataURI += "&windDirectionDegrees=" + String(decoded.windData.windSpeedDirection_degrees);
        }
      }
      if (decoded.has_currentData)
      {
        if (decoded.currentData.has_surfaceCurrentDirection_degrees)
        {
          Serial.println(F("Surface Current Direction: "));
          Serial.println(decoded.currentData.surfaceCurrentDirection_degrees);
          dataURI += "&waterDirection=" + String(decoded.currentData.surfaceCurrentDirection_degrees);
        }
        if (decoded.currentData.has_surfaceCurrentSpeed_kmh_scaled100)
        {
          Serial.println(F("Surface Current Speed: "));
          Serial.println(decoded.currentData.surfaceCurrentSpeed_kmh_scaled100 / 100.0, 2);
          dataURI += "&waterSpeedKn=" + String(decoded.currentData.surfaceCurrentSpeed_kmh_scaled100 * 1.85 / 100.0, 2);
        }
      }
      if (decoded.has_waveData)
      {
        if (decoded.waveData.has_maximumWaveHeight_meters_scaled100)
        {
          Serial.println(F("Maximum Wave Height: "));
          Serial.println(decoded.waveData.maximumWaveHeight_meters_scaled100 / 100.0, 2);
          dataURI += "&waveHeightFt=" + String(decoded.waveData.maximumWaveHeight_meters_scaled100 * 3.28 / 100.0, 2);
        }
        if (decoded.waveData.has_dominantWavePeriod_seconds_scaled100)
        {
          Serial.println(F("Dominant Wave Period: "));
          Serial.println(decoded.waveData.dominantWavePeriod_seconds_scaled100 / 100.0, 2);
          dataURI += "&wavePeriodSeconds=" + String(decoded.waveData.dominantWavePeriod_seconds_scaled100 / 100.0, 2);
        }
      }
      if (decoded.has_airData)
      {
        if (decoded.airData.has_airPressure_hpa_scaled100)
        {
          Serial.println(F("Air Pressure: "));
          Serial.println(decoded.airData.airPressure_hpa_scaled100 / 100.0, 2);
          dataURI += "&airPressureInHg=" + String(decoded.airData.airPressure_hpa_scaled100 / 33.8639 / 100.0, 4);
        }
        if (decoded.airData.has_airTemperature_celsius_scaled10)
        {
          Serial.println(F("Air Temperature: "));
          Serial.println(decoded.airData.airTemperature_celsius_scaled10 / 10.0, 2);
          dataURI += "&airTemperatureF=" + String(decoded.airData.airTemperature_celsius_scaled10 / 10.0 * 9 / 5.0 + 32, 2);
        }
        if (decoded.airData.has_airRelativeHumidity_percent_scaled10)
        {
          Serial.println(F("Air Relative Humidity: "));
          Serial.println(decoded.airData.airRelativeHumidity_percent_scaled10 / 10.0, 2);
          dataURI += "&airHumidityPercent=" + String(decoded.airData.airRelativeHumidity_percent_scaled10 / 10.0, 2);
        }
      }
      if (decoded.has_telemetry)
      {
        if (decoded.telemetry.has_gpsAltitude_meters_scaled100)
        {
          Serial.println(F("GPS Altitude: "));
          Serial.println(decoded.telemetry.gpsAltitude_meters_scaled100 / 100.0, 2);
          dataURI += "&gpsAltitudeMeters=" + String(decoded.telemetry.gpsAltitude_meters_scaled100 / 100.0, 2);
        }
        if (decoded.telemetry.has_gpsLatitude_degrees_scaled10000000)
        {
          Serial.println(F("GPS Latitude: "));
          Serial.println(decoded.telemetry.gpsLatitude_degrees_scaled10000000 / 10000000.0, 2);
          dataURI += "&gpsLat=" + String(decoded.telemetry.gpsLatitude_degrees_scaled10000000 / 10000000.0, 7);
        }
        if (decoded.telemetry.has_gpsLongitude_degrees_scaled10000000)
        {
          Serial.println(F("GPS Longitude: "));
          Serial.println(decoded.telemetry.gpsLongitude_degrees_scaled10000000 / 10000000.0, 2);
          dataURI += "&gpsLong=" + String(decoded.telemetry.gpsLongitude_degrees_scaled10000000 / 10000000.0, 7);
        }
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        http.begin("weewx.milwaukeesailing.net", 8089, dataURI);
        int httpCode = http.GET();
        if (httpCode > 0) {
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

          if (httpCode == HTTP_CODE_OK) {
            http.writeToStream(&Serial);
            Serial.println();
          }
        } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", HTTPClient::errorToString(httpCode).c_str());
        }

        http.end();
      }
      else
      {
        Serial.println("WiFi not connected, not uploading...");
      }
    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("[LoRa] CRC error!"));
    } else {
      Serial.print("[LoRa] Receive failed, code ");
      Serial.println(state);
    }

    // Restart receiver
    radio.startReceive();
  }
}
