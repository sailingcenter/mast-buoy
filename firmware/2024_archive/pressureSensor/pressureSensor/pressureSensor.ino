//example code from ovens garage video

const int pressureInputPin = 35; //select the analog input pin for the pressure transducer
const int pressureZero = 413; //analog reading of pressure transducer at 0psi, did the math again because ESP32 has 4096 range and esp32 low output causes resting 0.45V instead of 0.5V
const int pressureMax = 3683; //analog reading of pressure transducer at 100psi, changed this using 4096 as well, outlined above
const int pressuretransducermaxPSI = 10; //psi value of transducer being used, says 10 but psi on analog gauge reads 12 when at 3.3v on the multimeter with 3.3v power input
const int sensorReadDelay = 200; //constant integer to set the sensor read delay in milliseconds, so set to 200 is 5 reads per second

float pressureValue = 0; //variable to store the value coming from the pressure transducer

void setup() //setup routine, runs once when system turned on or reset
{
  Serial.begin(115200); //initializes serial communication at set baud rate bits per second
}

void loop() //loop routine runs over and over again forever
{
  pressureValue = analogRead(pressureInputPin); //reads value from input pin and assigns to variable
  pressureValue = ((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero) + 0.6; //conversion equation to convert analog reading to psi, 0.6 added to correct to 0psig when it should read 0psig
  Serial.print(pressureValue, 1); //prints value from previous line to serial
  Serial.println("psi"); //prints label to serial
  delay(sensorReadDelay); //delay in milliseconds between read values
}