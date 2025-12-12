#include <Arduino.h>
#include <Servo.h>

// Pin definitions
const int SERVO_RUDDER_PIN = 5;
const int SERVO_ELEVATOR_PIN = 6;

// Control parameters
const int RUDDER_NEUTRAL = 1500;
const int RUDDER_MAX = 1700;
const int RUDDER_MIN = 1300;

const int ELEVATOR_NEUTRAL = 1500;
const int ELEVATOR_MAX = 1700;
const int ELEVATOR_MIN = 1300;

const float K_P_RUDDER = 2.0;
const float K_P_ELEVATOR = 1.5;

const float TARGET_HEIGHT = 105.0;
const float TARGET_LEFT = 152.0;

// Global variables
Servo rudderServo;
Servo elevatorServo;

int constrainMicroseconds(int value, int minVal, int maxVal) {
  return constrain(value, minVal, maxVal);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  // Attach servos
  rudderServo.attach(SERVO_RUDDER_PIN);
  elevatorServo.attach(SERVO_ELEVATOR_PIN);
  
  // Center servos
  rudderServo.writeMicroseconds(RUDDER_NEUTRAL);
  elevatorServo.writeMicroseconds(ELEVATOR_NEUTRAL);
  
  delay(1000);
  
  Serial.println("========================================");
  Serial.println("CONTROL LAW TEST");
  Serial.println("========================================");
  Serial.println("Testing proportional feedback control");
  Serial.println("with simulated sensor readings");
  Serial.println("========================================");
  Serial.println("ErrorLeft(cm),ErrorHeight(cm),CorrectRudder,CorrectElevator,RudderCmd(us),ElevatorCmd(us)");
}

