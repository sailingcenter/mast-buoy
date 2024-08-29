#include <SoftwareSerial.h>

#define RE 4 //receiver enable GPIO
#define DE 0 //driver enable GPIO

const byte anemometer[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; //same interrogation frame for both the anemometors

byte values[9];
SoftwareSerial mod(14, 13); //(RO, DI) both GPIO's

void clearSerialInputBuffer() {
  while (Serial.available() > 0) {
    char garbage = Serial.read(); // Read and discard incoming data from serial input, necessary to prevent timing issue
  }
}

void clearmodInputBuffer() {
  while (mod.available() > 0) {
    char garbage1 = mod.read(); // Read and discard incoming data from mod input (anemometer), necessary to prevent timing issue
  }
}

void setup()
{
  Serial.begin(115200);
  mod.begin(4800);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW); //anemometer starts out off
  delay(4000);
}
 
void loop()
{
  digitalWrite(2, HIGH);
  delay(4000);
  // Transmit the request to the sensor
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  mod.write(anemometer, sizeof(anemometer));
  delay(1);
  
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
    int Wind_Speed_Int = int(values[3] << 8 | values[4]); //converting this value is dependent on voltages of system, it seems. For me, changing what was given from how2electronics to the below conversion factor worked for me
    float Wind_Speed_kph = Wind_Speed_Int * 0.042696629213483; //obtained from using testo equipment and determing conversion factor
 
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

  Serial.flush();
  clearSerialInputBuffer();//call the clear function defined at the top
  mod.flush();
  clearmodInputBuffer();//call clear function defined at the top

  delay(10);
  digitalWrite(2, LOW);
  delay(4000);
}