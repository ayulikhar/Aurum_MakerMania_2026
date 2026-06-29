// Define the pin connected to the Laser Signal (S) pin
// On ESP8266 NodeMCU, D1 maps to GPIO 5
const int LASER_PIN = D1; 

void setup() {
  // Initialize the laser pin as an output
  pinMode(LASER_PIN, OUTPUT);
}

void loop() {
  // Turn the laser ON
  digitalWrite(LASER_PIN, HIGH);
  delay(1000); // Wait for 1 second

  // Turn the laser OFF
  digitalWrite(LASER_PIN, LOW);
  delay(1000); // Wait for 1 second
}