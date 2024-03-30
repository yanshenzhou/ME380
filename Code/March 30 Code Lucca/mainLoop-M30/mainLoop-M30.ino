#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>
#include <math.h>

//initialize color sensor and servo objects
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

Servo servoGrip;
Servo servoRotateArm;
Servo servoSpinDie;

//initialize die arrays
double diePositions[20][3]; //x, y, z coordinates of face centerpoints
int diePotentials[20]; //integer potentials of each face (0, 1, 2, or 3)
double axisVectors[5][3]; //x, y, z components of rotation vectors
double potentialDistances[6][2]; //potentials for various distances

//The following indices always refer to these colors:
//0:Maroon, 1:Red, 2:Pink, 3:Brown, 4:Orange, 5:Apricot, 6:Yellow, 
//7:Lime, 8:Green, 9:Mint, 10:Teal, 11:Cyan, 12:Navy, 13:Blue, 
//14:Purple, 15:Lavender, 16:Magenta, 17:Black, 18:Grey, 19:White

char *colorList[] = {"Maroon", "Red", "Pink", "Brown", "Orange", "Apricot", "Yellow",
  "Lime", "Green", "Mint", "Teal", "Cyan", "Navy", "Blue", "Purple", "Lavender", "Magenta",
  "Black", "Grey", "White"};

int gripPos = 80; //position of gripper servo for die fully gripped

//assign variables for each arduino pin
int servoGripPin = 2; int servoRotateArmPin = 3; int servoSpinDiePin = 4;
int xMotorPin1 = 35; int xMotorPin2 = 34; int zMotorPin1 = 23; int zMotorPin2 = 22;
//int xMotorCtrlPin = 8; int zMotorCtrlPin = 9;

void setup() {

  //assign PWM pins to each servo, default to home position
  servoGrip.attach(servoGripPin);
  servoRotateArm.attach(servoRotateArmPin);
  servoSpinDie.attach(servoSpinDiePin);
  servoGrip.write(180);
  servoRotateArm.write(0);
  servoSpinDie.write(0);

  //assign pins to motor controls, default to home position
  pinMode(xMotorPin1, OUTPUT);
  pinMode(xMotorPin2, OUTPUT);
  pinMode(zMotorPin1, OUTPUT);
  pinMode(zMotorPin2, OUTPUT);
  digitalWrite(xMotorPin1, 0);
  digitalWrite(xMotorPin2, 0); 
  digitalWrite(zMotorPin1, 0);
  digitalWrite(zMotorPin2, 0);
  //pinMode(xMotorCtrlPin, OUTPUT);
  //pinMode(zMotorCtrlPin, OUTPUT);

  Serial.begin(9600);

  //turn on color sensor, check if on
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    //while (1); // halt!
  }

}


