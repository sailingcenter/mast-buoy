#include <Arduino.h>
#include <HardwareSerial.h>

#include <ArduinoLog.h>

#include "dual-stream.h"
#include "rtl-433-receiver.h"

// Configuration constants
#define OUTPUT_SERIAL Serial1
constexpr uint32_t SERIAL_BAUD_RATE = 115200;
constexpr uint8_t UART_TX_PIN = 4;
constexpr uint8_t UART_RX_PIN = 25;

struct SerialPortConfig
{
    unsigned long baudRate;
    uint32_t config;
    int8_t rxPin;
    int8_t txPin;
};

void initializeSerial(const SerialPortConfig& config)
{
    Serial.begin(config.baudRate);
    OUTPUT_SERIAL.begin(
        config.baudRate,
        config.config,
        config.rxPin,
        config.txPin
    );

    while (!Serial)
        delay(10);
}

void setup()
{
    SerialPortConfig config{
        SERIAL_BAUD_RATE,
        SERIAL_8N1,
        UART_RX_PIN,
        UART_TX_PIN
    };

    initializeSerial(config);

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_NOTICE
#endif

    DualStream dualLogger(Serial, OUTPUT_SERIAL);
    Log.begin(LOG_LEVEL, &dualLogger);

    Log.notice(F(" " CR));
    Log.notice(F("****** setup ******" CR));
    rtl_433_receiver_init();
    Log.notice(F("****** setup complete ******" CR));
}

void loop()
{
    rtl_433_receiver_loop();
}
