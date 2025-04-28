const int buttonPin = 27;
bool lastButtonState = HIGH; // Start assuming unpressed (HIGH with INPUT_PULLUP)
bool currentButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50ms debounce time
bool BUTTON = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    // reset the debounce timer if button changed
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has been stable longer than debounce delay
    if (reading != currentButtonState) {
      currentButtonState = reading;

      if (currentButtonState == LOW || currentButtonState == HIGH) {
        // Button toggled (either ON or OFF), count it
        Serial.print("Button pressed. Total clicks: ");
        BUTTON = HIGH;
      }
    }
  }

  lastButtonState = reading;
}




const int buttonPin = 27;
bool lastButtonState = HIGH; // Assume unpressed initially
bool currentButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50ms debounce time

// Global flag: becomes true for one loop when a click is detected
bool buttonPressed = false;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  buttonPressed = false; // Reset at the start of each loop

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != currentButtonState) {
      currentButtonState = reading;

      if (currentButtonState == LOW || currentButtonState == HIGH) {
        buttonPressed = true; // Set true for this loop if clicked
      }
    }
  }

  lastButtonState = reading;

  // Example: use the flag somewhere
  if (buttonPressed) {
    Serial.println("Detected a button click!");
  }
}