/*
Project: Senior Design Group M78 Remote Weather Navigation Buoy
File: Master Remote Weather Buoy
Institution: Marquette University
Engineer: Benjamin Doyle
Electrical Advisor: Dr. Cris Ababei
Mechanical Advisor: Dr. Tony Bowman
*/

#include <SoftwareSerial.h>
#include <RadioLib.h>
#include "boards.h" //this header is edited to get rid of unnecessary crud like delays and such
#include <bits/stdc++.h> //for payload to work
using namespace std; //for payload to work
#include <Wire.h>//IMU
#include <Adafruit_Sensor.h>//IMU
#include <Adafruit_BNO055.h>//IMU
#include <utility/imumaths.h>//IMU
#include <math.h>//IMU
#include <TinyGPSPlus.h>//GPS


// save transmission state between loops for LoRa
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate that a packet was sent for LoRa
volatile bool transmittedFlag = false;

// disable interrupt when it's not needed for LoRa
volatile bool enableInterrupt = true;

// this function is called when a complete LoRa packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void)
{
    // check if the interrupt is enabled
    if (!enableInterrupt) {
        return;
    }
    // we sent a packet, set the flag
    transmittedFlag = true;
}



//VARIABLES and DECLARATION section below:
const byte anemometer[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; //same interrogation frame for both the anemometors

byte values[9]; //bite array for reading anemometer
SoftwareSerial mod(14, 13); //anemometer object, (RO, DI) both GPIO's for anemometer

#define RE 4 //receiver enable GPIO for anemometer
#define DE 0 //driver enable GPIO for anemometer
#define anemometerSwitch 2 //pin used to control BJT anemometer switch

SX1276 radio = new Module(RADIO_CS_PIN, RADIO_DIO0_PIN, RADIO_RST_PIN, RADIO_BUSY_PIN); //LoRa object

Adafruit_BNO055 myIMU = Adafruit_BNO055(); //IMU object

#define BNO055_SAMPLERATE_DELAY_MS (100) //IMU sample delay of 100ms

//this giant brick of variables down to millisOld is for the tilt compensated compass
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
//float psi; //moved down to the payload variables below
float dt;
unsigned long millisOld;

//variable brick down to measurementWindow is for pressure sensor to measure wave period and height
double wave_pressure, min_pressure, max_pressure, mid_pressure, fudge_factor;
byte escaped, started;
unsigned long period_start, period_end;
float avg_period = -1;
const int pressureInputPin = 35; //select the analog input pin for the pressure transducer
const int pressureZero = 413; //analog reading of pressure transducer at 0psi, did the math again because ESP32 has 4096 range and esp32 low output causes resting 0.45V instead of 0.5V
const int pressureMax = 3683; //analog reading of pressure transducer at 10psi, changed this using 4096 as well, outlined above
const int pressuretransducermaxPSI = 10; //psi value of transducer being used, says 10 but psi on analog gauge reads 12 when at 3.3v on the multimeter with 3.3v power input, should be close enough. Going to use 3.3v for pressure transducer
const int sensorReadDelay = 200; //constant integer to set the sensor read delay in milliseconds, so set to 200 is 5 reads per second
float pressureValue = 0; //variable to store the value coming from the pressure transducer
int measurementWindow = 10*1000; //measurement windows for height and period in milliseconds

//variable brick down to water_speed_kn is for current speed detection
unsigned long startTime;
unsigned long endTime;
unsigned long paddleCheckFail;
unsigned long diff;
int val;
int oldval;
int paddleReadCounter;
float water_speed;
bool paddleReadFlag;
//float water_speed_kn; //moved to payload variables


//variable brick down to define RXD1 and TXD1 is for GPS
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps; //the TinyGPSPlus object
#define RXD1 25 
#define TXD1 15

int Uncorrected_Wind_Direction; //global wind direction variable




//below are all the variables that will eventually be sent in the payload, also known as the payload variables
int Corrected_Wind_Direction; //wind direction
float Wind_Speed_kph; //wind speed
double wave_height_ft; //wave height
float wave_period_seconds; //wave period in seconds
float gps_longitude_googlemaps; //longitude that you can plug into google maps, still unimplemented 4/16 gotta figure out how to just parse this info in the lego brick first
float gps_latitude_googlemaps; //latitude that you can plug into google maps, still unimplmented 4/16 gotta figure out how to just parse this info in the lego brick first
float water_speed_kn; //current speed
float psi; //compass direction


//custom functions have to go here because the modules have to begin before they can be modified
void clearSerialInputBuffer() { // Read and discard incoming data from serial input, necessary to prevent timing issue with anemometer
  while (Serial.available() > 0) {
    char garbageSerial = Serial.read(); 
  }
}

void clearmodInputBuffer() { // Read and discard incoming data from mod input (anemometer), necessary to prevent timing issue with anemometer
  while (mod.available() > 0) {
    char garbageMod = mod.read();
  }
}



void setup() //setup function
{ 
  initBoard(); //Serial.begin(115200) is in here, and we need this for the LoRa to work
  delay(1500);
  Serial.println("running setup");//debug
  Serial1.begin(GPSBaud, SERIAL_8N1, RXD1, TXD1);//for GPS hardware serial
  mod.begin(4800);//for anemometer
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  myIMU.begin();
  int8_t temp=myIMU.getTemp();//this is copied from IMU example, but why is it here?
  myIMU.setExtCrystalUse(true);
  millisOld=millis();
  pinMode(anemometerSwitch, OUTPUT);//using pin 0 as the switch to turn on the anemometer when we need to take a reading
  digitalWrite(anemometerSwitch, LOW);//anemometer starts out off
  delay(10);


  // initialize SX1276 with default settings
  //Serial.print(F("[SX1276] Initializing ... "));
  #ifndef LoRa_frequency
    int state = radio.begin(915.0);
    #else
    int state = radio.begin(LoRa_frequency);
  #endif
  if (state == RADIOLIB_ERR_NONE) {
    //Serial.println(F("success!"));
    radio.setOutputPower(20); //changed to 20 from 17 given in example code
    radio.setBandwidth(125);
    radio.setCurrentLimit(120);
    //using default spreading factor of 9
  } 
    else {
      Serial.print(F("failed, code "));
      Serial.println(state);
      while (true);
    }
  // set the function that will be called
  // when packet transmission is finished
  radio.setDio0Action(setFlag, RISING);

  // start transmitting the first packet
  //Serial.print(F("[SX1276] Sending first packet ... "));

  // you can transmit C-string or Arduino string up to
  // 256 characters long
  transmissionState = radio.startTransmit("Connection Established.");

  // you can also transmit byte array up to 256 bytes long
  /*
  byte byteArr[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
  state = radio.startTransmit(byteArr, 8);
  */
  Serial.println("ending setup");//debug
}





void loop() //loop function
{
  //BEGIN ANEMOMETER READ 

  digitalWrite(anemometerSwitch, HIGH);//turn on anemometer with BJT switch
  Serial.println("anemometer should be on");//debug
  delay(4000);//delay because anemometer takes awhile to boot up before it can give useful readings

  
  //current stopping point: here the anemometer gives us a square and stops working, next step is to try this configuration on another board. Even if I open it up to print out values to serial it stil gives me the brick

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
    int Wind_Speed_Int = int(values[3] << 8 | values[4]); //converting this value is dependent on voltages of system, it seems. For me, changing what was given from how2electronics to the below conversion factor worked for me
    Wind_Speed_kph = Wind_Speed_Int * 0.042696629213483; //obtained from using testo equipment and determing conversion factor

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
    Serial.println("Sensor timeout or incomplete frame.");
  }

  delay(1000);//debug

  Serial.flush();
  clearSerialInputBuffer();//call the clear function defined at the top
  mod.flush();
  clearmodInputBuffer();//call clear function defined at the top

  delay(10);//allow time to read garbage serial before turning anemometer off

  digitalWrite(anemometerSwitch, LOW);//turn off the anemometer after reading has been taken
  Serial.println("anemometer should be off");//debug
  delay(4000);//wait 5 seconds to make sure anemometer is off

  //END ANEMOMETER READ



  //BEGIN COMPASS CORRECTION

  //this was said to only work up to about 45 degrees, so if the buoy tilts more than that the readings will likely not make sense,
  //although, such a tilt is likely outisde the operational range anyways.
  for (int magCount = 0; magCount < 100; magCount++) { //needed to get accurate reading from magnetometer
    uint8_t system, gyro, accel, mg = 0; //I think calibrating is happening in the background, this just reads values so we can display in serial monitor/etc here
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
    //maybe in future make interface so people can make the +4 to whatever they need without trial and error, as ~4 degree decliation is for MKE specifically
    /*
    Serial.print(accel); //accelerometer calibration variable, I believe
    Serial.print(",");
    Serial.print(gyro); //gyro calibration variable, I believe
    Serial.print(",");
    Serial.print(mg); //magnetometer calibration variable, I believe
    Serial.print(",");
    Serial.print(system); //system calibration variable, I believe
    Serial.print(",");
    */
    if (psi < 0) { //converts values from range -180 <= psi <= 180, to 0 <= psi <= 360 for further processing
      psi += 360;
    }

    phiFold=phiFnew;
    thetaFold=thetaFnew;
  
    delay(BNO055_SAMPLERATE_DELAY_MS); //100ms sensor read delay
  }//end psi acquisition if

  Serial.print(" Magnetometer Direction, psi: ");
  Serial.print(psi); //this is our angle direction from 0 to 360 from magnetometer
  Serial.print(" degrees. ");

  Corrected_Wind_Direction = psi + Uncorrected_Wind_Direction; //corrects wind direction

  if (Corrected_Wind_Direction > 360) { //fixes issue if we go "around the origin" and get a value above 360
  Corrected_Wind_Direction -= 360;
  }

  Serial.print("Corrected Wind Direction: ");
  Serial.print(Corrected_Wind_Direction); //this is our angle direction from 0 to 360 from magnetometer
  Serial.println(" degrees.");
  
  //END COMPASS CORRECTION



  //BEGIN WATER SPEED READ

  paddleReadFlag = 0;  //use this read flag to determine if we've read 6 pulses, used to exit while loop if we have a full rotation
  paddleReadCounter = 0; //counter to count up to 12 signals

  startTime = millis();  //logs when in the code we got to this point before we enter the loop, this needs to be right above the loop of taking measurements, no delays between

  while (paddleReadFlag == 0) {
    val = analogRead(34);  //read analog value of pin
    if ((val == 0 && oldval > 0) || (oldval == 0 && val > 0)) {  // The paddle wheel has turned.
      paddleReadCounter++;
    }
    
    // Update the oldval
    oldval = val;

    if (paddleReadCounter == 12) {  //12 readings is what we're looking for, due to each count being EITHER a falling OR rising edge, so 12 counts represents 6 paddle wheels have gone by, also known as a full rotation of the wheel
      endTime = millis();
      diff = endTime - startTime;

      Serial.print(diff); //debug
      Serial.print("ms,\t"); //debug

       
      // circumference of wheel = 2 * 3.142 * 1.85 [cm] (2.pi.r) = 11.6 cm

      water_speed = (float)11600 / diff; //original guy's math was weird, the circumference of the whole circle is 11.6 cm, THEN you make it 11,600 because milliseconds so it works out to cm/s
      water_speed_kn = water_speed / 51.444;  //convert to knots from cm/s
      Serial.print(water_speed_kn);
      Serial.println(" knots");
      
      paddleReadFlag = 1;//sets the read flag to 1 to exit loop, as we have our measurements
    }

    paddleCheckFail = millis(); //for failure detection, like if the paddlewheel is tangled in seaweed or zebra mussels and isn't spinning/giving readings as a result
    if (paddleCheckFail - startTime > 20000 ) { //timeout after 20 seconds
      paddleReadFlag = 1;
      water_speed_kn = -1; //sets speed negative to indicate a bogus reading, instead of sending previously created reading
      Serial.println("exiting, paddle stuck"); //debug for checking if stuck
    }   
  } //end paddle while


  //END WATER SPEED READ



  //BEGIN PRESSURE SENSOR WAVE PERIOD/HEIGHT

  unsigned long start_time = millis();
  max_pressure = 0; //dummy starting value
  min_pressure = 0; //dummy starting value

  //  for 10 seconds
  while(millis() - start_time < measurementWindow){ //set seconds value of measurment window
    pressureValue = analogRead(pressureInputPin); //reads value from input pin and assigns to variable
    pressureValue = (((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero))-0.47+0.6; //conversion equation to convert analog reading to psi, Dr. Bowman offset for lowering the sensor into the water, then the 0.6 value from before to zero out the scale (possibly some nonlinearity)

    if (pressureValue < min_pressure) min_pressure = pressureValue;
    if (pressureValue > max_pressure) max_pressure = pressureValue;

    delay(sensorReadDelay); //delay in milliseconds between read values, for our application we expect between 0-5 gauge psi
  }
  wave_pressure = (max_pressure - min_pressure)/2.0; //ahhh when they say "6 ft wave" it means it looks like 12 foot if you're at the bottom of it, that's why divide by 2
  wave_height_ft = wave_pressure * 2.23247;  //thank you Dr. Bowman for the conversion factor
  Serial.print("Wave Height ft: ");
  Serial.println(wave_height_ft, 1);

  mid_pressure = (max_pressure + min_pressure)/2.0;
  fudge_factor = (max_pressure - mid_pressure)*0.15; //think the 0.15 is like a 15% deal
  Serial.print("Mid Point m: "); 
  Serial.println(mid_pressure);
  
  //now, wave period
  start_time = millis(); //why set it again?
  while(millis() - start_time < measurementWindow){ //set seconds value of measurement window

    pressureValue = analogRead(pressureInputPin); //reads value from input pin and assigns to variable
    pressureValue = ((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero); //conversion equation to convert analog reading to psi
    delay(sensorReadDelay); //delay in milliseconds between read values
    
    //    if within a range of 30% of wave height from the mid point
    //    start the timer else stop it
    if (pressureValue < mid_pressure + fudge_factor && pressureValue > mid_pressure - fudge_factor){
      if ( !started){
        period_start = millis();
        started = true;
      }
      else{
        if ( escaped ){
          // if it has started and escaped and is now returning
          period_end = millis();
          started = false;
          escaped = false;
          // if the variable has already been assigned
          // use its previous value and new value to work out avg
          if(avg_period != -1){
            avg_period = (avg_period + (period_end-period_start)*2)  / 2.0;
          }
          // assign it
          else{
            avg_period =  (period_end-period_start)*2; 
          }
        }
      }
    }
    else{
      escaped = true;
    } 
  }
  wave_period_seconds = avg_period/1000;
  Serial.print("  Period s: "); 
  Serial.println(wave_period_seconds); //debug
  Serial.println(); //debug

  //END PRESSURE SENSOR WAVE PERIOD/HEIGHT


  //BEGIN GPS

  //my print statements, parsing out the google maps latitude and longitude values 
  Serial.println();
  Serial.print("Latitude: ");
  Serial.println(gps.location.lat(), 8);//prints google maps latitude value
  Serial.print("Longitude: ");
  Serial.println(gps.location.lng(), 8);//prints google maps longitude value

  smartDelay(1000);

  gps_latitude_googlemaps = gps.location.lat();//latitude for sending in payload
  gps_longitude_googlemaps = gps.location.lng();//longitude for sending in payload

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));

  //END GPS



  //BEGIN LORA DATA TRANSMIT

  // check if the previous transmission finished
  if (transmittedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    transmittedFlag = false; 

    if (transmissionState == RADIOLIB_ERR_NONE) {
      // packet was successfully sent
      //Serial.println(F("transmission finished!"));

      // NOTE: when using interrupt-driven transmit method,
      //       it is not possible to automatically measure
      //       transmission data rate using getDataRate()

    } 
    else {
      //Serial.print(F("failed, code "));
      //Serial.println(transmissionState);
    }

  // clean up after transmission is finished
  // this will ensure transmitter is disabled,
  // RF switch is powered down etc.
  radio.finishTransmit();

  // wait a second before transmitting again
  delay(10); //1 second might not be necessary, changing to 10ms

  // send another one
  //Serial.print(F("[SX1276] Sending another packet ... "));


  // you can transmit C-string or Arduino string up to
  // 256 characters long

  //below block is turning everything into strings
  string Wind_Speed_kph_Str = to_string(Wind_Speed_kph);
  string Corrected_Wind_Direction_Str = to_string(Corrected_Wind_Direction);
  string wave_height_ft_Str = to_string(wave_height_ft); 
  string wave_period_seconds_Str = to_string(wave_period_seconds); //wave period in seconds
  string gps_longitude_googlemaps_Str = to_string(gps_longitude_googlemaps); //longitude that you can plug into google maps, still unimplemented 4/16 gotta figure out how to just parse this info in the lego brick first
  string gps_latitude_googlemaps_Str = to_string(gps_latitude_googlemaps); //latitude that you can plug into google maps, still unimplmented 4/16 gotta figure out how to just parse this info in the lego brick first
  string water_speed_kn_Str = to_string(water_speed_kn); //current speed
  string psi_Str = to_string(psi); //compass direction, assumed to be "coming from the: [psi]"

  //example string sent to claude: http://weewx.milwaukeesailing.net:8089/weatherstation/updateweatherstation.php?windspeedmph=15&winddir=45
  //so we need the payload string to be "windspeedmph=15&winddir=45"

  /* 
  Legend of variable acronyms, if needed:
  wsk = Wind speed kph
  wdd = Wind direction degrees aka Corrected Wind Direction
  whf = Wave height feet
  wps = Wave period seconds
  lng = longiutde
  lat = latitude
  wsk = water speed in knots
  watdd = water direciton degrees, "from the [psi]"
  */
  string payload = "windSpeedKph=" + Wind_Speed_kph_Str + "&" + 
  "windDirectionDegrees=" + Corrected_Wind_Direction_Str + "&" +
  "waveHeightFt=" + wave_height_ft_Str + "&" + 
  "wavePeriodSeconds=" + wave_period_seconds_Str + "&" + 
  "gpsLong=" + gps_longitude_googlemaps_Str + "&" + 
  "gpsLat=" + gps_latitude_googlemaps_Str + "&" + 
  "waterSpeedKn=" + water_speed_kn_Str + "&" + 
  "waterDirection=" + psi_Str; //THE PAYLOAD

  Serial.println(payload.c_str()); //debug print, also: in order to print to serial monitor, need the .c_str() appended to payload string
  
  transmissionState = radio.startTransmit(payload.c_str()); //THE DATA HAS LEFT THE BUOY


  #ifdef HAS_DISPLAY
        if (u8g2) {
            char buf[256];
            u8g2->clearBuffer();
            u8g2->drawStr(0, 12, "Transmitting: OK!");
            snprintf(buf, sizeof(buf), "Rate:%.2f bps", radio.getDataRate());
            u8g2->drawStr(5, 30, buf);
            u8g2->sendBuffer();
        }
  #endif

  #ifdef EDP_DISPLAY
    display.setRotation(1);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 15);
    display.println("Transmitting: OK!");
    display.update();
    #endif

  //Serial.end(); //don't think this is necessary

  // we're ready to send more packets,
  // enable interrupt service routine
  enableInterrupt = true;
  
  //END LORA TRANSMIT 
  

  
  if (millis() > 45*24*60*60*1000) { //if 45 days have passed, we need to restart, otherwise millis will overflow after ~49 days. math is conversion from 45 days to milliseconds
    ESP.restart();
  }

  //ALSO TODO: sleep function after sending packet? To save power. Will need to test if things like GPS will get messed up because of power/etc etc...etc
  
  
  //delay(10*60*1000);//10min delay between readings


  //NOW...DO IT AGAIN
  }
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) //this needs to be below loop for gps to work, is how it was in example code from internet
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available()) //changed to serial1
      gps.encode(Serial1.read()); //changed to serial1
  } while (millis() - start < ms);
}