void loop() {
  //servoGrip.write(50);
  //servoRotateArm.write(10);
  //motorMove(false, false, -0.25);//move up in z all the way
  //delay(1000);
  //motorMove(false, false, -5.3);//move up in z all the way
  //motorMove(true, true, 15);//move up in z all the way
  //motorMove(true, false, -40);//move up in z all the way
  //motorMove(true, false, 10);//move up in z all the way
  //while(1);
  //allow time to terminate the program (move arm a bit to let you know it's time to cancel)
  /*
  for(int i = 0; i < 10; i++) {
    
    if(analogRead(A2) > 200) {
      Serial.println("Opto triggered to stop program");
      while(1);
    }
    for(int pos = 0; pos <= 5; pos++) {
      servoRotateArm.write(pos);
      delay(50);
    }
    delay(100);
    for(int pos = 5; pos <= 0; pos--) {
      servoRotateArm.write(pos);
      delay(50);
    }
    delay(100);
  }
  */
  //reset die positions and set axis vectors
  arrayInitialize();
  outputDiePositions();

  //orient the model by checking locations of two die faces
  orientModel();

  //get the top face from array & check against sensor
  
  int arrayTopFace = getTopFace();
  
  //get the desired end color & assign potentials
  int goalColor = 17; //can be user input
  Serial.print("\nWant to get: ");
  Serial.print(colorList[goalColor]);
  assignPotentials(goalColor);

  //spin the die until the desired face is up
  while(diePotentials[arrayTopFace] > 0) {
    //reset tracker
    int currentPotential = diePotentials[arrayTopFace];

    //check all faces of the die
    for(int i = 0; i < 20; i++) {
      //transform to face i if it is reachable and has a lower potential
      if(diePotentials[i] == currentPotential - 1 && reachable(i)) {
        Serial.println("\nTransform to: ");
        Serial.println(colorList[i]);
        transform(i);
        break;
      }
    }
    arrayTopFace = getTopFace();
  }

  //move die to finish (move in x, rotate arm to not hit the platform, move in z, ungrip)
  delay(500);
  motorMove(false, true, 5.5);//move up in z all the way
  motorMove(true, true, 60);//move halfway to finish
  servoRotateArm.write(0);//align arm away from platform
  delay(500);
  motorMove(true, true, 58);//move die to finish
  motorMove(false, true, -1);//move down in z until hit platform
  delay(500);
  grip(false);

  //move die back to start (after delay)
  delay(20000);
  //motorMove(false, false, 0.5);
  motorMove(true, false, -118);//move die back to start
  delay(1000);
  motorMove(false, false, -3.9);
  delay(500);
  grip(false);
  rotateArm(0);
  delay(20000);

  //stop program so it only loops once
  Serial.println("Program ended before looped again");
  servoGrip.detach();
  servoSpinDie.detach();
  servoRotateArm.detach();
  while(1);
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
  //vector 0: pointing forward, vector 1: pointing back and left, vector 2: pointing back and right
  axisVectors[0][0] = 0.982247; axisVectors[0][1] = 0; axisVectors[0][2] = 0.187592;
  axisVectors[1][0] = -0.491123; axisVectors[1][1] = -0.850651; axisVectors[1][2] = 0.187592;
  axisVectors[2][0] = -0.491123; axisVectors[2][1] = 0.850651; axisVectors[2][2] = 0.187592;

  //extra axes reserved for initial model transformation (unknown axis and z axis)
  axisVectors[3][0] = 0; axisVectors[3][1] = 0; axisVectors[3][2] = 0;
  axisVectors[4][0] = 0; axisVectors[4][1] = 0; axisVectors[4][2] = 1;

}

