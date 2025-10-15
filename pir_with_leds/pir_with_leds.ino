const int pirPin = 9;
const int ledPin = 8;

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.println("Sensor warming up...");
  delay(10000);
  Serial.println("Ready!");
}

void loop() {
  int motion = digitalRead(pirPin);
  
  if (motion) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Motion detected!");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("No motion.");
  }
  delay(1000);
}
