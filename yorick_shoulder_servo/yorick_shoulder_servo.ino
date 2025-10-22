#include <Servo.h>

Servo myServo;
int servoPin = 2;

void setup() {
  myServo.attach(servoPin);
  myServo.write(30);  // start near center
  delay(2000);
}

void loop() {
  // move from 20 to 40 (±10° around 30)
  for (int pos = 40; pos >= 20; pos--) {
    myServo.write(pos);
    delay(20);
  }

  for (int pos = 20; pos <= 40; pos++) {
    myServo.write(pos);
    delay(20);
  }
}
