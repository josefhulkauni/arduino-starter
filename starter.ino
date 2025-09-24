// starter.ino
// Version: 0.2.0
// Date: 2025-09-24
//
// C++ code
//
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Definice pinů
const int buzzerPin = 13;
const int startPin = 12;
const int stopPin = 11;
const int pausePin = 10;
const int ledRedPin = A0;
const int ledGreenPin = A1;

// Definice klávesnice
const byte ROWS = 4; // čtyři řádky
const byte COLS = 4; // čtyři sloupce
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Piny pro řádky
byte colPins[COLS] = {5, 4, 3, 2}; // Piny pro sloupce

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Definice LCD displeje
LiquidCrystal_I2C lcd(0x27, 16, 2); // Změňte 0x27 na vaši I2C adresu, pokud je jiná

// Stavy programu
enum ProgramState {
  STATE_NONE,
  STATE_SETUP,
  STATE_COUNTDOWN,
  STATE_PAUSED
};

ProgramState currentState = STATE_SETUP;
ProgramState lastState = STATE_NONE;
unsigned long intervalInSeconds = 10; // Výchozí interval minuty
unsigned long startMillis;
unsigned long pauseMillis;
unsigned long remaining;
static unsigned long lastDisplayedRemaining = -1;

// --- NOVÉ KONSTANTY ---
const unsigned long DEFAULT_INTERVAL = 10; // Výchozí interval v sekundách
const unsigned long ONE_SECOND = 1000;
const unsigned long SHORT_BEEP_DURATION = 100;
const unsigned long RED_LED_DURATION = 200;
const unsigned long GREEN_LED_DURATION = 1000;
const unsigned long SHORT_BEEP_FREQ = 1000;
const unsigned long LONG_BEEP_FREQ = 1500;
const unsigned long LONG_BEEP_DURATION = 500;
const unsigned long START_SEQUENCE_SECONDS = 5; // Počet sekund startovací sekvence (nastavitelná)

// --- NOVÉ PROMĚNNÉ ---
unsigned long lastSecondUpdate = 0;
unsigned long lastMillis = 0;
bool timerRunning = false;

bool isFirstStart = true;
String inputString = "";

// Prototypes pro funkce
void updateDisplay();

//--------------------- Hlavní funkce ---------------------

void setup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(startPin, INPUT_PULLUP);
  pinMode(stopPin, INPUT_PULLUP);
  pinMode(pausePin, INPUT_PULLUP);

  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);

  intervalInSeconds = DEFAULT_INTERVAL;
  currentState = STATE_SETUP;
  lastState = STATE_NONE;
  timerRunning = false;
  inputString = "";

  lcd.setCursor(0, 0);
  lcd.print("Nastav interval");
  lcd.setCursor(0, 1);
  lcd.print("Akt: ");
  lcd.print(intervalInSeconds);
  lcd.print(" s");
}

void loop()
{
  // --- OVLÁDÁNÍ TLAČÍTEK ---
  char key = keypad.getKey();
  bool startPressed = digitalRead(startPin) == LOW;
  bool stopPressed = digitalRead(stopPin) == LOW;
  bool pausePressed = digitalRead(pausePin) == LOW;

  switch (currentState) {
    case STATE_SETUP:
      handleSetupState(key, startPressed, stopPressed);
      break;
    case STATE_PAUSED:
      handlePausedState(pausePressed, stopPressed);
      break;
    case STATE_COUNTDOWN:
      if (timerRunning) {
        handleCountdownState(pausePressed, stopPressed);
      }
      break;
    default:
      break;
  }
}

//--------------------- Pomocné funkce ---------------------

// Pomocné funkce pro akce
void beepAndRedLed() {
  digitalWrite(ledRedPin, HIGH);
  tone(buzzerPin, SHORT_BEEP_FREQ, SHORT_BEEP_DURATION);
  delay(RED_LED_DURATION);
  digitalWrite(ledRedPin, LOW);
}

void longBeepAndGreenLed() {
  digitalWrite(ledGreenPin, HIGH);
  tone(buzzerPin, LONG_BEEP_FREQ, LONG_BEEP_DURATION);
  delay(GREEN_LED_DURATION);
  digitalWrite(ledGreenPin, LOW);
}

void handleSetupState(char key, bool startPressed, bool stopPressed) {
  if (key != NO_KEY && isdigit(key)) {
    inputString += key;
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(inputString);
  }
  if (startPressed) {
    delay(200);
    if (inputString.length() > 0) {
      intervalInSeconds = inputString.toInt();
      inputString = "";
    }
    startMillis = millis();
    lastSecondUpdate = millis();
    remaining = START_SEQUENCE_SECONDS + 1;
    timerRunning = true;
    currentState = STATE_COUNTDOWN;
    updateDisplay();
    return;
  }
  if (stopPressed) {
    delay(200);
    intervalInSeconds += 5;
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(intervalInSeconds);
    lcd.print(" s");
    return;
  }
  if (key == '#') {
    if (inputString.length() > 0) {
      intervalInSeconds = inputString.toInt();
      inputString = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(intervalInSeconds);
      lcd.print(" s");
    }
    return;
  }
}

void handlePausedState(bool pausePressed, bool stopPressed) {
  if (pausePressed) {
    delay(200);
    startMillis += (millis() - pauseMillis);
    currentState = STATE_COUNTDOWN;
    updateDisplay();
    return;
  }
  if (stopPressed) {
    delay(200);
    intervalInSeconds = DEFAULT_INTERVAL;
    timerRunning = false;
    currentState = STATE_SETUP;
    inputString = "";
    updateDisplay();
    return;
  }
}

void handleCountdownState(bool pausePressed, bool stopPressed) {
  if (pausePressed) {
    delay(200);
    pauseMillis = millis();
    currentState = STATE_PAUSED;
    updateDisplay();
    return;
  }
  if (stopPressed) {
    delay(200);
    intervalInSeconds = DEFAULT_INTERVAL;
    timerRunning = false;
    currentState = STATE_SETUP;
    inputString = "";
    updateDisplay();
    return;
  }
  unsigned long now = millis();
  if (now - lastSecondUpdate >= ONE_SECOND) {
    lastSecondUpdate = now;
    remaining--;
    if (remaining > 0) {
      updateDisplay();
      if (remaining <= START_SEQUENCE_SECONDS) {
        beepAndRedLed();
      }
    } else {
      updateDisplay();
      longBeepAndGreenLed();
      startMillis = millis();
      lastSecondUpdate = millis();
      remaining = intervalInSeconds;
      updateDisplay();
    }
  }
}


void updateDisplay() {
  if (currentState != lastState) {
    lastState = currentState;
    lcd.clear();
    lcd.setCursor(0, 0);
  }
  switch(currentState) {
    case STATE_SETUP:
      lcd.print("Nastav interval");
      lcd.setCursor(0, 1);
      lcd.print("Akt: ");
      lcd.print(intervalInSeconds);
      lcd.print(" s");
      break;
    case STATE_COUNTDOWN:
      lcd.setCursor(0, 0);
      lcd.print("Odpocet:");
      if (remaining != lastDisplayedRemaining) {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(remaining);
        lcd.print(" s");
        lastDisplayedRemaining = remaining;
      }
      break;
    case STATE_PAUSED:
      lcd.print("PAUZA");
      lcd.setCursor(0, 1);
      lcd.print("Pokracuj na PAUSE");
      break;
  }
}