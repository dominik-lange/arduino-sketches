#include <DS3231.h>
#include <JC_Button.h>

//digital pins
#define DATA 12
#define STORE 11 //latch
#define SHIFT 10
#define HOUR_SET 9
#define MIN_SET 8
#define SEC_SET 7
#define BTN_SET 6
#define BTN_DOWN 5
#define BTN_UP 4
#define BRIGHTNESS_CTRL 3
//analog pins
#define POTI 3

const uint16_t LONG_PRESS = 2000;

DS3231 clock;
RTCDateTime dt;
Button setButton(BTN_SET);
Button upButton(BTN_UP);
Button downButton(BTN_DOWN);
bool showSeconds = true;

//set the time
void set(byte seconds, byte minutes, byte hours) {
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
  //74hc595 connectors
  pinMode(DATA, OUTPUT);
  pinMode(STORE, OUTPUT);  
  pinMode(SHIFT, OUTPUT);
  //buttons
  pinMode(HOUR_SET, OUTPUT);
  pinMode(MIN_SET, OUTPUT);
  pinMode(SEC_SET, OUTPUT);
  //connected to output enable, which is active low, a PWD signal controls the brightness, thus 0 is highest, brightness and 255 would lead to all LEDs turned off
  pinMode(BRIGHTNESS_CTRL, OUTPUT);
  //initial brightness value
  analogWrite(BRIGHTNESS_CTRL, 240);
  //init RTC
  clock.begin();
  //clear shift register
  set(0,0,0);
  delay(1000);
}

//LEDs wil blink loop times
void blink(uint8_t loops) {
  for(int i=0; i<loops; i++) {
    set(0,0,0);
    delay(200);
    set(59,59,23);
    delay(200);
  }
}

//handle a press on up button
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

//increases respective value and switch at maximum
void handlePlus(byte* val, byte max) {
  if(*val == max) {
    *val = 0;
  } else {
    (*val)++;
  }
}

//handle a press on down button
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

//decrease respective value and switch at maximum
void handleMinus(byte* val, byte max) {
  if(*val == 0) {
    *val = max;
  } else {
     (*val)--;
  }
}

//step through the set menu
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

void handleBrightnessControl() {
  int brightness = analogRead(POTI) / 4;
  Serial.println(brightness);
  analogWrite(POTI, brightness);
}

void loop() 
{
  handleBrightnessControl();
  setButton.read();
  upButton.read();
  downButton.read();
  //turn off displaying seconds by pressing the up and down button at the same time for LONG_PRESS millis
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
  //set the current time
  dt = clock.getDateTime();
  set(dt.second, dt.minute, dt.hour);
  //delays the loop, higher values below 1000 millis are possible but would lead to more sluggish button and poti reavtion after press
  delay(200);
}
