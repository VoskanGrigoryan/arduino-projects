// Pin assignments
const int redPin = 13;
const int greenPin = 12;
const int bluePin = 11;

// Define colors as {R, G, B} values (0-255)
int colors[][3] = {
  {255, 0, 0},    // Red
  {0, 255, 0},    // Green
  {0, 0, 255},    // Blue
  {255, 255, 0},  // Yellow
  {255, 0, 255},  // Purple
  {0, 255, 255},  // Cyan
  {255, 128, 0},  // Orange
  {128, 0, 128},  // Dark Purple
  {0, 128, 128},  // Teal
  {255, 255, 255} // White
};

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    analogWrite(redPin, colors[i][0]);
    analogWrite(greenPin, colors[i][1]);
    analogWrite(bluePin, colors[i][2]);
    delay(1000); // change color every second
  }
}
