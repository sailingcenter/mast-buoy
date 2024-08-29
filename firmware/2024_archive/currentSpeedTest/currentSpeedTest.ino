//TODO: make the measurmenet an average over a period, not an instantaneous value reading, also note millis overflow issue

//example code below should be plug and play with my esp32 setup, taken from https://www.roboticboat.uk/Microcontrollers/Uno/GarminPaddleWheel/GarminPaddleWheel.html courtesy of Dr. Bowman

// Timer
unsigned long startTime;
unsigned long endTime;
unsigned long paddleCheckFail;
unsigned long diff;

int val;
int oldval;
int readCounter;

float water_speed;
bool readFlag;

// the setup() method runs once, when the sketch starts

void setup() {
  Serial.begin(115200);
}

void loop() {
  //delay(1000); //debug for simulating delays in buoy loop

  // Read the speedometer
  // The speedometer signal oscillates between 0 and 1023 (when at 5v)
  // The speedometer signal oscillates between 0 and 659 (when at 3.3v)
  // So the signal is either 0 or a positive number.
  // Want to detect then the signal falls to zero or rises from zero
  // The speed of the oscillation is the rotation of the wheel

  readFlag = 0;  //use this read flag to determine if we've read 6 pulses, used to exit while loop if we have a full rotation
  readCounter = 0; //counter to count up to 12 signals

  startTime = millis();  //logs when in the code we got to this point before we enter the loop, this needs to be right above the loop of taking measurements, no delays between

  while (readFlag == 0) {
    val = analogRead(34);  //read analog value of pin
    if ((val == 0 && oldval > 0) || (oldval == 0 && val > 0)) {  // The paddle wheel has turned.
      readCounter++;
    }
    
    // Update the oldval
    oldval = val;

    if (readCounter == 12) {  //12 readings is what we're looking for, due to each count being EITHER a falling OR rising edge, so 12 counts represents 6 paddle wheels have gone by, also known as a full rotation of the wheel
      endTime = millis();
      diff = endTime - startTime;

      Serial.print(diff);
      Serial.print("ms,\t");

      // The wheel has moved 2 cm between the change 0 and 1.
      // Numbers are approximate
      // Diameter of wheel = 2 * 3.142 * 0.02 (2.pi.r)
      // 12.5cm / 6 paddles = ~2 cm per paddle, or just 12.5cm for the whole wheel
      water_speed = (float)12500 / diff; //changed to 125000 from 2000, because we are doing the whole wheel at once now
      water_speed = water_speed / 51.444;  //covert to knots from cm/s
      Serial.print(water_speed);
      Serial.println(" knots");
      
      readFlag = 1;//sets the read flag to 1 to exit loop, as we have our measurements
    }

    paddleCheckFail = millis(); //for failure detection, like if the paddlewheel is tangled in seaweed or zebra mussels
    if (paddleCheckFail - startTime > 20000 ) { //timeout after 20 seconds
      readFlag = 1;
      //Serial.println("exiting, buoy stuck"); //debug for checking if stuck
    }   
  } //end paddle while
  //delay(2000); //debug for simulating delays in buoy loop
}