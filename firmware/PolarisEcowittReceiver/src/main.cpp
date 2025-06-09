#include <Arduino.h>

#include "rtl-433-receiver.h"
#include <ecowitt-parser.h>

constexpr uint32_t SERIAL_BAUD_RATE = 115200;


void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    initializeOutputSerial(SERIAL_BAUD_RATE);

    while (!Serial)
        delay(10);

    ESP_LOGI("setup", "Initializing receiver...");
    rtl_433_receiver_init();
    ESP_LOGI("setup", "Receiver initialized");
}

void loop()
{
    rtl_433_receiver_loop();
}
