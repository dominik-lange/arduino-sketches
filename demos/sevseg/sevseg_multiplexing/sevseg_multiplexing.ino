/* Demo sketch to demonstrate how to multiplex a 4 digits seven segment display (SH5461AS - common cathode) without a library.
 * In order to save some GIO pins a 74HC595 shift register is used to drive the segments
 * 4 transistors are used to control the digit-position to be displayed.
 * 
 */



//hc595
#define HC595_SER 7
#define HC595_RCLK 6
#define HC595_SRCLK 5

#define DUTY_CYCLE 5

/*     a
 *    ==                Example                                   ==
 * f |  | b             | a | b | c | d | e | f | g | DP |       |  |
 *    ==  ---> g        ==================================  ==>     
 * e |  | c             | 1 | 1 | 1 | 1 | 1 | 1 | 0 | 0  |       |  |
 *    ==   * DP                                                   ==
 *    d
*/
byte digits[] = {
    0b11111100,0b01100000,0b11011010,0b11110010,0b01100110,0b10110110,0b10111110,0b11100000,0b11111110,0b11110110
};

//control gnd transistors --> values are the pins that are connected to the transistors that control the position to display a digit on
byte positions[] = {
    13,12,11,10
};

void set(byte value) {
   digitalWrite(HC595_RCLK, LOW);
   shiftOut(HC595_SER, HC595_SRCLK, LSBFIRST, value);
   digitalWrite(HC595_RCLK, HIGH);
}

void setup() {
  for(int i=0; i<sizeof(positions)/sizeof(positions[0]); i++) {
    pinMode(positions[i], OUTPUT);  
    delay(20);
    digitalWrite(positions[i], LOW);
  }
  pinMode(HC595_SER, OUTPUT);
  pinMode(HC595_RCLK, OUTPUT);
  pinMode(HC595_SRCLK, OUTPUT);
}

//18.26
byte num[] = {
  digits[1], digits[8]+1, digits[2], digits[6]
};

void loop() {
  //multiplexing --> showing 18.26
  for(int pos=0; pos<sizeof(positions)/sizeof(positions[0]); pos++) {
    set(num[pos]);
    digitalWrite(positions[pos], HIGH);
    delay(DUTY_CYCLE);
    digitalWrite(positions[pos], LOW);
  }
}
