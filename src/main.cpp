#include <Arduino.h>
#include <Servo.h>

// =========================================================
// 1. HARDWARE PIN CONFIGURATION
// =========================================================
const int PIN_SERVO_ELEVATOR = 1;
const int PIN_SERVO_RUDDER   = 2;
const int PIN_ECHO_HEIGHT    = 3;
const int PIN_TRIG_HEIGHT    = 4;
const int PIN_ECHO_RIGHT     = 5;
const int PIN_TRIG_RIGHT     = 6;



// =========================================================
// 2. SENSOR PHYSICS & LIMITS
// =========================================================
const float SPEED_OF_SOUND_DIVISOR = 58.0;   // Divide uS by this to get cm
const unsigned long SONAR_TIMEOUT_US = 30000; // 30ms ~ 400cm range
const float NO_READING_VAL         = -1.0;   // Return value for timeout

// Filter Settings
const float DIST_FILTER_ALPHA      = 0.50;   // Low pass filter strength (0.0 - 1.0)
const float MAX_DIST_JUMP_CM       = 60.0;   // Spike rejection threshold
const float FAILSAFE_DIST_CM       = 50.0;   // Default distance if sensor fails at startup

// Rate Calculation Settings
const float MAX_PHYSICAL_RATE_CM_S = 200.0;  // Clamp rates above this (noise rejection)
const int   RATE_AVG_WINDOW_SIZE   = 3;      // Average the last N rates (Smoothing)

// =========================================================
// 3. SERVO CALIBRATION
// =========================================================
const int SERVO_RUDDER_NEUTRAL   = 1700;
const int SERVO_RUDDER_LEFT      = 900;
const int SERVO_RUDDER_MIN       = 900;
const int SERVO_RUDDER_MAX       = 2100;

const int SERVO_ELEVATOR_NEUTRAL = 1100;
const int SERVO_ELEVATOR_UP      = 2100;
const int SERVO_ELEVATOR_MIN     = 900;
const int SERVO_ELEVATOR_MAX     = 2100;

const float SERVO_SMOOTHING_ALPHA = 0.70;     // Output smoothing
const int   SERVO_DEADBAND_US     = 300;       // Minimum change to write to servo

// =========================================================
// 4. CONTROL LAW PARAMETERS
// =========================================================
// Simple Control: Servo goes to MAX if rate exceeds threshold, neutral otherwise
float PARAM_RATE_RIGHT_THRESHOLD  = 50.0;    // cm/s - trigger rudder
float PARAM_RATE_HEIGHT_THRESHOLD = 50.0;    // cm/s - trigger elevator

// Timeout Settings
const float SERVO_TIMEOUT_SEC = 0.7;         // Return to neutral after this time (seconds)

// =========================================================
// 5. SYSTEM TIMING
// =========================================================
const int LOOP_PERIOD_MS         = 50;       // 20Hz Control Loop
const unsigned long LOG_INTERVAL_MS = 200;   // 5Hz Logging
const float MS_TO_SEC            = 1000.0;   // Conversion factor

const int DELAY_TRIG_LOW_1_US    = 2;
const int DELAY_TRIG_HIGH_US     = 10;
const int DELAY_STARTUP_MS       = 1000;
const int DELAY_SENSOR_STABLE_MS = 20;

// Launch Detection
const float LAUNCH_HEIGHT_CM     = 60.0;

// =========================================================
// CLASS: Rolling Average (To smooth Rate of Change)
// =========================================================
class RollingAverage {
  private:
    float* history;
    int size;
    int index;
    float sum;
    bool filled;
  
  public:
    RollingAverage(int windowSize) {
      size = windowSize;
      history = new float[size];
      index = 0;
      sum = 0.0;
      filled = false;
      for (int i = 0; i < size; i++) history[i] = 0.0;
    }

    float add(float val) {
      sum -= history[index];
      history[index] = val;
      sum += history[index];
      index++;
      if (index >= size) {
        index = 0;
        filled = true;
      }
      return filled ? (sum / size) : (sum / index);
    }
};

// =========================================================
// GLOBAL OBJECTS & VARIABLES
// =========================================================
Servo rudderServo;
Servo elevatorServo;

// Rolling Average Objects for Rate
RollingAverage rateSmootherRight(RATE_AVG_WINDOW_SIZE);
RollingAverage rateSmootherHeight(RATE_AVG_WINDOW_SIZE);

// Sensor State
float currentRight = 0.0;
float currentHeight = 0.0;
float prevRight = 0.0;
float prevHeight = 0.0;

// Control State
bool isCorrectingWall = false;
bool isCorrectingHeight = false;
unsigned long wallCorrectionStartTime = 0;
unsigned long heightCorrectionStartTime = 0;
bool flightStarted = false;
unsigned long flightStartTime = 0;

