#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <math.h>

//this was said to only work up to about 45 degrees, so if the buoy tilts more than that the readings will likely not make sense,
//although, such a tilt is likely outisde the operational range anyways.

float thetaM;
float phiM;
float thetaFold = 0;
float thetaFnew;
float phiFold = 0;
float phiFnew;

float thetaG = 0;
float phiG = 0;

float theta;
float phi;

float thetaRad;
float phiRad;

float Xm;
float Ym;
float psi;


float dt;
unsigned long millisOld;

#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 myIMU = Adafruit_BNO055();

void setup() {
  Serial.begin(115200);
  myIMU.begin();
  delay(1000);
  int8_t temp = myIMU.getTemp();
  myIMU.setExtCrystalUse(true);
  millisOld = millis();
}

void loop() {
  uint8_t system, gyro, accel, mg = 0;
  myIMU.getCalibration(&system, &gyro, &accel, &mg);
  imu::Vector<3> acc = myIMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> gyr = myIMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  imu::Vector<3> mag = myIMU.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  thetaM = -atan2(acc.x() / 9.8, acc.z() / 9.8) / 2 / 3.141592654 * 360;
  phiM = -atan2(acc.y() / 9.8, acc.z() / 9.8) / 2 / 3.141592654 * 360;
  phiFnew = .95 * phiFold + .05 * phiM;
  thetaFnew = .95 * thetaFold + .05 * thetaM;

  dt = (millis() - millisOld) / 1000.;
  millisOld = millis();
  theta = (theta + gyr.y() * dt) * .95 + thetaM * .05;
  phi = (phi - gyr.x() * dt) * .95 + phiM * .05;
  thetaG = thetaG + gyr.y() * dt;
  phiG = phiG - gyr.x() * dt;

  phiRad = phi / 360 * (2 * 3.14);
  thetaRad = theta / 360 * (2 * 3.14);

  Xm = mag.x() * cos(thetaRad) - mag.y() * sin(phiRad) * sin(thetaRad) + mag.z() * cos(phiRad) * sin(thetaRad);
  Ym = mag.y() * cos(phiRad) + mag.z() * sin(phiRad);



  psi = (atan2(Ym, Xm) / (2 * 3.14) * 360) + 4;  //added 4 at the end to make it geographic north from local declionation ~4 degrees
  
  //Serial.print(acc.x()/9.8);
  //Serial.print(",");
  //Serial.print(acc.y()/9.8);
  //Serial.print(",");
  //Serial.print(acc.z()/9.8);
  //Serial.print(",");
  Serial.print(accel);  //accelerometer calibration variable
  Serial.print(",");
  Serial.print(gyro);  //gyro calibration variable
  Serial.print(",");
  Serial.print(mg);  //magnetometer calibration variable
  Serial.print(",");
  //Serial.print(system); //system calibration variable
  //Serial.print(",");
  //Serial.print(thetaM);
  //Serial.print(",");
  //Serial.print(phiM);
  //Serial.print(",");
  //Serial.print(thetaFnew);
  //Serial.print(",");
  //Serial.print(phiFnew);
  //Serial.print(",");
  //Serial.print(thetaG);
  //Serial.print(",");
  //Serial.print(phiG);
  //Serial.print(",");
  //Serial.print(theta);
  //Serial.print(",");
  //Serial.print(phi);
  //Serial.print(",");
  if (psi < 0) {  //converts values from range -180 <= psi <= 180, to 0 <= psi <= 360
    psi += 360;
  }
  Serial.println(psi);  //this is our compass angle direction from 0 to 360: 0 or 360=N, 90=E, 180=S, 270=W

  phiFold = phiFnew;
  thetaFold = thetaFnew;


  delay(BNO055_SAMPLERATE_DELAY_MS);
}