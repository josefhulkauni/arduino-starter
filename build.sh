#!/bin/bash
# Build a upload pro Arduino pomocí arduino-cli
# Nastavte BOARD na typ vaší desky, např. arduino:avr:uno
# Nastavte PORT na sériový port vaší desky

BOARD="arduino:avr:uno"
PORT="/dev/ttyACM0"
SKETCH="starter.ino"

arduino-cli compile --fqbn $BOARD $SKETCH
arduino-cli upload -p $PORT --fqbn $BOARD $SKETCH

