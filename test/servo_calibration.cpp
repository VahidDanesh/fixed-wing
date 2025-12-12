#include <Arduino.h>
#include <Servo.h>

const int SERVO_RUDDER_PIN = 2;
const int SERVO_ELEVATOR_PIN = 1;

Servo rudderServo;
Servo elevatorServo;

int currentRudderUs = 1500;
int currentElevatorUs = 1500;

const int SERVO_RUDDER_NEUTRAL   = 1000;
const int SERVO_RUDDER_MIN       = 900;
const int SERVO_RUDDER_MAX       = 2100;

const int SERVO_ELEVATOR_NEUTRAL = 1100;
const int SERVO_ELEVATOR_MIN     = 900;
const int SERVO_ELEVATOR_MAX     = 2100;


void printMenu() {
  Serial.println("\n========== SERVO CALIBRATION TOOL ==========");
  Serial.println("Commands:");
  Serial.println("  r<value> - Set rudder (e.g., r1500)");
  Serial.println("  e<value> - Set elevator (e.g., e1500)");
  Serial.println("  r+<step> - Increase rudder (e.g., r+10)");
  Serial.println("  r-<step> - Decrease rudder (e.g., r-10)");
  Serial.println("  e+<step> - Increase elevator (e.g., e+10)");
  Serial.println("  e-<step> - Decrease elevator (e.g., e-10)");
  Serial.println("  c        - Center both servos (1500µs)");
  Serial.println("  s        - Show current values");
  Serial.println("  h        - Show this help menu");
  Serial.println("===========================================");
  Serial.println("Typical range: 1000µs to 2000µs");
  Serial.println("===========================================\n");
}

void showCurrentValues() {
  Serial.print("Current - Rudder: ");
  Serial.print(currentRudderUs);
  Serial.print("µs  Elevator: ");
  Serial.print(currentElevatorUs);
  Serial.println("µs");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for serial connection
  
  rudderServo.attach(SERVO_RUDDER_PIN);
  elevatorServo.attach(SERVO_ELEVATOR_PIN);
  
  // Center both servos
  rudderServo.writeMicroseconds(currentRudderUs);
  elevatorServo.writeMicroseconds(currentElevatorUs);
  
  delay(500);
  printMenu();
  showCurrentValues();
  Serial.println("\nEnter command:");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() == 0) return;
    
    char cmd = input.charAt(0);
    
    if (cmd == 'h' || cmd == 'H') {
      printMenu();
      showCurrentValues();
    }
    else if (cmd == 'c' || cmd == 'C') {
      currentRudderUs = 1500;
      currentElevatorUs = 1500;
      rudderServo.writeMicroseconds(currentRudderUs);
      elevatorServo.writeMicroseconds(currentElevatorUs);
      Serial.println("✓ Both servos centered to 1500µs");
      showCurrentValues();
    }
    else if (cmd == 's' || cmd == 'S') {
      showCurrentValues();
    }
    else if (cmd == 'r' || cmd == 'R') {
      if (input.length() > 1) {
        char op = input.charAt(1);
        if (op == '+') {
          int step = input.substring(2).toInt();
          if (step == 0) step = 10;
          currentRudderUs += step;
          currentRudderUs = constrain(currentRudderUs, 500, 2500);
          rudderServo.writeMicroseconds(currentRudderUs);
          Serial.print("✓ Rudder increased to ");
          Serial.print(currentRudderUs);
          Serial.println("µs");
        }
        else if (op == '-') {
          int step = input.substring(2).toInt();
          if (step == 0) step = 10;
          currentRudderUs -= step;
          currentRudderUs = constrain(currentRudderUs, 500, 2500);
          rudderServo.writeMicroseconds(currentRudderUs);
          Serial.print("✓ Rudder decreased to ");
          Serial.print(currentRudderUs);
          Serial.println("µs");
        }
        else {
          int value = input.substring(1).toInt();
          if (value >= 500 && value <= 2500) {
            currentRudderUs = value;
            rudderServo.writeMicroseconds(currentRudderUs);
            Serial.print("✓ Rudder set to ");
            Serial.print(currentRudderUs);
            Serial.println("µs");
          } else {
            Serial.println("✗ Error: Value out of range (500-2500µs)");
          }
        }
      }
    }
    else if (cmd == 'e' || cmd == 'E') {
      if (input.length() > 1) {
        char op = input.charAt(1);
        if (op == '+') {
          int step = input.substring(2).toInt();
          if (step == 0) step = 10;
          currentElevatorUs += step;
          currentElevatorUs = constrain(currentElevatorUs, 500, 2500);
          elevatorServo.writeMicroseconds(currentElevatorUs);
          Serial.print("✓ Elevator increased to ");
          Serial.print(currentElevatorUs);
          Serial.println("µs");
        }
        else if (op == '-') {
          int step = input.substring(2).toInt();
          if (step == 0) step = 10;
          currentElevatorUs -= step;
          currentElevatorUs = constrain(currentElevatorUs, 500, 2500);
          elevatorServo.writeMicroseconds(currentElevatorUs);
          Serial.print("✓ Elevator decreased to ");
          Serial.print(currentElevatorUs);
          Serial.println("µs");
        }
        else {
          int value = input.substring(1).toInt();
          if (value >= 500 && value <= 2500) {
            currentElevatorUs = value;
            elevatorServo.writeMicroseconds(currentElevatorUs);
            Serial.print("✓ Elevator set to ");
            Serial.print(currentElevatorUs);
            Serial.println("µs");
          } else {
            Serial.println("✗ Error: Value out of range (500-2500µs)");
          }
        }
      }
    }
    else {
      Serial.println("✗ Unknown command. Type 'h' for help.");
    }
    
    Serial.println("\nEnter command:");
  }
}