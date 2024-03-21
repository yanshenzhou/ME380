int xMotorPin1 = 22; int xMotorPin2 = 23; int zMotorPin1 = 26; int zMotorPin2 = 27;
int xMotorCtrlPin = 8; int zMotorCtrlPin = 9;
int xOptoPin = A10; int zOptoPin = A11;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(xMotorPin1, OUTPUT);
  pinMode(xMotorPin2, OUTPUT);
  pinMode(zMotorPin1, OUTPUT);
  pinMode(zMotorPin2, OUTPUT);
}

uint8_t state = 0;

void loop() {
// put your main code here, to run repeatedly:
  int esOut = analogRead(A0);
  //Serial.println(esOut);

  digitalWrite(xMotorPin1, state);
  digitalWrite(xMotorPin2, !state);


  if (esOut > 200) {
    digitalWrite(xMotorPin1, LOW);
    digitalWrite(xMotorPin2, LOW);
    delay(1000);
    state = !state;
  }

}
