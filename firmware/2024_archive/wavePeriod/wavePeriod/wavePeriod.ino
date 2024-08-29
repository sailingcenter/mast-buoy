//this is straight copy and paste from smartbuoy website

long pressure;
double altitude, min_height, max_height, mid_point, smudge_factor;
byte escaped, started;
unsigned long period_start, period_end;
float avg_period = -1;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long start_time = millis();
  pressure = baro.readPressure(true);
  altitude = baro.getAltitude(pressure);
  max_height = altitude;
  min_height = altitude; 

  //  for 15 seconds
  while(millis() - start_time < 15000){
    pressure = baro.readPressure(true);
    altitude = baro.getAltitude(pressure);
    if (altitude < min_height) min_height = altitude;
    if (altitude > max_height) max_height = altitude;
  }
  mid_point = (max_height + min_height)/2.0;
  smudge_factor = (max_height - mid_point)*0.15;
  Serial.print("Mid Point m: "); Serial.print(mid_point);
  
  start_time = millis();
  //  for 15 seconds 
  while(millis() - start_time < 15000){
    pressure = baro.readPressure(true);
    altitude = baro.getAltitude(pressure);
    //    if within a range of 30% of wave height from the mid point
    //    start the timer else stop it
    if (altitude < mid_point + smudge_factor && altitude > mid_point - smudge_factor){
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
}