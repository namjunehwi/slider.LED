#include <TaskScheduler.h>
#include <LiquidCrystal_I2C.h>

// =====================================
// 1. Pin Settings
// =====================================
const int pwmPin = 3;
const uint8_t BUTTON_PIN = 2;
const uint8_t LED_PIN    = 13;
const uint8_t POT_PIN    = A0;

// LCD: I2C Address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================
// 2. Global Variables
// =====================================
bool ledState = false;

// Last reported potentiometer value
int potValue = -1;

// Minimum potentiometer change
const int POT_THRESHOLD = 5;

// Interrupt variables
volatile bool buttonEvent = false;
volatile unsigned long lastInterruptTime = 0;

// Button debounce interval
const unsigned long DEBOUNCE_MS = 100;

// =====================================
// 3. Function Declarations
// =====================================
void buttonISR();
void taskButtonCallback();
void taskPotCallback();
void taskSerialCallback();
void taskLCDCallback();
void setLED(bool state);

// =====================================
// 4. TaskScheduler
// =====================================
Scheduler runner;

// Button event processing: 10 ms
Task tButton(
  10, TASK_FOREVER,
  &taskButtonCallback, &runner, true
);

// Potentiometer measurement: 100 ms
Task tPot(
  100, TASK_FOREVER,
  &taskPotCallback, &runner, true
);

// Serial command processing: 500 ms
Task tSerial(
  10, TASK_FOREVER,
  &taskSerialCallback, &runner, true
);

// LCD updating: 100 ms
Task tLCD(
  100, TASK_FOREVER,
  &taskLCDCallback, &runner, true
);

// =====================================
// 5. External Interrupt (D2)
// =====================================
void buttonISR() {
  unsigned long now = millis();

  if (now - lastInterruptTime >= DEBOUNCE_MS) {
    buttonEvent = true;
    lastInterruptTime = now;
  }
}

// =====================================
// 6. LED Control
// =====================================
void setLED(bool state) {

  // Do nothing if LED state is unchanged
  if (ledState == state) {
    return;
  }

  ledState = state;

  digitalWrite(
    LED_PIN,
    ledState ? HIGH : LOW
  );

  // Report LED status
  Serial.print("LED: ");
  Serial.println(ledState ? "ON" : "OFF");
}

// =====================================
// 7. Task 1: Button Event
// =====================================
void taskButtonCallback() {
  bool event = false;

  // Safely read and clear interrupt flag
  noInterrupts();

  if (buttonEvent) {
    event = true;
    buttonEvent = false;
  }

  interrupts();

  if (event) {
    setLED(!ledState);
  }
}

// =====================================
// 8. Task 2: Potentiometer
//    Measurement interval: 100 ms
//    Report only if change >= 3
// =====================================
void taskPotCallback() {

  int value = analogRead(POT_PIN);

  // First measurement OR change >= 3
  if (potValue == -1 ||
      abs(value - potValue) >= POT_THRESHOLD) {

    // Update last reported value
    potValue = value;

    // Report potentiometer value
    Serial.print("POT: ");
    Serial.println(potValue);
  }
}

// =====================================
// 9. Task 3: Serial Communication
//    Check interval: 500 ms
//    Commands: ON / OFF
// =====================================
void taskSerialCallback() {

  static char command[8];
  static uint8_t index = 0;
  static bool overflow = false;

  while (Serial.available() > 0) {

    char c = Serial.read();

    // End of command
    if (c == '\n' || c == '\r') {

      if (overflow) {
        Serial.println("Command too long");
      }
      else if (index > 0) {

        command[index] = '\0';

        if (strcmp(command, "ON") == 0) {
          setLED(true);
        }
        else if (strcmp(command, "OFF") == 0) {
          setLED(false);
        }
        else if (strncmp(command, "PWM:",4) == 0) {
          int bright = atoi(command+4);
          analogWrite(pwmPin, bright);
        }
        else {
          Serial.println("Invalid command");
        }
      }

      index = 0;
      overflow = false;
    }
    else if (!overflow) {

      // Convert lowercase to uppercase
      if (c >= 'a' && c <= 'z') {
        c -= ('a' - 'A');
      }

      if (index < sizeof(command) - 1) {
        command[index++] = c;
      }
      else {
        index = 0;
        overflow = true;
      }
    }
  }
}

// =====================================
// 10. Task 4: LCD Display
//     Update interval: 100 ms
// =====================================
void taskLCDCallback() {

  static bool previousLED = false;
  static int previousPOT = -1;
  static bool firstUpdate = true;

  // LCD Line 1: LED Status
  if (firstUpdate || ledState != previousLED) {

    lcd.setCursor(0, 0);

    if (ledState) {
      lcd.print("LED: ON         ");
    }
    else {
      lcd.print("LED: OFF        ");
    }

    previousLED = ledState;
  }

  // LCD Line 2: Potentiometer Value
  if (firstUpdate || potValue != previousPOT) {

    lcd.setCursor(0, 1);

    lcd.print("POT: ");
    if (potValue < 0) {
      lcd.print("----");
    }
    else {
      lcd.print(potValue);
    }
    lcd.print("          ");

    previousPOT = potValue;
  }

  firstUpdate = false;
}

// =====================================
// 11. Setup
// =====================================
void setup() {

  // Serial communication
  Serial.begin(9600);

  // Pin configuration
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, 0);

  digitalWrite(LED_PIN, LOW);

  // LCD initialization
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("LED: OFF");

  lcd.setCursor(0, 1);
  lcd.print("POT: ----");

  // Initialize debounce timer
  lastInterruptTime = millis() - DEBOUNCE_MS;

  // External interrupt
  attachInterrupt(
    digitalPinToInterrupt(BUTTON_PIN),
    buttonISR,
    FALLING
  );

  // Startup messages
  Serial.println("System Ready");
  Serial.println("Commands: ON / OFF");
  Serial.println("LED: OFF");
}

// =====================================
// 12. Main Loop
// =====================================
void loop() {

  runner.execute();
  
}
