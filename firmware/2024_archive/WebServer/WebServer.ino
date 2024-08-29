/*
Project: Senior Design Group M78 Remote Weather Navigation Buoy
File: Master Ground Station
Institution: Marquette University
Engineer: Benjamin Doyle
Electrical Advisor: Dr. Cris Ababei
Mechanical Advisor: Dr. Tony Bowman


Notes:
This code will be on the board in the main MCSC building, and will receive the spreadingfactor 8 data from the groundstation relay tower, then will upload that to the internet
Based on the RadioLib SX127x Receive Example from lilygo github

For full API reference, see the GitHub Pages
https://jgromes.github.io/RadioLib/
*/

#include <RadioLib.h>//LoRa
#include "utilities.h"//LoRa
#include "boards.h"//LoRa
#include <WiFi.h>//internet transmit
#include <WiFiClient.h>//internet transmit
#include <Wire.h>//internet transmit

SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN);

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

String payload;  //creates string that will be used to store payload from relay

//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================
const char* ssid      = "INSERT SSID"; //router (wi-fi) name
const char* password  = "INSERT PASSWORD"; //wi-fi password
//==================================================================================================================================================================================================================
//==================================================================================================================================================================================================================

const char* host      = "weewx.milwaukeesailing.net"; //"weatherstation.wunderground.com" originally. Claude provided this weewx url as it's what he uses and what we need to transmit to
const int httpPort    = 8089;


// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if (!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

void setup() {
  initBoard();
  // When the power is turned on, a delay is required.
  delay(1500);

  StartWiFi();

  // initialize SX1276 with default settings
  Serial.print(F("[SX1276] Initializing ... "));
#ifndef LoRa_frequency
  int state = radio.begin(915.0);
#else
  int state = radio.begin(LoRa_frequency);
#endif
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
    radio.setOutputPower(17);
    radio.setBandwidth(125);
    radio.setCurrentLimit(120);
    radio.setSpreadingFactor(8);  //9 is default value, changing to 8 for receiving from GroundStation relay
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
      ;
  }
  // set the function that will be called
  // when new packet is received
  radio.setDio0Action(setFlag, RISING);

  // start listening for LoRa packets
  Serial.print(F("[SX1276] Starting to listen ... "));
  state = radio.startReceive();
#ifdef HAS_DISPLAY
  if (u8g2) {
    if (state != RADIOLIB_ERR_NONE) {
      u8g2->clearBuffer();
      u8g2->drawStr(0, 12, "Initializing: FAIL!");
      u8g2->sendBuffer();
    }
  }
#endif
#ifdef EDP_DISPLAY
  if (state != RADIOLIB_ERR_NONE) {
    display.setRotation(1);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 15);
    display.println("Initializing: FAIL!");
    display.update();
  }
#endif
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true)
      ;
  }


  // if needed, 'listen' mode can be disabled by calling
  // any of the following methods:
  //
  // radio.standby()
  // radio.sleep()
  // radio.transmit();
  // radio.receive();
  // radio.readData();
  // radio.scanChannel();
}


