//TODO: make the measurmenet an average over a period, not an instantaneous value reading

//example code below should be plug and play with my esp32 setup, taken from https://www.roboticboat.uk/Microcontrollers/Uno/GarminPaddleWheel/GarminPaddleWheel.html courtesy of Dr. Bowman

// Timer
unsigned long mytime;
unsigned long lasttime = 0;
unsigned long diff;

int val;
int oldval;

float water_speed;

// the setup() method runs once, when the sketch starts

void setup() 
{
Serial.begin(115200);
}


void loop() 
{
  //delay(3000);
  // Read the speedometer
  // The speedometer signal oscillates between 0 and 1023 (when at 5v)
  // The speedometer signal oscillates between 0 and 659 (when at 3.3v)
  // So the signal is either 0 or a positive number.
  // Want to detect then the signal falls to zero or rises from zero
  // The speed of the oscillation is the rotation of the wheel
  
  val = analogRead(34);
  
  // The paddle wheel has turned. 
  if ((val == 0 && oldval>0) || (oldval == 0 && val>0)) {

    // Find change in milliseconds since last
    mytime = millis();
    diff = mytime - lasttime;
    lasttime = mytime;

    // The wheel has moved 2 cm between the change 0 and 1....taking his word for that here

    
    Serial.print(diff);
    Serial.print("ms,\t");
    
    water_speed = (float)1300 / diff; //because of math in the working example, trying with 1300
    water_speed = water_speed / 100; //for testing, the testo can't do knots but it can do m/s
    //water_speed = water_speed / 51.444; //covert to knots from cm/s
    Serial.print(water_speed);
    Serial.println(" m/s"); //again, for testing
    //Serial.println(" knots");
  }
  // Update the oldval
  oldval = val;
  //delay(1000);
}