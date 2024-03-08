#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>


//initialize color sensor and servo objects
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

Servo servoGrip;
Servo servoRotateArm;
Servo servoSpinDie;

//initialize die arrays
float diePositions[20][3]; //x, y, z coordinates of face centerpoints
int diePotentials[20]; //integer potentials of each face (0, 1, 2, or 3)
float axisVectors[3][3]; //x, y, z components of rotation vectors

//The following indices always refer to these colors:
//0:Maroon, 1:Red, 2:Pink, 3:Brown, 4:Orange, 5:Apricot, 6:Yellow, 
//7:Lime, 8:Green, 9:Mint, 10:Teal, 11:Cyan, 12:Navy, 13:Blue, 
//14:Purple, 15:Lavender, 16:Magenta, 17:Black, 18:Grey, 19:White

void setup() {

  //assign PWM pins to each servo
  servoGrip.attach(2);
  servoRotateArm.attach(4);
  servoSpinDie.attach(6);



}

void loop() {

  //reset die positions and set axis vectors
  void arrayInitialize();

  //get the current position of the die
  int seenTopFace = colorSeen();


  //transform the array to the current position
  

  //get the desired end color & assign potentials
  int goalColor = 17; //can be user input
  assignPotentials(goalColor);

  //get the top face from array & check against sensor
  int arrayTopFace = getTopFace();

  //spin the die until the desired face is up
  while(diePotentials[arrayTopFace] > 0) {
    //reset tracker
    int currentPotential = diePotentials[arrayTopFace];
    int nextFaceIndex = 20;

    for(i = 0; i < 20; i++) {
      if(diePotentials[i] == currentPotential - 1) {
        //transform die & model to new face
      }
    }

    arrayTopFace = getTopFace();

  }

  //move die to finish

  //move die back to start (after delay)


  exit(0);
}

//assign a position to each face of the die, and to each rotation vector
void arrayInitialize() {
  //top: black
  //second row: navy, maroon, grey
  //third row: purple, magenta, red, brown, green, blue
  //fourth row: cyan, pink, orange, yellow, lime, teal
  //fifth row: lavender, apricot, mint
  //bottom: white

  //x, y, z position for each color
  diePositions[0][0] = -0.333333; diePositions[0][1] = 0.577350; diePositions[0][2] = 0.745356; //Maroon
  diePositions[1][0] = -0.872678; diePositions[1][1] = 0.356822; diePositions[1][2] = 0.333333; //Red
  diePositions[2][0] = -0.127322; diePositions[2][1] = 0.934172; diePositions[2][2] = -0.333333; //Pink
  diePositions[3][0] = -0.872678; diePositions[3][1] = -0.356822; diePositions[3][2] = 0.333333; //Brown
  diePositions[4][0] = -0.745356; diePositions[4][1] = 0.577350; diePositions[4][2] = -0.333333; //Orange
  diePositions[5][0] = -0.666667; diePositions[5][1] = 0; diePositions[5][2] = -0.745356; //Apricot
  diePositions[6][0] = -0.745356; diePositions[6][1] = -0.577350; diePositions[6][2] = -0.333333; //Yellow
  diePositions[7][0] = -0.127322; diePositions[7][1] = -0.934172; diePositions[7][2] = -0.333333; //Lime
  diePositions[8][0] = 0.127322; diePositions[8][1] = -0.934172; diePositions[8][2] = 0.333333; //Green
  diePositions[9][0] = 0.333333; diePositions[9][1] = -0.577350; diePositions[9][2] = -0.745356; //Mint
  diePositions[10][0] = 0.872678; diePositions[10][1] = -0.356822; diePositions[10][2] = -0.333333; //Teal
  diePositions[11][0] = 0.872678; diePositions[11][1] = 0.356822; diePositions[11][2] = -0.333333; //Cyan
  diePositions[12][0] = 0.666667; diePositions[12][1] = 0; diePositions[12][2] = 0.745356; //Navy
  diePositions[13][0] = 0.745356; diePositions[13][1] = -0.577350; diePositions[13][2] = 0.333333; //Blue
  diePositions[14][0] = 0.745356; diePositions[14][1] = 0.577350; diePositions[14][2] = 0.333333; //Purple
  diePositions[15][0] = 0.333333; diePositions[15][1] = 0.577350; diePositions[15][2] = -0.745356; //Lavender
  diePositions[16][0] = 0.127322; diePositions[16][1] = 0.934172; diePositions[16][2] = 0.333333; //Magenta
  diePositions[17][0] = 0; diePositions[17][1] = 0; diePositions[17][2] = 1; //Black
  diePositions[18][0] = -0.333333; diePositions[18][1] = -0.577350; diePositions[18][2] = 0.745356; //Grey
  diePositions[19][0] = 0; diePositions[19][1] = 0; diePositions[19][2] = -1; //White


  //x, y, z component for each vector
  //vector 0: pointing forward, vector 1: pointing back and right, vector 2: pointing back and left
  axisVectors[0][0] = 0.982247; axisVectors[0][1] = 0; axisVectors[0][2] = 0.187592;
  axisVectors[1][0] = -0.491123; axisVectors[1][1] = 0.850651; axisVectors[1][2] = 0.187592;
  axisVectors[2][0] = -0.491123; axisVectors[2][1] = -0.850651; axisVectors[2][2] = 0.187592;
}

