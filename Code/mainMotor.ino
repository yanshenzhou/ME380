void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

uint8_t state = 0;

void loop() {
// put your main code here, to run repeatedly:
  int esOut = analogRead(A0);
  //Serial.println(esOut);

  digitalWrite(5, state);
  digitalWrite(6, !state);


  if (esOut > 200) {
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    delay(1000);
    state = !state;
  }

}
