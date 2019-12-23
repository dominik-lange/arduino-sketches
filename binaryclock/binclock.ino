int DATA = 12;
int STORE = 11;
int SHIFT = 10;

void set(byte seconds, byte minutes, byte hours)
{
   digitalWrite(STORE, LOW);
   shiftOut(DATA, SHIFT, LSBFIRST, seconds);
   shiftOut(DATA, SHIFT, LSBFIRST, minutes);
   shiftOut(DATA, SHIFT, LSBFIRST, hours);
   digitalWrite(STORE, HIGH);
}

void setup() 
{
  pinMode(DATA, OUTPUT);
  pinMode(STORE, OUTPUT);  
  pinMode(SHIFT, OUTPUT);
  set(0,0,0);
  delay(1000);
}

void loop() 
{
  set((byte) 12, (byte) 52, (byte) 11);
  delay(500);
  set((byte) 16, (byte) 34, (byte) 22);
  delay(500);
  set((byte) 4, (byte) 21, (byte) 33);
  delay(500);
  set((byte) 8, (byte) 10, (byte) 44);
  delay(500);
}