// Servo Hold Timers
unsigned long rudderActivatedTime = 0;
unsigned long elevatorActivatedTime = 0;
bool rudderActive = false;
bool elevatorActive = false;
const unsigned long SERVO_HOLD_TIME_MS = 500; // Hold servo position for 500ms (0.5 seconds)

// Servo State
int prevRudderPWM = SERVO_RUDDER_NEUTRAL;
int prevElevatorPWM = SERVO_ELEVATOR_NEUTRAL;

// Timing State
unsigned long prevLoopTime = 0;
unsigned long lastLogTime = 0;

// =========================================================
// HELPER FUNCTIONS
// =========================================================

float readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(DELAY_TRIG_LOW_1_US);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(DELAY_TRIG_HIGH_US);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, SONAR_TIMEOUT_US);
  
  if (duration == 0) return NO_READING_VAL;
  return duration / SPEED_OF_SOUND_DIVISOR;
}

float getFilteredDistance(int trigPin, int echoPin, float prevSmoothed) {
  float raw = readUltrasonic(trigPin, echoPin);
  
  // Timeout check
  if (raw == NO_READING_VAL) return prevSmoothed;
  
  // Spike rejection
  if (abs(raw - prevSmoothed) > MAX_DIST_JUMP_CM) return prevSmoothed;

  // Low pass filter
  return (DIST_FILTER_ALPHA * raw) + ((1.0 - DIST_FILTER_ALPHA) * prevSmoothed);
}

void logTelemetry(float timeVal, float distR, float distH, float rateR, float rateH, int rudPWM, int elePWM) {
  // Simple direct printing for debugging
  Serial.print("T:");
  Serial.print(timeVal, 2);
  Serial.print(" | DistR:");
  Serial.print(distR, 1);
  Serial.print(" | DistH:");
  Serial.print(distH, 1);
  Serial.print(" | RateR:");
  Serial.print(rateR, 1);
  Serial.print(" | RateH:");
  Serial.print(rateH, 1);
  Serial.print(" | Rud:");
  Serial.print(rudPWM);
  Serial.print(" | Ele:");
  Serial.println(elePWM);
}

// =========================================================
// MAIN SETUP
// =========================================================
void setup() {
  Serial.begin(115200);

  pinMode(PIN_TRIG_RIGHT, OUTPUT);
  pinMode(PIN_ECHO_RIGHT, INPUT);
  pinMode(PIN_TRIG_HEIGHT, OUTPUT);
  pinMode(PIN_ECHO_HEIGHT, INPUT);

  rudderServo.attach(PIN_SERVO_RUDDER);
  elevatorServo.attach(PIN_SERVO_ELEVATOR);
  
  rudderServo.writeMicroseconds(SERVO_RUDDER_NEUTRAL);
  elevatorServo.writeMicroseconds(SERVO_ELEVATOR_NEUTRAL);

  // Initialize sensors with stable data
  Serial.println("Testing sensors...");
  float sumR = 0, sumH = 0;
  int validCount = 0;
  
  for(int i=0; i<5; i++) {
    float r = readUltrasonic(PIN_TRIG_RIGHT, PIN_ECHO_RIGHT);
    float h = readUltrasonic(PIN_TRIG_HEIGHT, PIN_ECHO_HEIGHT);
    Serial.print("Test ");
    Serial.print(i+1);
    Serial.print(" - Right: ");
    Serial.print(r);
    Serial.print(" cm, Height: ");
    Serial.print(h);
    Serial.println(" cm");
    if (r > 0 && h > 0) {
      sumR += r; sumH += h; validCount++;
    }
    delay(DELAY_SENSOR_STABLE_MS);
  }

  if (validCount > 0) {
    currentRight = sumR / validCount;
    currentHeight = sumH / validCount;
    Serial.print("Sensor Init OK - Right: ");
    Serial.print(currentRight);
    Serial.print(" cm, Height: ");
    Serial.print(currentHeight);
    Serial.println(" cm");
  } else {
    currentRight = FAILSAFE_DIST_CM;
    currentHeight = FAILSAFE_DIST_CM;
    Serial.println("WARNING: Sensor init failed! Using failsafe values.");
  }

  prevRight = currentRight;
  prevHeight = currentHeight;

  delay(DELAY_STARTUP_MS);
  Serial.println("System Ready. Waiting for launch...");
}

