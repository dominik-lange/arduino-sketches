int lastValue = 0;
int rd = 4;
int grn = 3;
int bl = 2;

void setup() {
  Serial.begin(9600);
  pinMode(rd, OUTPUT);
  pinMode(grn, OUTPUT);
  pinMode(bl, OUTPUT);
}

void loop() {
  int value = (int) (analogRead(1) / 100);
  if(value <= 3) {
    digitalWrite(rd, HIGH);
    digitalWrite(grn, LOW);
    digitalWrite(bl, LOW);
  } else if(value <= 6) {
    digitalWrite(rd, LOW);
    digitalWrite(grn, HIGH);
    digitalWrite(bl, LOW);
  } else {
    digitalWrite(rd, LOW);
    digitalWrite(grn, LOW);
    digitalWrite(bl, HIGH);
  }
}
