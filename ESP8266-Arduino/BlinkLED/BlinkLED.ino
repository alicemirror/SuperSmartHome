/**
 * @file BlinkLED.ino
 * 
 */

void setup() {
  pinMode(0, OUTPUT);
}

void loop() {
  digitalWrite(0, HIGH);
  delay(3000); 
  digitalWrite(0, LOW);
  delay(100);
}
