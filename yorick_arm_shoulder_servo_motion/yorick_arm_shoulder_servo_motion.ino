#include <Servo.h>

Servo myServo;     // head
Servo myServoJaw;  // jaw

int servoPin = 3;
int servoJawPin = 4;
int center = 50;

int headPos = 50;
int headDir = 1;
unsigned long lastHeadMove = 0;
const int headInterval = 30;  // ms between head steps

int jawPos = 74;
int jawDir = 1;
unsigned long lastJawMove = 0;
const int jawInterval = 10;   // faster movement

void setup() {
  myServo.attach(servoPin);
  myServoJaw.attach(servoJawPin);
  myServo.write(center);
  myServoJaw.write(jawPos);
}

void loop() {
  unsigned long now = millis();

  // head motion
  if (now - lastHeadMove >= headInterval) {
    headPos += headDir;
    if (headPos >= center + 30 || headPos <= center - 30)
      headDir = -headDir;  // bounce back
    myServo.write(headPos);
    lastHeadMove = now;
  }

  // jaw motion
  if (now - lastJawMove >= jawInterval) {
    jawPos += jawDir;
    if (jawPos >= 96 || jawPos <= 74)
      jawDir = -jawDir;  // bounce back
    myServoJaw.write(jawPos);
    lastJawMove = now;
  }
}