// =========================================================
// MAIN LOOP
// =========================================================
void loop() {
  unsigned long currentTime = millis();

  // 1. Loop Frequency Control
  if (currentTime - prevLoopTime < LOOP_PERIOD_MS) return;
  float dt = (currentTime - prevLoopTime) / MS_TO_SEC;
  prevLoopTime = currentTime;

  // 2. Read Sensors
  float rightDist = getFilteredDistance(PIN_TRIG_RIGHT, PIN_ECHO_RIGHT, currentRight);
  float height = getFilteredDistance(PIN_TRIG_HEIGHT, PIN_ECHO_HEIGHT, currentHeight);
  
  // Update current values
  currentRight = rightDist;
  currentHeight = height;

  // 3. Launch Detect
  if (!flightStarted && height > LAUNCH_HEIGHT_CM) {
    flightStarted = true;
    flightStartTime = currentTime;
    prevRight = rightDist; 
    prevHeight = height;
  }

  int targetRudder = SERVO_RUDDER_NEUTRAL;
  int targetElevator = SERVO_ELEVATOR_NEUTRAL;
  float avgRateRight = 0.0;
  float avgRateHeight = 0.0;

  if (flightStarted) {
    // 4. Calculate Raw Rate
    float rawRateRight = - (rightDist - prevRight) / dt;
    float rawRateHeight = - (height - prevHeight) / dt;

    // // 5. Clamp Noise (Hard Limit)
    // if (abs(rawRateRight) > MAX_PHYSICAL_RATE_CM_S) rawRateRight = 0.0;
    // if (abs(rawRateHeight) > MAX_PHYSICAL_RATE_CM_S) rawRateHeight = 0.0;

    // 6. Smooth Rate (Rolling Average)
    avgRateRight = rateSmootherRight.add(rawRateRight);
    avgRateHeight = rateSmootherHeight.add(rawRateHeight);

    // 7. Simple Control Logic: If rate exceeds threshold -> Apply correction, else stay neutral
    
    // Rudder Control with Hold Timer
    if (avgRateRight > PARAM_RATE_RIGHT_THRESHOLD) {
      // Rate exceeded threshold - activate rudder
      targetRudder = SERVO_RUDDER_LEFT;
      rudderActivatedTime = currentTime;
      rudderActive = true;
    } else if (rudderActive && (currentTime - rudderActivatedTime < SERVO_HOLD_TIME_MS)) {
      // Hold rudder position for SERVO_HOLD_TIME_MS
      targetRudder = SERVO_RUDDER_LEFT;
    } else {
      // Return to neutral
      targetRudder = SERVO_RUDDER_NEUTRAL;
      rudderActive = false;
    }

    // Elevator Control with Hold Timer
    if (avgRateHeight > PARAM_RATE_HEIGHT_THRESHOLD) {
      // Rate exceeded threshold - activate elevator
      targetElevator = SERVO_ELEVATOR_UP;
      elevatorActivatedTime = currentTime;
      elevatorActive = true;
    } else if (elevatorActive && (currentTime - elevatorActivatedTime < SERVO_HOLD_TIME_MS)) {
      // Hold elevator position for SERVO_HOLD_TIME_MS
      targetElevator = SERVO_ELEVATOR_UP;
    } else {
      // Return to neutral
      targetElevator = SERVO_ELEVATOR_NEUTRAL;
      elevatorActive = false;
    }

    prevRight = rightDist;
    prevHeight = height;
  }

  // 9. Output Smoothing & Constraint
  targetRudder = constrain(targetRudder, SERVO_RUDDER_MIN, SERVO_RUDDER_MAX);
  targetElevator = constrain(targetElevator, SERVO_ELEVATOR_MIN, SERVO_ELEVATOR_MAX);

  prevRudderPWM = (SERVO_SMOOTHING_ALPHA * targetRudder) + ((1.0 - SERVO_SMOOTHING_ALPHA) * prevRudderPWM);
  prevElevatorPWM = (SERVO_SMOOTHING_ALPHA * targetElevator) + ((1.0 - SERVO_SMOOTHING_ALPHA) * prevElevatorPWM);

  // 10. Write to Servos (Deadband check)
  // We use static variables to track what we ACTUALLY wrote last time to hardware
  static int lastWrittenRudder = -1;
  static int lastWrittenElevator = -1;

  if (abs(prevRudderPWM - lastWrittenRudder) > SERVO_DEADBAND_US) {
    rudderServo.writeMicroseconds(prevRudderPWM);
    lastWrittenRudder = prevRudderPWM;
  }
  
  if (abs(prevElevatorPWM - lastWrittenElevator) > SERVO_DEADBAND_US) {
    elevatorServo.writeMicroseconds(prevElevatorPWM);
    lastWrittenElevator = prevElevatorPWM;
  }

  // // 11. Logging
  // if (currentTime - lastLogTime >= LOG_INTERVAL_MS) {
  //   // Convert millis to seconds for easier reading
  //   float timeSec = flightStarted ? (currentTime - flightStartTime) / 1000.0 : 0.0;
    
  //   logTelemetry(
  //     timeSec, 
  //     rightDist,       // Passed from your sensor read
  //     height,          // Passed from your sensor read
  //     avgRateRight,    // The AVERAGED rate
  //     avgRateHeight,   // The AVERAGED rate
  //     prevRudderPWM,   // The actual command sent
  //     prevElevatorPWM  // The actual command sent
  //   );
    
  //   lastLogTime = currentTime;
  // }
}