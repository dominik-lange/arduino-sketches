#define CHOICE_OFF      0 //Used to control LEDs
#define CHOICE_NONE     0 //Used to check buttons
#define CHOICE_RED  (1 << 0)
#define CHOICE_GREEN    (1 << 1)
#define CHOICE_BLUE (1 << 2)
#define CHOICE_YELLOW   (1 << 3)
#define CHOICE_INTERRUPT (1 << 4)

#define LED_RED     10
#define LED_GREEN   11
#define LED_BLUE    9
#define LED_YELLOW  12

// Button pin definitions
#define BUTTON_INTERRUPT 8
#define BUTTON_RED    3
#define BUTTON_GREEN  4
#define BUTTON_BLUE   2
#define BUTTON_YELLOW 5

// Buzzer pin definition
#define BUZZER  6

// Define game parameters
#define ROUNDS_TO_WIN      13 //Number of rounds to succesfully remember before you win. 13 is do-able.
#define ENTRY_TIME_LIMIT   5000 //Amount of time to press a button before game times out. 3000ms = 3 sec

#define MODE_SIMON_SAYS  0
#define MODE_MUSIC 1

// Game state variables
byte gameBoard[32]; //Contains the combination of buttons as we advance
byte gameRound = 0; //Counts the number of succesful rounds the player has made it through
uint8_t gameMode = MODE_SIMON_SAYS; //Default mode is simon says

void setup() {
  Serial.begin(9600);
  //Setup hardware inputs/outputs. These pins are defined in the hardware_versions header file

  //Enable pull ups on inputs
  pinMode(BUTTON_INTERRUPT, INPUT_PULLUP);
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  pinMode(BUZZER, OUTPUT);
}

void loop() {
  // Blink lights while waiting for user to press a button
  byte buttonPress = attractMode(); 
  gameMode = determineGameMode(buttonPress);

  if(gameMode == MODE_SIMON_SAYS) {
    // Indicate the start of game play
    setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Turn all LEDs on
    delay(1000);
    setLEDs(CHOICE_OFF); // Turn off LEDs
    delay(250);
    // Play memory game and handle result
    if (play_memory() == true) 
      playWinner(); // Player won, play winner tones
    else 
      playLoser(); // Player lost, play loser tones
  } else if (gameMode == MODE_MUSIC) {
    bool exit = false;
    while(!exit) {
      byte buttonPress = checkButton();
      if(buttonPress == CHOICE_INTERRUPT) {
        exit = true;
      } else {
        soundAndLightForButton(buttonPress, 200);
      }
    }
  }
}

// Show an "attract mode" display while waiting for user to press button.
byte attractMode() {
  byte buttonPress = CHOICE_NONE;
  while(1) 
  {
    setLEDs(CHOICE_RED);
    delay(175);
    buttonPress = checkButton();
    if (buttonPress != CHOICE_NONE) return buttonPress;

    setLEDs(CHOICE_BLUE);
    delay(175);
    buttonPress = checkButton();
    if (buttonPress != CHOICE_NONE) return buttonPress;

    setLEDs(CHOICE_GREEN);
    delay(175);
    buttonPress = checkButton();
    if (buttonPress != CHOICE_NONE) return buttonPress;

    setLEDs(CHOICE_YELLOW);
    delay(175);
    buttonPress = checkButton();
    if (buttonPress != CHOICE_NONE) return buttonPress;
  }
}

byte determineGameMode(byte button) {
  if(button == CHOICE_BLUE) {
    return MODE_MUSIC;
  }
  //default game mode
  return MODE_SIMON_SAYS;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//The following functions are related to game play only

// Play the regular memory game
// Returns 0 if player loses, or 1 if player wins
boolean play_memory() {
  randomSeed(millis()); // Seed the random generator with random amount of millis()

  gameRound = 0; // Reset the game to the beginning

  while (gameRound < ROUNDS_TO_WIN) {
    add_to_moves(); // Add a button to the current moves, then play them back

    playMoves(); // Play back the current game board

    // Then require the player to repeat the sequence.
    for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) {
      byte choice = waitForButton(); // See what button the user presses

      // If wait timed out, player loses
      if (choice == 0) {
        return false; 
      }

      // If the choice is incorect, player loses
      if (choice != gameBoard[currentMove]) {
        return false; 
      }
    }

    // Player was correct, delay before playing moves
    playCorrect();
    delay(1000); 
  }

  return true; // Player made it through all the rounds to win!
}



