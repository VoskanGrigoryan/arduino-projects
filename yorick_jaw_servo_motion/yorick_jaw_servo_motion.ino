#include <Servo.h>

Servo myServo;
int servoPin = 3;

void setup() {
  myServo.attach(servoPin);
  myServo.write(74);
  delay(500);
}

void loop() {
  // move up faster (2° steps)
  for (int pos = 70; pos <= 96; pos += 1) {
    myServo.write(pos);
  }

  // // move down faster (2° steps)
  for (int pos = 96; pos >= 74; pos -= 1) {
    myServo.write(pos);
  }
}
