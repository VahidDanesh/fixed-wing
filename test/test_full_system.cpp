#include <Arduino.h>
#include <Servo.h>

// Pin definitions
const int TRIG_RIGHT = 1;
const int ECHO_RIGHT = 2;
const int TRIG_HEIGHT = 3;
const int ECHO_HEIGHT = 4;
const int SERVO_RUDDER_PIN = 5;
const int SERVO_ELEVATOR_PIN = 6;

Servo rudderServo;
Servo elevatorServo;

float readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration / 58.0;
}

void setup() {
  Serial.begin(115200);
  
  // Initialize ultrasonic sensors
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_HEIGHT, OUTPUT);
  pinMode(ECHO_HEIGHT, INPUT);
  
  // Initialize servos
  rudderServo.attach(SERVO_RUDDER_PIN);
  elevatorServo.attach(SERVO_ELEVATOR_PIN);
  
  // Set servos to neutral
  rudderServo.write(90);
  elevatorServo.write(90);
  
  Serial.println("Full System Integration Test");
  delay(1000);
}

void loop() {
  // Read sensors
  float leftDistance = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);
  delay(50);
  float height = readUltrasonic(TRIG_HEIGHT, ECHO_HEIGHT);
  
  // Simple test: move rudder based on left distance
  // If wall is close (<30cm), turn away (right)
  // If wall is far (>60cm), turn toward it (left)
  int rudderAngle = 90;  // neutral
  if(leftDistance < 30) {
    rudderAngle = 110;  // Turn right
    Serial.println("AVOIDING LEFT WALL - Turn Right");
  } else if(leftDistance > 60) {
    rudderAngle = 70;  // Turn left
    Serial.println("TOO FAR FROM LEFT - Turn Left");
  }
  
  // Simple test: maintain altitude around 100cm
  int elevatorAngle = 90;  // neutral
  if(height < 80) {
    elevatorAngle = 100;  // Pitch up
    Serial.println("TOO LOW - Pitch Up");
  } else if(height > 120) {
    elevatorAngle = 80;  // Pitch down
    Serial.println("TOO HIGH - Pitch Down");
  }
  
  // Apply servo commands
  rudderServo.write(rudderAngle);
  elevatorServo.write(elevatorAngle);
  
  // Debug output
  Serial.print("Left: ");
  Serial.print(leftDistance);
  Serial.print(" cm | Height: ");
  Serial.print(height);
  Serial.print(" cm | Rudder: ");
  Serial.print(rudderAngle);
  Serial.print("° | Elevator: ");
  Serial.print(elevatorAngle);
  Serial.println("°");
  
  delay(100);
}