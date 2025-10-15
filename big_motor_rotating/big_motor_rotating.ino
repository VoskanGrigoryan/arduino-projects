#include <Stepper.h>

int rpm = 10;
int stepsPerRevolution = 2048;  //steps to full 360-degree rotation, change according the motor type

//Pin order IN1, IN3, IN2, IN4
Stepper MyStepper(stepsPerRevolution, 8, 10, 9, 11);

  void setup() {
  // put your setup code here, to run once:

  MyStepper.setSpeed(rpm);
  }

void loop() {
  // put your main code here, to run repeatedly:

  MyStepper.step(stepsPerRevolution);
  delay(500);

  MyStepper.step(-stepsPerRevolution);
  delay(500);
}
