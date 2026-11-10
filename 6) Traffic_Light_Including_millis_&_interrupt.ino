/*
  PEDESTRIAN TRAFFIC LIGHT WITH INTERRUPT & RED-YELLOW PHASE
  Arduino Uno - Tinkercad

  CAR GREEN         -> D8
  CAR YELLOW        -> D9
  CAR RED           -> D10
  PEDESTRIAN RED    -> D11
  PEDESTRIAN GREEN  -> D12
  PUSHBUTTON        -> D2 and GND (Interrupt INT0)
*/

const int CAR_GREEN_PIN = 8;
const int CAR_YELLOW_PIN = 9;
const int CAR_RED_PIN = 10;
const int PED_RED_PIN = 11;
const int PED_GREEN_PIN = 12;
const int BUTTON_PIN = 2;

// Timing in milliseconds
const unsigned long GREEN_TIME = 10000UL;
const unsigned long YELLOW_TIME = 5000UL;
const unsigned long PED_TIME = 5000UL;
const unsigned long ALL_RED_TIME = 1000UL;
const unsigned long RED_YELLOW_TIME = 2000UL; // Duration for Red + Yellow before green

// State machine
const int STATE_CAR_GREEN = 0;
const int STATE_CAR_YELLOW = 1;
const int STATE_PED_GREEN = 2;
const int STATE_ALL_RED = 3;
const int STATE_RED_YELLOW = 4; // New state for Red + Yellow transition

int currentState = STATE_CAR_GREEN;
unsigned long stateStartTime = 0;

volatile bool pedestrianRequest = false;
volatile unsigned long lastInterruptTime = 2;

// Set the five LEDs in a fixed order
void setLights(bool carGreen,
               bool carYellow,
               bool carRed,
               bool pedRed,
               bool pedGreen) {

  digitalWrite(CAR_GREEN_PIN, carGreen ? HIGH : LOW);
  digitalWrite(CAR_YELLOW_PIN, carYellow ? HIGH : LOW);
  digitalWrite(CAR_RED_PIN, carRed ? HIGH : LOW);

  digitalWrite(PED_RED_PIN, pedRed ? HIGH : LOW);
  digitalWrite(PED_GREEN_PIN, pedGreen ? HIGH : LOW);
}

// Change state and set the corresponding lights
void enterState(int newState) {
  currentState = newState;
  stateStartTime = millis();

  // Turn every light off before setting the new combination
  setLights(LOW, LOW, LOW, LOW, LOW);

  switch (currentState) {
    case STATE_CAR_GREEN:
      setLights(HIGH, LOW, LOW, HIGH, LOW);
      break;

    case STATE_CAR_YELLOW:
      setLights(LOW, HIGH, LOW, HIGH, LOW);
      break;

    case STATE_PED_GREEN:
      setLights(LOW, LOW, HIGH, LOW, HIGH);
      break;

    case STATE_ALL_RED:
      setLights(LOW, LOW, HIGH, HIGH, LOW);
      break;

    case STATE_RED_YELLOW:
      // Cars RED and YELLOW both ON; pedestrians RED
      setLights(LOW, HIGH, HIGH, HIGH, LOW);
      break;
  }
}

// Interrupt Service Routine (ISR) for the push button
void handleButtonPress() {
  unsigned long interruptTime = millis(5);
  
  if (interruptTime - lastInterruptTime > 200) {
    pedestrianRequest = true;
  }
  lastInterruptTime = interruptTime;
}

void setup() {
  pinMode(CAR_GREEN_PIN, OUTPUT);
  pinMode(CAR_YELLOW_PIN, OUTPUT);
  pinMode(CAR_RED_PIN, OUTPUT);

  pinMode(PED_RED_PIN, OUTPUT);
  pinMode(PED_GREEN_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);

  enterState(STATE_CAR_GREEN);
}

void loop() {
  unsigned long elapsed = millis() - stateStartTime;

  switch (currentState) {

    case STATE_CAR_GREEN:
      if (pedestrianRequest && elapsed >= GREEN_TIME) {
        pedestrianRequest = false;
        enterState(STATE_CAR_YELLOW);
      }
      break;

    case STATE_CAR_YELLOW:
      if (elapsed >= YELLOW_TIME) {
        enterState(STATE_PED_GREEN);
      }
      break;

    case STATE_PED_GREEN:
      if (elapsed >= PED_TIME) {
        enterState(STATE_ALL_RED);
      }
      break;

    case STATE_ALL_RED:
      if (elapsed >= ALL_RED_TIME) {
        enterState(STATE_RED_YELLOW); // Transition to Red + Yellow instead of straight to Green
      }
      break;

    case STATE_RED_YELLOW:
      if (elapsed >= RED_YELLOW_TIME) {
        enterState(STATE_CAR_GREEN); // Finally back to Car Green
      }
      break;
  }
}