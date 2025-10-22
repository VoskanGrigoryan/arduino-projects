#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <IRremote.hpp>
#include <Servo.h>

#define IR_RECEIVE_PIN 7

// --- DFPlayer ---
SoftwareSerial mySerial(10, 9);
DFRobotDFPlayerMini myDF;
int currentTrack = 1;
int currentFolder = 1;
const int maxTracks = 40;
bool isPlaying = false;

// --- Volume ---
int volume = 16;

// --- IR Remote ---
uint8_t lastCommand = 0;
unsigned long lastTime = 0;
const unsigned long debounceDelay = 250;
const unsigned long fileSelectTimeout = 1000;
bool folderMode = false;
uint8_t folderSelected = 0;
uint8_t fileFirstDigit = 0;
unsigned long fileDigitTime = 0;
bool waitingSecondDigit = false;

// --- Jaw Servo ---
Servo jawServo;
int jawPin = 4;
int jawRest = 30;  // jaw closed
int jawOpen = 0;   // jaw open while playing
int lastJawPos = -1; // for non-blocking updates

// --- Eye LEDs ---
int led1 = 5;
int led2 = 6;
bool blinkState = false;
unsigned long blinkTime = 0;
unsigned long nextBlink = 0;
const unsigned long blinkDuration = 3000;

// --- Eye Servo ---
Servo eyeServo;
int eyeServoPin = 3;
int eyePos = 0;
int eyeDirection = 1;
const int eyeMin = 0;
const int eyeMax = 80;
const unsigned long eyeStepDelay = 30;
unsigned long lastEyeMove = 0;

// --- Additional Servo ---
Servo addServo;
int addServoPin = 2;
int addPos = 30; // start near center
int addDirection = 1;
const int addMin = 20;
const int addMax = 40;
const unsigned long addStepDelay = 20;
unsigned long lastAddMove = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  // DFPlayer init
  if (myDF.begin(mySerial)) {
    delay(100);
    Serial.println("DFPlayer OK");
    myDF.volume(volume);
  } else {
    Serial.println("DFPlayer NOT found");
  }

  // IR init
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  // Jaw servo
  jawServo.attach(jawPin);
  jawServo.write(jawRest);

  // Eye servo
  eyeServo.attach(eyeServoPin);
  eyeServo.write(eyePos);

  // Additional servo
  addServo.attach(addServoPin);
  addServo.write(addPos);

  // LEDs
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  // schedule first idle blink
  nextBlink = millis() + random(0, 60000); // 0–10 min
}