//update the positions of each die face in code to match reality
void orientModel() {
  
  //get top and second faces
  delay(2000);
  servoRotateArm.write(0);
  grip(true);
  int seenTopFace = colorSeen(); //get top face
  grip(false);
  delay(300);
  spinDie(75); //pick up, move up, spin
  int secondSeenFace = colorSeen(); //get second face
  for(int pos = 75; pos >= 0; pos--) { //spin die the other way
      servoSpinDie.write(pos);
      delay(15);
  }
  motorMove(false, true, -2.8); //move back down
  grip(false);
  //int seenTopFace = 1;
  //int secondSeenFace = 2;

  //transform the model to have the correct top face up
  //get coordinates of desired top face and do cross product to find rotation axis
  double x1 = diePositions[seenTopFace][0]; double y1 = diePositions[seenTopFace][1]; double z1 = diePositions[seenTopFace][2];
  double x2 = 0; double y2 = 0; double z2 = 1;
  double norm = sqrt(pow(x1, 2) + pow(y1, 2));

  //assign a rotation axis. if norm equals 0, the vectors are parallel, so set the rotation axis to x-axis
  if(norm > 0.01) {
    axisVectors[3][0] = y1 / norm;
    axisVectors[3][1] = -1 * x1 / norm;
    axisVectors[3][2] = 0;
  }
  else {
    axisVectors[3][0] = 1;
    axisVectors[3][1] = 0;
    axisVectors[3][2] = 0;
  }

  //use dot product to find rotation angle
  double dotProduct = x1 * x2 + y1 * y2 + z1 * z2;//really just =z1
  double norm1 = sqrt(pow(x1, 2) + pow(y1, 2) + pow(z1, 2));
  double norm2 = sqrt(pow(x2, 2) + pow(y2, 2) + pow(z2, 2));//really just =1
  double rotation = acos(dotProduct / (norm1 * norm2));
  
  rotation *= 180 / 3.14159265; //convert to degrees

  //for debugging
  Serial.print("\nOrientation axis 1: ");
  Serial.print("\tX:");
  Serial.print(axisVectors[3][0]);
  Serial.print("\tY:");
  Serial.print(axisVectors[3][1]);
  Serial.print("\tZ:");
  Serial.print(axisVectors[3][2]);
  Serial.print("\nOrientation rotation angle 1: ");
  Serial.print(rotation);

  transformModel(3, rotation); //transform

  //rotate the model around z axis to align second face
  //get coordinates of checked face, compare to expected coordinates and use dot product to find rotation angle
  double x_1 = diePositions[secondSeenFace][0]; double y_1 = diePositions[secondSeenFace][1];
  double x_2 = 0.127322; double y_2 = 0.934172; //original x, y coord for magenta
  double dot_product = x_1 * x_2 + y_1 * y_2;
  double norm_1 = sqrt(pow(x_1, 2) + pow(y_1, 2));
  double norm_2 = sqrt(pow(x_2, 2) + pow(y_2, 2));
  double rotationz = acos(dot_product / (norm_1 * norm_2));

  //acos only returns on (0, pi) so need to make rotation negative in some cases
  if (y_1 - x_1 * (y_2 / x_2) > 0) {
    rotationz *= -1;
  }

  rotationz *= 180 / 3.14159265; //convert to degrees
  
  //for debugging
  Serial.print("\nOrientation axis 2: ");
  Serial.print("\tX:");
  Serial.print(axisVectors[4][0]);
  Serial.print("\tY:");
  Serial.print(axisVectors[4][1]);
  Serial.print("\tZ:");
  Serial.print(axisVectors[4][2]);
  Serial.print("\nOrientation rotation angle 2: ");
  Serial.print(rotationz);

  transformModel(4, rotationz); //transform
}

//give potentials to all faces based on the desired face
void assignPotentials(int goalColor) {
  //top: 0 (dist=0)
  //second row: 2 (dist=0.713644)
  //third row: 1 (dist=1.154700)
  //fourth row: 2 (dist=1.632993)
  //fifth row: 1 (dist=1.868345)
  //bottom: 3 (dist=2)

  potentialDistances[0][0] = 0; potentialDistances[0][1] = 0; 
  potentialDistances[1][0] = 2; potentialDistances[1][1] = 0.713644; 
  potentialDistances[2][0] = 1; potentialDistances[2][1] = 1.154700; 
  potentialDistances[3][0] = 2; potentialDistances[3][1] = 1.632993;
  potentialDistances[4][0] = 1; potentialDistances[4][1] = 1.868345;
  potentialDistances[5][0] = 3; potentialDistances[5][1] = 2;

  double dist = 0;
  double tol = 0.05;

  for(int i = 0; i < 20; i++) {
    //distance between goal point and selected point
    dist = sqrt(pow(diePositions[goalColor][0] - diePositions[i][0], 2) + pow(diePositions[goalColor][1] - diePositions[i][1], 2) + pow(diePositions[goalColor][2] - diePositions[i][2], 2));

    //set die potential to 4 to make sure it gets changed
    diePotentials[i] = 4;

    //select potential based on distance
    for(int j = 0; j < 6; j++) {
      if(dist < potentialDistances[j][1] + tol && dist > potentialDistances[j][1] - tol) {
        diePotentials[i] = potentialDistances[j][0];
      }
    }

    Serial.print("\n");
    Serial.print(colorList[i]);
    Serial.print(": Potential is ");
    Serial.print(diePotentials[i], 4);

    //throw error if potential did not apply
    if(diePotentials[i] == 4) {
      while(1);
    }
  }
}

