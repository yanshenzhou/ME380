#include <Servo.h>

Servo grip;  // create servo object to control a servo
Servo dex;
// twelve servo objects can be created on most boards

int pos_grip = 0;
int pos_dex = 0;    // variable to store the servo position

void setup() {
  grip.attach(9);
  dex.attach(10);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
}

void loop() {
  dex.write(180);
  grip.write(180);
  delay(5000);
  for (pos_grip = 180; pos_grip >= 10; pos_grip -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    grip.write(pos_grip);
    dex.write(0);
    Serial.print(pos_grip);
    Serial.println(" ");              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  pos_dex = 0;
  delay(5000);
    for (pos_dex = 0; pos_dex <= 180; pos_dex += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    dex.write(pos_dex);
    Serial.print(pos_dex);
    Serial.println(" ");              // tell servo to go to position in variable 'pos'
    delay(20);   
  }
  delay(1000);
   for (pos_dex = 180; pos_dex >= 0; pos_dex -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    dex.write(pos_dex);
    Serial.print(pos_dex);
    Serial.println(" ");              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
     for (pos_grip = 10; pos_grip <= 90; pos_grip += 1) { // goes from 180 degrees to 0 degrees
    grip.write(pos_grip);  
    Serial.print(pos_grip);
    Serial.println(" ");            // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(5000); 
}

