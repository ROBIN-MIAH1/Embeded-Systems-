
/*
  PEDESTRIAN TRAFFIC LIGHT
  Arduino Uno - Tinkercad

  CAR GREEN        -> D8
  CAR YELLOW       -> D9
  CAR RED          -> D10
  PEDESTRIAN RED   -> D11
  PEDESTRIAN GREEN -> D12
  PUSHBUTTON       -> D2 and GND
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
const unsigned long DEBOUNCE_TIME = 40UL;

// State machine
const int STATE_CAR_GREEN = 0;
const int STATE_CAR_YELLOW = 1;
const int STATE_PED_GREEN = 2;
const int STATE_ALL_RED = 3;

int currentState = STATE_CAR_GREEN;

unsigned long stateStartTime = 0;
unsigned long lastDebounceTime = 0;

int lastButtonReading = HIGH;
int stableButtonState = HIGH;

bool pedestrianRequest = false;

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
      // Cars green; pedestrians red
      setLights(HIGH, LOW, LOW, HIGH, LOW);
      break;

    case STATE_CAR_YELLOW:
      // Cars yellow; pedestrians red
      setLights(LOW, HIGH, LOW, HIGH, LOW);
      break;

    case STATE_PED_GREEN:
      // Cars red; pedestrians green
      setLights(LOW, LOW, HIGH, LOW, HIGH);
      break;

    case STATE_ALL_RED:
      // Both red for a safety interval
      setLights(LOW, LOW, HIGH, HIGH, LOW);
      break;
  }
}

// Detect button presses and debounce
void readButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
    lastButtonReading = reading;
  }

  if (millis() - lastDebounceTime >= DEBOUNCE_TIME) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      // Button pressed: LOW because INPUT_PULLUP is enabled
      if (stableButtonState == LOW) {
        pedestrianRequest = true;
      }
    }
  }
}

void setup() {
  pinMode(CAR_GREEN_PIN, OUTPUT);
  pinMode(CAR_YELLOW_PIN, OUTPUT);
  pinMode(CAR_RED_PIN, OUTPUT);

  pinMode(PED_RED_PIN, OUTPUT);
  pinMode(PED_GREEN_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initial condition: cars green, pedestrians red
  enterState(STATE_CAR_GREEN);
}

void loop() {
  readButton();

  unsigned long elapsed = millis() - stateStartTime;

  switch (currentState) {

    case STATE_CAR_GREEN:
      // Wait for a button request and minimum green time
      if (pedestrianRequest && elapsed >= GREEN_TIME) {
        pedestrianRequest = false;
        enterState(STATE_CAR_YELLOW);
      }
      break;

    case STATE_CAR_YELLOW:
      // Yellow for 5 seconds, then cars stop
      if (elapsed >= YELLOW_TIME) {
        enterState(STATE_PED_GREEN);
      }
      break;

    case STATE_PED_GREEN:
      // Pedestrians cross for 5 seconds
      if (elapsed >= PED_TIME) {
        enterState(STATE_ALL_RED);
      }
      break;

    case STATE_ALL_RED:
      // Both red for 1 second, then cars move again
      if (elapsed >= ALL_RED_TIME) {
        enterState(STATE_CAR_GREEN);
      }
      break;
  }
}