void loop() {
  // check if the flag is set
  if (receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    int state = radio.readData(payload);  //THE DATA HAS ENTERED THE BUILDING


    boolean this_upload = UploadDataToWU(); //SEND DATA TO CLAUDE SERVER
    if (!this_upload) {
      Serial.println("Error uploading, trying next time");
    } else {
      // do nothing; uploading successsful;
    }


    // you can also read received data as byte array
    /*
          byte byteArr[8];
          int state = radio.readData(byteArr, 8);
        */

    if (state == RADIOLIB_ERR_NONE) {
      // packet was successfully received
      Serial.println(F("[SX1276] Received packet!"));

      // print data of the packet
      Serial.print(F("[SX1276] Data:\t\t"));
      Serial.println(payload);

      // print RSSI (Received Signal Strength Indicator)
      Serial.print(F("[SX1276] RSSI:\t\t"));
      Serial.print(radio.getRSSI());
      Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
      Serial.print(F("[SX1276] SNR:\t\t"));
      Serial.print(radio.getSNR());
      Serial.println(F(" dB"));

      // print frequency error
      Serial.print(F("[SX1276] Frequency error:\t"));
      Serial.print(radio.getFrequencyError());
      Serial.println(F(" Hz"));
#ifdef HAS_DISPLAY
      if (u8g2) {
        u8g2->clearBuffer();
        char buf[256];
        u8g2->drawStr(0, 12, "Received OK!");
        u8g2->drawStr(5, 26, payload.c_str());
        snprintf(buf, sizeof(buf), "RSSI:%.2f", radio.getRSSI());
        u8g2->drawStr(0, 40, buf);
        snprintf(buf, sizeof(buf), "SNR:%.2f", radio.getSNR());
        u8g2->drawStr(0, 54, buf);
        u8g2->sendBuffer();
      }
#endif
#ifdef EDP_DISPLAY
      display.setRotation(1);
      display.fillScreen(GxEPD_WHITE);
      display.setTextColor(GxEPD_BLACK);
      display.setFont(&FreeMonoBold9pt7b);
      display.setCursor(0, 15);
      display.println("[SX1262] Received:");
      display.setCursor(0, 35);
      display.println("DATA:");
      display.setCursor(55, 35);
      display.print(payload);  //received string is displayed
      /*display.setCursor(55, 45); // i tried
            display.print(" kph"); */
      display.setCursor(0, 55);
      display.println("RSSI:");
      display.setCursor(55, 55);
      display.println(radio.getRSSI());
      display.setCursor(0, 75);
      display.println("SNR :");
      display.setCursor(55, 75);
      display.println(radio.getSNR());
      display.update();
#endif
    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("[SX1276] CRC error!"));

    } else {
      // some other error occurred
      Serial.print(F("[SX1276] Failed, code "));
      Serial.println(state);
    }

    // put module back to listen mode
    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }
}

//custom functions for uploading to internet below:
boolean UploadDataToWU() {
  int my_counter = 0;
  WiFiClient client;
  // Use WiFiClient class to create connection
  Serial.println("Connecting to   : " + String(host));
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return false;
  }
  String url = "/weatherstation/updateweatherstation.php?" + payload;

  // http://weewx.milwaukeesailing.net:8089/weatherstation/updateweatherstation.php?windspeedmph=15&winddir=45 //ultimately this is the query

  Serial.println("Requesting      : " + url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: G6EJDFailureDetectionFunction\r\n" + "Connection: close\r\n\r\n");
  // GET /weatherstation/updateweatherstation.php?windspeedmph=100&winddir=200 HTTP/1.1\r\nHost: weewx.milwaukeesailing.net\r\nUser-Agent: G6EJDFailureDetectionFunction\r\nConnection: close\r\n\r\n
  Serial.println("Request sent    : ");
  String line = client.readStringUntil('\n');  // read 1st line
  while (line != "\r" && my_counter < 5) {     //client.connected() used to be in condition of while loop, suspected to be what bugged the function though as being disconnected would prematurely exit the loop
    my_counter++;
    line = client.readStringUntil('\n');
    //Serial.println(line); // debug
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }
  line = client.readStringUntil('\n');
  //Serial.println(line);
  boolean Status = true;
  if (line == "success") line = "SUCCESS: Server confirmed all data received";
  if (line == "INVALIDPASSWORDID|Password or key and/or id are incorrect") {
    line = "Invalid PWS/User data entered in the ID and PASSWORD or GET parameters";
    Status = false;
  }
  if (line == "RapidFire Server") {
    line = "The minimum GET parameters of ID, PASSWORD, action and dateutc were not set correctly";
    Status = false;
  }
  Serial.println("Server Response : " + line);
  Serial.println("Status          : Closing connection");
  return Status;
}

void StartWiFi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
