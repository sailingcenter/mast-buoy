
#ifndef BUOY_ANEMOMETER_H
#define BUOY_ANEMOMETER_H

#include <soc/uart_struct.h>

class Anemometer {
public:
    static Anemometer INSTANCE;

    // TODO: Some research https://blog.dest-unreach.be/2020/04/26/uart-half-duplex-esp32/

    // TODO: https://github.com/eModbus/eModbus/tree/master/examples
    // possible alternative: https://forum.arduino.cc/t/using-modbusmaster-with-esp32-max485-rk330-01b/1189739/17
    void init() {
        // Suggested wiring options: https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32/api-reference/peripherals/uart.html#interface-connection-options
        // IDF sample code: https://github.com/espressif/esp-idf/tree/v5.2.2/examples/peripherals/uart/uart_echo_rs485
        // Arduino sample code: https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/Serial/RS485_Echo_Demo/RS485_Echo_Demo.ino

        pinMode(PIN_RS485_RE, OUTPUT);
        pinMode(PIN_RS485_DE, OUTPUT);
        pinMode(PIN_ANEMOMETER_EN, OUTPUT);

        digitalWrite(PIN_RS485_DE, LOW);
        // TODO: This would be circuit A from https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32/api-reference/peripherals/uart.html#interface-connection-options
        // TODO: This would mean we'd pull RE low all the time
        digitalWrite(PIN_RS485_RE, LOW);
        // TODO: Can we leave the anemometer enabled all the time?
        digitalWrite(PIN_ANEMOMETER_EN, HIGH);

        rs485->begin(4800, SERIAL_8N1, PIN_RS485_RX, PIN_RS485_TX);
        while (!*rs485) {
            delay(10);
        }
        if (!rs485->setPins(-1, -1, -1, PIN_RS485_RTS)) {
            Serial.print("Failed to set RS485 pins");
        }
        if (!rs485->setMode(UART_MODE_RS485_HALF_DUPLEX)) {
            Serial.print("Failed to set RS485 mode");
        }
        // TODO: This would be circuit A from https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32/api-reference/peripherals/uart.html#interface-connection-options
        // TODO: This would mean we'd pull RE low all the time
        UART2.rs485_conf.rs485tx_rx_en = 1;
    }

private:
    HardwareSerial *rs485;
};

#endif //BUOY_ANEMOMETER_H
