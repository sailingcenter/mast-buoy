//based off schematic from this video at 2:19 https://www.youtube.com/watch?v=oLvHawyH4Eo

//this code turns off the anemomter by using a 2n2222a BJT as a switch to control the 12v rail, using a GPIO pin pulled high as the gate voltage control.

void setup() {
  pinMode(2, OUTPUT);  //probably need to change pin in final design
}

void loop() {
  digitalWrite(2, HIGH);
  delay(7000);
  digitalWrite(2, LOW);
  delay(7000);
}