// Plays the current contents of the game moves
void playMoves() {
  for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) 
  {
    soundAndLightForButton(gameBoard[currentMove], 150);
    // Wait some amount of time between button playback
    // Shorten this to make game harder
    delay(300); // 150 works well. 75 gets fast.
  }
}



// Adds a new random button to the game sequence, by sampling the timer
void add_to_moves(void) {
  byte newButton = random(0, 4); //min (included), max (exluded)

  // We have to convert this number, 0 to 3, to CHOICEs
  if(newButton == 0) newButton = CHOICE_RED;
  else if(newButton == 1) newButton = CHOICE_GREEN;
  else if(newButton == 2) newButton = CHOICE_BLUE;
  else if(newButton == 3) newButton = CHOICE_YELLOW;

  gameBoard[gameRound++] = newButton; // Add this new button to the game array
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//The following functions control the hardware

// Lights a given LEDs
// Pass in a byte that is made up from CHOICE_RED, CHOICE_YELLOW, etc
void setLEDs(byte leds) {
  if ((leds & CHOICE_RED) != 0)
    digitalWrite(LED_RED, HIGH);
  else
    digitalWrite(LED_RED, LOW);

  if ((leds & CHOICE_GREEN) != 0)
    digitalWrite(LED_GREEN, HIGH);
  else
    digitalWrite(LED_GREEN, LOW);

  if ((leds & CHOICE_BLUE) != 0)
    digitalWrite(LED_BLUE, HIGH);
  else
    digitalWrite(LED_BLUE, LOW);

  if ((leds & CHOICE_YELLOW) != 0)
    digitalWrite(LED_YELLOW, HIGH);
  else
    digitalWrite(LED_YELLOW, LOW);
}

// Wait for a button to be pressed. 
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out
byte waitForButton() {
  long startTime = millis(); // Remember the time we started the this loop

  while ( (millis() - startTime) < ENTRY_TIME_LIMIT) // Loop until too much time has passed
  {
    byte button = checkButton();

    if (button != CHOICE_NONE){ 
      soundAndLightForButton(button, 150);
      while(checkButton() != CHOICE_NONE) ;  // Now let's wait for user to release button
      delay(10); // This helps with debouncing and accidental double taps
      return button;
    }

  }

  return CHOICE_NONE; // If we get here, we've timed out!
}

// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
byte checkButton() {
  if (digitalRead(BUTTON_RED) == 0) return CHOICE_RED; 
  else if (digitalRead(BUTTON_GREEN) == 0) return CHOICE_GREEN; 
  else if (digitalRead(BUTTON_BLUE) == 0) return CHOICE_BLUE; 
  else if (digitalRead(BUTTON_YELLOW) == 0) return CHOICE_YELLOW;
  else if (digitalRead(BUTTON_INTERRUPT) == 0) return CHOICE_INTERRUPT;

  return(CHOICE_NONE); // If no button is pressed, return none
}

//=======================================================================annoying sound stuff====================================================================

#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978

int winnerMelody[] = {
  NOTE_D8
};

int winnerMelodyDurations[] = {
  1
};

int loserMelody[] = {
  NOTE_G3, NOTE_G2
};

int loserMelodyDurations[] = {
  4, 2
};

int correctMelody[] = {
  NOTE_B6, NOTE_D7
};

int correctMelodyDurations[] = {
  8, 4
};

void playTune(int melody[], int durations[], int tuneLength) {
  for (int thisNote = 0; thisNote < tuneLength; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    sound(melody[thisNote], 1000 / durations[thisNote]);
  }
}

void sound(uint16_t note, uint16_t duration) {
  tone(BUZZER, note, duration);
  delay(duration * 1.30);
  noTone(BUZZER);
}

void playWinner() {
  playTune(winnerMelody, winnerMelodyDurations, sizeof(winnerMelody)/sizeof(winnerMelody[0]));
}

void playLoser() {
   playTune(loserMelody, loserMelodyDurations, sizeof(loserMelody)/sizeof(loserMelody[0]));
}

void playCorrect() {
  playTune(correctMelody, correctMelodyDurations, sizeof(correctMelody)/sizeof(correctMelody[0]));
}

void soundAndLightForButton(byte button, uint16_t duration) {
  setLEDs(button);
  switch (button) {
    case CHOICE_YELLOW:
      sound(NOTE_C4, duration);
      break;
    case CHOICE_GREEN:
      sound(NOTE_D4, duration);
      break;
    case CHOICE_RED:
      sound(NOTE_E4, duration);
      break;
    case CHOICE_BLUE:
      sound(NOTE_F4, duration);
      break;
  }
  setLEDs(CHOICE_OFF);
}