//determine the axis of rotation and degrees of rotation
void transform(int desiredFace) {
  int axis; //0: first axis, 1: second axis, 2: third axis
  int rotation; //can be 72, 144, -72, -144 degrees of rotation, CCW is +ve (looking at higher axis endpoint)

  double tol = 0.1;
  double minAcceptableDist = 0.763932;
  double distPosEndpoint; double distNegEndpoint; double distAxis; double distFaces;

  //an axis will work if neither of its endpoints touch the desired face 
  //so, check if both endpoints are beyond "minAcceptableDist" to the face centerpoint
  for(int i = 0; i < 3; i++) {

    //calculate distance between centerpoint of desired face and nearest axis endpoint
    distPosEndpoint = sqrt(pow(diePositions[desiredFace][0] - axisVectors[i][0], 2) + pow(diePositions[desiredFace][1] - axisVectors[i][1], 2) +pow(diePositions[desiredFace][2] - axisVectors[i][2], 2));
    distNegEndpoint = sqrt(pow(diePositions[desiredFace][0] + axisVectors[i][0], 2) + pow(diePositions[desiredFace][1] + axisVectors[i][1], 2) +pow(diePositions[desiredFace][2] + axisVectors[i][2], 2));
    distAxis = min(distPosEndpoint, distNegEndpoint);

    if(distAxis > minAcceptableDist - tol) {
      axis = i;
      break; //break the for loop
    }
  }

  //get the distance from top face to desired top face to know how much to rotate by
  distFaces = sqrt(pow(diePositions[desiredFace][0], 2) + pow(diePositions[desiredFace][1], 2) + pow(diePositions[desiredFace][2] - 1, 2));
  Serial.print("Distance between faces: ");
  Serial.println(distFaces);

  if(distFaces > potentialDistances[2][1] - tol && distFaces < potentialDistances[2][1] + tol) {
    rotation = 72;
  }
  else if(distFaces > potentialDistances[4][1] - tol && distFaces < potentialDistances[4][1] + tol) {
    rotation = 144;
  }
  else {
    Serial.println("Error: Rotation distance not calculated");
    while (1); // halt!
  }

  //check location of desired face to know if rotation is positive or negative
  if(axis == 0) {
    if(diePositions[desiredFace][1] < 0 - tol) {
      rotation *= -1; 
    }
  }
  else if(axis == 2) {
    if(diePositions[desiredFace][0] < 0 - tol) {
      rotation *= -1;
    }
  }
  else if(axis == 1) {
    if(diePositions[desiredFace][1] > 0 + tol) {
      rotation *= -1;
    }
  }
  else {
    Serial.println("Error: Axis not found - rotation direction not calculated");
    while (1); // halt!
  }

  //print axis and rotation (for debugging)
  Serial.println("\n");
  Serial.println("Axis and rotation:\t");
  Serial.println(axis);
  Serial.println("\t");
  Serial.println(rotation);
  Serial.println("\n");

  transformModel(axis, rotation);
  transformDie(axis, rotation);
}

//transform the model using given axis and rotation 
void transformModel(int axis, double rotation) {

  //get values for rotation matrix
  double thetaRad = rotation * 3.14159265 / 180;
  double ct = cos(thetaRad);
  double st = sin(thetaRad);
  double ux = axisVectors[axis][0]; double uy = axisVectors[axis][1]; double uz = axisVectors[axis][2];
  double x; double y; double z;

  double tol = 0.01;
  double vectorNorm = sqrt(pow(ux, 2) + pow(uy, 2) + pow(uz, 2));
  if(vectorNorm < 1 - tol || vectorNorm > 1 + tol) {
    Serial.println("Error: rotation axis not normalized");
    while(1);
  }

  //apply rotation matrix
  for(int i = 0; i < 20; i++) {
    x = diePositions[i][0]; y = diePositions[i][1]; z = diePositions[i][2];
    diePositions[i][0] = x*(ct + ux*ux*(1 - ct)) + y*(ux*uy*(1 - ct) - uz*st) + z*(ux*uz*(1 - ct) + uy*st);
    diePositions[i][1] = x*(uy*ux*(1 - ct) + uz*st) + y*(ct + uy*uy*(1 - ct)) + z*(uy*uz*(1 - ct) - ux*st);
    diePositions[i][2] = x*(uz*ux*(1 - ct) - uy*st) + y*(uz*uy*(1 - ct) + ux*st) + z*(ct + uz*uz*(1 - ct));
  }

  outputDiePositions();
}

