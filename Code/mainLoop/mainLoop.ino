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

void setup() {

  //assign PWM pins to each servo
  servoGrip.attach(2);
  servoRotateArm.attach(4);
  servoSpinDie.attach(6);

}

void loop() {

  //reset die positions and set axis vectors
  arrayInitialize();

  //orient the model by checking locations of two die faces
  orientModel();

  //get the desired end color & assign potentials
  int goalColor = 17; //can be user input
  assignPotentials(goalColor);

  //get the top face from array & check against sensor
  int arrayTopFace = getTopFace();

  //spin the die until the desired face is up
  while(diePotentials[arrayTopFace] > 0) {
    //reset tracker
    int currentPotential = diePotentials[arrayTopFace];

    for(int i = 0; i < 20; i++) {
      if(diePotentials[i] == currentPotential - 1) {
        transform(i);
      }
    }

    arrayTopFace = getTopFace();

  }

  //move die to finish

  //move die back to start (after delay)

  //stop program so it only loops once
  while(1);
  Serial.println("Program ended before looped again");
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

  //extra axes reserved for initial model transformation (unknown axis and z axis)
  axisVectors[3][0] = 0; axisVectors[3][1] = 0; axisVectors[3][2] = 0;
  axisVectors[4][0] = 0; axisVectors[4][1] = 0; axisVectors[4][2] = 1;

}

void orientModel() {

  //get top face
  int seenTopFace = colorSeen();

  //pick up die and rotate 72


  //get second face
  int secondSeenFace = colorSeen();

  //rotate -72 and put down die


  //transform the model to have the correct top face up
  //get coordinates of desired top face and do cross product to find rotation axis
  double x1 = diePositions[seenTopFace][0]; double y1 = diePositions[seenTopFace][1]; double z1 = diePositions[seenTopFace][2];
  double x2 = 0; double y2 = 0; double z2 = 1;

  double norm = sqrt(pow(x1, 2) + pow(y1, 2));
  axisVectors[3][0] = y1 / norm;
  axisVectors[3][1] = -1 * x1 / norm;
  axisVectors[3][2] = 0;

  //use dot product to find rotation angle
  double dotProduct = x1 * x2 + y1 * y2 + z1 * z2;//really just =z1
  double norm1 = sqrt(pow(x1, 2) + pow(y1, 2) + pow(z1, 2));
  double norm2 = sqrt(pow(x2, 2) + pow(y2, 2) + pow(z2, 2));//really just =1
  double rotation = acos(dotProduct / (norm1 * norm2));

  //should have to only rotate clockwise because of cross product??, so make rotation negative
  rotation *= -1;

  rotation *= 180 / 3.14159265;
  transformModel(3, rotation);


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

  rotationz *= 180 / 3.14159265;
  transformModel(4, rotationz);
}

//give potentials to all faces based on the desired face
void assignPotentials(int goalColor) {
  //top: 0 (dist=0)
  //second row: 2 (dist=0.713644)
  //third row: 1 (dist=1.154700)
  //fourth row: 2 (dist=1.632993)
  //fifth row: 1 (dist=1.868345)
  //bottom: 3 (dist=2)

  potentialDistances[0][0] = 0; potentialDistances[0][1] = 0; //top face
  potentialDistances[1][0] = 2; potentialDistances[1][1] = 0.713644; //top face
  potentialDistances[2][0] = 1; potentialDistances[2][1] = 1.154700; //top face
  potentialDistances[3][0] = 2; potentialDistances[3][1] = 1.632993; //top face
  potentialDistances[4][0] = 1; potentialDistances[4][1] = 1.868345; //top face
  potentialDistances[5][0] = 3; potentialDistances[5][1] = 2; //top face

  double dist = 0;
  double tol = 0.05;

  for(int i = 0; i < 20; i++) {
    //distance between goal point and selected point
    dist = sqrt(pow(diePositions[goalColor][0] - diePositions[i][0], 2) + pow(diePositions[goalColor][1] - diePositions[i][1], 2) + pow(diePositions[goalColor][1] - diePositions[i][1], 2));

    //set die potential to 4 to make sure it gets changed
    diePotentials[i] = 4;

    //select potential based on distance
    for(int j = 0; j < 6; j++) {
      if(dist < potentialDistances[j][1] + tol && dist > potentialDistances[j][1] - tol) {
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
      i = 2; //break the for loop
    }
  }

  //get the distance from top face to desired top face to know how much to rotate by
  distFaces = sqrt(pow(diePositions[desiredFace][0], 2) + pow(diePositions[desiredFace][1], 2) + pow(diePositions[desiredFace][1] - 1, 2));

  if(distFaces > potentials[2][1] - tol && distFaces < potentials[2][1] + tol) {
    rotation = 72;
  }
  else if(distFaces > potentials[4][1] - tol && distFaces < potentials[4][1] + tol) {
    rotation = 144;
  }
  else {
    Serial.println("Error: Rotation distance not calculated");
    while (1); // halt!
  }

  //check location of desired face to know if rotation is positive or negative
  if(axis == 0) {
    if(diePositions[desiredFace][0] > 0) {
      rotation = -1 * rotation;
    }
  }
  else if(axis == 1 || axis == 2) {
    if(diePositions[desiredFace][1] < 0) {
      rotation = -1 * rotation;
    }
  }
  else {
    Serial.println("Error: Axis not found - rotation direction not calculated");
    while (1); // halt!
  }

  //print axis and rotation (for debugging)
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
    diePositions[i][0] *= x*(ct + ux*ux*(1 - ct)) + y*(ux*uy*(1 - ct) - uz*st) + z*(ux*uz*(1 - ct) + uy*st);
    diePositions[i][1] *= x*(uy*ux*(1 - ct) + uz*st) + y*(ct + uy*uy*(1 - ct)) + z*(uy*uz*(1 - ct) - ux*st);
    diePositions[i][2] *= x*(uz*ux*(1 - ct) - uy*st) + y*(uz*uy*(1 - ct) + ux*st) + z*(ct + uz*uz*(1 - ct));
  }

}

//transform the die using given axis and rotation 
void transformDie(int axis, int rotation) {

  //zero the arm servo
  //move arm to desired axis
  //zero the gripper servo (open) & zero the spin servo(0 or 180->144 depending on direction of rotation)
  //grip the die
  //raise traverse
  //spin the die
  //lower traverse until hard stops
  //ungrip the die

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
  for(int i = 0; i < 20; i++) {

    //if the x and y coordinates are zero, and the z coordinate is positive, that face must be pointing up
    if(diePositions[i][0] == 0 && diePositions[i][1] == 0 && diePositions[i][2] > 0) {
      arrayTopFace = i;
    }
  }

  //check sensor reading to array, throw error if doesn't match
  int seenTopFace = colorSeen();
  if(arrayTopFace != seenTopFace) {
    Serial.println("Top face does not match");
    while (1);
  }
  
  return arrayTopFace;
}

void rotateArm(int axis) {

}

void spinDie(int rotation) {

}
