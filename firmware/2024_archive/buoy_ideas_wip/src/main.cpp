#include <Arduino.h>
#include <Wire.h>

#if defined(T3_V1_6_SX1276) or defined(T3_S3_V1_2_SX1262)

#include <LoRaBoards.h>

#endif

#define CONFIG_LORA_FREQUENCY           915.00
#define CONFIG_LORA_RADIO_BW            125.0
#define CONFIG_LORA_SPREADING_FACTOR    10
#define CONFIG_LORA_CODING_RATE         8
#define CONFIG_LORA_RADIO_OUTPUT_POWER  20
#define CONFIG_LORA_PREAMBLE_LENGTH     10
#define CONFIG_LORA_ENABLE_CRC          true

// TODO: RS485 pins
#define PIN_RS485_RX 14
#define PIN_RS485_TX 13
// See suggested wiring options: https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32/api-reference/peripherals/uart.html#interface-connection-options
// RS485 Driver Enable pin
// Pulled high while transmitting data
// Pulled low while receiving data
#define PIN_RS485_DE 0
// RS485 Receiver Enable pin
// Pulled low while receiving data
#define PIN_RS485_RE 4
#define PIN_RS485_RTS PIN_RS485_DE
#define PIN_ANEMOMETER_EN 2

// TODO: Enable
// #include "transmitter.h"
// #include "anemometer.h"

// TODO: Extract
#include "SparkFunBMP384.h"
BMP384 bmp;
#define SEALEVELPRESSURE_HPA (1013.25)
float zeroAltitude = 0;

float readAltitude(float seaLevel);

void setup() {
#if defined(T3_V1_6_SX1276) or defined(T3_S3_V1_2_SX1262)
    Serial.begin(115200);
    delay(1500);
    setupBoards();
    // When the power is turned on, a delay is required.
    delay(1500);
    printResult(false); // TODO: Remove
#endif
    // TODO: Enable
    // LoraTransmitter::INSTANCE.init();
    // Anemometer::INSTANCE.init();

    Wire.begin(I2C_SDA, I2C_SCL);
    Wire1.begin(EXTERNAL_I2C_SDA, EXTERNAL_I2C_SCL);
    scanDevices(&Wire);
    scanDevices(&Wire1);

    if (BMP3_OK != bmp.setODRFrequency(BMP3_ODR_200_HZ)) {
        Serial.println("Could not set ODR frequency!");
    }

    if (BMP3_OK != bmp.beginI2C(BMP384_I2C_ADDRESS_DEFAULT, Wire1)) {
        // hardware I2C mode, can pass in address & alt Wire
        Serial.println("Could not find a valid BMP3 sensor, check wiring!");
        while (1); // TODO
    }
    bmp3_odr_filter_settings osrMultipliers =
    {
        .press_os = BMP3_OVERSAMPLING_16X,
        .temp_os = BMP3_OVERSAMPLING_2X,
        0, 0
    };
    if (BMP3_OK != bmp.setOSRMultipliers(osrMultipliers)) {
        Serial.println("Could not set OSR multiplier!");
    }
    if (BMP3_OK != bmp.setFilterCoefficient(BMP3_IIR_FILTER_COEFF_3)) {
        Serial.println("Could not set filter coefficient!");
    }

    uint8_t odr;
    if (BMP3_OK == bmp.getODRFrequency(&odr)) {
        Serial.printf("ODR frequency: %fHz\n", 200 / pow(2, odr));
    }

    zeroAltitude = readAltitude(SEALEVELPRESSURE_HPA) * 100;
}

void loop() {
    float altitude = readAltitude(SEALEVELPRESSURE_HPA) * 100 - zeroAltitude;
    Serial.println(altitude);

    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_fur11_tf);
    u8g2->drawStr(0, 16, ("Alt: " + String(altitude)).c_str());
    u8g2->sendBuffer();

    delay(10);
}

float readAltitude(float seaLevel) {
    // Equation taken from BMP180 datasheet (page 16):
    //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

    // Note that using the equation from wikipedia can give bad results
    // at high altitude. See this thread for more information:
    //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

    bmp3_data bmpData;
    if (BMP3_OK != bmp.getSensorData(&bmpData)) {
        Serial.println("Could not read sensor data!");
    }

    float atmospheric = bmpData.pressure / 100.0F;
    return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}
