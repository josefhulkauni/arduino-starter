// starter.ino
// Version: 0.1.0
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
  STATE_WAITING_TO_START,
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

bool isFirstStart = true;
String inputString = "";

// Prototypes pro funkce
void playStartSignal();
void updateDisplay();
void handleSetupState();
void handleWaitingState();
void handleCountdownState();
void handlePausedState();

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
  
  // Zobrazí úvodní obrazovku
  lcd.setCursor(0, 0);
  lcd.print("Nastav interval");
  lcd.setCursor(0, 1);
  lcd.print("Akt: ");
  lcd.print(intervalInSeconds);
  lcd.print(" s");
}

void loop()
{
  /*
    // Digitální pin je ve stavu LOW, když je tlačítko stisknuto
  if (digitalRead(startPin) == LOW) {
    // Snížíte šum (debounce) při stisku
    delay(50); 
    
    // Zobrazí text po stisku
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tlacitko START");
    lcd.setCursor(0, 1);
    lcd.print("stisknuto!");
  } else {
    // Pokud tlačítko není stisknuto, zobrazí původní text
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Test tlacitka");
    lcd.setCursor(0, 1);
    lcd.print("Stiskni START");
  }
  */
  switch (currentState) {
    case STATE_SETUP:
      handleSetupState();
      break;
    case STATE_WAITING_TO_START:
      handleWaitingState();
      break;
    case STATE_COUNTDOWN:
      handleCountdownState();
      break;
    case STATE_PAUSED:
      handlePausedState();
      break;
  }
  
}

//--------------------- Obsluha stavů ---------------------

void handleSetupState() {
  char key = keypad.getKey();
  
  if (key != NO_KEY && isdigit(key)) {
    inputString += key;
    lcd.setCursor(0, 1);
    lcd.print("                "); // Vyčistí řádek
    lcd.setCursor(0, 1);
    lcd.print(inputString);
  }
  
  // Stisk klávesy '#' potvrdí nastavený interval
  if (key == '#' || millis() > 5000) {
    if (inputString.length() > 0) {
      intervalInSeconds = inputString.toInt();
      inputString = "";
    }
    currentState = STATE_WAITING_TO_START;
    updateDisplay();
  }
}

void handleWaitingState() {
  if (digitalRead(startPin) == LOW) { // Tlačítko START stisknuto
    delay(200); // Debounce
    
    // Pro první spuštění startuje hned
    if(isFirstStart) {
      isFirstStart = false;
    }
      playStartSignal();
      startMillis = millis();
      currentState = STATE_COUNTDOWN;
    
    updateDisplay();
  }
}

void handleCountdownState() {
  Serial.println("Vstup do stavu: COUNTDOWN");
  unsigned long elapsed = (millis() - startMillis) / 1000;
  remaining = intervalInSeconds - elapsed;
  
  // Vypisuje uplynulý a zbývající čas
  Serial.print(millis());
  Serial.print("Uplynuly cas: ");
  Serial.print(elapsed);
  Serial.print("s, Zbyvajici cas: ");
  Serial.print(remaining);
  Serial.println("s");
  
  if (remaining > 0) {
    updateDisplay();
  } else {
    // Odpočet dokončen, přehraje signál
    playStartSignal();
    // Resetuje pro další odpočet
    startMillis = millis(); 
    isFirstStart = false;
  }
  
  // Kontrola tlačítek
  if (digitalRead(pausePin) == LOW) {
    delay(200);
    pauseMillis = millis();
    currentState = STATE_PAUSED;
    updateDisplay();
  }
  
  if (digitalRead(stopPin) == LOW) {
    delay(200);
    currentState = STATE_SETUP;
    isFirstStart = true;
    updateDisplay();
  }
}

void handlePausedState() {
  if (digitalRead(pausePin) == LOW) {
    delay(200);
    // Vypočte uplynulý čas od pauzy a aktualizuje startMillis
    startMillis += (millis() - pauseMillis);
    currentState = STATE_COUNTDOWN;
    updateDisplay();
  }
  
  if (digitalRead(stopPin) == LOW) {
    delay(200);
    currentState = STATE_SETUP;
    isFirstStart = true;
    updateDisplay();
  }
}

//--------------------- Pomocné funkce ---------------------

void playStartSignal() {
  // Čtyři krátká pípnutí
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledRedPin, HIGH); 
    tone(buzzerPin, 1000, 100);
    delay(100);
    digitalWrite(ledRedPin, LOW); 
    delay(800);
  }
  // Jedno dlouhé pípnutí
    digitalWrite(ledGreenPin, HIGH); 
    tone(buzzerPin, 1000, 500);
    delay(500);
    digitalWrite(ledGreenPin, LOW); 
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
    case STATE_WAITING_TO_START:
      lcd.print("Pripraveno");
      lcd.setCursor(0, 1);
      lcd.print("Start:");
      lcd.print(intervalInSeconds);
      lcd.print(" s");
      break;
    case STATE_COUNTDOWN:
      //unsigned long elapsed = (millis() - startMillis) / 1000;
  	  //unsigned long remaining = 10; //intervalInSeconds - elapsed;
      
      lcd.setCursor(0, 0);
      lcd.print("Odpocet:");
      if (remaining != lastDisplayedRemaining) {
        lcd.setCursor(0, 1);
        lcd.print("                "); // 16 mezer pro 16 znaků
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