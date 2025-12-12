#include <Arduino.h>

// Pin definitions
const int TRIG_HEIGHT = 3;  // D3
const int ECHO_HEIGHT = 4;  // D4

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_HEIGHT, OUTPUT);
  pinMode(ECHO_HEIGHT, INPUT);
  
  Serial.println("Ultrasonic Height Sensor Test");
  delay(1000);
}

void loop() {
  // Send trigger pulse
  digitalWrite(TRIG_HEIGHT, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_HEIGHT, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_HEIGHT, LOW);
  
  // Read echo pulse
  long duration = pulseIn(ECHO_HEIGHT, HIGH, 30000);
  
  // Calculate distance
  float height_cm = duration / 58.0;
  
  Serial.print("Height: ");
  Serial.print(height_cm);
  Serial.println(" cm");
  
  delay(100);
}