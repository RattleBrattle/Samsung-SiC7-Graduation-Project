#include "led_controller.h"
#include <Arduino.h>

LEDController::LEDController() {
  manualMode = false;  // Start in auto mode
  for(int i = 0; i < 3; i++) {
    ledStates[i] = false;
  }
}

void LEDController::begin() {
  pinMode(LED1_GPIO, OUTPUT);
  pinMode(LED2_GPIO, OUTPUT);
  pinMode(LED3_GPIO, OUTPUT);
  
  // Turn off all LEDs initially
  setAllLEDs(false);
}

void LEDController::setLED(int ledNumber, bool state) {
  if (ledNumber < 1 || ledNumber > 3) return;
  
  ledStates[ledNumber-1] = state;
  
  switch(ledNumber) {
    case 1: updateLED(LED1_GPIO, state); break;
    case 2: updateLED(LED2_GPIO, state); break;
    case 3: updateLED(LED3_GPIO, state); break;
  }
}

void LEDController::setAllLEDs(bool state) {
  for(int i = 1; i <= 3; i++) {
    setLED(i, state);
  }
}

void LEDController::toggleLED(int ledNumber) {
  if (ledNumber < 1 || ledNumber > 3) return;
  setLED(ledNumber, !ledStates[ledNumber-1]);
}

void LEDController::autoControlLEDs(int lightIntensity) {
  if (manualMode) {
    return;  // Don't auto control if in manual mode
  }
  
  if (lightIntensity < LIGHT_INTENSITY_THRESHOLD) {
    // Night time - turn on LEDs
    setAllLEDs(true);
  } else {
    // Day time - turn off LEDs
    setAllLEDs(false);
  }
}

bool LEDController::getLEDState(int ledNumber) {
  if (ledNumber < 1 || ledNumber > 3) return false;
  return ledStates[ledNumber-1];
}

void LEDController::setManualMode(bool manual) {
  manualMode = manual;
  Serial.print("LED Mode changed to: ");
  Serial.println(manual ? "MANUAL" : "AUTO");
}

bool LEDController::isManualMode() {
  return manualMode;
}

void LEDController::updateLED(int ledPin, bool state) {
  digitalWrite(ledPin, state ? HIGH : LOW);
}