#include <SoftwareSerial.h>
 
#define RE 8
#define DE 7

const byte anemometer[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; //same interrogation frame for both the anemometors

byte values[9];
SoftwareSerial mod(2, 3);
 
void setup()
{
  Serial.begin(9600);
  mod.begin(4800);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  delay(1000);
}
 
void loop()
{
  // Transmit the request to the sensor
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  
  mod.write(anemometer, sizeof(anemometer));
  
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(10);
 
  // Wait until we have the expected number of bytes or timeout
  unsigned long startTime = millis();
  while (mod.available() < 9 && millis() - startTime < 1000) 
  {
    delay(1);
  }
 
  if (mod.available() >= 9)
  {
    // Read the response
    byte index = 0;
    while (mod.available() && index < 9) 
    {
      values[index] = mod.read();
      Serial.print(values[index], HEX);
      Serial.print(" ");
      index++;
    }
    Serial.println();
 
    // Parse the Wind Speed value
    int Wind_Speed_Int = int(values[3] << 8 | values[4]);
    float Wind_Speed_m_s = Wind_Speed_Int / 100.0;
    float Wind_Speed_kph = Wind_Speed_m_s * 3.6; // Conversion to km/h
 
    Serial.print("Wind Speed: ");
    Serial.print(Wind_Speed_kph);
    Serial.println(" km/h");
 
    // Parse the Wind Direction value
    int Wind_Direction = int(values[5] << 8 | values[6]);
    Serial.print("Wind Direction: ");
    Serial.print(Wind_Direction);
    Serial.println(" degrees");
  }
  else
  {
    Serial.println("Sensor timeout or incomplete frame");
  }
 
  delay(1000);
}