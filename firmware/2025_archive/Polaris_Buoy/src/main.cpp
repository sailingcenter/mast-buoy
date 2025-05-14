/// Libraries ///
#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_BNO055.h>
#include "Buoy_Sensors.h"
#include "LoRaBoards.h"
#include "utilities.h"
#include <RadioLib.h>
#include <EEPROM.h>


#include "buoy_data.pb.h""
#include "pb_encode.h"

////////////////////////////////////////////////////////
/////////// Initialization of variables ////////////////
////////////////////////////////////////////////////////

////////////LoRa//////////////////////////
// Configuration for SX1262
SX1262 radio = new Module(RADIO_CS_PIN, RADIO_DIO1_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

#define CONFIG_RADIO_FREQUENCY      915.0
#define CONFIG_RADIO_BW             500.0
#define CONFIG_RADIO_SF             12
#define CONFIG_RADIO_OUTPUT_POWER   22
#define CONFIG_RADIO_OUTPUT_CURRENT 140
#define CONFIG_RADIO_CODING_RATE    8

// Initialize the SX1262 module

// Transmission finished flag
volatile bool transmittedFlag = false;
volatile bool transmitting = false;

// Function called when transmission complete
void setFlag(void) {
  transmittedFlag = true;
}

///////////////// IMU ///////////////////////////////
double DEG_2_RAD = 0.01745329251;
double windDir = 0;
double trueWindDir = 0;

// --- Timing and Constants --- //
uint16_t BNO055_SAMPLERATE_DELAY_MS = 10;  // (100Hz)
uint16_t PRINT_RATE_MS = 4000;
uint16_t printCounter = 0;

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire1);


///////////////// Anemometer ///////////////////////////////
HardwareSerial Anem_UART(2); // Use UART2 (ESP32-S3)

// Define GPIO TX and RX pins
#define Anem_RX 42
#define Anem_TX 46

///////////////// Paddle Wheel ///////////////////////////////
float currentWaterSpeed = 0;


////////////////////////////////////////////////////////
///////////////////// SET UP /////////////////////////////
////////////////////////////////////////////////////////

void setup() {
  delay(1000);
  Serial.begin(115200);

  // initialize the board via the LoRaBoards library. Will automatically detect I2C
  setupBoards();
  //setupBoards turns on the wifi access point, needs to be turned off
  WiFi.softAPdisconnect(true);

  delay(1500);
  // GPS //
  initializeGPS();

  // Anemometer //
  Anem_UART.begin(115200, SERIAL_8N1, Anem_TX, Anem_RX); // UART 2
  Serial.println("UART Receiver Initialized..."); //Confirm UART2/ Anemometer begins

  // IMU //
  if (!bno.begin()) { //check to see if the LilyGo detects the I2C from the BNO055
    Serial.println("No BNO055 detected on Wire1... Check wiring or address!");
  } else {
    Serial.println("BNO055 initialized successfully on Wire1!");
  }
  //Checking to see if EEPROM is available and can load in calibration onto the IMU
  if (isEEPROMDataAvailable()) {
    loadCalibration(bno);
    Serial.println("Loaded calibration data from EEPROM.");
  } else {
    Serial.println("No calibration data in EEPROM.");
  }

  delay(1000);
  bno.setExtCrystalUse(true);
  Serial.println("BNO055 Initialized.");
  Serial.println("--------------------------------------------------");

  pinMode(BOOT_PIN, INPUT);

  if (!Anem_UART) {
    Serial.println("Anemometer UART failed to initialize!");
  } //check for no anemometer UART start

  // Initialize SX1262
  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin();  //putting the state into an int so if there is an error, can match the code
  radio.setFrequency(CONFIG_RADIO_FREQUENCY);
  radio.setBandwidth(CONFIG_RADIO_BW);
  radio.setSpreadingFactor(CONFIG_RADIO_SF);
  radio.setOutputPower(CONFIG_RADIO_OUTPUT_POWER);
  radio.setCurrentLimit(CONFIG_RADIO_OUTPUT_CURRENT);
  radio.setCodingRate(CONFIG_RADIO_CODING_RATE);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state); //print state in integer, going to library will give error code
    while (true);
  }

  // Set interrupt action for LoRa
  radio.setPacketSentAction(setFlag);
}

unsigned long lastPrintTime = 0;

////////////////////////////////////////////////////////
///////////////////// LOOP /////////////////////////////
////////////////////////////////////////////////////////

void loop() {
  // 1. Read GPS (feed GPS data)
  Serial.println("Getting GPS...");
  Telemetry telemetry = getGPS();  // Feed getGPS function into string for packaging

  // 2. Read water speed
  Serial.println("Getting water speed. . . ");
  CurrentData current_data = getWaterSpeedInfo();  // ~20s blocking pulse count

  // 3. Read wave height (also ~6s of samples)
  Serial.println("Getting Wave height. . . ");
  WaveData wave_data = getWaveHeightInfo(bno);

  // 4. Read wind info
  BuoyData buoy_data = BuoyData_init_zero;
  Serial.println("Getting anemometer data. . . ");
  getWeatherData(Anem_UART, bno, buoy_data);  // will parse JSON and give wind speed, parsed direction, and true corrected direction (based on IMU)

  // 5. Combine and send/report
  buoy_data.has_telemetry = telemetry.has_gpsLatitude_degrees_scaled10000000;
  buoy_data.telemetry = telemetry;
  buoy_data.has_currentData = current_data.has_surfaceCurrentSpeed_kmh_scaled100;
  buoy_data.currentData = current_data;
  buoy_data.has_waveData = wave_data.has_maximumWaveHeight_meters_scaled100;
  buoy_data.waveData = wave_data;

  uint8_t buffer[256];
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  if (!pb_encode(&ostream, &BuoyData_msg, &buoy_data)) {
    Serial.println("Failed to encode message");
  } else {
    Serial.printf("[LoRa] Sending %d bytes which will take %dms... [%fMHz SF%d BW%.1f]:\n", ostream.bytes_written, radio.getTimeOnAir(ostream.bytes_written) / 1000, CONFIG_RADIO_FREQUENCY, CONFIG_RADIO_SF, CONFIG_RADIO_BW);
    int state = radio.transmit(buffer, ostream.bytes_written);

    if (state == RADIOLIB_ERR_NONE) {
      Serial.println("[LoRa] Transmission successful!");
    } else {
      Serial.print("[LoRa] Failed to transmit. Code: ");
      Serial.println(state);
    }
  }

  // Wait before sending again
  delay(5000);  // Adjust depending on your sampling time
}