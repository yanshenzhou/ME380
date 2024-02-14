#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);


int rgb[22][3];
char *colors[22];

void setup() {
  
  //turn on color sensor, check if on
  Serial.begin(9600);
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  char *colorList[] = {"Maroon", "Red", "Pink", "Brown", "Orange", "Apricot", "Olive", "Yellow", "Beige", 
  "Lime", "Green", "Mint", "Teal", "Cyan", "Navy", "Blue", "Purple", "Lavender", "Magenta",
  "Black", "Grey", "White"};

  for (int i = 0; i < 22; i++){
    colors[i] = colorList[i];
  }
  

  rgb[0][0] = 115; rgb[0][1] = 75; rgb[0][2] = 65; //Maroon
  rgb[1][0] = 120; rgb[1][1] = 65; rgb[1][2] = 65; //Red
  rgb[2][0] = 95; rgb[2][1] = 80; rgb[2][2] = 70; //Pink
  rgb[3][0] = 110; rgb[3][1] = 80; rgb[3][2] = 60; //Brown
  rgb[4][0] = 125; rgb[4][1] = 75; rgb[4][2] = 50; //Orange
  rgb[5][0] = 100; rgb[5][1] = 85; rgb[5][2] = 65; //Apricot
  rgb[6][0] = 95; rgb[6][1] = 90; rgb[6][2] = 65; //Olive
  rgb[7][0] = 110; rgb[7][1] = 90; rgb[7][2] = 50; //Yellow
  rgb[8][0] = 95; rgb[8][1] = 90; rgb[8][2] = 70; //Beige
  rgb[9][0] = 100; rgb[9][1] = 100; rgb[9][2] = 50; //Lime
  rgb[10][0] = 80; rgb[10][1] = 100; rgb[10][2] = 70; //Green
  rgb[11][0] = 90; rgb[11][1] = 90; rgb[11][2] = 75; //Mint
  rgb[12][0] = 80; rgb[12][1] = 90; rgb[12][2] = 80; //Teal
  rgb[13][0] = 70; rgb[13][1] = 95; rgb[13][2] = 80; //Cyan
  rgb[14][0] = 60; rgb[14][1] = 85; rgb[14][2] = 105; //Navy
  rgb[15][0] = 65; rgb[15][1] = 90; rgb[15][2] = 100; //Blue
  rgb[16][0] = 85; rgb[16][1] = 70; rgb[16][2] = 95; //Purple
  rgb[17][0] = 85; rgb[17][1] = 80; rgb[17][2] = 80; //Lavender
  rgb[18][0] = 110; rgb[18][1] = 65; rgb[18][2] = 80; //Magenta
  rgb[19][0] = 85; rgb[19][1] = 85; rgb[19][2] = 75; //Black
  rgb[20][0] = 90; rgb[20][1] = 80; rgb[20][2] = 80; //Grey
  rgb[21][0] = 90; rgb[21][1] = 85; rgb[21][2] = 75; //White

  //int rgb [][] = {
  //  {},
  //  {},
  //  {}, 
  //  };
  
}

void loop() {
  delay(1000);
  
  float red, green, blue;

  //read colors
  tcs.setInterrupt(false); // turn on LED
  delay (60); // wait to read
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true); // turn off LED
  red = int(red); green = int(green); blue = int(blue);

  double dist; double bestDist = 10000;
  int bestDistIndex = 0;

  for (int i = 0; i < 22; i++) {
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
  Serial.print("\n");
  Serial.print("This is: "); Serial.print(colors[bestDistIndex]);
  Serial.print("\n");
}
