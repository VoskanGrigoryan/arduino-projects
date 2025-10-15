#include <Servo.h>

long duration;
long distance;

Servo myservo;

int pos = 0;
int direction = 1;               // 1 = forward, -1 = backward
const int trigPin = 8;
const int echoPin = 13;
const int ledPin = 12;           // LED connected to pin 12

void setup() {
  myservo.attach(9);             // Servo on pin 9

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}

long measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  // --- Move servo faster ---
  pos += direction * 1;  // increase step size to 5 degrees per loop
  if (pos >= 180) { 
    pos = 180;
    direction = -1;
  }
  if (pos <= 0) {
    pos = 0;
    direction = 1;
  }
  myservo.write(pos);

  // --- Measure distance ---
  distance = measureDistance();
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- LED control ---
  if (distance <= 15 && distance > 0) {
    digitalWrite(ledPin, HIGH);
    delay(5);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(5); // small delay for stability
}
