#include <Servo.h>
#include <EEPROM.h>       // For saving mode state
#include "pitches.h"      // Make sure this file is in the same folder

// --- Pins ---

const int modeLedTwo = 4;   // LED for Scream mode
const int modeLedOne = 5;   // LED for Pirates mode
const int buttonPin = 7;    // Button to switch mode
const int ledPin = 8;       // Status LED
const int pirPin = 9;       // PIR sensor
const int servoPin = 12;    // Servo control pin
const int buzzerPin = 13;   // Buzzer pin

// --- State variables ---
bool motionPreviouslyDetected = false;
bool lastButtonState = HIGH;   // for button debouncing
int mode = 1;                  // 1 = Pirates, 2 = Scream
Servo jawServo;

// --------------------- Pirates of the Caribbean ---------------------
int melody[] = {
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
  NOTE_A4, NOTE_G4, NOTE_A4, REST,

  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, REST,
  NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, REST,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, REST,

  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_E5, REST,
  NOTE_E5, NOTE_F5, NOTE_G5, NOTE_G5, REST,
  NOTE_F5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4
};

int durations[] = {
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8,

  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,

  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 8, 8, 8, 4
};

// --------------------- Setup ---------------------
void setup() {
  Serial.begin(9600);

  pinMode(modeLedOne, OUTPUT);
  pinMode(modeLedTwo, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // important: internal pull-up
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  jawServo.attach(servoPin);
  jawServo.write(20);

  randomSeed(analogRead(A0));

  // --- Load saved mode from EEPROM ---
  mode = EEPROM.read(0);
  if (mode != 1 && mode != 2) {
    mode = 1; // default
    EEPROM.write(0, mode);
  }

  updateModeLeds();

  Serial.println("Sensor warming up...");
  Serial.print("Initial mode: ");
  Serial.println(mode == 1 ? "Pirates" : "Scream");
  Serial.println("Ready!");
}

// --------------------- Update LEDs ---------------------
void updateModeLeds() {
  digitalWrite(modeLedOne, mode == 1 ? HIGH : LOW);
  digitalWrite(modeLedTwo, mode == 2 ? HIGH : LOW);
}

// --------------------- Pirates song ---------------------
void playPiratesSongWithJaw() {
  int size = sizeof(durations) / sizeof(int);
  int minNote = 2000;
  int maxNote = 0;

  for (int i = 0; i < size; i++) {
    if (melody[i] != REST) {
      if (melody[i] < minNote) minNote = melody[i];
      if (melody[i] > maxNote) maxNote = melody[i];
    }
  }

  for (int note = 0; note < size; note++) {
    int duration = 1000 / durations[note];

    if (melody[note] == REST) {
      noTone(buzzerPin);
      jawServo.write(20);
    } else {
      tone(buzzerPin, melody[note], duration);
      int jawPos = map(melody[note], minNote, maxNote, 20, 0);
      jawServo.write(jawPos);
    }

    int pauseBetweenNotes = duration * 1.3;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }

  jawServo.write(20);
}

// --------------------- Scream ---------------------
void screamWithJaw(int repetitions = 3) {
  for (int rep = 0; rep < repetitions; rep++) {
    for (int pos = 20; pos >= 0; pos -= 2) {
      jawServo.write(pos);
      int freq = map(pos, 20, 0, 150, 800);
      tone(buzzerPin, freq);
      delay(5);
    }

    for (int pos = 0; pos <= 20; pos += 2) {
      jawServo.write(pos);
      int freq = 700 + random(-80, 80);
      tone(buzzerPin, freq);
      delay(10);
    }
  }

  noTone(buzzerPin);
  jawServo.write(20);
}

// --------------------- Handle Button Toggle ---------------------
void handleButtonPress() {
  bool buttonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && buttonState == LOW) {
    Serial.println("Button pressed");  // <-- added log

    // Toggle mode
    mode = (mode == 1) ? 2 : 1;

    // Save mode permanently
    EEPROM.write(0, mode);

    Serial.print("Mode changed to: ");
    Serial.println(mode == 1 ? "Pirates" : "Scream");

    updateModeLeds();
    delay(200); // debounce
  }

  lastButtonState = buttonState;
}

// --------------------- Main Loop ---------------------
void loop() {
  handleButtonPress();

  int motion = digitalRead(pirPin);

  if (motion && !motionPreviouslyDetected) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Motion detected!");

    if (mode == 1) {
      playPiratesSongWithJaw();
    } else {
      screamWithJaw(3);
    }

    digitalWrite(ledPin, LOW);
    motionPreviouslyDetected = true;
  }

  if (!motion) {
    digitalWrite(ledPin, LOW);
    if (motionPreviouslyDetected) {
      Serial.println("Motion ended.");
    }
    motionPreviouslyDetected = false;
  }

  delay(10);
}
