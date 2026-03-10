// IOManager.cpp

#include "IOManager.h"
#include "config.h"
#include <Arduino.h>

void IOManager::init() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(RGB_R_PIN, OUTPUT);
  pinMode(RGB_G_PIN, OUTPUT);
  pinMode(RGB_B_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(BUZZ_PIN, LOW);
  digitalWrite(RGB_R_PIN, LOW);
  digitalWrite(RGB_G_PIN, LOW);
  digitalWrite(RGB_B_PIN, LOW);
}

// output managers
void IOManager::setLed(bool on) {
  digitalWrite(LED_PIN, on ? HIGH : LOW);
}

void IOManager::setLed2(bool on) {
  digitalWrite(LED2_PIN, on ? HIGH : LOW);
}

void IOManager::buzz(bool on) {
  digitalWrite(BUZZ_PIN, on ? HIGH : LOW);
}

void IOManager::setRGBColor(int r, int g, int b) {
  analogWrite(RGB_R_PIN, r);
  analogWrite(RGB_G_PIN, g);
  analogWrite(RGB_B_PIN, b);
}