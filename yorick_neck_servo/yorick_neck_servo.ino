#include <Servo.h>

Servo myServo;
int servoPin = 3;
int eyeLED1 = 5;
int eyeLED2 = 6;

void setup() {
  myServo.attach(servoPin);

  pinMode(eyeLED1, OUTPUT);
  pinMode(eyeLED2, OUTPUT);

  digitalWrite(eyeLED1, HIGH);
  digitalWrite(eyeLED2, HIGH);
  delay(100);
  myServo.write(0);  // center
  delay(2000);
}

void loop() {
  for (int pos = 0; pos <= 80; pos++) { 
    myServo.write(pos);
    delay(30);  // slower movement
  }

  for (int pos = 80; pos >= 0; pos--) {
    myServo.write(pos);
    delay(30);
  }
}
