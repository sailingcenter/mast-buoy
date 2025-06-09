#include "ecowitt-parser.h"

#include <ArduinoJson.h>
#include <HardwareSerial.h>

#define OUTPUT_SERIAL Serial1

void initializeOutputSerial(const unsigned long baudRate)
{
    OUTPUT_SERIAL.begin(
        baudRate
    );
}

void logJson(const JsonDocument& doc)
{
    char JSONmessageBuffer[measureJson(doc) + 1];
    serializeJson(doc, JSONmessageBuffer, sizeof(JSONmessageBuffer));
#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
    ESP_LOGI("ecowitt", ".");
#else
    ESP_LOGI("ecowitt", "%s", JSONmessageBuffer);
    OUTPUT_SERIAL.println(JSONmessageBuffer);
#endif
}

/**
 * Sample messages:
 * {"model":"Fineoffset-WS90","id":23068,"battery_ok":0,"battery_mV":680,"temperature_C":23.4,"humidity":68,"wind_dir_deg":23,"wind_avg_m_s":0,"wind_max_m_s":0,"uvi":0,"light_lux":1910,"flags":130,"rain_mm":0,"supercap_V":0.8,"firmware":156,"data":"3fff000000------0000ffbffb0000","mic":"CRC","protocol":"Fine Offset Electronics WS90 weather station","rssi":-32,"duration":28983}
 * {"model":"Fineoffset-WN34D","id":37868,"battery_ok":1,"battery_mV":1780,"temperature_C":24.5,"mic":"CRC","protocol":"Fine Offset Electronics WN34S/L/D and Froggit DP150/D35 temperature sensor","rssi":-44,"duration":7999}
 * {"model":"AmbientWeather-WH31B","id":12,"channel":1,"battery_ok":1,"temperature_C":23.2,"humidity":51,"data":"7e00000000","mic":"CRC","protocol":"Ambient Weather WH31E Thermo-Hygrometer Sensor, EcoWitt WH40 rain gauge, WS68 weather station","rssi":-58,"duration":52000}
 */
void parse_ecowitt(char* message)
{
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, message);
    if (error)
    {
        ESP_LOGW("ecowitt", "Failed to parse JSON: %s", error.c_str());
        return;
    }
    logJson(doc);
}
