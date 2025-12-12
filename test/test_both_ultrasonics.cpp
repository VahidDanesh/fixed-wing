#include <Arduino.h>

// Pin definitions
const int TRIG_RIGHT = 6;
const int ECHO_RIGHT = 5;
const int TRIG_HEIGHT = 4;
const int ECHO_HEIGHT = 3;

float readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration / 58.0;  // Convert to cm
}

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_HEIGHT, OUTPUT);
  pinMode(ECHO_HEIGHT, INPUT);
  
  Serial.println("Both Ultrasonic Sensors Test");
  delay(1000);
}

void loop() {
  // Read left wall sensor
  float leftDistance = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);
  delay(50);  // Small delay between readings
  
  // Read height sensor
  float height = readUltrasonic(TRIG_HEIGHT, ECHO_HEIGHT);
  
  Serial.print("Left: ");
  Serial.print(leftDistance);
  Serial.print(" cm | Height: ");
  Serial.print(height);
  Serial.println(" cm");
  
  delay(100);
}