void loop() {
  // Test Case 1: Positive errors (need correction right and up)
  Serial.println("\n--- Test Case 1: Positive Errors ---");
  float errorLeft = 50.0;   // 50cm too far left
  float errorHeight = 20.0;  // 20cm too high
  
  float corrRudder = K_P_RUDDER * errorLeft;
  float corrElevator = K_P_ELEVATOR * errorHeight;
  
  int rudderCmd = RUDDER_NEUTRAL + (int)corrRudder;
  int elevatorCmd = ELEVATOR_NEUTRAL + (int)corrElevator;
  
  rudderCmd = constrainMicroseconds(rudderCmd, RUDDER_MIN, RUDDER_MAX);
  elevatorCmd = constrainMicroseconds(elevatorCmd, ELEVATOR_MIN, ELEVATOR_MAX);
  
  rudderServo.writeMicroseconds(rudderCmd);
  elevatorServo.writeMicroseconds(elevatorCmd);
  
  Serial.print(errorLeft);
  Serial.print(",");
  Serial.print(errorHeight);
  Serial.print(",");
  Serial.print(corrRudder, 2);
  Serial.print(",");
  Serial.print(corrElevator, 2);
  Serial.print(",");
  Serial.print(rudderCmd);
  Serial.print(",");
  Serial.println(elevatorCmd);
  
  delay(500);
  
  // Test Case 2: Negative errors (need correction left and down)
  Serial.println("--- Test Case 2: Negative Errors ---");
  errorLeft = -40.0;  // 40cm too close to left wall
  errorHeight = -30.0; // 30cm too low
  
  corrRudder = K_P_RUDDER * errorLeft;
  corrElevator = K_P_ELEVATOR * errorHeight;
  
  rudderCmd = RUDDER_NEUTRAL + (int)corrRudder;
  elevatorCmd = ELEVATOR_NEUTRAL + (int)corrElevator;
  
  rudderCmd = constrainMicroseconds(rudderCmd, RUDDER_MIN, RUDDER_MAX);
  elevatorCmd = constrainMicroseconds(elevatorCmd, ELEVATOR_MIN, ELEVATOR_MAX);
  
  rudderServo.writeMicroseconds(rudderCmd);
  elevatorServo.writeMicroseconds(elevatorCmd);
  
  Serial.print(errorLeft);
  Serial.print(",");
  Serial.print(errorHeight);
  Serial.print(",");
  Serial.print(corrRudder, 2);
  Serial.print(",");
  Serial.print(corrElevator, 2);
  Serial.print(",");
  Serial.print(rudderCmd);
  Serial.print(",");
  Serial.println(elevatorCmd);
  
  delay(500);
  
  // Test Case 3: Mixed errors
  Serial.println("--- Test Case 3: Mixed Errors ---");
  errorLeft = 25.0;   // 25cm too far left (need right turn)
  errorHeight = -15.0; // 15cm too low (need pitch up)
  
  corrRudder = K_P_RUDDER * errorLeft;
  corrElevator = K_P_ELEVATOR * errorHeight;
  
  rudderCmd = RUDDER_NEUTRAL + (int)corrRudder;
  elevatorCmd = ELEVATOR_NEUTRAL + (int)corrElevator;
  
  rudderCmd = constrainMicroseconds(rudderCmd, RUDDER_MIN, RUDDER_MAX);
  elevatorCmd = constrainMicroseconds(elevatorCmd, ELEVATOR_MIN, ELEVATOR_MAX);
  
  rudderServo.writeMicroseconds(rudderCmd);
  elevatorServo.writeMicroseconds(elevatorCmd);
  
  Serial.print(errorLeft);
  Serial.print(",");
  Serial.print(errorHeight);
  Serial.print(",");
  Serial.print(corrRudder, 2);
  Serial.print(",");
  Serial.print(corrElevator, 2);
  Serial.print(",");
  Serial.print(rudderCmd);
  Serial.print(",");
  Serial.println(elevatorCmd);
  
  delay(500);
  
  // Test Case 4: Zero error (center)
  Serial.println("--- Test Case 4: Zero Error (Centered) ---");
  errorLeft = 0.0;
  errorHeight = 0.0;
  
  corrRudder = K_P_RUDDER * errorLeft;
  corrElevator = K_P_ELEVATOR * errorHeight;
  
  rudderCmd = RUDDER_NEUTRAL + (int)corrRudder;
  elevatorCmd = ELEVATOR_NEUTRAL + (int)corrElevator;
  
  rudderCmd = constrainMicroseconds(rudderCmd, RUDDER_MIN, RUDDER_MAX);
  elevatorCmd = constrainMicroseconds(elevatorCmd, ELEVATOR_MIN, ELEVATOR_MAX);
  
  rudderServo.writeMicroseconds(rudderCmd);
  elevatorServo.writeMicroseconds(elevatorCmd);
  
  Serial.print(errorLeft);
  Serial.print(",");
  Serial.print(errorHeight);
  Serial.print(",");
  Serial.print(corrRudder, 2);
  Serial.print(",");
  Serial.print(corrElevator, 2);
  Serial.print(",");
  Serial.print(rudderCmd);
  Serial.print(",");
  Serial.println(elevatorCmd);
  
  delay(500);
  
  // Test Case 5: Large errors (test saturation)
  Serial.println("--- Test Case 5: Large Errors (Saturation Test) ---");
  errorLeft = 100.0;   // Very large error
  errorHeight = 100.0;
  
  corrRudder = K_P_RUDDER * errorLeft;
  corrElevator = K_P_ELEVATOR * errorHeight;
  
  rudderCmd = RUDDER_NEUTRAL + (int)corrRudder;
  elevatorCmd = ELEVATOR_NEUTRAL + (int)corrElevator;
  
  rudderCmd = constrainMicroseconds(rudderCmd, RUDDER_MIN, RUDDER_MAX);
  elevatorCmd = constrainMicroseconds(elevatorCmd, ELEVATOR_MIN, ELEVATOR_MAX);
  
  rudderServo.writeMicroseconds(rudderCmd);
  elevatorServo.writeMicroseconds(elevatorCmd);
  
  Serial.print(errorLeft);
  Serial.print(",");
  Serial.print(errorHeight);
  Serial.print(",");
  Serial.print(corrRudder, 2);
  Serial.print(",");
  Serial.print(corrElevator, 2);
  Serial.print(",");
  Serial.print(rudderCmd);
  Serial.print(",");
  Serial.println(elevatorCmd);
  
  delay(500);
  
  Serial.println("========================================");
  delay(2000);
}
