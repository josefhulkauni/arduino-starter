# Arduino projekt

## Tinkercad Simulation

[View the starting device simulation in Tinkercad](https://www.tinkercad.com/things/9P838sbefMS-starter)


## Použití v Arduino IDE

1. Otevřete Arduino IDE.
2. Zvolte Soubor > Otevřít a vyberte soubor starter.ino v této složce.
3. Nahrajte do vašeho Arduino zařízení.

Knihovny použité v projektu:
- LiquidCrystal_I2C
- Keypad

Pokud nejsou nainstalované, nainstalujte je přes Správce knihoven v Arduino IDE.

## Poznámky
- Složky cmake-build-debug/ a soubor CMakeLists.txt nejsou potřeba pro Arduino IDE.
- Projekt je připraven pro vývoj a nahrávání přímo v Arduino IDE.

# Arduino projekt pro CLion + Arduino CLI

## Vývoj v CLion

1. Otevřete složku starter v CLion.
2. Editujte soubor starter.ino jako běžný C++ kód.
3. Build a upload proveďte pomocí skriptu build.sh:
   - Upravte build.sh podle vaší desky (BOARD) a portu (PORT).
   - Spusťte v terminálu: `./build.sh`
   - Skript použije arduino-cli pro kompilaci a nahrání.

## Požadavky
- Nainstalujte [arduino-cli](https://arduino.github.io/arduino-cli/).
- Přidejte do PATH.
- Nainstalujte potřebné knihovny (LiquidCrystal_I2C, Keypad) přes arduino-cli nebo Arduino IDE.

## Poznámky
- CMakeLists.txt slouží pouze pro CLion, build řeší build.sh.
- starter.ino musí být v kořeni složky starter.
- .gitignore ignoruje build artefakty a CLion soubory.