void assignPotentials(goalColor) {
  //top: 0 (dist=0)
  //second row: 2 (dist=0.713644)
  //third row: 1 (dist=1.154700)
  //fourth row: 2 (dist=1.632993)
  //fifth row: 1 (dist=1.868345)
  //bottom: 3 (dist=2)

  float potentials[6][2];
  potentials[0][0] = 0; potentials[0][1] = 0; //top face
  potentials[1][0] = 2; potentials[1][1] = 0.713644; //top face
  potentials[2][0] = 1; potentials[2][1] = 1.154700; //top face
  potentials[3][0] = 2; potentials[3][1] = 1.632993; //top face
  potentials[4][0] = 1; potentials[4][1] = 1.868345; //top face
  potentials[5][0] = 3; potentials[5][1] = 2; //top face

  float dist = 0;
  float tol = 0.001

  for(i = 0; i < 20; i++) {
    //distance between goal point and selected point
    dist = sqrt(pow(diePositions[goalColor][0] - diePositions[i][0], 2)) + pow(diePositions[goalColor][1] - diePositions[i][1], 2)) + pow(diePositions[goalColor][1] - diePositions[i][1], 2)) )

    //set die potential to 4 to make sure it gets changed
    diePotentials[i] = 4;

    //select potential based on distance
    for(j = 0; j < 6; j++) {
      if(dist < potentials[j][1] + tol && dist > potentials[j][1] - tol) {
        diePotentials[i] = potentials[j][0];
      }
    }

    //throw error if potential did not apply
    if(diePotentials[i] == 4) {
      Serial.println("\n");
      Serial.println("Potential did not apply for: ");
      Serial.println(diePotentials[i]);
      while(1);
    }

  }

}

//return a value 0-19 to correspond to a color
int colorSeen() {
  
  delay(200);

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
  
  rgb[0][0] = 171; rgb[0][1] = 140; rgb[0][2] = 131; //Maroon
  rgb[1][0] = 359; rgb[1][1] = 165; rgb[1][2] = 154; //Red
  rgb[2][0] = 600; rgb[2][1] = 479; rgb[2][2] = 455; //Pink
  rgb[3][0] = 262; rgb[3][1] = 255; rgb[3][2] = 196; //Brown
  rgb[4][0] = 538; rgb[4][1] = 317; rgb[4][2] = 212; //Orange
  rgb[5][0] = 677; rgb[5][1] = 713; rgb[5][2] = 495; //Apricot
  rgb[6][0] = 698; rgb[6][1] = 688; rgb[6][2] = 305; //Yellow
  rgb[7][0] = 362; rgb[7][1] = 529; rgb[7][2] = 266; //Lime
  rgb[8][0] = 120; rgb[8][1] = 255; rgb[8][2] = 166; //Green
  rgb[9][0] = 525; rgb[9][1] = 862; rgb[9][2] = 608; //Mint
  rgb[10][0] = 142; rgb[10][1] = 298; rgb[10][2] = 280; //Teal
  rgb[11][0] = 292; rgb[11][1] = 586; rgb[11][2] = 597; //Cyan
  rgb[12][0] = 107; rgb[12][1] = 189; rgb[12][2] = 207; //Navy
  rgb[13][0] = 134; rgb[13][1] = 260; rgb[13][2] = 352; //Blue
  rgb[14][0] = 139; rgb[14][1] = 191; rgb[14][2] = 243; //Purple
  rgb[15][0] = 368; rgb[15][1] = 530; rgb[15][2] = 620; //Lavender
  rgb[16][0] = 270; rgb[16][1] = 228; rgb[16][2] = 277; //Magenta
  rgb[17][0] = 88; rgb[17][1] = 135; rgb[17][2] = 121; //Black
  rgb[18][0] = 273; rgb[18][1] = 366; rgb[18][2] = 310; //Grey
  rgb[19][0] = 815; rgb[19][1] = 1060; rgb[19][2] = 901; //White
  

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

//get index of top face according to die array and check against sensor
int getTopFace() {
  //get index of top face according to die array
  int arrayTopFace = 20;
  for(i = 0; i < 20; i++) {

    //if the x and y coordinates are zero, and the z coordinate is positive, that face must be pointing up
    if(diePositions[i][0] == 0 && diePositions[i][1] == 0 && diePositions[i][2] > 0) {
      arrayTopFace = i;
    }
  }

  //check sensor reading to array, throw error if doesn't match
  seenTopFace = colorSeen();
  if(arrayTopFace != seenTopFace) {
    Serial.println("Top face does not match");
    while (1);
  }
  
  return arrayTopFace;
}

void rotateArm() {

}

void spinDie() {

}

