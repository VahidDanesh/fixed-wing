#include <Arduino.h>
#include <Servo.h>

const int SERVO_RUDDER_PIN = 2;
const int SERVO_ELEVATOR_PIN = 1;

Servo rudderServo;
Servo elevatorServo;

int currentRudderDeg = 90;
int currentElevatorDeg = 90;

void printMenu() {
  Serial.println("\n========== SERVO CALIBRATION TOOL ==========");
  Serial.println("Commands:");
  Serial.println("  r<value> - Set rudder (e.g., r90)");
  Serial.println("  e<value> - Set elevator (e.g., e90)");
  Serial.println("  r+<step> - Increase rudder (e.g., r+5)");
  Serial.println("  r-<step> - Decrease rudder (e.g., r-5)");
  Serial.println("  e+<step> - Increase elevator (e.g., e+5)");
  Serial.println("  e-<step> - Decrease elevator (e.g., e-5)");
  Serial.println("  c        - Center both servos (90°)");
  Serial.println("  s        - Show current values");
  Serial.println("  h        - Show this help menu");
  Serial.println("===========================================");
  Serial.println("Typical range: 0° to 180°");
  Serial.println("===========================================\n");
}

void showCurrentValues() {
  Serial.print("Current - Rudder: ");
  Serial.print(currentRudderDeg);
  Serial.print("°  Elevator: ");
  Serial.print(currentElevatorDeg);
  Serial.println("°");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for serial connection
  
  rudderServo.attach(SERVO_RUDDER_PIN);
  elevatorServo.attach(SERVO_ELEVATOR_PIN);
  
  // Center both servos
  rudderServo.write(currentRudderDeg);
  elevatorServo.write(currentElevatorDeg);
  
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
      currentRudderDeg = 90;
      currentElevatorDeg = 90;
      rudderServo.write(currentRudderDeg);
      elevatorServo.write(currentElevatorDeg);
      Serial.println("✓ Both servos centered to 90°");
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
          if (step == 0) step = 5;
          currentRudderDeg += step;
          currentRudderDeg = constrain(currentRudderDeg, 0, 180);
          rudderServo.write(currentRudderDeg);
          Serial.print("✓ Rudder increased to ");
          Serial.print(currentRudderDeg);
          Serial.println("°");
        }
        else if (op == '-') {
          int step = input.substring(2).toInt();
          if (step == 0) step = 5;
          currentRudderDeg -= step;
          currentRudderDeg = constrain(currentRudderDeg, 0, 180);
          rudderServo.write(currentRudderDeg);
          Serial.print("✓ Rudder decreased to ");
          Serial.print(currentRudderDeg);
          Serial.println("°");
        }
        else {
          int value = input.substring(1).toInt();
          if (value >= 0 && value <= 180) {
            currentRudderDeg = value;
            rudderServo.write(currentRudderDeg);
            Serial.print("✓ Rudder set to ");
            Serial.print(currentRudderDeg);
            Serial.println("°");
          } else {
            Serial.println("✗ Error: Value out of range (0-180°)");
          }
        }
      }
    }
    else if (cmd == 'e' || cmd == 'E') {
      if (input.length() > 1) {
        char op = input.charAt(1);
        if (op == '+') {
          int step = input.substring(2).toInt();
          if (step == 0) step = 5;
          currentElevatorDeg += step;
          currentElevatorDeg = constrain(currentElevatorDeg, 0, 180);
          elevatorServo.write(currentElevatorDeg);
          Serial.print("✓ Elevator increased to ");
          Serial.print(currentElevatorDeg);
          Serial.println("°");
        }
        else if (op == '-') {
          int step = input.substring(2).toInt();
          if (step == 0) step = 5;
          currentElevatorDeg -= step;
          currentElevatorDeg = constrain(currentElevatorDeg, 0, 180);
          elevatorServo.write(currentElevatorDeg);
          Serial.print("✓ Elevator decreased to ");
          Serial.print(currentElevatorDeg);
          Serial.println("°");
        }
        else {
          int value = input.substring(1).toInt();
          if (value >= 0 && value <= 180) {
            currentElevatorDeg = value;
            elevatorServo.write(currentElevatorDeg);
            Serial.print("✓ Elevator set to ");
            Serial.print(currentElevatorDeg);
            Serial.println("°");
          } else {
            Serial.println("✗ Error: Value out of range (0-180°)");
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