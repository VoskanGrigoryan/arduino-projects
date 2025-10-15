#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySerial(10, 9); 
DFRobotDFPlayerMini myDF;

int currentTrack = 1;          
const int folderNumber = 2;    // set to 0 for root folder
const int maxTracks = 40;      // number of files in folder/root
unsigned long lastCheck = 0;   // for timing check
const unsigned long checkInterval = 2000; // ms

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  if (myDF.begin(mySerial)) {
    delay(100);                
    Serial.println("DFPlayer OK");
    myDF.volume(16);           
    if (folderNumber > 0) myDF.playFolder(folderNumber, currentTrack);
    else myDF.play(currentTrack);
  } else {
    Serial.println("DFPlayer NOT found");
  }
}

void loop() {
  if (myDF.available()) {
    uint8_t type = myDF.readType();
    if (type == DFPlayerPlayFinished) {
      nextTrack();
    } else if (type == DFPlayerError) {
      Serial.println("DFPlayer error detected, skipping track");
      nextTrack();
    }
  }

  // Safety check: if no track is playing for more than checkInterval, restart
  if (millis() - lastCheck > checkInterval) {
    lastCheck = millis();
    if (!myDF.isPlaying()) {
      Serial.println("Playback stalled, restarting next track");
      nextTrack();
    }
  }
}

void nextTrack() {
  currentTrack++;
  if (currentTrack > maxTracks) currentTrack = 1;  

  Serial.print("Playing track: ");
  Serial.println(currentTrack);

  if (folderNumber > 0) myDF.playFolder(folderNumber, currentTrack);
  else myDF.play(currentTrack);
}