void loop() {
  unsigned long now = millis();

  // --- Check if DFPlayer finished playing ---
  if (myDF.available()) {
    uint8_t type = myDF.readType();
    int value = myDF.read();
    if (type == DFPlayerPlayFinished) {
      Serial.println("Track finished");
      isPlaying = false;
    }
  }

  // --- IR Remote handling ---
  if (IrReceiver.decode()) {
    uint8_t cmd = IrReceiver.decodedIRData.command;
    if (cmd != 0 && (cmd != lastCommand || (now - lastTime) > debounceDelay)) {
      lastCommand = cmd;
      lastTime = now;

      if (cmd == 70) { // CH button
        folderMode = true;
        waitingSecondDigit = false;
        Serial.println("Folder selection mode ON");
      }
      else if (cmd == 67) { // PLAY/PAUSE
        if (isPlaying) {
          myDF.pause();
          isPlaying = false;
          Serial.println("Playback PAUSED");
        } else {
          myDF.start();
          isPlaying = true;
          Serial.println("Playback RESUMED");
        }
      }
      else if (cmd == 21) { // VOL+
        volume++;
        if (volume > 30) volume = 30;
        myDF.volume(volume);
        Serial.print("Volume increased to: "); Serial.println(volume);
      }
      else if (cmd == 7) { // VOL-
        volume--;
        if (volume < 0) volume = 0;
        myDF.volume(volume);
        Serial.print("Volume decreased to: "); Serial.println(volume);
      }
      else if (cmd == 64) nextTrack();
      else if (cmd == 68) prevTrack();

      // numeric buttons 0-9
      else if (cmd == 22 || cmd == 12 || cmd == 24 || cmd == 94 || cmd == 8 || cmd == 28 || cmd == 90 || cmd == 66 || cmd == 82 || cmd == 74) {
        uint8_t digit = mapIRToDigit(cmd);
        if (folderMode && !waitingSecondDigit) {
          folderSelected = digit;
          waitingSecondDigit = true;
          fileFirstDigit = 0;
          fileDigitTime = now;
          Serial.print("Folder selected: "); Serial.println(folderSelected);
        } else if (waitingSecondDigit) {
          if (fileFirstDigit == 0) {
            fileFirstDigit = digit;
            fileDigitTime = now;
            Serial.print("First file digit: "); Serial.println(fileFirstDigit);
          } else {
            uint8_t fileSelected = fileFirstDigit * 10 + digit;
            Serial.print("File selected: "); Serial.println(fileSelected);
            playFolderTrack(folderSelected, fileSelected);
            waitingSecondDigit = false;
            folderMode = false;
          }
        }
      }
    }
    IrReceiver.resume();
  }

  // handle timeout for single-digit file
  if (waitingSecondDigit && fileFirstDigit != 0 && (now - fileDigitTime > fileSelectTimeout)) {
    Serial.print("File selected (single digit): "); Serial.println(fileFirstDigit);
    playFolderTrack(folderSelected, fileFirstDigit);
    waitingSecondDigit = false;
    folderMode = false;
  }

  // --- Jaw logic (non-blocking) ---
  int targetJaw = isPlaying ? jawOpen : jawRest;
  if (targetJaw != lastJawPos) {
    jawServo.write(targetJaw);
    lastJawPos = targetJaw;
  }

  // --- Eye servo sweep ---
  if (isPlaying) {
    if (now - lastEyeMove >= eyeStepDelay) {
      eyePos += eyeDirection;
      if (eyePos >= eyeMax) { eyePos = eyeMax; eyeDirection = -1; }
      else if (eyePos <= eyeMin) { eyePos = eyeMin; eyeDirection = 1; }
      eyeServo.write(eyePos);
      lastEyeMove = now;
    }
  } else if (eyePos != 0) { // return to center when idle
    eyePos = 0;
    eyeServo.write(eyePos);
    eyeDirection = 1;
  }

  // --- Additional servo sweep ---
  if (isPlaying) {
    if (now - lastAddMove >= addStepDelay) {
      addPos += addDirection;
      if (addPos >= addMax) { addPos = addMax; addDirection = -1; }
      else if (addPos <= addMin) { addPos = addMin; addDirection = 1; }
      addServo.write(addPos);
      lastAddMove = now;
    }
  } else if (addPos != 30) { // return near center
    addPos = 30;
    addServo.write(addPos);
    addDirection = 1;
  }

  // --- Eyes LEDs logic ---
  if (isPlaying) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  } else {
    if (blinkState && now - blinkTime > blinkDuration) {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      blinkState = false;
      nextBlink = now + random(0, 600000); // schedule next blink 0–10 min
    } else if (!blinkState && now >= nextBlink) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      blinkState = true;
      blinkTime = now;
    }
  }

  delay(20); // small delay for smoother servo timing
}

// --- Helpers ---
uint8_t mapIRToDigit(uint8_t cmd) {
  switch (cmd) {
    case 22: return 0;
    case 12: return 1;
    case 24: return 2;
    case 94: return 3;
    case 8:  return 4;
    case 28: return 5;
    case 90: return 6;
    case 66: return 7;
    case 82: return 8;
    case 74: return 9;
    default: return 0xFF;
  }
}

void playFolderTrack(uint8_t folder, uint8_t track) {
  currentFolder = folder;
  currentTrack = track;
  Serial.print(">>> PLAYING folder "); Serial.print(folder);
  Serial.print(", file "); Serial.println(track);
  myDF.playFolder(folder, track);
  isPlaying = true;
}

void nextTrack() {
  if (currentTrack < maxTracks) {
    currentTrack++;
    Serial.print("Next track: "); Serial.println(currentTrack);
    myDF.playFolder(currentFolder, currentTrack);
    isPlaying = true;
  } else {
    Serial.println("Already at last track, cannot skip forward");
  }
}

void prevTrack() {
  if (currentTrack > 1) {
    currentTrack--;
    Serial.print("Previous track: "); Serial.println(currentTrack);
    myDF.playFolder(currentFolder, currentTrack);
    isPlaying = true;
  } else {
    Serial.println("Already at first track, cannot skip backward");
  }
}
