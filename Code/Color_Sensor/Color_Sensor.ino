#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);


int rgb[20][3];
char *colors[20];

void setup() {
  
  //turn on color sensor, check if on
  Serial.begin(9600);
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  char *colorList[] = {"Maroon", "Red", "Pink", "Brown", "Orange", "Apricot", "Yellow", 
  "Lime", "Green", "Mint", "Teal", "Cyan", "Navy", "Blue", "Purple", "Lavender", "Magenta",
  "Black", "Grey", "White"};

  for (int i = 0; i < 20; i++){
    colors[i] = colorList[i];
  }
  
  int dist = 15;

  if(dist == 5) {
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
  }

  if (dist == 15) {
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
  }
  
  
}

void loop() {
  delay(1000);
  
  int red, green, blue, clear;

  //read colors
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

  //print colors
  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\tC:\t"); Serial.print(int(clear));
  Serial.print("\n");
  Serial.print("This is: "); Serial.print(colors[bestDistIndex]);
  Serial.print("\n");
}
