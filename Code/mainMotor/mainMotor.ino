int xMotorPin1 = 29; int xMotorPin2 = 28; int zMotorPin1 = 23; int zMotorPin2 = 22;
int xMotorCtrlPin = 8; int zMotorCtrlPin = 9;
int xOptoPin = A10; int zOptoPin = A11;
int servoGripPin = 2; int servoRotateArmPin = 3; int servoSpinDiePin = 4;

int gripPos = 90; //position of gripper servo for die fully gripped

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
  digitalWrite(xMotorPin1, 0);
  digitalWrite(xMotorPin2, 0); 
  digitalWrite(zMotorPin1, 0);
  digitalWrite(zMotorPin2, 0);

  //assign PWM pins to each servo & set initial values
  servoGrip.attach(servoGripPin);
  servoRotateArm.attach(servoRotateArmPin);
  servoSpinDie.attach(servoSpinDiePin);
  servoGrip.write(180);
  servoRotateArm.write(0);
  servoSpinDie.write(0);
  delay(2000);
}

void loop() {
// put your main code here, to run repeatedly:
  motorMove(true, 20);

  servoGrip.detach();
  servoSpinDie.detach();
  servoRotateArm.detach();
  while(1);
}

//run for a certain time (+ve and -ve are different directions) (0.05s increments)
//for z, +ve time is up
//for x, +ve time is up
void motorMove(bool xMove, double time) {

  //default to operate z motor, if bool xMove is true then do x motor
  int pin1 = zMotorPin1; int pin2 = zMotorPin2; //int pinCtrl = zMotorCtrlPin; int pinOpto = zOptoPin;
  if(xMove) {
    pin1 = xMotorPin1; pin2 = xMotorPin2; //pinCtrl = xMotorCtrlPin; pinOpto = xOptoPin;
  }

  //default pin values to spin motor forwards, flip if moving in -ve direction
  int pin1Val = 1; int pin2Val = 1;
  if(time < 0) {
    //pin1Val = 0;
    pin2Val = 0;
  }
  
  //operate the motor by writing 0 and 1 to the h bridge pins (or 1 and 0)
  //analogWrite(pinCtrl, 255);
  digitalWrite(pin1, pin1Val);
  digitalWrite(pin2, pin2Val);

  //print out the direction of movement
  if(pin1Val > pin2Val) {Serial.print("Moving up");}
  else if (pin1Val < pin2Val) {Serial.print("Moving down");}
  else {Serial.print("Pins equal, not moving");}
  if(xMove) {Serial.println(" in X");}
  else {Serial.println(" in Z");}

  //turn off the motor when opto is hit or enough time has passed
  double timeCounter = 0;
  while(timeCounter < abs(time)*1000 && analogRead(A2) < 200) {
    servoGrip.write(gripPos);//keep grippers closed
    delay(100);//delay to continue moving the motor
    Serial.print(timeCounter/1000);
    Serial.println(" elapsed");
    timeCounter += 100;//increment counter
  }

  //turn off motor by writing 0 to both pins
  digitalWrite(pin1, 0);
  digitalWrite(pin2, 0);

}

//grip the die, lift small amount, spin the die
void spinDie(int rotation) {
  //note: the driving cup should be set to align with die for 0 position

  int pos = 0;
  //if +ve rotation, go to 0, grip, then spin
  if(rotation > 0) {
    servoSpinDie.write(0);//make sure the grippers are aligned
    grip(true);//close the grippers
    motorMove(false, 2);//move up in z for 2 seconds
    for(pos = 0; pos <= rotation; pos++) {
      servoSpinDie.write(pos);//increment gripper position
      servoGrip.write(gripPos);//keep grippers closed
      delay(15);
    }
  }
  //if -ve rotation, go to 144, grip, then spin
  else {
    servoSpinDie.write(144);//make sure the grippers are aligned
    grip(true);//close grippers
    motorMove(false, 2);
    for(pos = 144; pos >= (144 - rotation); pos--) {
      servoSpinDie.write(pos);//increment gripper position
      servoGrip.write(gripPos);//keep grippers closed
      delay(15);
    }
  }
}

//grip or un-grip the grippers (note: does not align the gripper with die)
void grip(bool close) {

  int pos = 0;

  if (close) {
    Serial.println("\nClosing gripper..");
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

//move the arm between axes
void rotateArm(int axis) {
  //note: the arm should be aligned with the 0 axis at 0 position
  Serial.print("Moving arm to axis ");
  Serial.println(axis);

  int prevRotation = servoRotateArm.read();

  //if axis 0, done. if axis 1, go to 60. if axis 2, go to 120
  int rotation = axis*65;

  if(rotation > prevRotation) {
    Serial.println("Moving arm CCW");
    for(int pos = prevRotation; pos <= rotation; pos++) {
      servoRotateArm.write(pos);
      delay(20);
    }
  }
  else if(rotation < prevRotation) {
    Serial.println("Moving arm CW");
    for(int pos = prevRotation; pos >= rotation; pos--) {
      servoRotateArm.write(pos);
      delay(20);
    }
  }
  else {
    Serial.println("Arm already in position");
  }
  
}