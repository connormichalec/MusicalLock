#include <Servo.h>

Servo servo;

const int ledInterval = 100;

using note = float;


struct Keys {
  int C = 4;
  int C_s = 5;
  int D = 6;
  int D_s = 7;
  int E = 8;
  int F = 9;
  int F_s = 10;
  int G = A0;
  int G_s = A1;
  int A = A2;
  int A_s = A3;
  int B = A4;
  int C2 = A5;
};

struct Misc {
  int speaker = 10;
  int led = 12;
  int servo = 11;
  int shackle_sense = 13;
  int master_button = 2; //maser button
};

struct Notes {
  note C = 261.63*2;
  note C_s = 277.18*2;
  note D = 293.66*2;
  note D_s = 311.13*2;
  note E = 329.63*2;
  note F = 349.23*2;
  note F_s = 369.99*2;
  note G = 392.00*2;
  note G_s = 415.30*2;
  note A = 440.00*2;
  note A_s = 466.16*2;
  note B = 493.88*2;
  note C2 = 523.25*2;
};

Misc misc = Misc();
Keys keys = Keys();
Notes notes = Notes();

int keyArray[13] = {keys.C,keys.C_s,keys.D,keys.D_s,keys.E,keys.F,keys.F_s,keys.G,keys.G_s,keys.A,keys.A_s,keys.B,keys.C2};

struct State {
  int mode = 0; // 0=Reproduce notes, 1=Perfect pitch
  int locked = 0; //0=locked, 1=not locked
  int ledState = 0; //0 = led off, 
  long buttonTimeout = 0; //used to make sure no double inputs for the button being held down.
  int enteredComboLength = 0; // length of combo already entered

  unsigned int breakBlink = 9;           // dont start off blinking
  unsigned int breakBlinkThreshold = 8; //make even number to stop at off
  unsigned int blinkPreviousMillis = 0;
  note combo[3] = {notes.C, notes.D, notes.E};
  note entered_combo[100];
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
    state.locked = 0;
  }
  else if (action == 1) {  //lock 
    state.locked = 1;
  }
}

void setup() {
  pinSetup();
}

void checkShackle() {
  if(digitalRead(misc.shackle_sense)) {
    servo_ctrl(1);
  }
  else {
    servo_ctrl(0);
  }
}

void checkKeys() {  //checks button input
  if(state.mode == 0) {

    state.buttonTimeout++;

    if(!digitalRead(misc.master_button)) {
      // master button is being pushed, user wants to enter this combo


      //reset combo (will be overwritten)
      state.enteredComboLength = 0;
    }

    //check all keys
    for(int keyIndex = 0; keyIndex<sizeof(keyArray); keyIndex++) {
      int key = keyArray[keyIndex];
      if(!digitalRead(key)) {
        // that key is being pushed, add it to the combo, ONLY if the timeout
        if(state.buttonTimeout >= 1000) { //wait time
          state.buttonTimeout = 0; //reset the timeout

          //now register the button into the entered combination arrays.
          state.entered_combo[state.enteredComboLength] = fetchAssociatedNote(key);
          state.enteredComboLength++;
          
        }
      }
    }
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
    if (currentMillis - state.blinkPreviousMillis >= ledInterval) {
      state.blinkPreviousMillis = currentMillis;
      if (state.ledState == 1) {
        ledOff(); 
        state.breakBlink++;
      } else if (state.ledState == 0) {
        ledOn();
        state.breakBlink++;
      }
  }
}

void ledTick() {
  if(state.breakBlink <= state.breakBlinkThreshold) {
    ledBlink();
  }
}

void startLedBlink() {
  state.breakBlink = 0;
}

void loop() { 
  checkShackle();
  checkKeys();

  ledTick();

}