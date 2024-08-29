//for wave period and height, we are going to have to go with a pressure transducer angle, going to need to talk to Dr. Bowman on that one. 
//basing the code off this smart buoy example, then going to map that other example with the dude using the exact pressure sensor I have to get a working program
//note: need to do some calibration, probably, by measuring the voltages when nothing is happening.

//below is the code combined from the smart buoy and ovens garage examples, and also added the smart buoy wave period example


double wave_pressure, min_pressure, max_pressure, wave_height_ft, mid_pressure, fudge_factor;
byte escaped, started;
unsigned long period_start, period_end;
float avg_period = -1;

const int pressureInputPin = 35; //select the analog input pin for the pressure transducer
const int pressureZero = 413; //analog reading of pressure transducer at 0psi, did the math again because ESP32 has 4096 range and esp32 low output causes resting 0.45V instead of 0.5V
const int pressureMax = 3683; //analog reading of pressure transducer at 100psi, changed this using 4096 as well, outlined above
const int pressuretransducermaxPSI = 10; //psi value of transducer being used, says 10 but psi on analog gauge reads 12 when at 3.3v on the multimeter with 3.3v power input
const int sensorReadDelay = 200; //constant integer to set the sensor read delay in milliseconds, so set to 200 is 5 reads per second

float pressureValue = 0; //variable to store the value coming from the pressure transducer

int measurementWindow = 10*1000; //measurement windows for height and period in milliseconds

void setup() {
  Serial.begin(115200);
}

void loop() {
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
  Serial.print("  Period s: "); Serial.println(avg_period/1000);
  Serial.println();
}

/* Note:
  Serial.print(pressureValue, 1); //prints value from previous line to serial, after the comma is decimal point precision specification, didn't know that
  Serial.println("psi"); //prints label to serial
*/