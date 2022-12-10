#include <Servo.h>

Servo servo;

using note_freq = float;

unsigned int ledBlinkInterval = 100;
unsigned int ledBlinkTimes = 8;



struct Keys {
  int C = 3;
  int C_s = 4;
  int D = 5;
  int D_s = 6;
  int E = 1; // pin tx (will be put into digital mode)
  int F = 8;
  int F_s = 9;
  int G = 14;
  int G_s = 15;
  int A = 16;
  int A_s = 17;
  int B = 18;
  int C2 = 19;
};

struct Misc {
  int speaker = 10;
  int led = 12;
  int servo = 11;
  int shackle_sense = 13;
  int master_button = 0; //rx (will be put into digital mode)
};

struct Notes {
  note_freq C = 261.63*2;
  note_freq C_s = 277.18*2;
  note_freq D = 293.66*2;
  note_freq D_s = 311.13*2;
  note_freq E = 329.63*2;
  note_freq F = 349.23*2;
  note_freq F_s = 369.99*2;
  note_freq G = 392.00*2;
  note_freq G_s = 415.30*2;
  note_freq A = 440.00*2;
  note_freq A_s = 466.16*2;
  note_freq B = 493.88*2;
  note_freq C2 = 523.25*2;
};

Misc misc = Misc();
Keys keys = Keys();
Notes notes = Notes();

int keyArray[13] = {keys.C, keys.C_s,keys.D,keys.D_s,keys.E,keys.F,keys.F_s,keys.G,keys.G_s,keys.A,keys.A_s,keys.B,keys.C2};

struct State {
  int mode = 0; // 0=Reproduce notes, 1=Perfect pitch
  int shackle_locked = 0; //0=locked, 1=not locked
  int ledState = 0; //0 = led off, 
  int enteredComboLength = 0; // length of combo already entered

  bool masterButtonBreak = false; // used to make sure button is only registered once when pressed
  bool keyButtonBreak = false;

  unsigned int breakBlink = 0;
  unsigned int blinkPreviousMillis = 0;

  bool unlockedShackleStillIn = false;  // when device in unlocked, wait for user to pull shackle out.

  int combo[10] = {keys.E, keys.G, keys.A, keys.A_s, keys.A, keys.G, keys.E, keys.D, keys.F_s, keys.E}; // need to !amongus! to unlock

  int entered_combo[100];
};
 
State state = State();

void pinSetup() {
  pinMode(misc.speaker, OUTPUT);
  pinMode(misc.led, OUTPUT);
  servo.attach(misc.servo);
  pinMode(misc.shackle_sense, INPUT_PULLUP);
  pinMode(misc.master_button, INPUT_PULLUP);
  pinMode(keys.C, INPUT_PULLUP);
  pinMode(keys.C_s, INPUT_PULLUP);
  pinMode(keys.D, INPUT_PULLUP);
  pinMode(keys.D_s, INPUT_PULLUP);
  pinMode(keys.E, INPUT_PULLUP);
  pinMode(keys.F, INPUT_PULLUP);
  pinMode(keys.F_s, INPUT_PULLUP);
  pinMode(keys.G, INPUT_PULLUP);
  pinMode(keys.G_s, INPUT_PULLUP);
  pinMode(keys.A, INPUT_PULLUP);
  pinMode(keys.A_s, INPUT_PULLUP);
  pinMode(keys.B, INPUT_PULLUP);
  pinMode(keys.C2, INPUT_PULLUP);
}

int fetchAssociatedNote(int key) { //fetch note from pin
  if(key == keys.C) {return(notes.C);}
  if(key == keys.C_s) {return(notes.C_s);}
  if(key == keys.D) {return(notes.D);}
  if(key == keys.D_s) {return(notes.D_s);}
  if(key == keys.E) {return(notes.E);}
  if(key == keys.F) {return(notes.F);}
  if(key == keys.F_s) {return(notes.F_s);}
  if(key == keys.G) {return(notes.G);}
  if(key == keys.G_s) {return(notes.G_s);}
  if(key == keys.A) {return(notes.A);}
  if(key == keys.A_s) {return(notes.A_s);}
  if(key == keys.B) {return(notes.B);}
  if(key == keys.C2) {return(notes.C2);}
}

