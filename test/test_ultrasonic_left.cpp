#include <Arduino.h>

// Pin definitions
const int TRIG_LEFT = 1;  // D1
const int ECHO_LEFT = 2;  // D2

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  
  Serial.println("Ultrasonic Left Wall Sensor Test");
  delay(1000);
}

void loop() {
  // Send trigger pulse
  digitalWrite(TRIG_LEFT, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_LEFT, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_LEFT, LOW);
  
  // Read echo pulse
  long duration = pulseIn(ECHO_LEFT, HIGH, 30000); // 30ms timeout
  
  // Calculate distance (speed of sound = 343 m/s)
  // distance = (duration / 2) / 29.1 cm
  float distance_cm = duration / 58.0;
  
  Serial.print("Left Wall Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  
  delay(100);
}