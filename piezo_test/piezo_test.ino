int piezoPin = A0;  // Connect piezo signal wire here
int val = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  val = analogRead(piezoPin);  // Read piezo vibration level
  Serial.println(val);
  delay(50);
}