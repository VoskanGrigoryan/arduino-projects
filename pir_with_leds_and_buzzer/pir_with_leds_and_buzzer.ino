#include <Servo.h>

const int pirPin = 9;      // PIR sensor output pin
const int ledPin = 8;      // LED pin
const int buzzerPin = 13;  // Passive buzzer pin
const int servoPin = 12;   // Servo control pin


bool motionPreviouslyDetected = false;  // tracks previous state
Servo jawServo;                         // servo object

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  jawServo.attach(servoPin);
  jawServo.write(20);  // jaw starts closed

  Serial.println("Sensor warming up...");
  // delay(10000);  // allow PIR to stabilize
  Serial.println("Ready!");

  randomSeed(analogRead(A0));
}

void screamWithJaw(int repetitions = 3) {
  // Synchronize jaw movement with sound
  for (int rep = 0; rep < repetitions; rep++) {
    // Open jaw while rising tone
    for (int pos = 20; pos >= 0; pos -= 2) {
      jawServo.write(pos);
      int freq = map(pos, 20, 0, 150, 800);  // map jaw position to frequency
      tone(buzzerPin, freq);
      delay(5);
    }

    // Close jaw while rough vibrate
    for (int pos = 0; pos <= 20; pos += 2) {
      jawServo.write(pos);
      int freq = 700 + random(-80, 80);  // low-mid pitch roughness
      tone(buzzerPin, freq);
      delay(10);
    }
  }

  noTone(buzzerPin);  // <--- stop the buzzer when done
}

// removed because of timing issues & motion detection
// void longScreamWithJaw() {
//   //Opens the mouth for a prolongated sound (scream mimic)
//   for (int pos = 20; pos >= 0; pos -= 2) {
//     jawServo.write(pos);
//     int freq(80);
//     tone(buzzerPin, freq);
//     delay(50);
//   }

//   for (int pos = 0; pos <= 20; pos += 2) {
//     jawServo.write(pos);
//     noTone(buzzerPin);
//   }
// }

void loop() {
  int motion = digitalRead(pirPin);

  if (motion && !motionPreviouslyDetected) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Motion detected!");

    // if (random(0, 2) == 0) {
    //   screamWithJaw(3);  // move jaw and play sound together 3 times
    // } else {
    //   longScreamWithJaw();
    // }

    screamWithJaw(3);

    digitalWrite(ledPin, LOW);

    motionPreviouslyDetected = true;  // remember motion is active
  }

  if (!motion) {
    digitalWrite(ledPin, LOW);
    if (motionPreviouslyDetected) {
      Serial.println("Motion ended.");
    }
    motionPreviouslyDetected = false;
  }

  delay(10);  // stability
}
