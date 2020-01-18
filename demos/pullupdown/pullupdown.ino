#define greenLed 12
#define blueLed 11
#define pu 7
#define pd 6

void setup() {
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(pu, INPUT_PULLUP);
  pinMode(pd, INPUT);
}

void loop() {
  int green = digitalRead(pu);
  int blue = digitalRead(pd);
  digitalWrite(greenLed, !green);
  digitalWrite(blueLed, blue);
  delay(100);
}
