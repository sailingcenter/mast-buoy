//example code from ovens garage video

double wave_height_ft;

const int pressureInputPin = 35; //select the analog input pin for the pressure transducer

const int pressureZero = 344; //analog reading of pressure transducer at 0psi, did the math again because ESP32 has 4096 range and 5v pin outputs 4.3v, calculated with v_min = 0.37V and v_max = 4.4V
const int pressureMax = 3752; //analog reading of pressure transducer at 10psi, changed this using 4096 as well, outlined above

const int pressuretransducermaxPSI = 10; //psi value of transducer being used
const int sensorReadDelay = 200; //constant integer to set the sensor read delay in milliseconds, so set to 200 is 5 reads per second

float pressureValue = 0; //variable to store the value coming from the pressure transducer

void setup() //setup routine, runs once when system turned on or reset
{
  Serial.begin(115200); //initializes serial communication at set baud rate bits per second
}

void loop() //loop routine runs over and over again forever
{
  pressureValue = analogRead(pressureInputPin); //reads value from input pin and assigns to variable
  Serial.print("ADC Value: ");
  Serial.println(pressureValue); //pretty sure this works because we print this before the other values

  pressureValue = (((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero)); //conversion equation to convert analog reading to psi, maxes out at 11.2 psi
  Serial.print(pressureValue, 1); //prints value from previous line to serial
  Serial.println(" psi_gauge"); //prints label to serial

  wave_height_ft = pressureValue * 2.23247;  //thank you Dr. Bowman for the conversion factor
  Serial.print("Wave Height ft: "); //10psig equals 22.4ft which isn't far from Dr. Bowman 23ft for 10psig
  Serial.println(wave_height_ft, 1);
  Serial.println(" ");
  
  delay(sensorReadDelay); //delay in milliseconds between read values
}