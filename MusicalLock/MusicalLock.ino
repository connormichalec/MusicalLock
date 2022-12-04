#include <Servo.h>

Servo servo;

using note = float;
long clockCycles = 0;

struct Buttons {
  int master = 2;
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
Buttons buttons = Buttons();
Notes notes = Notes();

struct State {
  int mode = 0; // 0=Reproduce notes, 1=Perfect pitch
  int locked = 0; //0=locked, 1=not locked
  int ledState = 0; //0 = led off, 
  long ledTimer = 0; //used as a basis to compare led effect time to clockCycles
  note combo[3] = {notes.C, notes.D, notes.E};
  note entered_combo[sizeof(combo)];
};
 
State state = State();

void pinSetup() {
  pinMode(misc.speaker, OUTPUT);
  pinMode(misc.led, OUTPUT);
  servo.attach(misc.servo);
  pinMode(misc.shackle_sense, INPUT_PULLUP);
  pinMode(buttons.master, INPUT_PULLUP);
  pinMode(buttons.C, INPUT_PULLUP);
  pinMode(buttons.C_s, INPUT_PULLUP);
  pinMode(buttons.D, INPUT_PULLUP);
  pinMode(buttons.D_s, INPUT_PULLUP);
  pinMode(buttons.E, INPUT_PULLUP);
  pinMode(buttons.F, INPUT_PULLUP);
  pinMode(buttons.F_s, INPUT_PULLUP);
  pinMode(buttons.G, INPUT_PULLUP);
  pinMode(buttons.G_s, INPUT_PULLUP);
  pinMode(buttons.A, INPUT_PULLUP);
  pinMode(buttons.A_s, INPUT_PULLUP);
  pinMode(buttons.B, INPUT_PULLUP);
  pinMode(buttons.C2, INPUT_PULLUP);
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

}

void ledEffects() { //tick the LED effects based on state

}

void loop() { 
  checkShackle();
  checkKeys();
  ledEffects();   
  clockCycles++;  //used for timing effects
}