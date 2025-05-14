#include "GPS.h"
#include <TinyGPS++.h>
#include "buoy_data.pb.h"

HardwareSerial GPS_UART(GPS_UART_NR);
Telemetry telemetry = Telemetry_init_zero;
portMUX_TYPE gpsMutex = portMUX_INITIALIZER_UNLOCKED;

void gpsReaderTask(void* pvParameters)
{
    TinyGPSPlus gps;

    Serial.println("GPS task started");

    for (;;)
    {
        while (GPS_UART.available())
        {
            int c = GPS_UART.read();
            if (gps.encode(c))
            {
                if (gps.location.isValid())
                {
                    portENTER_CRITICAL(&gpsMutex);
                    telemetry.has_gpsLatitude_degrees_scaled10000000 = true;
                    telemetry.gpsLatitude_degrees_scaled10000000 = gps.location.lat() * 10000000;
                    telemetry.has_gpsLongitude_degrees_scaled10000000 = true;
                    telemetry.gpsLongitude_degrees_scaled10000000 = gps.location.lng() * 10000000;
                    telemetry.has_gpsAltitude_meters_scaled100 = true;
                    telemetry.gpsAltitude_meters_scaled100 = gps.altitude.meters() * 100;
                    portEXIT_CRITICAL(&gpsMutex);
                }
                else
                {
                    portENTER_CRITICAL(&gpsMutex);
                    telemetry = Telemetry_init_zero;
                    portEXIT_CRITICAL(&gpsMutex);
                }
            }
        }
        // Wait a little before trying to read more from the UART
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

bool initializeGPS()
{
    GPS_UART.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX); //Start GPS object

    if (!GPS_UART)
    {
        Serial.println("GPS UART failed to initialize!");
        return false;
    }

    BaseType_t taskCreated = xTaskCreatePinnedToCore(
        gpsReaderTask,
        "GPS Data",
        4096,
        nullptr,
        1, // Priority of the task (1 is reasonable for non-critical)
        nullptr,
        1); // Core ID (0 or 1) - 1 is APP_CPU, recommended for Arduino
    if (taskCreated != pdPASS)
    {
        Serial.println("Failed to create GPS task");
        return false;
    }

    return true;
}

Telemetry getGPS()
{
    Telemetry returnValue = Telemetry_init_zero;
    portENTER_CRITICAL(&gpsMutex);
    returnValue = telemetry;
    portEXIT_CRITICAL(&gpsMutex);
    return returnValue;
}
