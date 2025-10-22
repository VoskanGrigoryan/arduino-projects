#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <IRremote.hpp>
#define IR_RECEIVE_PIN 7

// --- DFPlayer ---
SoftwareSerial mySerial(10, 9);
DFRobotDFPlayerMini myDF;
int currentTrack = 1;
int currentFolder = 1;
const int maxTracks = 40;  // max tracks for safety check 
bool isPlaying = false;    // track if DFPlayer is currently playing //

// --Volume--
 int volume = 22;  // starting volume (0-30)

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
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  if (myDF.begin(mySerial)) {
    delay(100);
    Serial.println("DFPlayer OK");
    myDF.volume(volume);
  } else {
    Serial.println("DFPlayer NOT found");
  }

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  unsigned long now = millis();  // --- IR Remote handling ---

  if (IrReceiver.decode()) {
    uint8_t cmd = IrReceiver.decodedIRData.command;
    if (cmd != 0 && (cmd != lastCommand || (now - lastTime) > debounceDelay)) {
      lastCommand = cmd;
      lastTime = now;   // CH button starts folder selection
      if (cmd == 70) {  // CH
        folderMode = true;
        waitingSecondDigit = false;
        Serial.println("Folder selection mode ON");
      }

      // PLAY/PAUSE button
      else if (cmd == 67) {
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
      // VOL+ button
      else if (cmd == 21) {
        volume++;
        if (volume > 30) volume = 30;
        myDF.volume(volume);
        Serial.print("Volume increased to: ");
        Serial.println(volume);
      }

      // VOL- button
      else if (cmd == 7) {
        volume--;
        if (volume < 0) volume = 0;
        myDF.volume(volume);
        Serial.print("Volume decreased to: ");
        Serial.println(volume);
      }

      // NEXT button
      else if (cmd == 64) {
        nextTrack();
      }

      // PREV button
      else if (cmd == 68) {
        prevTrack();
      }

      // numeric buttons 0-9
      else if (cmd == 22 || cmd == 12 || cmd == 24 || cmd == 94 || cmd == 8 || cmd == 28 || cmd == 90 || cmd == 66 || cmd == 82 || cmd == 74) {
        uint8_t digit = mapIRToDigit(cmd);
        if (folderMode && !waitingSecondDigit) {
          folderSelected = digit;
          waitingSecondDigit = true;
          fileFirstDigit = 0;
          fileDigitTime = now;
          Serial.print("Folder selected: ");
          Serial.println(folderSelected);
        } else if (waitingSecondDigit) {
          if (fileFirstDigit == 0) {
            fileFirstDigit = digit;
            fileDigitTime = now;
            Serial.print("First file digit: ");
            Serial.println(fileFirstDigit);
          } else {
            uint8_t fileSelected = fileFirstDigit * 10 + digit;
            Serial.print("File selected: ");
            Serial.println(fileSelected);
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
    Serial.print("File selected (single digit): ");
    Serial.println(fileFirstDigit);
    playFolderTrack(folderSelected, fileFirstDigit);
    waitingSecondDigit = false;
    folderMode = false;
  }
}
// --- helpers ---
uint8_t mapIRToDigit(uint8_t cmd) {
  switch (cmd) {
    case 22: return 0;
    case 12: return 1;
    case 24: return 2;
    case 94: return 3;
    case 8: return 4;
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
  Serial.print(">>> PLAYING folder ");
  Serial.print(folder);
  Serial.print(", file ");
  Serial.println(track);
  myDF.playFolder(folder, track);
  isPlaying = true;
}
void nextTrack() {
  if (currentTrack < maxTracks) {
    currentTrack++;
    Serial.print("Next track: ");
    Serial.println(currentTrack);
    myDF.playFolder(currentFolder, currentTrack);
    isPlaying = true;
  } else {
    Serial.println("Already at last track, cannot skip forward");
  }
}
void prevTrack() {
  if (currentTrack > 1) {
    currentTrack--;
    Serial.print("Previous track: ");
    Serial.println(currentTrack);
    myDF.playFolder(currentFolder, currentTrack);
    isPlaying = true;
  } else {
    Serial.println("Already at first track, cannot skip backward");
  }
}