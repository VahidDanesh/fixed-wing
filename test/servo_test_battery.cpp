#include <Arduino.h>
#include <Servo.h>

const int SERVO_RUDDER_PIN = 2;
const int SERVO_ELEVATOR_PIN = 1;

Servo rudderServo;
Servo elevatorServo;

const int SERVO_RUDDER_NEUTRAL   = 1700;
const int SERVO_RUDDER_MAX       = 900;

const int SERVO_ELEVATOR_NEUTRAL = 1100;
const int SERVO_ELEVATOR_MAX     = 2100;

// Test timing (in milliseconds)
const unsigned long HOLD_TIME = 2000;  // Hold each position for 2 seconds
const unsigned long DELAY_START = 3000; // Wait 3 seconds before starting

void setup() {
  Serial.begin(115200);
  
  rudderServo.attach(SERVO_RUDDER_PIN);
  elevatorServo.attach(SERVO_ELEVATOR_PIN);
  
  // Start at neutral
  rudderServo.writeMicroseconds(SERVO_RUDDER_NEUTRAL);
  elevatorServo.writeMicroseconds(SERVO_ELEVATOR_NEUTRAL);
  
  Serial.println("=== SERVO TEST MODE ===");
  Serial.println("Starting in 3 seconds...");
  Serial.println("Sequence: Neutral -> Max -> Neutral (repeating)");
  
  delay(DELAY_START);
  Serial.println("Test started!");
}

void loop() {
  // Move to MAX
  Serial.println("Position: MAX");
  rudderServo.writeMicroseconds(SERVO_RUDDER_MAX);
  elevatorServo.writeMicroseconds(SERVO_ELEVATOR_MAX);
  delay(HOLD_TIME);
  
  // Return to NEUTRAL
  Serial.println("Position: NEUTRAL");
  rudderServo.writeMicroseconds(SERVO_RUDDER_NEUTRAL);
  elevatorServo.writeMicroseconds(SERVO_ELEVATOR_NEUTRAL);
  delay(HOLD_TIME);
}