//transform the die using given axis and rotation 
void transformDie(int axis, int rotation) {
  rotateArm(axis);//move arm to axis
  delay(200);
  spinDie(rotation);//grip die, move up, and spin
  delay(200);
  motorMove(false, true, -2.9);//move back down in z for 2 seconds
  delay(500);
  grip(false);//ungrip
}

//return a value 0-19 to correspond to a color
int colorSeen() {
  
  delay(500);

  //initialize RGB and color name arrays
  int rgb[20][3];
  char *colors[20];
  
  char *colorList[] = {"Maroon", "Red", "Pink", "Brown", "Orange", "Apricot", "Yellow", 
  "Lime", "Green", "Mint", "Teal", "Cyan", "Navy", "Blue", "Purple", "Lavender", "Magenta",
  "Black", "Grey", "White"};
  
  for (int i = 0; i < 20; i++){
    colors[i] = colorList[i];
  }
  
  rgb[0][0] = 133; rgb[0][1] = 136; rgb[0][2] = 123; //Maroon
  rgb[1][0] = 241; rgb[1][1] = 152; rgb[1][2] = 138; //Red
  rgb[2][0] = 360; rgb[2][1] = 312; rgb[2][2] = 293; //Pink
  rgb[3][0] = 180; rgb[3][1] = 195; rgb[3][2] = 158; //Brown
  rgb[4][0] = 330; rgb[4][1] = 230; rgb[4][2] = 174; //Orange
  rgb[5][0] = 384; rgb[5][1] = 414; rgb[5][2] = 302; //Apricot
  rgb[6][0] = 418; rgb[6][1] = 418; rgb[6][2] = 217; //Yellow
  rgb[7][0] = 242; rgb[7][1] = 347; rgb[7][2] = 199; //Lime
  rgb[8][0] = 105; rgb[8][1] = 202; rgb[8][2] = 148; //Green
  rgb[9][0] = 297; rgb[9][1] = 473; rgb[9][2] = 344; //Mint
  rgb[10][0] = 121; rgb[10][1] = 230; rgb[10][2] = 211; //Teal
  rgb[11][0] = 192; rgb[11][1] = 360; rgb[11][2] = 359; //Cyan
  rgb[12][0] = 91; rgb[12][1] = 157; rgb[12][2] = 160; //Navy
  rgb[13][0] = 106; rgb[13][1] = 193; rgb[13][2] = 226; //Blue
  rgb[14][0] = 110; rgb[14][1] = 161; rgb[14][2] = 179; //Purple
  rgb[15][0] = 247; rgb[15][1] = 344; rgb[15][2] = 377; //Lavender
  rgb[16][0] = 189; rgb[16][1] = 182; rgb[16][2] = 200; //Magenta
  rgb[17][0] = 86; rgb[17][1] = 136; rgb[17][2] = 121; //Black
  rgb[18][0] = 186; rgb[18][1] = 256; rgb[18][2] = 220; //Grey
  rgb[19][0] = 444; rgb[19][1] = 565; rgb[19][2] = 482; //White

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
  Serial.print("\nR:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\n");
  Serial.print("This is: "); 
  Serial.println(colors[bestDistIndex]);

  return bestDistIndex;
}

