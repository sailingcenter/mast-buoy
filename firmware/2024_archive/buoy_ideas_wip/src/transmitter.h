
#ifndef BUOY_TRANSMITTER_H
#define BUOY_TRANSMITTER_H

#include <RadioLib.h>

class LoraTransmitter {
public:
    static LoraTransmitter INSTANCE;

    bool transmit(const char *data) {
        if (!transmitting) {
            return (radio.startTransmit(data) == RADIOLIB_ERR_NONE);
        } else {
            // TODO: log something out
            return false;
        }
    }

    void init() {
#ifdef  RADIO_TCXO_ENABLE
        pinMode(RADIO_TCXO_ENABLE, OUTPUT);
        digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif
        // initialize radio with default settings
        int state = radio.begin();

        printResult(state == RADIOLIB_ERR_NONE);
        radio.setPacketSentAction(LoraTransmitter::transmitDone);

        /*
        *   Sets carrier frequency.
        *   SX1278/SX1276 : Allowed values range from 137.0 MHz to 525.0 MHz.
        *   SX1268/SX1262 : Allowed values are in range from 150.0 to 960.0 MHz.
        *   SX1280        : Allowed values are in range from 2400.0 to 2500.0 MHz.
        *   LR1121        : Allowed values are in range from 150.0 to 960.0 MHz, 1900 - 2200 MHz and 2400 - 2500 MHz. Will also perform calibrations.
        * * * */
        if (radio.setFrequency(frequency) == RADIOLIB_ERR_INVALID_FREQUENCY) {
            Serial.println(F("Selected frequency is invalid for this module!"));
            while (true);
        }

        /*
        *   Sets LoRa link bandwidth.
        *   SX1278/SX1276 : Allowed values are 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250 and 500 kHz. Only available in %LoRa mode.
        *   SX1268/SX1262 : Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
        *   SX1280        : Allowed values are 203.125, 406.25, 812.5 and 1625.0 kHz.
        *   LR1121        : Allowed values are 62.5, 125.0, 250.0 and 500.0 kHz.
        * * * */
        if (radio.setBandwidth(bandwidth) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            while (true);
        }

        /*
        * Sets LoRa link spreading factor.
        * SX1278/SX1276 :  Allowed values range from 6 to 12. Only available in LoRa mode.
        * SX1262        :  Allowed values range from 5 to 12.
        * SX1280        :  Allowed values range from 5 to 12.
        * LR1121        :  Allowed values range from 5 to 12.
        * * * */
        if (radio.setSpreadingFactor(spreadingFactor) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
            Serial.println(F("Selected spreading factor is invalid for this module!"));
            while (true);
        }

        /*
        * Sets LoRa coding rate denominator.
        * SX1278/SX1276/SX1268/SX1262 : Allowed values range from 5 to 8. Only available in LoRa mode.
        * SX1280        :  Allowed values range from 5 to 8.
        * LR1121        :  Allowed values range from 5 to 8.
        * * * */
        if (radio.setCodingRate(codingRate) == RADIOLIB_ERR_INVALID_CODING_RATE) {
            Serial.println(F("Selected coding rate is invalid for this module!"));
            while (true);
        }

        /*
        * Sets LoRa sync word.
        * SX1278/SX1276/SX1268/SX1262/SX1280 : Sets LoRa sync word. Only available in LoRa mode.
        * * */
        if (radio.setSyncWord(0xAB) != RADIOLIB_ERR_NONE) {
            Serial.println(F("Unable to set sync word!"));
            while (true);
        }

        /*
        * Sets transmission output power.
        * SX1278/SX1276 :  Allowed values range from -3 to 15 dBm (RFO pin) or +2 to +17 dBm (PA_BOOST pin). High power +20 dBm operation is also supported, on the PA_BOOST pin. Defaults to PA_BOOST.
        * SX1262        :  Allowed values are in range from -9 to 22 dBm. This method is virtual to allow override from the SX1261 class.
        * SX1268        :  Allowed values are in range from -9 to 22 dBm.
        * SX1280        :  Allowed values are in range from -18 to 13 dBm. PA Version range : -18 ~ 3dBm
        * LR1121        :  Allowed values are in range from -9 to 22 dBm (high-power PA) or -17 to 14 dBm (low-power PA)
        * * * */
        if (radio.setOutputPower(outputPower) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
            Serial.println(F("Selected output power is invalid for this module!"));
            while (true);
        }

#if !defined(USING_SX1280) && !defined(USING_LR1121)
        /*
        * Sets current limit for over current protection at transmitter amplifier.
        * SX1278/SX1276 : Allowed values range from 45 to 120 mA in 5 mA steps and 120 to 240 mA in 10 mA steps.
        * SX1262/SX1268 : Allowed values range from 45 to 120 mA in 2.5 mA steps and 120 to 240 mA in 10 mA steps.
        * NOTE: set value to 0 to disable overcurrent protection
        * * * */
        if (radio.setCurrentLimit(140) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
            Serial.println(F("Selected current limit is invalid for this module!"));
            while (true);
        }
#endif

        /*
        * Sets preamble length for LoRa or FSK modem.
        * SX1278/SX1276 : Allowed values range from 6 to 65535 in %LoRa mode or 0 to 65535 in FSK mode.
        * SX1262/SX1268 : Allowed values range from 1 to 65535.
        * SX1280        : Allowed values range from 1 to 65535. preamble length is multiple of 4
        * LR1121        : Allowed values range from 1 to 65535.
        * * */
        if (radio.setPreambleLength(preambleLength) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
            Serial.println(F("Selected preamble length is invalid for this module!"));
            while (true);
        }

        // Enables or disables CRC check of received packets.
        if (radio.setCRC(enableCRC) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
            Serial.println(F("Selected CRC is invalid for this module!"));
            while (true);
        }
    }

private:
    static volatile bool transmitting;

    static IRAM_ATTR void transmitDone() {
        transmitting = false;
    }

    explicit LoraTransmitter(
            float frequency = CONFIG_LORA_FREQUENCY,
            float bandwidth = CONFIG_LORA_RADIO_BW,
            uint8_t spreadingFactor = CONFIG_LORA_SPREADING_FACTOR,
            uint8_t codingRate = CONFIG_LORA_CODING_RATE,
            int8_t outputPower = CONFIG_LORA_RADIO_OUTPUT_POWER,
            size_t preambleLength = CONFIG_LORA_PREAMBLE_LENGTH,
            bool enableCRC = CONFIG_LORA_ENABLE_CRC
    ) :
            frequency(frequency),
            bandwidth(bandwidth),
            spreadingFactor(spreadingFactor),
            codingRate(codingRate),
            outputPower(outputPower),
            preambleLength(preambleLength),
            enableCRC(enableCRC),
            radio(nullptr) {
        radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);
    }

    float frequency;
    float bandwidth;
    uint8_t spreadingFactor;
    uint8_t codingRate;
    int8_t outputPower;
    size_t preambleLength;
    bool enableCRC;

#if     defined(USING_SX1276)
    SX1276 radio;
#endif
};

#endif //BUOY_TRANSMITTER_H
