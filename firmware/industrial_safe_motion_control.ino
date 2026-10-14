/*
 * Industrial Safe Motion Control
 * Arduino Nano compatible
 * Emergency double-press return logic
 */

// ================= STATES =================
enum SystemState {
  IDLE,
  RUNNING,
  EMERGENCY_STOPPED,
  RETURNING_HOME
};

volatile SystemState currentState = IDLE;

// ================= PIN DEFINITIONS =================
const int STEP_PIN = 4;
const int DIR_PIN  = 5;
const int EN_PIN   = 6;

const int VALVE_EXTEND  = 7;
const int VALVE_RETRACT = 8;

const int SENSOR_PIN = 2;          // INT0
const int EMERGENCY_BUTTON = 3;    // INT1
const int START_BUTTON = 9;

// ================= VARIABLES =================
volatile bool stopMotor = false;
volatile bool emergencyPressed = false;

long stepsMoved = 0;

unsigned long firstEmergencyTime = 0;
bool waitingForSecondPress = false;
const unsigned long DOUBLE_PRESS_WINDOW = 2000; // 2 seconds

// ===== DEBOUNCE VARIABLES =====
unsigned long lastStartPressTime = 0;
const unsigned long START_DEBOUNCE = 30;

volatile unsigned long lastEmergencyInterrupt = 0;
const unsigned long EMERGENCY_DEBOUNCE = 50;

// ================= SETUP =================
void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  pinMode(VALVE_EXTEND, OUTPUT);
  pinMode(VALVE_RETRACT, OUTPUT);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(EMERGENCY_BUTTON, INPUT_PULLUP);
  pinMode(START_BUTTON, INPUT_PULLUP);

  digitalWrite(EN_PIN, LOW);
  stopCylinder();

  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), sensorISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(EMERGENCY_BUTTON), emergencyISR, FALLING);
}

// ================= MAIN LOOP =================
void loop() {

  switch (currentState) {

    case IDLE:
      if (startPressed()) {
        startCycle();
      }
      break;

    case RUNNING:
      runCycle();
      break;

    case EMERGENCY_STOPPED:
      handleEmergencyDoublePress();
      break;

    case RETURNING_HOME:
      returnToInitial();
      currentState = IDLE;
      break;
  }
}

// ================= ISR =================
void sensorISR() {
  stopMotor = true;
}

void emergencyISR() {

  unsigned long now = millis();

  // Debounce for emergency button
  if (now - lastEmergencyInterrupt < EMERGENCY_DEBOUNCE) return;
  lastEmergencyInterrupt = now;

  // Immediate safety action
  stopMotor = true;
  digitalWrite(EN_PIN, HIGH);
  stopCylinder();

  currentState = EMERGENCY_STOPPED;
  emergencyPressed = true;
}

// ================= EMERGENCY LOGIC =================
void handleEmergencyDoublePress() {

  if (emergencyPressed) {
    emergencyPressed = false;
    unsigned long now = millis();

    if (!waitingForSecondPress) {
      // First press detected
      firstEmergencyTime = now;
      waitingForSecondPress = true;
    } else {
      // Second press
      if (now - firstEmergencyTime <= DOUBLE_PRESS_WINDOW) {
        waitingForSecondPress = false;
        currentState = RETURNING_HOME;
      }
    }
  }

  // Timeout handling
  if (waitingForSecondPress && (millis() - firstEmergencyTime > DOUBLE_PRESS_WINDOW)) {
    waitingForSecondPress = false;
  }
}

// ================= LOGIC FUNCTIONS =================
bool startPressed() {

  bool now = digitalRead(START_BUTTON);

  if (now == LOW && (millis() - lastStartPressTime > START_DEBOUNCE)) {
    lastStartPressTime = millis();
    return true;
  }

  return false;
}

void startCycle() {
  stepsMoved = 0;
  stopMotor = false;
  digitalWrite(EN_PIN, LOW);
  currentState = RUNNING;
}

void runCycle() {

  extendCylinder();
  delay(2000);

  digitalWrite(DIR_PIN, LOW);

  while (!stopMotor && currentState == RUNNING) {
    stepOnce();
    stepsMoved++;
  }

  if (currentState != RUNNING) return;

  retractCylinder();
  delay(1000);

  returnToInitial();
  currentState = IDLE;
}

// ================= MOTION =================
void stepOnce() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(50);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(50);
}

void returnToInitial() {
  digitalWrite(EN_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);

  for (long i = 0; i < stepsMoved; i++) {
    stepOnce();
  }
}

// ================= PNEUMATICS =================
void extendCylinder() {
  digitalWrite(VALVE_RETRACT, LOW);
  delay(10);
  digitalWrite(VALVE_EXTEND, HIGH);
}

void retractCylinder() {
  delay(1000);
  digitalWrite(VALVE_EXTEND, LOW);
  delay(10);
  digitalWrite(VALVE_RETRACT, HIGH);
}

void stopCylinder() {
  digitalWrite(VALVE_EXTEND, LOW);
  digitalWrite(VALVE_RETRACT, LOW);
}
