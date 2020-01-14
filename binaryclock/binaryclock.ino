#include <DS3231.h>
#include <JC_Button.h>

#define DATA 12
#define STORE 11 //latch
#define SHIFT 10
#define HOUR_SET 9
#define MIN_SET 8
#define SEC_SET 7
#define BTN_SET 6
#define BTN_DOWN 5
#define BTN_UP 4

const uint16_t LONG_PRESS = 2000;

DS3231 clock;
RTCDateTime dt;
Button setButton(BTN_SET);
Button upButton(BTN_UP);
Button downButton(BTN_DOWN);
bool showSeconds = true;

void set(byte seconds, byte minutes, byte hours)
{
   digitalWrite(STORE, LOW);
   shiftOut(DATA, SHIFT, LSBFIRST, showSeconds ? seconds : 0);
   shiftOut(DATA, SHIFT, LSBFIRST, minutes);
   shiftOut(DATA, SHIFT, LSBFIRST, hours);
   digitalWrite(STORE, HIGH);
}

void setup() 
{
  Serial.begin(9600);
  setButton.begin();
  upButton.begin();
  downButton.begin();
  pinMode(DATA, OUTPUT);
  pinMode(STORE, OUTPUT);  
  pinMode(SHIFT, OUTPUT);
  pinMode(HOUR_SET, OUTPUT);
  pinMode(MIN_SET, OUTPUT);
  pinMode(SEC_SET, OUTPUT);
  clock.begin();
  clock.setDateTime(__DATE__,__TIME__);
  set(0,0,0);
  delay(1000);
}

void blink(uint8_t loops) {
  for(int i=0; i<loops; i++) {
    set(0,0,0);
    delay(200);
    set(59,59,23);
    delay(200);
  }
}

void handleUp(byte step, byte* hour, byte* mnt, byte* sec) {
  switch(step) {
    case 0:
      handlePlus(hour, 23);
      Serial.println(*hour);
      break;
    case 1:
        handlePlus(mnt, 59);
        Serial.println(*mnt);
      break;
    case 2:
      handlePlus(sec, 59);
      Serial.println(*sec);
      break;
  }
  set(*sec, *mnt, *hour);
}

void handlePlus(byte* val, byte max) {
  if(*val == max) {
    *val = 0;
  } else {
    (*val)++;
  }
}

void handleDown(byte step, byte* hour, byte* mnt, byte* sec) {
  switch(step) {
    case 0:
      handleMinus(hour, 23);
        Serial.println(*hour);
      break;
    case 1:
        handleMinus(mnt, 59);
        Serial.println(*mnt);
      break;
    case 2:
        handleMinus(sec, 59); 
        Serial.println(*sec);
      break;
  }
  set(*sec, *mnt, *hour);
}

void handleMinus(byte* val, byte max) {
  if(*val == 0) {
    *val = max;
  } else {
     (*val)--;
  }
}

void menu() {
  byte step = 0;
  byte hour = 0;
  byte mnt = 0;
  byte sec = 0;
  digitalWrite(HOUR_SET, HIGH);
  set(0,0,0);
  delay(25);
  while(step < 3) {
    upButton.read();
    downButton.read();
    setButton.read();
    if(upButton.wasReleased()) {
      handleUp(step, &hour, &mnt, &sec);
    }
    if(downButton.wasReleased()) {
      handleDown(step, &hour, &mnt, &sec);
    }
    if(setButton.wasReleased()) {
      switch(++step) {
        case 1:
          digitalWrite(HOUR_SET, LOW);
          digitalWrite(MIN_SET, HIGH);
          break;
        case 2:
          digitalWrite(MIN_SET, LOW);
          digitalWrite(SEC_SET, HIGH);
          break;
        case 3:
          digitalWrite(SEC_SET, LOW);
      }
    }
    delay(100);
  }
  if(hour > 0 || mnt > 0 || sec > 0) {
    clock.setDateTime(2020, 1, 1, hour, mnt, sec);
  }
}

void loop() 
{
  setButton.read();
  upButton.read();
  downButton.read();
  if(upButton.pressedFor(LONG_PRESS) && downButton.pressedFor(LONG_PRESS)) {
      showSeconds = !showSeconds;
      blink(2);
      //set correct time before delay not to confuse the user
      dt = clock.getDateTime();
      set(dt.second, dt.minute, dt.hour);
      //delay is necessary to avoid flip again when buttons are pressed slightly too long
      delay(2000);
  } 
  if(setButton.wasReleased()) {
    menu();
  }
  dt = clock.getDateTime();
  set(dt.second, dt.minute, dt.hour);
  delay(100);
}
