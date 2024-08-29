#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <math.h>

float thetaM;
float phiM;
float thetaFold=0;
float thetaFnew;
float phiFold=0;
float phiFnew;

float thetaG=0;
float phiG=0;

float theta;
float phi;

float thetaRad;
float phiRad;

float Xm;
float Ym;
float psi;


float dt;
unsigned long millisOld;


int Uncorrected_Wind_Direction;

#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 myIMU = Adafruit_BNO055();

#define RE 4 //receiver enable GPIO
#define DE 0 //driver enable GPIO

const byte anemometer[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; //same interrogation frame for both the anemometors

byte values[9];
SoftwareSerial mod(14, 13); //(RO, DI) both GPIO's

void clearSerialInputBuffer() {
  while (Serial.available() > 0) {
    char garbage_serial = Serial.read(); // Read and discard incoming data from serial input, necessary to prevent timing issue
  }
}

void clearmodInputBuffer() {
  while (mod.available() > 0) {
    char garbage_mod = mod.read(); // Read and discard incoming data from mod input (anemometer), necessary to prevent timing issue
  }
}


void setup()
{
  Serial.begin(115200);//changed to 115200 from 4800
  mod.begin(4800);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  myIMU.begin();
  delay(10);
  int8_t temp=myIMU.getTemp();
  myIMU.setExtCrystalUse(true);
  millisOld=millis();
  delay(10);
}
 
void loop()
{

  //BEGIN ANEMOMETER SECTION

  // Transmit the request to the sensor
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  mod.write(anemometer, sizeof(anemometer));
  //delay(1); //no shot this delay is needed
  
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
    Uncorrected_Wind_Direction = int(values[5] << 8 | values[6]);
    Serial.print("Anemometer Direction: ");
    Serial.print(Uncorrected_Wind_Direction);
    Serial.println(" degrees");
  }
  else
  {
    Serial.println("Sensor timeout or incomplete frame");
  }

  Serial.flush(); //clean the pipes
  clearSerialInputBuffer();//call the clear function defined at the top
  mod.flush(); //clean the pipes
  clearmodInputBuffer();//call the clear function defined at the top

  delay(10); //delay to let settle
  
  //END ANEMOMETER SECTION


  //BEGIN COMPASS SECTION

  //this was said to only work up to about 45 degrees, so if the buoy tilts more than that the readings will likely not make sense,
  //although, such a tilt is likely outisde the operational range anyways.
  
  uint8_t system, gyro, accel, mg = 0; //I think calibrating is happening in the background, this just reads values locally
  myIMU.getCalibration(&system, &gyro, &accel, &mg); 
  imu::Vector<3> acc =myIMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> gyr =myIMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  imu::Vector<3> mag =myIMU.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

  thetaM=-atan2(acc.x()/9.8,acc.z()/9.8)/2/3.141592654*360;
  phiM=-atan2(acc.y()/9.8,acc.z()/9.8)/2/3.141592654*360;
  phiFnew=.95*phiFold+.05*phiM;
  thetaFnew=.95*thetaFold+.05*thetaM;

  dt=(millis()-millisOld)/1000.;
  millisOld=millis();
  theta=(theta+gyr.y()*dt)*.95+thetaM*.05;
  phi=(phi-gyr.x()*dt)*.95+ phiM*.05;
  thetaG=thetaG+gyr.y()*dt;
  phiG=phiG-gyr.x()*dt;

  phiRad=phi/360*(2*3.141592654);
  thetaRad=theta/360*(2*3.141592654);

  Xm=mag.x()*cos(thetaRad)-mag.y()*sin(phiRad)*sin(thetaRad)+mag.z()*cos(phiRad)*sin(thetaRad);
  Ym=mag.y()*cos(phiRad)+mag.z()*sin(phiRad);

  psi=(atan2(Ym,Xm)/(2*3.141592654)*360)+4;//added 4 at the end to make it geographic north from local declionation ~4 degrees
  //TODO: make algorithm so people can make the +4 to whatever they need without trial and error, as ~4 degree decliation is for MKE specifically

  Serial.print(accel); //accelerometer calibration variable, I believe
  Serial.print(",");
  Serial.print(gyro); //gyro calibration variable, I believe
  Serial.print(",");
  Serial.print(mg); //magnetometer calibration variable, I believe
  //Serial.print(",");
  //Serial.print(system); //system calibration variable, I believe
  //Serial.print(",");
  if (psi < 0){ //converts values from range -180 <= psi <= 180, to 0 <= psi <= 360 for further processing
    psi += 360;
  }
  Serial.print(" Magnetometer Direction, psi: ");
  Serial.print(psi); //this is our angle direction from 0 to 360 from magnetometer
  Serial.print(" degrees. ");

  float Corrected_Wind_Direction = psi + Uncorrected_Wind_Direction; //corrects wind direction
  if (Corrected_Wind_Direction > 360){ //fixes issue if we go "around the origin" and get a value above 360
    Corrected_Wind_Direction -= 360;
  }
  Serial.print("Corrected Wind Direction: ");
  Serial.print(Corrected_Wind_Direction); //this is our angle direction from 0 to 360 from magnetometer
  Serial.println(" degrees.");


  phiFold=phiFnew;
  thetaFold=thetaFnew;

  
  delay(BNO055_SAMPLERATE_DELAY_MS);
  
  //END COMPASS SECTION
} //end big for loop