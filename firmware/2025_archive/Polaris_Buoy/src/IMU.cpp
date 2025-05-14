#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#include "buoy_data.pb.h"

#define EEPROM_CALIBRATION_START 0 // Calling IMU memory to get calibration data saved 

static bool parseWeatherData(const String& message, BuoyData &buoyData, Adafruit_BNO055 &bno) {
  // Clean the message
  String cleanedMessage = message;
  cleanedMessage.trim();
  cleanedMessage.replace("\r", "");
  cleanedMessage.replace("\n", "");

  int startIdx = cleanedMessage.indexOf("{");
  int endIdx = cleanedMessage.lastIndexOf("}");

  if (startIdx == -1 || endIdx == -1 || endIdx <= startIdx) {
    Serial.println("Error: No valid JSON object found in message.");
    return false;
  }

  String json = cleanedMessage.substring(startIdx, endIdx + 1);
  Serial.println("Extracted JSON: " + json);

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print("Parsing failed: ");
    Serial.println(error.f_str());
    return false;
  }

  if (doc.containsKey("wind_dir_deg"))
  {
    sensors_event_t event;
    bno.getEvent(&event);
    double yaw = event.orientation.x;

    buoyData.has_windData = true;
    uint32_t wind_dir_deg = doc["wind_dir_deg"];
    wind_dir_deg = fmod(wind_dir_deg + yaw + 360, 360);  // Normalize to 0–360
    buoyData.windData.has_windSpeedDirection_degrees = true;
    buoyData.windData.windSpeedDirection_degrees = wind_dir_deg;
    Serial.printf("wind_dir_deg: %d\n", wind_dir_deg);
  }
  if (doc.containsKey("wind_avg_m_s"))
  {
    buoyData.has_windData = true;
    double wind_avg_m_s = doc["wind_avg_m_s"];
    buoyData.windData.has_windSpeedAverage_kmh_scaled100 = true;
    buoyData.windData.windSpeedAverage_kmh_scaled100 = wind_avg_m_s * 3.6 * 100.0;
    Serial.printf("wind_avg_m_s: %f.1\n", wind_avg_m_s);
  }
  if (doc.containsKey("wind_max_m_s"))
  {
    buoyData.has_windData = true;
    double wind_max_m_s = doc["wind_max_m_s"];
    buoyData.windData.has_windSpeedGust_kmh_scaled100 = true;
    buoyData.windData.windSpeedGust_kmh_scaled100 = wind_max_m_s * 3.6 * 100.0;
    Serial.printf("wind_max_m_s: %f.1\n", wind_max_m_s);
  }
  if (doc.containsKey("temperature_C"))
  {
    buoyData.has_airData = true;
    double temperature_C = doc["temperature_C"];
    buoyData.airData.has_airTemperature_celsius_scaled10 = true;
    buoyData.airData.airTemperature_celsius_scaled10 = temperature_C * 10.0;
    Serial.printf("temperature_C: %f.1\n", temperature_C);
  }
  if (doc.containsKey("humidity"))
  {
    buoyData.has_airData = true;
    double humidity = doc["humidity"];
    buoyData.airData.has_airRelativeHumidity_percent_scaled10 = true;
    buoyData.airData.airRelativeHumidity_percent_scaled10 = humidity * 10.0;
    Serial.printf("humidity_percent: %f.1\n", humidity);
  }

  return buoyData.has_windData || buoyData.has_airData;
}



// Collects wind info and returns a report String
static bool getWeatherData(HardwareSerial &serial, Adafruit_BNO055 &bno, BuoyData &buoy_data) {

  unsigned long startTime = millis();

  // Wait for data to be available or timeout after 10 seconds
  while (!serial.available()) {
    if (millis() - startTime > 15000) {  // 15 seconds timeout
      Serial.println("Timeout: No wind data received.");
      return false;
    }
    delay(10);  // Prevent CPU hogging
  }

  // Read the incoming data
  String received = serial.readString();

  return parseWeatherData(received, buoy_data, bno);
}


/////EEPROM FUNCTIONS/////////

static void loadCalibration(Adafruit_BNO055 &bno) {
  uint8_t calibData[22];
  for (int i = 0; i < 22; i++) {
    calibData[i] = EEPROM.read(EEPROM_CALIBRATION_START + i);
  }

  bno.setSensorOffsets(calibData);
}

static bool isEEPROMDataAvailable() {
  for (int i = 0; i < 22; i++) {
    if (EEPROM.read(EEPROM_CALIBRATION_START + i) == 0xFF) {
      return false; // Data likely not present
    }
  }
  return true;
}