//get index of top face according to die array and check against sensor
int getTopFace() {
  //get index of top face according to die array
  int arrayTopFace = 20;
  for(int i = 0; i < 20; i++) {
    //if the x and y coordinates are zero, and the z coordinate is positive, that face must be pointing up
    if(diePositions[i][2] > 0.97) {
      arrayTopFace = i;
      break;
    }
  }
  if(arrayTopFace == 20) {
    Serial.println("Top face not calculated ");
    while(1);
  }


  //check sensor reading to array, throw error if doesn't match
  int seenTopFace = colorSeen();
  if(arrayTopFace != seenTopFace) {
    Serial.println("Top face does not match");
    //while (1);
  }
  
  Serial.println("\n");
  Serial.print(colorList[arrayTopFace]);
  Serial.print(" is on top");

  return arrayTopFace;
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

//grip the die, lift small amount, spin the die
void spinDie(int rotation) {
  //note: the driving cup should be set to align with die for 0 position

  int pos = 0;
  //if +ve rotation, go to 0, grip, then spin
  if(rotation > 0) {
    servoSpinDie.write(0);//make sure the grippers are aligned
    grip(true);//close the grippers
    motorMove(false, true, 3);//move up in z for 2 seconds
    for(pos = 0; pos <= rotation; pos++) {
      servoSpinDie.write(pos);//increment gripper position
      delay(10);
      servoGrip.write(gripPos);//keep grippers closed
      delay(10);
    }
  }
  //if -ve rotation, go to 144, grip, then spin
  else {
    servoSpinDie.write(144);//make sure the grippers are aligned
    grip(true);//close grippers
    motorMove(false, true, 3);
    for(pos = 144; pos >= (144 + rotation); pos--) {
      servoSpinDie.write(pos);//increment gripper position
      delay(10);
      servoGrip.write(gripPos);//keep grippers closed
      delay(10);
    }
  }
}

//grip or un-grip the grippers (note: does not align the gripper with die)
void grip(bool close) {

  int pos = 0;

  if (close) {
    Serial.println("\nClosing gripper..");
    //close grippers to 100 degree position (closed)
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

//run for a certain time (+ve and -ve are different directions) (0.05s increments)
//for x and z, +ve time is moving up
void motorMove(bool xMove, bool grip, double time) {

  //default to operate z motor, if bool xMove is true then do x motor
  int pin1 = zMotorPin1; int pin2 = zMotorPin2; //int pinCtrl = zMotorCtrlPin; int pinOpto = zOptoPin;
  if(xMove) {
    pin1 = xMotorPin1; pin2 = xMotorPin2; //pinCtrl = xMotorCtrlPin; pinOpto = xOptoPin;
  }

  //default pin values to spin motor forwards, flip if moving in -ve direction
  int pin1Val = 1; int pin2Val = 0;
  if(time < 0) {
    pin1Val = 0; 
    pin2Val = 1;
  }
  
  //operate the motor
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
  if(xMove) {
    delay(5000);
  }
  while(timeCounter < abs(time)*10.0) { 
    if(analogRead(A2) > 200 && xMove) {
      Serial.println(analogRead(A2));
      Serial.println("opto triggered");
      break; //for x motion, turn off motor when opto is tripped
    }
  
    if(grip) {
      servoGrip.write(gripPos);//keep grippers closed
    }
    delay(100);//delay to continue moving the motor
    timeCounter += 1.0;//increment counter
    Serial.print(timeCounter*100.0, 1);
    Serial.print(" ");
    //Serial.println(" ms have elapsed");
  }
  
  //turn off motor by writing 0 to both pins
  digitalWrite(pin1, 0);
  digitalWrite(pin2, 0);

}

//output x,y,z position of each die face for debugging
void outputDiePositions() {
  for(int i = 0; i < 20; i++) {
    Serial.print("\n");
    Serial.print(colorList[i]);
    Serial.print(":    ");
    Serial.print("\tX:");
    Serial.print(diePositions[i][0], 4);
    Serial.print("\tY:");
    Serial.print(diePositions[i][1], 4);
    Serial.print("\tZ:");
    Serial.print(diePositions[i][2], 4);
  }
}

//output true if face "i" can be reached in 1 move from the current face
bool reachable(int i) {
  double tol = 0.01;
  double dist = sqrt(pow(diePositions[i][0], 2) + pow(diePositions[i][1], 2) + pow(diePositions[i][2] - 1, 2));

  //j = 2 and j = 4 are the indices in potentialDistances which correspond to a potential of 1 (reachable face)
  for(int j = 2; j <= 4; j+=2) {
    if(dist < potentialDistances[j][1] + tol && dist > potentialDistances[j][1] - tol) {
      return true;
    }
  }

  return false;
}