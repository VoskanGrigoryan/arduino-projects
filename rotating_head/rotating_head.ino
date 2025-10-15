//using commands from the premade servo library
#include <Servo.h>

Servo Servo1; //Creates a Servo object

int servoPin = 9;  //Which pin is connected to ServoMotor
int potPin = A0;  //Potentiometer is exxternal device, need to declare which port will receive a signal from
int whiteLEDPin = 3;
int secondWhiteLEDPin = 4;

void setup() {
  pinMode(whiteLEDPin, OUTPUT);
  pinMode(secondWhiteLEDPin, OUTPUT);
  Servo1.attach(servoPin); //Link the name Servo1 to pin
}

void loop() {
  int reading = analogRead(potPin); //Reads value from the potentiometer
  int angle = map(reading, 0, 1023, 0, 180); ///converts 0-1023 volts to 0-180°
  Servo1.write(angle);  //Tells the servo the angle it should take

  digitalWrite(whiteLEDPin, HIGH);
  digitalWrite(secondWhiteLEDPin, HIGH);
  delay(500);

  digitalWrite(whiteLEDPin, LOW);
  digitalWrite(secondWhiteLEDPin, LOW);
  delay(500);
}
