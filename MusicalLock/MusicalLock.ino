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

struct Note_freq {
  float C = 261.63*2;
  float C_s = 277.18*2;
  float D = 293.66*2;
  float D_s = 311.13*2;
  float E = 329.63*2;
  float F = 349.23*2;
  float F_s = 369.99*2;
  float G = 392.00*2;
  float G_s = 415.30*2;
  float A = 440.00*2;
  float A_s = 466.16*2;
  float B = 493.88*2;
  float C2 = 523.25*2;
};

Misc misc = Misc();
Buttons buttons = Buttons();
Note_freq note_freq = Note_freq();

void pinSetup() {
  pinMode(misc.speaker, OUTPUT);
}

void setup() {
  pinSetup();
  tone(misc.speaker, note_freq.C2);
}

void loop() {    
}