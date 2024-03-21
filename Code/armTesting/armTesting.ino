#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>
#include <math.h>

//initialize color sensor and servo objects
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

Servo servoGrip;
Servo servoRotateArm;
Servo servoSpinDie;

char *colorList[] = {"Maroon", "Red", "Pink", "Brown", "Orange", "Apricot", "Yellow", 
  "Lime", "Green", "Mint", "Teal", "Cyan", "Navy", "Blue", "Purple", "Lavender", "Magenta",
  "Black", "Grey", "White"};

//assign variables for each arduino pin
int servoGripPin = 2; int servoRotateArmPin = 3; int servoSpinDiePin = 4;
int xMotorPin1 = 22; int xMotorPin2 = 24; int zMotorPin1 = 26; int zMotorPin2 = 28;
int xMotorCtrlPin = 8; int zMotorCtrlPin = 9;
int xOptoPin = 32; int zOptoPin = 33;

int gripPos = 100; //position of gripper servo for die fully gripped

void setup() {
  //assign PWM pins to each servo
  servoGrip.attach(servoGripPin);
  servoRotateArm.attach(servoRotateArmPin);
  servoSpinDie.attach(servoSpinDiePin);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //spinDie(72);
  //grip(false);

  
  
  for(int i = 0; i < 3; i++) {
    servoRotateArm.write(0);
    int rotation = i*60;
    for(int pos = 0; pos <= rotation; pos++) {
      servoRotateArm.write(pos);
      delay(30);
      
    }
    Serial.println(i);
  }

  

}

//return a value 0-19 to correspond to a color
int colorSeen() {
  
  delay(100);

  //turn on color sensor, check if on
  Serial.begin(9600);
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  //initialize RGB and color name arrays
  int rgb[20][3];
  char *colors[20];
  
  char *colorList[] = {"Maroon", "Red", "Pink", "Brown", "Orange", "Apricot", "Yellow", 
  "Lime", "Green", "Mint", "Teal", "Cyan", "Navy", "Blue", "Purple", "Lavender", "Magenta",
  "Black", "Grey", "White"};
  
  for (int i = 0; i < 20; i++){
    colors[i] = colorList[i];
  }

  rgb[0][0] = 96; rgb[0][1] = 110; rgb[0][2] = 101; //Maroon
  rgb[1][0] = 166; rgb[1][1] = 124; rgb[1][2] = 112; //Red
  rgb[2][0] = 236; rgb[2][1] = 218; rgb[2][2] = 207; //Pink
  rgb[3][0] = 130; rgb[3][1] = 150; rgb[3][2] = 125; //Brown
  rgb[4][0] = 210; rgb[4][1] = 167; rgb[4][2] = 132; //Orange
  rgb[5][0] = 252; rgb[5][1] = 280; rgb[5][2] = 210; //Apricot
  rgb[6][0] = 252; rgb[6][1] = 269; rgb[6][2] = 156; //Yellow
  rgb[7][0] = 156; rgb[7][1] = 231; rgb[7][2] = 151; //Lime
  rgb[8][0] = 85; rgb[8][1] = 154; rgb[8][2] = 118; //Green
  rgb[9][0] = 207; rgb[9][1] = 322; rgb[9][2] = 243; //Mint
  rgb[10][0] = 88; rgb[10][1] = 166; rgb[10][2] = 157; //Teal
  rgb[11][0] = 130; rgb[11][1] = 237; rgb[11][2] = 231; //Cyan
  rgb[12][0] = 72; rgb[12][1] = 123; rgb[12][2] = 125; //Navy
  rgb[13][0] = 81; rgb[13][1] = 143; rgb[13][2] = 155; //Blue
  rgb[14][0] = 86; rgb[14][1] = 129; rgb[14][2] = 137; //Purple
  rgb[15][0] = 166; rgb[15][1] = 240; rgb[15][2] = 254; //Lavender
  rgb[16][0] = 128; rgb[16][1] = 138; rgb[16][2] = 143; //Magenta
  rgb[17][0] = 71; rgb[17][1] = 113; rgb[17][2] = 101; //Black
  rgb[18][0] = 125; rgb[18][1] = 175; rgb[18][2] = 151; //Grey
  rgb[19][0] = 276; rgb[19][1] = 358; rgb[19][2] = 310; //White

  //read colors
  int red, green, blue, clear;
  tcs.setInterrupt(false); // turn on LED
  delay (60); // wait to read
  tcs.getRawData(&red, &green, &blue, &clear);
  tcs.setInterrupt(true); // turn off LED
  red = int(red); green = int(green); blue = int(blue); clear = int(clear);

  double dist; double bestDist = 10000;
  int bestDistIndex = 0;

  for (int i = 0; i < 20; i++) {
    dist = sqrt(pow((red - rgb[i][0]), 2) + pow((green - rgb[i][1]), 2) + pow((blue - rgb[i][2]), 2));
    
    if(dist < bestDist) {
      bestDist = dist;
      bestDistIndex = i;
    }
  }

  //print color (for debugging)
  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\n");
  Serial.print("This is: "); Serial.print(colors[bestDistIndex]);
  Serial.print("\n");

  return bestDistIndex;
}

//grip the die, lift small amount, spin the die
void spinDie(int rotation) {
  //note: the driving cup should be set to align with die for 0 position

  int pos = 0;
  //if +ve rotation, go to 0, grip, then spin
  if(rotation > 0) {
    servoSpinDie.write(0);
    grip(true);
    for(int i = 0; i < 5; i++) {
      int color = colorSeen();
      delay(500);
    }
    //motorMove(true, spinDist);
    for(pos = 0; pos <= rotation; pos++) {
      servoSpinDie.write(pos);
      servoGrip.write(gripPos);
      delay(15);
    }
    for(int i = 0; i < 3; i++) {
      int color = colorSeen();
      delay(500);
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