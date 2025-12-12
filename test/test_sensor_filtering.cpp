#include <Arduino.h>

// Pin definitions
const int TRIG_RIGHT = 1;
const int ECHO_RIGHT = 2;
const int TRIG_HEIGHT = 3;
const int ECHO_HEIGHT = 4;

// Sensor filtering
const int FILTER_SAMPLES = 3;

// ========== HELPER FUNCTIONS ==========

float readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  
  if (duration == 0) {
    return -1.0;  // Timeout
  }
  
  return duration / 58.0;
}

float medianFilter(float *readings, int n) {
  // Create temporary array and copy values
  float temp[n];
  for (int i = 0; i < n; i++) {
    temp[i] = readings[i];
  }
  
  // Bubble sort
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (temp[j] > temp[j + 1]) {
        float swap = temp[j];
        temp[j] = temp[j + 1];
        temp[j + 1] = swap;
      }
    }
  }
  
  // Return median
  return temp[n / 2];
}

float getFilteredDistance(int trigPin, int echoPin) {
  float readings[FILTER_SAMPLES];
  
  // Take FILTER_SAMPLES readings
  for (int i = 0; i < FILTER_SAMPLES; i++) {
    readings[i] = readUltrasonic(trigPin, echoPin);
    if (i < FILTER_SAMPLES - 1) {
      delay(15);
    }
  }
  
  // Apply median filter
  return medianFilter(readings, FILTER_SAMPLES);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_HEIGHT, OUTPUT);
  pinMode(ECHO_HEIGHT, INPUT);
  
  digitalWrite(TRIG_RIGHT, LOW);
  digitalWrite(TRIG_HEIGHT, LOW);
  
  Serial.println("========================================");
  Serial.println("SENSOR FILTERING TEST");
  Serial.println("========================================");
  Serial.println("This test demonstrates median filtering");
  Serial.println("reducing noise in sensor readings.");
  Serial.println("Time(ms),Raw_Left,Raw_Height,Filtered_Left,Filtered_Height");
  delay(1000);
}

void loop() {
  unsigned long loopTime = millis();
  
  // Read raw values (single readings)
  float rawLeft = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);
  delay(50);
  float rawHeight = readUltrasonic(TRIG_HEIGHT, ECHO_HEIGHT);
  delay(50);
  
  // Read filtered values
  float filteredLeft = getFilteredDistance(TRIG_RIGHT, ECHO_RIGHT);
  delay(50);
  float filteredHeight = getFilteredDistance(TRIG_HEIGHT, ECHO_HEIGHT);
  
  // Print CSV data
  Serial.print(loopTime);
  Serial.print(",");
  Serial.print(rawLeft, 2);
  Serial.print(",");
  Serial.print(rawHeight, 2);
  Serial.print(",");
  Serial.print(filteredLeft, 2);
  Serial.print(",");
  Serial.println(filteredHeight, 2);
  
  delay(200);
}
