int xMotorPin1 = 27; int xMotorPin2 = 26; int zMotorPin1 = 22; int zMotorPin2 = 23;
int xMotorCtrlPin = 8; int zMotorCtrlPin = 9;
int xOptoPin = A10; int zOptoPin = A11;
int servoGripPin = 2; int servoRotateArmPin = 3; int servoSpinDiePin = 4;

int gripPos = 100; //position of gripper servo for die fully gripped

#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>
#include <math.h>

Servo servoGrip;
Servo servoRotateArm;
Servo servoSpinDie;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(xMotorPin1, OUTPUT);
  pinMode(xMotorPin2, OUTPUT);
  pinMode(zMotorPin1, OUTPUT);
  pinMode(zMotorPin2, OUTPUT);

  //assign PWM pins to each servo
  servoGrip.attach(servoGripPin);
  servoRotateArm.attach(servoRotateArmPin);
  servoSpinDie.attach(servoSpinDiePin);
}

void loop() {
// put your main code here, to run repeatedly:
  grip(true);
  delay(500);
  grip(false);
  delay(500);

}

//run for a certain time (+ve and -ve are different directions) (0.01s increments)
//for z, +ve time is up
//for x, +ve time is up
void motorMove(bool xMove, double time) {

  //default to operate z motor, if bool xMove is true then do x motor
  int pin1 = zMotorPin1; int pin2 = zMotorPin2; int pinCtrl = zMotorCtrlPin; int pinOpto = zOptoPin;
  if(xMove) {
    pin1 = xMotorPin1; pin2 = xMotorPin2; pinCtrl = xMotorCtrlPin; pinOpto = xOptoPin;
  }

  //default pin values to spin motor forwards, flip if moving in -ve direction
  int pin1Val = 1; int pin2Val = 0;
  if(time < 0) {
    pin1Val = 0;
    pin2Val = 1;
    Serial.println("flipping to down");
  }
  
  //operate the motor
  analogWrite(pinCtrl, 255);
  digitalWrite(pin1, pin1Val);
  digitalWrite(pin2, pin2Val);

  if(pin1Val > pin2Val) {
    Serial.println("Moving up");
  }
  else if (pin1Val < pin2Val) {
    Serial.println("Moving down");
  }
  else {
    Serial.println("pins equal, not moving");
  }
  //turn off the motor when opto is hit or enough time has passed
  int timeCounter = 0;
  while(timeCounter < abs(time)*1000) {
    delay(10);
    timeCounter += 10;
  }
  digitalWrite(pin1, 0);
  digitalWrite(pin2, 0);

}

//grip the die, lift small amount, spin the die
void spinDie(int rotation) {
  //note: the driving cup should be set to align with die for 0 position

  int pos = 0;
  //if +ve rotation, go to 0, grip, then spin
  if(rotation > 0) {
    servoSpinDie.write(0);
    grip(true);
    //motorMove(true, spinDist);
    for(pos = 0; pos <= rotation; pos++) {
      servoSpinDie.write(pos);
      servoGrip.write(gripPos);
      delay(15);
    }
  }
  //if -ve rotation, go to 180(**is this necessary??**), then 144, grip, then spin
  else {
    servoSpinDie.write(180);
    for(pos = 180; pos >= 144; pos--) {
      servoSpinDie.write(pos);
      delay(15);
    }
    grip(true);
    //motorMove(false, 20);
    for(pos = 144; pos >= (144 - rotation); pos--) {
      servoSpinDie.write(pos);
      delay(15);
    }
  }
}

//grip or un-grip the grippers (note: does not align the gripper with die)
void grip(bool close) {

  int pos = 0;

  if (close) {
    Serial.println("\nClosing gripper..");
    //open grippers to 180 degree position (fully open)
    servoGrip.write(180);
    delay(2000);

    //close grippers to 130 degree position (closed)
    for (pos = 180; pos >= gripPos; pos--) { 
      servoGrip.write(pos);
      delay(15);
    }
  }

  else {
    Serial.println("\nOpening gripper..");
    //open grippers to 180 degree position (fully open)
    for (pos = gripPos; pos <= 180; pos++) { 
      servoGrip.write(pos);
      delay(15);
    }
  }
  
}