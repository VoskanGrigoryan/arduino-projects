#include <Servo.h>

Servo myServo;
int servoPin = 4;

void setup() {
  myServo.attach(servoPin);
  myServo.write(44);
  delay(500);
}

void loop() {
  for (int pos = 0; pos <= 44; pos += 1) {
    myServo.write(pos);
    delay(10);
  }

  for (int pos = 44; pos >= 0; pos -= 1) {
    myServo.write(pos);
    delay(10);
  }
}
