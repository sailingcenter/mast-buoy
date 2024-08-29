/*
Adapted from https://www.youtube.com/watch?v=L4dY8FabHW0 

I have removed much of the code, only intersted in the PWS upload protocol
*/
  
#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>

const char* ssid      = "INSERT SSID"; //router (wi-fi) name
const char* password  = "INSERT PASSWORD"; //wi-fi password

const char* host      = "weewx.milwaukeesailing.net"; //"weatherstation.wunderground.com" originally. Claude provided this weewx url as it's what he uses and what we need to transmit to
const int httpPort   = 8089;

void setup() {
  Serial.begin(115200);
  StartWiFi();
}

void loop() 
{
  boolean this_upload = UploadDataToWU();
  if (!this_upload) {
    Serial.println("Error uploading to Weather Underground, trying next time");
  } else {
    // do nothing; uploading successsful;
  } 
  //Serial.println("Error uploading to Weather Underground, trying next time");
  //delay(30*1000); // for a 30 second update rate, for testing
}

boolean UploadDataToWU()
{
  int my_counter = 0;
  WiFiClient client;
  // Use WiFiClient class to create connection
  Serial.println("Connecting to   : "+String(host));
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return false;
  }
  String url = "/weatherstation/updateweatherstation.php?windspeedmph=100&winddir=200"; //THAT'S THE PAYLOAD

  // http://weewx.milwaukeesailing.net:8089/weatherstation/updateweatherstation.php?windspeedmph=15&winddir=45 //ultimately this is the query

  Serial.println("Requesting      : "+url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: G6EJDFailureDetectionFunction\r\n" +
               "Connection: close\r\n\r\n");
  // GET /weatherstation/updateweatherstation.php?windspeedmph=100&winddir=200 HTTP/1.1\r\nHost: weewx.milwaukeesailing.net\r\nUser-Agent: G6EJDFailureDetectionFunction\r\nConnection: close\r\n\r\n
  Serial.println("Request sent    : ");
  String line = client.readStringUntil('\n'); // read 1st line
  while (line != "\r" && my_counter < 5) { //client.connected() used to be in condition of while loop, suspected to be what bugged the function though as being disconnected would prematurely exit the loop
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
  Serial.println("Server Response : "+line);
  Serial.println("Status          : Closing connection");
  return Status;
}

void StartWiFi(){
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