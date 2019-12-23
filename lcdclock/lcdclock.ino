/*
  DS3231: Real-Time Clock. Simple example
  Read more: www.jarzebski.pl/arduino/komponenty/zegar-czasu-rzeczywistego-rtc-ds3231.html
  GIT: https://github.com/jarzebski/Arduino-DS3231
  Web: http://www.jarzebski.pl
  (c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <JC_Button.h>

#define BACKLIGHT_PWR 13
#define BACKLIGHT_SW 6
#define SETUP_UP 5
#define SETUP_DOWN 4
#define SETUP_OK 3

DS3231 clock;
RTCDateTime dt;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
Button backlightSwBtn(BACKLIGHT_SW);
Button setupUpBtn(SETUP_UP);
Button setupDownBtn(SETUP_DOWN);
Button setupOkBtn(SETUP_OK);
bool backlightStatus = true;
const int8_t displayHeight = 2;
const int8_t displayWidth = 16;
const int16_t LONG_PRESS = 1000;

struct MenuItem {
  String desc;
  int minValue;
  int maxValue;
};

MenuItem setupItems [] = {
  {"Day", 1, 31},
  {"Month", 1, 12},
  {"Year", 2019, 2100},
  {"Hour", 0, 23},
  {"Minute", 0, 59},
  {"Second", 0, 59}
};

String getDay(RTCDateTime dt) {
  switch(dt.dayOfWeek) {
    case 1: return "SUN";
    case 2: return "MON";
    case 3: return "TUE";
    case 4: return "WED";
    case 5: return "THU";
    case 6: return "FRI";
    case 7: return "SAT";
    default: return "UNKNOWN";
  }
}

void setup()
{
  Serial.begin(9600);
  //turn backlight on
  digitalWrite(BACKLIGHT_PWR, backlightStatus);
  pinMode(BACKLIGHT_PWR, OUTPUT);
  //buttons  
  pinMode(BACKLIGHT_SW, OUTPUT);
  backlightSwBtn.begin();
  pinMode(SETUP_UP, OUTPUT);
  setupUpBtn.begin();
  pinMode(SETUP_DOWN, OUTPUT);
  setupDownBtn.begin();
  pinMode(SETUP_OK, OUTPUT);
  setupOkBtn.begin();

  // Initialize DS3231
  Serial.println("Initialize DS3231");;
  clock.begin();
  //Dec  1 2019
  //15:44:12
  clock.setDateTime(__DATE__, __TIME__);

  lcd.begin(displayWidth,displayHeight);
  
}

void handleBacklightSwitch() {
  backlightSwBtn.read();
  if(backlightSwBtn.wasReleased()) {
    backlightStatus = !backlightStatus;
    digitalWrite(BACKLIGHT_PWR, backlightStatus);
  }
}

int selectValue(MenuItem *item) {
  clearDisplay();
  lcd.setCursor(0,0);
  lcd.print(item->desc);
  int value = item->minValue;
  while(true) {
    setupOkBtn.read();
    setupDownBtn.read();
    setupUpBtn.read();
    if(setupDownBtn.wasReleased()) {
       if(value == item->minValue) {
          value = item->maxValue;
       } else {
          value--;
       }
    }
    if(setupUpBtn.wasReleased()) {
      if(setupUpBtn.wasReleased()) {
        if(value == item->maxValue) {
           value = item->minValue;
        } else {
          value++;
        }
      }  
    }
    if(setupOkBtn.wasReleased()) {
      return value; 
    }
    lcd.setCursor(0,1);
    lcd.print(value);
    lcd.print("   ");
  }
}

/*
 * Menu iteration : 
 * 1) Day (1-31)
 * 2) Month (1-12)
 * 3) Year (4 digit number)
 * 4) hour (0-23)
 * 5) minute (0-59)
 * 6) second (0-59)
 */
void startSetupMode() {
  clearDisplay();
  uint8_t day = setupItems[0].minValue;
  uint8_t month = setupItems[1].minValue;
  int year = setupItems[2].minValue;
  uint8_t hours = setupItems[3].minValue;
  uint8_t minutes = setupItems[4].minValue;
  uint8_t seconds = setupItems[5].minValue;
  for(uint8_t i=0; i < sizeof(setupItems)/sizeof(setupItems[0]); i++) {
    int value = selectValue(&setupItems[i]);
    switch(i) {
      case 0: day = value;
              break;
      case 1: month = value;
              break;
      case 2: year = value;
              break;
      case 3: hours = value;
              break;
      case 4: minutes = value;
              break;
      case 5: seconds = value;
              break;
    }
  }
  clock.setDateTime(year, month, day, hours, minutes, seconds);
  clearDisplay();
}

void clearDisplay() {
  for(int i=0; i<displayHeight; i++) {
    lcd.setCursor(0,i);
    for(int j=0;j<displayWidth;j++) {
      lcd.print(" ");
    }
  }
  lcd.setCursor(0,0);
}

void showCurrentTime() {
  dt = clock.getDateTime();

    lcd.setCursor(0,0);
    lcd.print(dt.day);
    lcd.print(".");
    lcd.print(dt.month);
    lcd.print(".");
    lcd.print(dt.year);
    lcd.print("  ");
    lcd.print(getDay(dt));

    lcd.setCursor(0,1);
    lcd.print(dt.hour);
    lcd.print(":");
    lcd.print(dt.minute);   
    lcd.print(":");
    lcd.print(dt.second);
    lcd.print(" ");
}

void loop()
{
  setupOkBtn.read();
  if(setupOkBtn.wasReleased()) {
    startSetupMode();
  }
  handleBacklightSwitch();
  showCurrentTime();
  delay(20);
}