void servo_ctrl(int action) {
  if( action == 0) {      // unlock
    state.shackle_locked = 0;
    servo.write(45);
  }
  else if (action == 1) {  //lock
    state.shackle_locked = 1;
    servo.write(0);
  }
}

void checkShackle() {
  if(digitalRead(misc.shackle_sense) && !state.unlockedShackleStillIn) {
    lock();
  }
  else if(!digitalRead(misc.shackle_sense)) {
    state.unlockedShackleStillIn = false; //user pulled out shackle.
  }
}

void ledOn() {
  digitalWrite(misc.led, HIGH);
  state.ledState = 1;
}

void ledOff() {
  digitalWrite(misc.led, LOW);
  state.ledState = 0;
}

void ledBlink() {

  unsigned long currentMillis = millis();
    if (currentMillis - state.blinkPreviousMillis >= ledBlinkInterval) {
      state.blinkPreviousMillis = currentMillis;
      if (state.ledState == 1) {
        ledOff(); 
        state.breakBlink--;
      } else if (state.ledState == 0) {
        ledOn();
        state.breakBlink--;
      }
  }
}

void ledBlinkTick() {
  if(state.breakBlink > 0) {
    ledBlink();
  }
  else {
    ledOff(); // just make sure the led is shut off at the end
  }
}

void startLedBlink() {
  state.breakBlink = ledBlinkTimes;
}

void unlock() {
  servo_ctrl(0);
  state.unlockedShackleStillIn = true;  // now wait for user to pull out shackle
}

void lock() {
  servo_ctrl(1);
}

void checkKeys() {  //checks button input

  if(state.mode == 0) {

    if(digitalRead(misc.master_button) == LOW) {
      // master button is being pushed, user wants to enter this combo
      if(!state.masterButtonBreak) {
        int correctNotes = 0;
        int comboIndex;
        for(comboIndex = 0; comboIndex<state.enteredComboLength; comboIndex++) {
          
          if(state.entered_combo[comboIndex] == state.combo[comboIndex]) {
            //combo match
            correctNotes++;
          }
        }

        if((comboIndex == correctNotes) && (state.enteredComboLength == (sizeof(state.combo)/sizeof(int)))) {
          //correct combo
          ledBlinkInterval = 30;
          ledBlinkTimes = 20;
          startLedBlink();
          unlock();
        }
        else {
          //incorrect combo
          ledBlinkInterval = 100;
          ledBlinkTimes = 8;
          startLedBlink();
        }


        //reset combo (will be overwritten)
        state.enteredComboLength = 0;

        state.masterButtonBreak = true; // enable button break

        delay(50);  // needed because button appears to oscillate
      }
    }
    else {
      // master button went high, turn off button break
      state.masterButtonBreak = false;
    }

    int highButtons = 0;
    //check all keys
    for(int keyIndex = 0; keyIndex<(sizeof(keyArray)/sizeof(int)); keyIndex++) {
      int key = keyArray[keyIndex];
      if(digitalRead(key) == LOW) {

        if(!state.keyButtonBreak) {
          // that key is being pushed, add it to the combo

          //now register the button into the entered combination arrays.
          state.entered_combo[state.enteredComboLength] = key;
          state.enteredComboLength++;

          tone(misc.speaker, fetchAssociatedNote(key), 300);

          state.keyButtonBreak = true; // enable button break for all keys

          delay(50);  // needed because button appears to oscillate
        }
      }
      else {
        // a button went high, only disable button break if all buttons went high.
        highButtons++;
      }
    }

    if(highButtons == sizeof(keyArray)/sizeof(int)) {
      // all buttons were high, disable button break
      state.keyButtonBreak = false;
    }
  }
}

void setup() {
  pinSetup();
  unlock();
  startLedBlink();
}

void loop() { 

  checkShackle();
  checkKeys();
  ledBlinkTick();

}